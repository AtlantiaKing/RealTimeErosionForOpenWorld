#include "TerrainManager.h"

#include "../Components/TerrainGeneratorComponent.h"

#include <Presets/Presets.h>

#include "../ErosionAlgorithms/HansBeyer.h"

#include <algorithm>

#include <Components/RenderComponents/TerrainComponent.h>

Erosion::TerrainManager::TerrainManager()
{
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

				if (chunk.eroded && (!m_ErodedChunks.contains(chunk.x) || !m_ErodedChunks[chunk.x].contains(chunk.y)))
				{
					m_ErodedChunks[chunk.x].insert(chunk.y);
					Erode(chunk.x, chunk.y, pErosion);
				}

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

				const std::lock_guard lock{ m_QueueMutex };
				if (chunk.eroded) m_ActiveChunks[chunk.x][chunk.y] = chunk.pTerrain;
			}
		}
	};
}

Erosion::TerrainManager::~TerrainManager()
{
	m_Running = false;
}

void Erosion::TerrainManager::Generate(int x, int y, leap::TerrainComponent* pTerrain, bool eroded)
{
	if (x < 0 || y < 0) return;

	const std::lock_guard lock{ m_QueueMutex };

	if (m_ActiveChunks.contains(x) && m_ActiveChunks[x].contains(y)) return;

	m_ChunkQueue.push(Chunk{ x,y, pTerrain, eroded });
}

void Erosion::TerrainManager::Unregister(int x, int y)
{
	const std::lock_guard lock{ m_QueueMutex };
	if (!m_ActiveChunks.contains(x) || !m_ActiveChunks[x].contains(y)) return;

	m_ActiveChunks[x].erase(y);
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
