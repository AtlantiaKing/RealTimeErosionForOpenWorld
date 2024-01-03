#pragma once

#include <map>
#include <vector>
#include <algorithm>

#include <Generator.h>
#include <Presets/Presets.h>

namespace Erosion
{
	class Heightmap final
	{
	public:
		Heightmap(int chunkSize) : m_ChunkSize{ chunkSize }
		{
			srand(static_cast<unsigned int>(time(nullptr)));

			constexpr float multiplier{ 3.0f };

			m_Continentalness.GetGraph().AddNode(0.0f, 0.0f);
			m_Continentalness.GetGraph().AddNode(1.0f, 1.0f);
			m_Continentalness.GetPerlin().AddOctave(0.7f, 0.0003f * multiplier);
			m_Continentalness.GetPerlin().AddOctave(0.3f, 0.0003f * multiplier);

			m_Mountainness.GetGraph().AddNode(0.0f, 0.0f);
			m_Mountainness.GetGraph().AddNode(1.0f, 1.0f);
			m_Mountainness.GetPerlin().AddOctave(1.0f, 0.0006f * multiplier);

			m_DefaultDetails.GetGraph().AddNode(0.0f, 0.0f);
			m_DefaultDetails.GetGraph().AddNode(1.0f, 1.0f);
			m_DefaultDetails.GetPerlin().AddOctave(0.676f, 0.0015f / 2.0f * multiplier);
			m_DefaultDetails.GetPerlin().AddOctave(0.27f, 0.005f / 2.0f * multiplier);

			const int graphPrecision{ 1000 };
			for (int i{}; i <= graphPrecision; ++i)
			{
				const float x{ static_cast<float>(i) / graphPrecision };

				float functionX{ x };
				if (functionX > 0.5f)
				{
					functionX -= 0.5f;
					functionX /= 0.5f;
					functionX = 1.0f - functionX;
				}
				else
				{
					functionX /= 0.5f;
				}

				constexpr float pi{ 3.14159265359f };
				const float y{ ((2 * functionX - 1) * (2 * functionX - 1) - 0.2f * sinf(8 * pi * functionX) - 0.1f * cosf(12 * pi * functionX)) / 1.5f - 0.08f + 0.3f };

				m_MountainDiversity.GetGraph().AddNode(x, std::clamp(y, 0.0f, 1.0f));
			}
			m_MountainDiversity.GetPerlin().AddOctave(0.9f, 0.001f / 8.0f * multiplier);
			m_MountainDiversity.GetPerlin().AddOctave(0.1f, 0.0075f / 8.0f * multiplier);


			m_MountainDetails.GetGraph().AddNode(0.0f, 0.0f);
			m_MountainDetails.GetGraph().AddNode(1.0f, 1.0f);
			m_MountainDetails.GetPerlin().AddOctave(0.67f, 0.003f / 2.0f * multiplier);
			m_MountainDetails.GetPerlin().AddOctave(0.27f, 0.011f / 2.0f * multiplier);
			m_MountainDetails.GetPerlin().AddOctave(0.05f, 0.031f / 2.0f * multiplier);
			m_MountainDetails.GetPerlin().AddOctave(0.01f, 0.078f / 2.0f * multiplier);


			//m_Perlin.SetSize(static_cast<float>(m_ChunkSize));
			//that::NoiseMap continentalNess{};
			//continentalNess.GetGraph().AddNode(0.0f, 0.0f);
			//continentalNess.GetGraph().AddNode(1.0f, 1.0f);
			//continentalNess.GetPerlin().AddOctave(0.678f, 0.003f);
			//continentalNess.GetPerlin().AddOctave(0.27f, 0.011f);
			//continentalNess.GetPerlin().AddOctave(0.05f, 0.031f);
			//continentalNess.GetPerlin().AddOctave(0.002f, 0.078f);
			//m_Perlin.GetHeightMap().AddNoiseMap(continentalNess);


			//////// Create generator
			//m_Perlin.SetSize(static_cast<float>(m_ChunkSize));
			//that::NoiseMap continentalNess2{};
			//continentalNess2.GetGraph().AddNode(0.0f, 0.0f);
			//continentalNess2.GetGraph().AddNode(0.45f, 0.3f);
			//continentalNess2.GetGraph().AddNode(0.7f, 0.86f);
			//continentalNess2.GetGraph().AddNode(1.0f, 1.0f);
			//continentalNess2.GetPerlin().AddOctave(0.1f, 0.0006f);
			//continentalNess2.GetPerlin().AddOctave(0.9f, 0.001f);
			//m_Perlin.GetHeightMap().AddNoiseMap(continentalNess2);

			//m_Perlin.SetSize(static_cast<float>(m_ChunkSize));
			//that::NoiseMap continentalNess3{};
			//const int graphPrecision{ 100 };
			//for (int i{}; i <= graphPrecision; ++i)
			//{
			//	const float x{ static_cast<float>(i) / graphPrecision };

			//	constexpr float pi{ 3.14159265359f };
			//	const float y{ (((2 * x - 1) * (2 * x - 1) + sinf(8 * pi * x) + cosf(12 * pi * x)) / 2.2f - 0.08f + 1.0f) / 2.0f };

			//	continentalNess3.GetGraph().AddNode(x, y);
			//}
			//continentalNess3.GetPerlin().AddOctave(1.0f, 0.001f);
			//m_Perlin.GetHeightMap().AddNoiseMap(continentalNess3);

			//that::NoiseMap continentalNess3{};
			//continentalNess3.GetGraph().AddNode(0.0f, 0.0f);
			//continentalNess3.GetGraph().AddNode(1.0f, 0.001f);
			//continentalNess3.GetPerlin().AddOctave(1.0f, 2.0f * m_PerlinMultiplier);
			//m_Perlin.GetHeightMap().AddNoiseMap(continentalNess3);

			//that::NoiseMap continentalNess2{};
			//continentalNess2.GetGraph().AddNode(0.0f, 0.1f);
			//continentalNess2.GetGraph().AddNode(1.0f, 1.0f);
			//continentalNess2.GetPerlin().AddOctave(1.0f, 100.0f * m_PerlinMultiplier);
			//m_Perlin.GetHeightMap().AddNoiseMap(continentalNess2);
			////that::preset::Presets::CreateDefaultTerrain(m_Perlin, static_cast<unsigned int>(time(nullptr)), m_PerlinMultiplier);
			//m_Perlin.GetHeightMap().SetBlendMode(that::HeightMap::BlendMode::Multiply);
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
						float continentalness{ m_Continentalness.GetNoise(static_cast<float>(curX + chunkX * m_ChunkSize), static_cast<float>((curY + chunkY * m_ChunkSize))) };
						const float defaultDetails{ m_DefaultDetails.GetNoise(static_cast<float>(curX + chunkX * m_ChunkSize), static_cast<float>(curY + chunkY * m_ChunkSize)) };
						continentalness += defaultDetails / 10.0f;

						if (continentalness < 0.47f)
						{
							chunk[curX + curY * m_ChunkSize] = 0.02f;
							continue;
						}

						continentalness -= 0.47f;
						continentalness *= 1.3f;

						float mountainness{ m_Mountainness.GetNoise(static_cast<float>(curX + chunkX * m_ChunkSize), static_cast<float>(curY + chunkY * m_ChunkSize)) };
						if (mountainness < 0.5f)
						{
							mountainness = 0.0F;
						}
						else if(mountainness < 0.75f)
						{
							mountainness -= 0.5f;
							mountainness /= 0.25f;

							mountainness = mountainness * mountainness;

							mountainness *= 0.85f;
						}
						else
						{
							mountainness -= 0.75f;
							mountainness /= 0.25f;
							mountainness *= 0.15f;
							mountainness += 0.85f;
						}
						mountainness = std::max(mountainness, 1.5f * (continentalness / 0.6f - 0.3f));

						float mountainRange{ m_MountainDiversity.GetNoise(static_cast<float>(curX + chunkX * m_ChunkSize), static_cast<float>(curY + chunkY * m_ChunkSize)) };
						//const float mountainDetails{ m_MountainDetails.GetNoise(static_cast<float>(curX + chunkX * m_ChunkSize), static_cast<float>(curY + chunkY * m_ChunkSize)) };

						float totalHeight{ continentalness };
						if (totalHeight > 0.0f)
						{
							if (continentalness < 0.03f)
							{
								const float mountainFactor{ continentalness / 0.03f };
								const float continentalFactor{ mountainFactor * mountainFactor };
								totalHeight += continentalFactor * 0.03f * 2.0f * mountainness * mountainRange /** mountainDetails*/;
							}
							else
							{
								totalHeight += continentalness * 2.0f * mountainness * mountainRange /** mountainDetails*/;
							}
						}

						chunk[curX + curY * m_ChunkSize] = totalHeight + 0.02f;
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
		const float m_PerlinMultiplier{ /*23.726f*/900 };

		that::NoiseMap m_Continentalness{};
		that::NoiseMap m_Mountainness{};
		that::NoiseMap m_MountainDiversity{};
		that::NoiseMap m_MountainDetails{};
		that::NoiseMap m_DefaultDetails{};
	};
};
