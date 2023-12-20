#include "RiverLand.h"

#include <queue>
#include <set>
#include <ImGui/imgui.h>

void Erosion::RiverLand::GetHeights(std::vector<float>& heights)
{
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
			const float slope{ heights[x + z * terrainSize] };

			// Create a cell
			RiverLandCell& cell{ cells[x + z * terrainSize] };
			cell.slope = slope;
			cell.x = x;
			cell.z = z;

			// If the cell is below water height, add it to the queue and set the distance from water to 0
			if (heights[x + z * terrainSize] < m_RiverHeight)
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
				if (abs(x) == 1 && abs(z) == 1) continue;
				if (x == 0 && z == 0) continue;

				// Calculate coordinates of neighbouring cell
				const int otherX{ cell.x + x };
				const int otherZ{ cell.z + z };

				// Bounds check on the neighbouring cell
				if (otherX < 0 || otherX >= terrainSize || otherZ < 0 || otherZ >= terrainSize) continue;

				// Get the neighbouring cell
				RiverLandCell& other{ cells[otherX + otherZ * terrainSize] };

				// Add the neighbour to the queue if it hasn't been added before
				if (!other.isChecked)
				{
					other.isChecked = true;
					riverCells.push(&other);
				}

				// Calculate the distance from the cell
				const float distance{ cell.distance + other.slope };

				// If the neighbour is further away from a water source via the current cell, discard it
				if (distance > other.distance) continue;

				// Set the new distance for the neighbouring cell
				other.distance = distance;
			}
		}
	}

	// Normalize the distance so the farthest distance is 1.0 and set the squared distance as the height of the cell
	for (RiverLandCell& cell : cells)
	{
		const float normalizedDistance{ cell.distance * cell.slope / m_Divider };
		heights[cell.x + cell.z * terrainSize] = normalizedDistance;
	}

	// Cliff removal
	if (m_DoCliffDetection)
	{
		std::set<int> cliffCellsUnique{};
		std::queue<int> cliffCells{};
		for (RiverLandCell& cell : cells)
		{
			// Get the idx and height of the current cell
			const int cellIdx{ cell.x + cell.z * terrainSize };
			const float height{ heights[cellIdx] };

			// Find the neighbour with the highest point
			float highestPeak{};
			int highestNeighbour{};
			for (int x{ -1 }; x < 1; ++x)
			{
				for (int z{ -1 }; z < 1; ++z)
				{
					if (abs(x) == 1 && abs(z) == 1) continue;
					if (x == 0 && z == 0) continue;

					if (cell.x + x < 0 || cell.x + x >= terrainSize) continue;
					if (cell.z + z < 0 || cell.z + z >= terrainSize) continue;

					const int neighbourIdx{ cell.x + x + (cell.z + z) * terrainSize };
					const float neighbourHeight{ heights[neighbourIdx] };
					if (highestPeak < neighbourHeight)
					{
						highestPeak = neighbourHeight;
						highestNeighbour = neighbourIdx;
					}
				}
			}

			// If none of the neighbours is higher then the cliff threshold, continue to the next cell
			if (highestPeak - height < m_CliffThreshold) continue;

			// Add the cell that is higher then the cliff threshold to the queue
			cliffCells.push(cellIdx);
			cliffCellsUnique.insert(cellIdx);

			// Set the height of the current cell so it is inside the cliff threshold from its highest neighbour
			heights[cellIdx] = highestPeak - m_CliffThreshold;
		}

		// While there are still cells that form a cliff
		while (!cliffCells.empty())
		{
			// Get the first cell idx in the queue
			int cellIdx{ cliffCells.front() };
			cliffCells.pop();
			cliffCellsUnique.erase(cellIdx);

			// Get the current cell
			const RiverLandCell& cell{ cells[cellIdx] };

			// For each neighbour
			for (int x{ -1 }; x < 1; ++x)
			{
				for (int z{ -1 }; z < 1; ++z)
				{
					// Discard diagonal neighbours and itself
					if (abs(x) == 1 && abs(z) == 1) continue;
					if (x == 0 && z == 0) continue;

					// Bounds check on the neighbour coordinates
					if (cell.x + x < 0 || cell.x + x >= terrainSize) continue;
					if (cell.z + z < 0 || cell.z + z >= terrainSize) continue;

					const int neighbourIdx{ cell.x + x + (cell.z + z) * terrainSize };

					// If the neighbour is within cliff threshold, continue to the next neighbour
					if (heights[neighbourIdx] > heights[cellIdx] - m_CliffThreshold) continue;

					// Add the neighbour to the queue if it isn't already added
					if (!cliffCellsUnique.contains(neighbourIdx))
					{
						cliffCells.push(neighbourIdx);
						cliffCellsUnique.insert(neighbourIdx);
					}

					// Move the neighbour so it is within the cliff threshold of the current cell
					heights[neighbourIdx] = heights[cellIdx] - m_CliffThreshold;
				}
			}
		}
	}

	// Blur the heightmap
	std::vector<float> blurredHeights(heights.size());
	for (int x{}; x < terrainSize; ++x)
	{
		for (int z{}; z < terrainSize; ++z)
		{
			constexpr int blurSize{ 9 };
			constexpr float kernel{ 1.0f / (blurSize * blurSize) };
			float sum{};
			for (int k{}; k < blurSize; ++k)
			{
				for (int l{}; l < blurSize; ++l)
				{
					int row{ x + k - blurSize / 2 };
					int col{ z + l - blurSize / 2 };

					if (row < 0 || row >= terrainSize || col < 0 || col >= terrainSize) continue;

					sum += heights[row + col * terrainSize] * kernel;
				}
			}
			blurredHeights[x + z * terrainSize] = sum;
		}
	}
	heights = std::move(blurredHeights);
}

void Erosion::RiverLand::OnGUI()
{
	ImGui::SliderFloat("RiverHeight", &m_RiverHeight, 0.0f, 0.5f, "%.5f");
	ImGui::SliderFloat("Height Divider", &m_Divider, 16.0f, 1024.0f);
	ImGui::SliderFloat("Cliff", &m_CliffThreshold, 0.0f, 0.015f, "%.5f");
	ImGui::Checkbox("Do Cliff Detection", &m_DoCliffDetection);
}