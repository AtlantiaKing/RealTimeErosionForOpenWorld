#pragma once

#include <Singleton.h>

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <memory>

#include "../ErosionAlgorithms/ITerrainGenerator.h"
#include <Generator.h>

#include "../Data/Heightmap.h"

namespace leap
{
	class TerrainComponent;
}

namespace Erosion
{
	class TerrainManager final : public leap::Singleton<TerrainManager>
	{
	public:
		TerrainManager();
		~TerrainManager();

		TerrainManager(const TerrainManager& other) = delete;
		TerrainManager(TerrainManager&& other) = delete;
		TerrainManager& operator=(const TerrainManager& other) = delete;
		TerrainManager& operator=(TerrainManager&& other) = delete;

		void Generate(int x, int y, leap::TerrainComponent* pTerrain);
		Heightmap& GetHeightmap() { return m_Heightmap; }

	private:
		struct Chunk final
		{
			int x{};
			int y{};
			leap::TerrainComponent* pTerrain{};
		};

		void EvaluateChunkPadding(int x, int y);
		void GeneratePerlin(int x, int y);
		void Erode(int x, int y, const std::unique_ptr<ITerrainGenerator>& pErosion);
		void UpdateComponents(int x, int y);

		that::Generator m_Perlin{};

		std::mutex m_QueueMutex{};
		std::queue<Chunk> m_ChunkQueue{};

		static const int m_ChunkSize{ 257 };
		const float m_PerlinMultiplier{ 23.726f };

		Heightmap m_Heightmap{ m_ChunkSize };
		int m_HeightmapSize{};

		bool m_Running{ true };

		std::jthread m_Thread{};
	};
}