#include "RealtimeGenerator.h"

#include <Components/Transform/Transform.h>
#include <SceneGraph/GameObject.h>
#include <SceneGraph/SceneManager.h>
#include <Components/RenderComponents/TerrainComponent.h>
#include "TerrainGeneratorComponent.h"

#include <GameContext/GameContext.h>
#include <GameContext/Timer.h>
#include "../Manager/TerrainManager.h"

void Erosion::RealtimeGenerator::SetPlayerTransform(leap::Transform* pPlayer)
{
	m_pPlayer = pPlayer;
}

void Erosion::RealtimeGenerator::Awake()
{
	const int width{ m_Range * 2 + 1 };
	const int worldSize{ width * width };
	m_Chunks.reserve(worldSize);
	m_Pool.resize(worldSize);

	for (int i{}; i < worldSize; ++i)
	{
		const auto pTerrain{ leap::SceneManager::GetInstance().GetActiveScene()->CreateGameObject("Terrain") };
		auto pTerrainComponent{ pTerrain->AddComponent<leap::TerrainComponent>() };
		pTerrainComponent->SetSize(256);

		m_Pool[i] = pTerrainComponent;
	}
}

void Erosion::RealtimeGenerator::Update()
{
	if (m_CurTime < m_TimePerChunk)
	{
		m_CurTime += leap::GameContext::GetInstance().GetTimer()->GetDeltaTime();
		return;
	}

	m_CurTime = 0.0f;

	const auto& playerPos{ m_pPlayer->GetWorldPosition() };

	const int x{ static_cast<int>(playerPos.x / 256) };
	const int z{ static_cast<int>(playerPos.z / 256) };

	if (m_PrevX == x && m_PrevZ == z) return;

	for (int i{ static_cast<int>(m_Chunks.size()) - 1 }; i >= 0; --i)
	{
		const auto& chunk{ m_Chunks[i] };
		if (chunk.x >= x - m_Range && chunk.x <= x + m_Range && chunk.y >= z - m_Range && chunk.y <= z + m_Range) continue;

		m_Pool.push_back(chunk.pTerrain);
		m_Chunks.erase(begin(m_Chunks) + i);
	}

	for (int xPos{ x - m_Range }; xPos <= x + m_Range; ++xPos)
	{
		for (int zPos{ z - m_Range }; zPos <= z + m_Range; ++zPos)
		{
			const auto it{ std::find_if(begin(m_Chunks), end(m_Chunks), [=](const auto& chunk) { return chunk.x == xPos && chunk.y == zPos; }) };

			if (it != end(m_Chunks))
			{
				TerrainManager::GetInstance().Generate(xPos, zPos, it->pTerrain, abs(xPos - x) < m_Range / 2 && abs(zPos - z) < m_Range / 2);
				continue;
			}

			auto pTerrain{ m_Pool[0] };
			m_Pool.erase(begin(m_Pool));
			pTerrain->GetTransform()->SetLocalPosition(static_cast<float>(xPos * 256), 0.0f, static_cast<float>(zPos * 256));
			TerrainManager::GetInstance().Unregister(xPos, zPos);
			TerrainManager::GetInstance().Generate(xPos, zPos, pTerrain, abs(xPos - x) < m_Range / 2 && abs(zPos - z) < m_Range / 2);
			m_Chunks.emplace_back(xPos, zPos, pTerrain);
		}
	}

	m_PrevX = x;
	m_PrevZ = z;
}
