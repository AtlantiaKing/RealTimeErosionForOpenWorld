#pragma once

#include <Components/Component.h>

namespace Erosion
{
	class PlaneFollow final : public leap::Component
	{
	public:
		PlaneFollow() = default;
		~PlaneFollow() = default;
		PlaneFollow(const PlaneFollow& other) = delete;
		PlaneFollow(PlaneFollow&& other) = delete;
		PlaneFollow& operator=(const PlaneFollow& other) = delete;
		PlaneFollow& operator=(PlaneFollow&& other) = delete;

		void SetPlayer(leap::Transform* pTransform);

	private:
		virtual void Start() override;
		virtual void Update() override;

		leap::Transform* m_pPlayer{};
	};
}