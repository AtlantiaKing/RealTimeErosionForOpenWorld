#include "TerrainManager.h"

#include "../Components/TerrainGeneratorComponent.h"
#include <SceneGraph/GameObject.h>
#include <Components/RenderComponents/TerrainComponent.h>

void Erosion::TerrainManager::AddTerrain(int x, int y, const TerrainGeneratorComponent* pTerrain, std::vector<float> heights)
{
	// Calculate the chunk position
	const int chunkX{ x / 256 };
	const int chunkY{ y / 256 };

	// Add the new chunk to the vector
	m_Chunks.emplace_back(chunkX, chunkY, pTerrain, std::move(heights));
	auto& chunk{ m_Chunks[m_Chunks.size() - 1] };

	// Evaluate all neighbouring chunks and update their shared vertices if needed
	EvaluateChunk(chunk, chunkX - 1, chunkY, EvaluateDirection::Left);
	EvaluateChunk(chunk, chunkX + 1, chunkY, EvaluateDirection::Right);
	EvaluateChunk(chunk, chunkX, chunkY - 1, EvaluateDirection::Back);
	EvaluateChunk(chunk, chunkX, chunkY + 1, EvaluateDirection::Forward);
}

void Erosion::TerrainManager::RemoveTerrain(int x, int y)
{
	std::erase_if(m_Chunks, [=](const auto& chunk) { return chunk.x == x && chunk.y == y; });
}

void Erosion::TerrainManager::EvaluateChunk(Chunk& cur, int evalX, int evalY, EvaluateDirection dir)
{
	// Get the neighbouring chunk
	const auto evalIt{ std::find_if(begin(m_Chunks), end(m_Chunks), [=](const auto& chunk) { return chunk.x == evalX && chunk.y == evalY; }) };

	// If the neighbouring chunk doesn't exist, stop here
	if (evalIt == end(m_Chunks)) return;

	// Get the terrain components and their height vectors
	auto pCurTerrain{ cur.pTerrain->GetGameObject()->GetComponent<leap::TerrainComponent>() };
	auto pEvalTerrain{ evalIt->pTerrain->GetGameObject()->GetComponent<leap::TerrainComponent>() };
	auto& curHeights{ cur.heights };
	auto& evalHeights{ evalIt->heights };

	// Get the terrain size
	const int terrainSize{ static_cast<int>(sqrtf(static_cast<float>(curHeights.size()))) };

	// Fix gaps between terrains by averaging the outer edges
	switch (dir)
	{
	case EvaluateDirection::Forward:
	{
		const int curZValue{ (terrainSize - 1) * terrainSize };
		for (int x{}; x < terrainSize; ++x)
		{
			float& curHeight{ curHeights[x + curZValue] };
			float& evalHeight{ evalHeights[x] };

			float averageHeight{ (curHeight + evalHeight) / 2.0f };
			curHeight = averageHeight;
			evalHeight = averageHeight;
		}

		break;
	}
	case EvaluateDirection::Back:
	{
		const int evalZValue{ (terrainSize - 1) * terrainSize };
		for (int x{}; x < terrainSize; ++x)
		{
			float& curHeight{ curHeights[x] };
			float& evalHeight{ evalHeights[x + evalZValue] };

			float averageHeight{ (curHeight + evalHeight) / 2.0f };
			curHeight = averageHeight;
			evalHeight = averageHeight;
		}

		break;
	}
	case EvaluateDirection::Right:
	{
		const int curXValue{ terrainSize - 1 };
		for (int z{}; z < terrainSize; ++z)
		{
			float& curHeight{ curHeights[curXValue + z * terrainSize] };
			float& evalHeight{ evalHeights[z * terrainSize] };

			float averageHeight{ (curHeight + evalHeight) / 2.0f };
			curHeight = averageHeight;
			evalHeight = averageHeight;
		}

		break;
	}
	case EvaluateDirection::Left:
	{
		const int evalXValue{ terrainSize - 1 };
		for (int z{}; z < terrainSize; ++z)
		{
			float& curHeight{ curHeights[z * terrainSize] };
			float& evalHeight{ evalHeights[evalXValue + z * terrainSize] };

			float averageHeight{ (curHeight + evalHeight) / 2.0f };
			curHeight = averageHeight;
			evalHeight = averageHeight;
		}

		break;
	}
	}

	// Apply the heights
	pCurTerrain->SetHeights(curHeights);
	pEvalTerrain->SetHeights(evalHeights);
}