#pragma once

#include <Singleton.h>
#include <vector>
#include <mutex>

namespace Erosion
{
	class TerrainGeneratorComponent;

	class TerrainManager final : public leap::Singleton<TerrainManager>
	{
	public:
		~TerrainManager() = default;

		TerrainManager(const TerrainManager& other) = delete;
		TerrainManager(TerrainManager&& other) = delete;
		TerrainManager& operator=(const TerrainManager& other) = delete;
		TerrainManager& operator=(TerrainManager&& other) = delete;

		void AddTerrain(int x, int y, const TerrainGeneratorComponent* pTerrain, std::vector<float> heights);

	private:
		TerrainManager() = default;
		friend leap::Singleton<TerrainManager>;

		enum class EvaluateDirection
		{
			Forward,
			Back,
			Right,
			Left
		};
		struct Chunk final
		{
			int x{};
			int y{};
			const TerrainGeneratorComponent* pTerrain{};
			std::vector<float> heights{};
		};

		void EvaluateChunk(Chunk& cur, int evalX, int evalY, EvaluateDirection dir);

		std::vector<Chunk> m_Chunks{};
		std::mutex m_Mutex{};
	};
}