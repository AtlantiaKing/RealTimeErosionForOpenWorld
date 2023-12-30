#pragma once

#include <map>
#include <vector>

#include <Generator.h>
#include <Presets/Presets.h>

namespace Erosion
{
	class Heightmap final
	{
	public:
		Heightmap(int chunkSize) : m_ChunkSize{ chunkSize }
		{
			// Create generator
			m_Perlin.SetSize(static_cast<float>(m_ChunkSize));
			that::preset::Presets::CreateDefaultTerrain(m_Perlin, static_cast<unsigned int>(time(nullptr)), m_PerlinMultiplier);
			m_Perlin.GetHeightMap().SetBlendMode(that::HeightMap::BlendMode::Multiply);
		}

		float& GetHeight(int x, int y)
		{
			const int chunkX{ x / m_ChunkSize };
			const int chunkY{ y / m_ChunkSize };
			const int xInChunk{ x % m_ChunkSize };
			const int yInChunk{ y % m_ChunkSize };

			auto& rowOfChunks{ m_HeightmapPerChunk[chunkX] };
			auto& chunk{ rowOfChunks[chunkY] };

			if (chunk.empty())
			{
				chunk.resize(m_ChunkSize * m_ChunkSize);

				for (int curX{}; curX < m_ChunkSize; ++curX)
				{
					for (int curY{}; curY < m_ChunkSize; ++curY)
					{
						float perlin{ m_Perlin.GetHeight(static_cast<float>(curX + chunkX * m_ChunkSize), static_cast<float>(curY + chunkY * m_ChunkSize)) };
						constexpr float mountainHeight{ 0.25f };
						if (perlin > mountainHeight)
						{
							const float xOffset{ 0.5f - mountainHeight };
							perlin += xOffset;
							perlin = perlin * perlin;
							constexpr float yOffset{ 0.5f - 0.5f * 0.5f };
							perlin += yOffset;
							perlin -= xOffset;
						}
						chunk[curX + curY * m_ChunkSize] = perlin;
					}
				}
			}

			return chunk[xInChunk + yInChunk * m_ChunkSize];
		}

		int GetSize() const { return m_ChunkSize; }

	private:
		int m_ChunkSize{};
		std::map<int, std::map<int, std::vector<float>>> m_HeightmapPerChunk{};
		that::Generator m_Perlin{};
		const float m_PerlinMultiplier{ 23.726f };
	};
}