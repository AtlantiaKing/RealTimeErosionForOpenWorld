#pragma once

#include <Components/Component.h>

#include <vector>
#include <utility>

namespace Erosion
{
	class TerrainGeneratorComponent;

	class RealtimeGenerator final : public leap::Component
	{
	public:
		RealtimeGenerator() = default;
		virtual ~RealtimeGenerator() = default;
		RealtimeGenerator(const RealtimeGenerator& other) = delete;
		RealtimeGenerator(RealtimeGenerator&& other) = delete;
		RealtimeGenerator& operator=(const RealtimeGenerator& other) = delete;
		RealtimeGenerator& operator=(RealtimeGenerator&& other) = delete;

		void SetPlayerTransform(leap::Transform* pPlayer);

	private:
		struct Chunk final
		{
			int x{};
			int y{};
			TerrainGeneratorComponent* pTerrain{};
		};

		void Awake() override;
		void Update() override;

		leap::Transform* m_pPlayer{};

		std::vector<std::pair<int,int>> m_DiscoveredChunks{};
		std::vector<Chunk> m_Chunks{};
		std::vector<TerrainGeneratorComponent*> m_Pool{};

		int m_Range{ 5 };

		float m_TimePerChunk{ 0.01f };
		float m_CurTime{};

		int m_PrevX{};
		int m_PrevZ{};
	};
}