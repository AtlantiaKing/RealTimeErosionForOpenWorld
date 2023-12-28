#pragma once

#include <Components/Component.h>

#include <vector>
#include <utility>

namespace Erosion
{
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
		void Update() override;

		leap::Transform* m_pPlayer{};

		std::vector<std::pair<int,int>> m_DiscoveredChunks{};

		float m_TimePerChunk{ 0.01f };
		float m_CurTime{};
	};
}