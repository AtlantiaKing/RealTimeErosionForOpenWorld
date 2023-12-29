#include "TerrainManager.h"

#include "../Components/TerrainGeneratorComponent.h"

#include <Presets/Presets.h>

#include "../ErosionAlgorithms/HansBeyer.h"

#include <algorithm>

#include <Components/RenderComponents/TerrainComponent.h>

Erosion::TerrainManager::TerrainManager()
{
	// Create generator
	m_Perlin.SetSize(static_cast<float>(m_ChunkSize));
	that::preset::Presets::CreateDefaultTerrain(m_Perlin, static_cast<unsigned int>(time(nullptr)), m_PerlinMultiplier);
	m_Perlin.GetHeightMap().SetBlendMode(that::HeightMap::BlendMode::Multiply);

	m_Thread = std::jthread
	{
		[this]()
		{
			std::unique_ptr<ITerrainGenerator> pErosion = std::make_unique<HansBeyer>();
			while (true)
			{
				if (!m_Running) return;

				m_QueueMutex.lock();
				if (m_ChunkQueue.empty())
				{
					m_QueueMutex.unlock();
					continue;
				}
				Chunk chunk{ m_ChunkQueue.front() };
				m_ChunkQueue.pop();
				m_QueueMutex.unlock();

				EvaluateChunkPadding(chunk.x, chunk.y);

				GeneratePerlin(chunk.x, chunk.y);

				Erode(chunk.x, chunk.y, pErosion);

				std::vector<float> chunkData(m_ChunkSize * m_ChunkSize);
				const int paddingSize{ m_ChunkSize / 2 };
				for (int x{}; x < m_ChunkSize; ++x)
				{
					for (int y{}; y < m_ChunkSize; ++y)
					{
						chunkData[x + y * m_ChunkSize] = m_Heightmap.GetHeight(paddingSize + chunk.x * (m_ChunkSize - 1) + x, paddingSize + chunk.y * (m_ChunkSize - 1) + y);
					}
				}
				chunk.pTerrain->SetHeights(chunkData);
			}
		}
	};
}

Erosion::TerrainManager::~TerrainManager()
{
	m_Running = false;
}

void Erosion::TerrainManager::Generate(int x, int y, leap::TerrainComponent* pTerrain)
{
	if (x < 0 || y < 0) return;

	std::lock_guard lock{ m_QueueMutex };
	m_ChunkQueue.push(Chunk{ x,y, pTerrain });
}

