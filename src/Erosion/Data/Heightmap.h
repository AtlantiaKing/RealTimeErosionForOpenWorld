#pragma once

#include <map>
#include <vector>

namespace Erosion
{
	class Heightmap final
	{
	public:
		Heightmap(int chunkSize) : m_ChunkSize{ chunkSize } {}

		float& GetHeight(int x, int y)
		{
			const int chunkX{ x / m_ChunkSize };
			const int chunkY{ y / m_ChunkSize };
			const int xInChunk{ x % m_ChunkSize };
			const int yInChunk{ y % m_ChunkSize };

			auto& rowOfChunks{ m_HeightmapPerChunk[chunkX] };
			auto& chunk{ rowOfChunks[chunkY] };

			if (chunk.empty()) chunk.resize(m_ChunkSize * m_ChunkSize);

			return chunk[xInChunk + yInChunk * m_ChunkSize];
		}

		int GetSize() const { return m_ChunkSize; }

	private:
		int m_ChunkSize{};
		std::map<int, std::map<int, std::vector<float>>> m_HeightmapPerChunk{};
	};
}