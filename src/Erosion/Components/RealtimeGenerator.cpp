#include "RealtimeGenerator.h"

#include <Components/Transform/Transform.h>
#include <SceneGraph/GameObject.h>
#include <SceneGraph/SceneManager.h>
#include <Components/RenderComponents/TerrainComponent.h>
#include "TerrainGeneratorComponent.h"

#include <GameContext/GameContext.h>
#include <GameContext/Timer.h>

void Erosion::RealtimeGenerator::SetPlayerTransform(leap::Transform* pPlayer)
{
	m_pPlayer = pPlayer;
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

	constexpr int range{ 7 };
	for (int xPos{ x - range }; xPos <= x + range; ++xPos)
	{
		for (int zPos{ z - range }; zPos <= z + range; ++zPos)
		{
			const auto it{ std::find_if(begin(m_DiscoveredChunks), end(m_DiscoveredChunks), [=](const auto& chunk) { return chunk.first == xPos && chunk.second == zPos; }) };

			if (it != end(m_DiscoveredChunks)) continue;

			m_DiscoveredChunks.push_back(std::pair{ xPos,zPos });

			const auto pTerrain{ leap::SceneManager::GetInstance().GetActiveScene()->CreateGameObject("Terrain") };
			pTerrain->AddComponent<leap::TerrainComponent>()->SetSize(256, 1);
			pTerrain->GetTransform()->Translate(static_cast<float>(xPos * 256), 0.0f, static_cast<float>(zPos * 256));
			pTerrain->AddComponent<Erosion::TerrainGeneratorComponent>()->GenerateAt(xPos * 256, zPos * 256);

			return;
		}
	}
}
