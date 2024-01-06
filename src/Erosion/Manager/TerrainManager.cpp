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
				if (m_MainThreadBusy) continue;

				m_QueueMutex.lock();
				if (m_ChunkQueue.empty())
				{
					m_QueueMutex.unlock();
					continue;
				}
				Chunk chunk{ m_ChunkQueue.front() };
				m_ChunkQueue.pop();
				m_QueueMutex.unlock();

				const bool exists{ m_ActiveChunks.contains(chunk.x) && m_ActiveChunks[chunk.x].contains(chunk.y) && m_ActiveChunks[chunk.x][chunk.y].first };

				if (!chunk.eroded && exists) continue;

				bool isChunkEroded{ m_ErodedChunks.contains(chunk.x) && m_ErodedChunks[chunk.x].contains(chunk.y) };
				if (chunk.eroded && !isChunkEroded)
				{
					m_ErodedChunks[chunk.x].insert(chunk.y);
					Erode(chunk.x, chunk.y, pErosion);
				}

				// Generate perlin for non eroded chunks
				if (!chunk.eroded && !isChunkEroded)
				{
					const int paddingSize{ m_ChunkSize / 2 };
					for (int x{}; x < m_ChunkSize; ++x)
					{
						for (int y{}; y < m_ChunkSize; ++y)
						{
							m_Heightmap.GetHeight(paddingSize + chunk.x * (m_ChunkSize - 1) + x, paddingSize + chunk.y * (m_ChunkSize - 1) + y);
						}
					}
				}

				m_ActiveChunks[chunk.x][chunk.y] = std::make_pair(chunk.pTerrain, isChunkEroded);

				if ((chunk.eroded && isChunkEroded) || !chunk.eroded)
				{
					m_ChangedChunks.emplace_back(chunk.x, chunk.y, m_ActiveChunks[chunk.x][chunk.y].first);
				}
				else
				{
					for (int x{ -1 }; x <= 1; ++x)
					{
						for (int y{ -1 }; y <= 1; ++y)
						{
							if (!m_ActiveChunks.contains(chunk.x + x)) continue;
							if (!m_ActiveChunks[chunk.x].contains(chunk.y + y)) continue;

							auto pTerrain{ m_ActiveChunks[chunk.x + x][chunk.y + y].first };

							if (pTerrain == nullptr) continue;

							m_ChangedChunks.emplace_back(chunk.x + x, chunk.y + y, m_ActiveChunks[chunk.x + x][chunk.y + y].first);
						}
					}
				}

				m_Reload = true;
				m_MainThreadBusy = true;
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

	if (m_ActiveChunks.contains(x) && m_ActiveChunks[x].contains(y) && m_ActiveChunks[x][y].second) return;

	const std::lock_guard lock{ m_QueueMutex };
	m_ChunkQueue.push(Chunk{ x,y, pTerrain, eroded });
}

void Erosion::TerrainManager::Unregister(int x, int y)
{
	if (!m_ActiveChunks.contains(x) || !m_ActiveChunks[x].contains(y)) return;

	m_ActiveChunks[x].erase(y);
}

void Erosion::TerrainManager::Update()
{
	if (!m_Reload) return;

	m_Reload = false;

	const int paddingSize{ m_ChunkSize / 2 };
	for (auto& chunk : m_ChangedChunks)
	{
		std::vector<float> chunkData(m_ChunkSize * m_ChunkSize);
		for (int x{}; x < m_ChunkSize; ++x)
		{
			for (int y{}; y < m_ChunkSize; ++y)
			{
				chunkData[x + y * m_ChunkSize] = m_Heightmap.GetHeight(paddingSize + chunk.x * (m_ChunkSize - 1) + x, paddingSize + chunk.y * (m_ChunkSize - 1) + y);
			}
		}
		chunk.pTerrain->SetHeights(chunkData);
	}

	m_ChangedChunks.clear();
	m_MainThreadBusy = false;
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
