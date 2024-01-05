#include "RiverLand.h"

#include <queue>
#include <set>
#include <ImGui/imgui.h>

Erosion::RiverLand::RiverLand()
{
	NewMaps();
}

void Erosion::RiverLand::GetHeights(std::vector<float>& heights)
{
	NewMaps();

	// Retrieve the terrain size
	const int terrainSize{ static_cast<int>(sqrtf(static_cast<float>(heights.size()))) };

	// Create a grid of cells
	std::vector<RiverLandCell> cells(heights.size());
	// Create a queue of cells that need to be checked
	std::queue<RiverLandCell*> riverCells{};

	// For each cell
	for (int x{}; x < terrainSize; ++x)
	{
		for (int z{}; z < terrainSize; ++z)
		{
			// Use the height from the heightmap as a slope
			const float slope{ m_SlopeMap.GetNoise(static_cast<float>(x), static_cast<float>(z)) };
			//const float height{ heights[x + z * terrainSize] };

			// Create a cell
			RiverLandCell& cell{ cells[x + z * terrainSize] };
			cell.slope = slope;
			cell.x = x;
			cell.z = z;
			//cell.minHeight = height;

			float river{ 1.0f };
			for (auto& map : m_RiverMaps)
			{
				float river2 = map.first.GetNoise(static_cast<float>(x), static_cast<float>(z));
				if (river2 > 0.4f) river2 = 0.5f;
				else
				{
					river2 = map.second.GetNoise(static_cast<float>(x), static_cast<float>(z));
					if (river2 < 0.5f) river2 = 0.5f;
					else river2 = 0.1f;
				}
				river = std::min(river, river2);

			}

			// If the cell is below water height, add it to the queue and set the distance from water to 0
			if (river < 0.5f)
			{
				riverCells.push(&cell);
				cell.isChecked = true;
				cell.distance = 0.0f;
			}
		}
	}

	// While there are still cells in the queue
	while (!riverCells.empty())
	{
		// Pop the first cell from the queue
		RiverLandCell& cell{ *riverCells.front() };
		riverCells.pop();

		// For each neighbour
		for (int x{ -1 }; x <= 1; ++x)
		{
			for (int z{ -1 }; z <= 1; ++z)
			{
				// Don't check diagonally or itself
				//if (abs(x) == 1 && abs(z) == 1) continue;
				if (x == 0 && z == 0) continue;

				// Calculate coordinates of neighbouring cell
				const int otherX{ cell.x + x };
				const int otherZ{ cell.z + z };

				// Bounds check on the neighbouring cell
				if (otherX < 0 || otherX >= terrainSize|| otherZ < 0 || otherZ >= terrainSize) continue;

				// Get the neighbouring cell
				RiverLandCell& other{ cells[otherX + otherZ * terrainSize] };

				// Add the neighbour to the queue if it hasn't been added before
				if (!other.isChecked)
				{
					other.isChecked = true;
					riverCells.push(&other);
				}

				// Calculate the distance from the cell
				const float distance{ cell.distance + cell.slope };

				// If the neighbour is further away from a water source via the current cell, discard it
				if (distance * distance * distance * distance > other.distance * other.distance * other.distance * other.distance) continue;

				// Set the new distance for the neighbouring cell
				other.distance = distance;
				//other.ptrIdx = cell.x + cell.z * terrainSize * 2;
				other.slope = cell.slope;
				//other.minHeight = cell.minHeight;
			}
		}
	}

	// Normalize the distance so the farthest distance is 1.0 and set the squared distance as the height of the cell
	for (RiverLandCell& cell : cells)
	{
		const float normalizedDistance{ /*cell.minHeight +*/ cell.distance * cell.slope / m_Divider };

		/*if (cell.z < terrainSize / 2) continue;
		if (cell.x < terrainSize / 2) continue;
		if (cell.x > terrainSize / 2 * 3) continue;
		if (cell.z > terrainSize / 2 * 3) continue;
		heights[(cell.x - terrainSize / 2) + (cell.z - terrainSize / 2) * terrainSize] = normalizedDistance;*/
		heights[cell.x + cell.z * terrainSize] = normalizedDistance;
	}

	//// Cliff removal
	//if (m_DoCliffDetection)
	//{
	//	std::set<int> cliffCellsUnique{};
	//	std::queue<int> cliffCells{};
	//	for (RiverLandCell& cell : cells)
	//	{
	//		if (cell.z < terrainSize / 2) continue;
	//		if (cell.x < terrainSize / 2) continue;
	//		if (cell.x > terrainSize / 2 * 3) continue;
	//		if (cell.z > terrainSize / 2 * 3) continue;

	//		// Get the idx and height of the current cell
	//		const int cellIdx{ (cell.x - terrainSize / 2) + (cell.z - terrainSize / 2) * terrainSize };
	//		const float height{ heights[cellIdx] };

	//		// Find the neighbour with the highest point
	//		float highestPeak{};
	//		int highestNeighbour{};
	//		for (int x{ -1 }; x < 1; ++x)
	//		{
	//			for (int z{ -1 }; z < 1; ++z)
	//			{
	//				if (abs(x) == 1 && abs(z) == 1) continue;
	//				if (x == 0 && z == 0) continue;

	//				if (cell.x + x < 0 || cell.x + x >= terrainSize) continue;
	//				if (cell.z + z < 0 || cell.z + z >= terrainSize) continue;

	//				const int neighbourIdx{ cell.x + x + (cell.z + z) * terrainSize };
	//				const float neighbourHeight{ heights[neighbourIdx] };
	//				if (highestPeak < neighbourHeight)
	//				{
	//					highestPeak = neighbourHeight;
	//					highestNeighbour = neighbourIdx;
	//				}
	//			}
	//		}

	//		// If none of the neighbours is higher then the cliff threshold, continue to the next cell
	//		if (highestPeak - height < m_CliffThreshold) continue;

	//		// Add the cell that is higher then the cliff threshold to the queue
	//		cliffCells.push(cellIdx);
	//		cliffCellsUnique.insert(cellIdx);

	//		// Set the height of the current cell so it is inside the cliff threshold from its highest neighbour
	//		heights[cellIdx] = highestPeak - m_CliffThreshold;
	//	}

	//	// While there are still cells that form a cliff
	//	while (!cliffCells.empty())
	//	{
	//		// Get the first cell idx in the queue
	//		int cellIdx{ cliffCells.front() };
	//		cliffCells.pop();
	//		cliffCellsUnique.erase(cellIdx);

	//		// Get the current cell
	//		const RiverLandCell& cell{ cells[cellIdx] };

	//		// For each neighbour
	//		for (int x{ -1 }; x < 1; ++x)
	//		{
	//			for (int z{ -1 }; z < 1; ++z)
	//			{
	//				// Discard diagonal neighbours and itself
	//				if (abs(x) == 1 && abs(z) == 1) continue;
	//				if (x == 0 && z == 0) continue;

	//				// Bounds check on the neighbour coordinates
	//				if (cell.x + x < 0 || cell.x + x >= terrainSize) continue;
	//				if (cell.z + z < 0 || cell.z + z >= terrainSize) continue;

	//				const int neighbourIdx{ cell.x + x + (cell.z + z) * terrainSize };

	//				// If the neighbour is within cliff threshold, continue to the next neighbour
	//				if (heights[neighbourIdx] > heights[cellIdx] - m_CliffThreshold) continue;

	//				// Add the neighbour to the queue if it isn't already added
	//				if (!cliffCellsUnique.contains(neighbourIdx))
	//				{
	//					cliffCells.push(neighbourIdx);
	//					cliffCellsUnique.insert(neighbourIdx);
	//				}

	//				// Move the neighbour so it is within the cliff threshold of the current cell
	//				heights[neighbourIdx] = heights[cellIdx] - m_CliffThreshold;
	//			}
	//		}
	//	}
	//}

	// Blur the heightmap
	const float kernel{ 1.0f / (m_BlurSize * m_BlurSize) };
	std::vector<float> blurredHeights(heights.size());
	for (int x{}; x < terrainSize; ++x)
	{
		for (int z{}; z < terrainSize; ++z)
		{
			float sum{};
			for (int k{}; k < m_BlurSize; ++k)
			{
				for (int l{}; l < m_BlurSize; ++l)
				{
					int row{ x + k - m_BlurSize / 2 };
					int col{ z + l - m_BlurSize / 2 };

					if (row < 0 || row >= terrainSize || col < 0 || col >= terrainSize) continue;

					sum += heights[row + col * terrainSize] * kernel;
				}
			}
			blurredHeights[x + z * terrainSize] = sum * sum;
		}
	}
	heights = std::move(blurredHeights);

	/*for (float& height : heights)
	{
		height *= height;
	}*/

	/*for (int x{}; x < terrainSize; ++x)
	{
		for (int z{}; z < terrainSize; ++z)
		{
			float river{ 1.0f };
			for (auto& map : m_RiverMaps)
			{
				float river2 = map.first.GetNoise(static_cast<float>(x), static_cast<float>(z));
				if (river2 > 0.4f) river2 = 0.5f;
				else
				{
					river2 = map.second.GetNoise(static_cast<float>(x), static_cast<float>(z));
					if (river2 < 0.5f) river2 = 0.5f;
					else river2 = 0.1f;
				}
				river = std::min(river, river2);

			}

			heights[x + z * terrainSize] = river;
		}
	}*/

}