void Erosion::TerrainManager::EvaluateChunkPadding(int chunkX, int chunkY)
{
	const int paddingSize{ m_ChunkSize / 2 };

	const int startX{ chunkX * m_ChunkSize + paddingSize };
	const int startY{ chunkY * m_ChunkSize + paddingSize };

	const int left{ startX - paddingSize / 2 };
	const int right{ startX + (m_ChunkSize - 1) + paddingSize / 2 };
	const int bottom{  startY - paddingSize / 2 };
	const int top{ startY + (m_ChunkSize - 1) + paddingSize / 2 };
	const int centerX{ startX + m_ChunkSize / 2 };
	const int centerY{ startY + m_ChunkSize / 2 };

	if (m_Heightmap.GetHeight(left, centerY) < FLT_EPSILON)
	{
		for (int x{ startX - paddingSize }; x < startX; ++x)
		{
			for (int y{ startY }; y < startY + m_ChunkSize; ++y)
			{
				float noiseValue{ m_Perlin.GetHeight(static_cast<float>(x), static_cast<float>(y)) };
				m_Heightmap.GetHeight(x,y) = noiseValue;
			}
		}
	}
	if (m_Heightmap.GetHeight(right, centerY) < FLT_EPSILON)
	{
		for (int x{ startX + m_ChunkSize }; x < startX + m_ChunkSize + paddingSize; ++x)
		{
			for (int y{ startY }; y < startY + m_ChunkSize; ++y)
			{
				float noiseValue{ m_Perlin.GetHeight(static_cast<float>(x), static_cast<float>(y)) };
				m_Heightmap.GetHeight(x, y) = noiseValue;
			}
		}
	}
	if (m_Heightmap.GetHeight(centerX, top) < FLT_EPSILON)
	{
		for (int x{ startX }; x < startX + m_ChunkSize; ++x)
		{
			for (int y{ startY + m_ChunkSize }; y < startY + m_ChunkSize + paddingSize; ++y)
			{
				float noiseValue{ m_Perlin.GetHeight(static_cast<float>(x), static_cast<float>(y)) };
				m_Heightmap.GetHeight(x, y) = noiseValue;
			}
		}
	}
	if (m_Heightmap.GetHeight(centerX, bottom) < FLT_EPSILON)
	{
		for (int x{ startX }; x < startX + m_ChunkSize; ++x)
		{
			for (int y{ startY - paddingSize }; y < startY; ++y)
			{
				float noiseValue{ m_Perlin.GetHeight(static_cast<float>(x), static_cast<float>(y)) };
				m_Heightmap.GetHeight(x, y) = noiseValue;
			}
		}
	}
	if (m_Heightmap.GetHeight(left, bottom) < FLT_EPSILON)
	{
		for (int x{ startX - paddingSize }; x < startX; ++x)
		{
			for (int y{ startY - paddingSize }; y < startY; ++y)
			{
				float noiseValue{ m_Perlin.GetHeight(static_cast<float>(x), static_cast<float>(y)) };
				m_Heightmap.GetHeight(x, y) = noiseValue;
			}
		}
	}
	if (m_Heightmap.GetHeight(right, bottom) < FLT_EPSILON)
	{
		for (int x{ startX + m_ChunkSize }; x < startX + m_ChunkSize + paddingSize; ++x)
		{
			for (int y{ startY - paddingSize }; y < startY; ++y)
			{
				float noiseValue{ m_Perlin.GetHeight(static_cast<float>(x), static_cast<float>(y)) };
				m_Heightmap.GetHeight(x, y) = noiseValue;
			}
		}
	}
	if (m_Heightmap.GetHeight(left, top) < FLT_EPSILON)
	{
		for (int x{ startX - paddingSize }; x < startX; ++x)
		{
			for (int y{ startY + m_ChunkSize }; y < startY + m_ChunkSize + paddingSize; ++y)
			{
				float noiseValue{ m_Perlin.GetHeight(static_cast<float>(x), static_cast<float>(y)) };
				m_Heightmap.GetHeight(x, y) = noiseValue;
			}
		}
	}
	if (m_Heightmap.GetHeight(right, top) < FLT_EPSILON)
	{
		for (int x{ startX + m_ChunkSize }; x < startX + m_ChunkSize + paddingSize; ++x)
		{
			for (int y{ startY + m_ChunkSize }; y < startY + m_ChunkSize + paddingSize; ++y)
			{
				float noiseValue{ m_Perlin.GetHeight(static_cast<float>(x), static_cast<float>(y)) };
				m_Heightmap.GetHeight(x, y) = noiseValue;
			}
		}
	}
}

void Erosion::TerrainManager::GeneratePerlin(int chunkX, int chunkY)
{
	const int paddingSize{ m_ChunkSize / 2 };

	const int startX{ chunkX * m_ChunkSize + paddingSize };
	const int startY{ chunkY * m_ChunkSize + paddingSize };

	for (int x{ startX }; x < startX + m_ChunkSize; ++x)
	{
		for (int y{ startY }; y < startY + m_ChunkSize; ++y)
		{
			float& height{ m_Heightmap.GetHeight(x, y) };
			if (height > FLT_EPSILON) continue;

			const float noiseValue{ m_Perlin.GetHeight(static_cast<float>(x), static_cast<float>(y)) };
			height = noiseValue;
		}
	}
}

void Erosion::TerrainManager::Erode(int x, int y, const std::unique_ptr<ITerrainGenerator>& pErosion)
{
	pErosion->SetChunk(x, y);
	pErosion->GetHeights(m_Heightmap);
}

void Erosion::TerrainManager::UpdateComponents(int /*chunkX*/, int /*chunkY*/)
{
	/*std::vector<float> chunkData(m_ChunkSize * m_ChunkSize);
	const int paddingSize{ m_ChunkSize / 2 };
	for (int x{}; x < m_ChunkSize; ++x)
	{
		for (int y{}; y < m_ChunkSize; ++y)
		{
			chunkData[x + y * m_ChunkSize] = m_Heightmap.GetHeight(paddingSize + chunkX * m_ChunkSize + x, paddingSize + chunkY * m_ChunkSize + y);
		}
	}
	chunk.pTerrain->SetHeights(chunkData);*/
}
