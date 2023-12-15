#pragma once
#include "Components/Component.h"
#include <vector>
#include <memory>
#include "Command.h"

namespace Erosion
{
	class FreeCamMovement final : public leap::Component
	{
	public:
		FreeCamMovement() = default;
		virtual ~FreeCamMovement() = default;
		FreeCamMovement(const FreeCamMovement& other) = delete;
		FreeCamMovement(FreeCamMovement&& other) = delete;
		FreeCamMovement& operator=(const FreeCamMovement& other) = delete;
		FreeCamMovement& operator=(FreeCamMovement&& other) = delete;
	private:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void OnDestroy() override;

		std::vector <std::unique_ptr<leap::Command>> m_Commands{};
		bool m_Sprinting{};
	};
}