void Erosion::RiverLand::OnGUI()
{
	ImGui::Spacing();
	ImGui::Text("River Land Settings");
	ImGui::SliderFloat("RiverHeight", &m_RiverHeight, 0.0f, 1.0f, "%.5f");
	ImGui::SliderFloat("Height Divider", &m_Divider, 16.0f, 1024.0f);
	ImGui::SliderInt("Blur Intensity", &m_BlurSize, 1, 50);
	ImGui::SliderFloat("Cliff Threshold", &m_CliffThreshold, 0.0f, 0.015f, "%.5f");
	ImGui::Checkbox("Do Cliff Detection", &m_DoCliffDetection);
}

void Erosion::RiverLand::NewMaps()
{
	m_RiverMaps.clear();

	m_SlopeMap.GetGraph().AddNode(0.0f, 0.0f);
	m_SlopeMap.GetGraph().AddNode(1.0f, 1.0f);
	m_SlopeMap.GetPerlin().AddOctave(1.0f, 400.0f);

	float multiplier{ 4.0f };

	that::NoiseMap map1{};
	map1.GetGraph().AddNode(0.0f, 1.0f);
	map1.GetGraph().AddNode(0.45f, 1.0f);
	map1.GetGraph().AddNode(0.5f, 0.0f);
	map1.GetGraph().AddNode(0.55f, 1.0f);
	map1.GetGraph().AddNode(1.0f, 1.0f);
	map1.GetPerlin().AddOctave(1.0f, 300.0f * multiplier);

	that::NoiseMap map12{};
	map12.GetGraph().AddNode(0.0f, 0.0f);
	map12.GetGraph().AddNode(1.0f, 1.0f);
	map12.GetPerlin().AddOctave(1.0f, 200.0f * multiplier);

	m_RiverMaps.push_back(std::make_pair(map1, map12));

	that::NoiseMap map2{};
	map2.GetGraph().AddNode(0.0f, 1.0f);
	map2.GetGraph().AddNode(0.45f, 1.0f);
	map2.GetGraph().AddNode(0.5f, 0.0f);
	map2.GetGraph().AddNode(0.55f, 1.0f);
	map2.GetGraph().AddNode(1.0f, 1.0f);
	map2.GetPerlin().AddOctave(1.0f, 300.0f * multiplier);

	that::NoiseMap map22{};
	map22.GetGraph().AddNode(0.0f, 0.0f);
	map22.GetGraph().AddNode(1.0f, 1.0f);
	map22.GetPerlin().AddOctave(1.0f, 200.0f * multiplier);

	m_RiverMaps.push_back(std::make_pair(map2, map22));

	that::NoiseMap map3{};
	map3.GetGraph().AddNode(0.0f, 1.0f);
	map3.GetGraph().AddNode(0.45f, 1.0f);
	map3.GetGraph().AddNode(0.5f, 0.0f);
	map3.GetGraph().AddNode(0.55f, 1.0f);
	map3.GetGraph().AddNode(1.0f, 1.0f);
	map3.GetPerlin().AddOctave(1.0f, 50 * multiplier);

	that::NoiseMap map32{};
	map32.GetGraph().AddNode(0.0f, 0.0f);
	map32.GetGraph().AddNode(1.0f, 1.0f);
	map32.GetPerlin().AddOctave(1.0f, 120 * multiplier);

	m_RiverMaps.push_back(std::make_pair(map3, map32));
}
