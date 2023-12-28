#include <Windows.h>
#include <Leap.h>

#include <SceneGraph/SceneManager.h>

#include <ServiceLocator/ServiceLocator.h>
#include <GameContext/GameContext.h>

#include <GameContext/Logger/ImGuiLogger.h>
#include <GameContext/Logger/ConsoleLogger.h>
#include <Interfaces/IPhysics.h>

#include <Debug.h>

// Visual leak detector
#if _DEBUG
#include <vld.h>
#endif

// Default scene
#include "Scenes/Sample.h"

int main()
{
	leap::GameContext::GetInstance().AddLogger<leap::ConsoleLogger>();

	leap::LeapEngine engine{ 1280, 720, "Example" };

	auto afterInitializing = []()
		{
			leap::ServiceLocator::GetPhysics().SetEnabledDebugDrawing(true);
			leap::SceneManager::GetInstance().AddScene("Sample", Erosion::SampleScene::Load);
			//leap::GameContext::GetInstance().GetWindow()->SetIcon("Data/Example.png");
		};

	engine.Run(afterInitializing, 60);
	return 0;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return main();
}