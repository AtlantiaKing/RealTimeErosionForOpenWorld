#include "HansBeyer.h"

#include <vec2.hpp>
#include <ext/scalar_constants.hpp>
#include <geometric.hpp>

#include <ImGui/imgui.h>

void Erosion::HansBeyer::GetHeights(Heightmap& heights)
{
	struct Droplet 
	{
		glm::vec2 position{};
		glm::vec2 direction{};
		float speed{};
		float amountWater{};
		float amountSediment{};
		int pathLength{};
	};

	// Terrain data
	const int terrainSize{ heights.GetSize() };

	// Erosion radius data
	std::vector<float> radiusWeights{};
	radiusWeights.resize(m_ErosionRadius * m_ErosionRadius);

	// X cycles
	for (int cycleIdx{}; cycleIdx < m_Cycles; ++cycleIdx)
	{
		// Create a droplet
		const float angle{ Random01() * glm::pi<float>() };
		Droplet droplet
		{
			glm::vec2{ terrainSize / 2 + m_ChunkX * (terrainSize - 1) + Random01() * (terrainSize - 1), terrainSize / 2 + m_ChunkY * (terrainSize - 1) + Random01() * (terrainSize - 1) },
			glm::vec2{ cosf(angle), sinf(angle) },
			1.0f,
			1.0f,
			0.0f,
			0
		};

		for (int lifeTime{}; lifeTime < m_MaxPathLength; ++lifeTime)
		{
			// Calculate grid position and position inside cell
			const int gridPosX{ static_cast<int>(droplet.position.x) };
			const int gridPosY{ static_cast<int>(droplet.position.y) };
			const float cellPosX{ droplet.position.x - gridPosX };
			const float cellPosY{ droplet.position.y - gridPosY };

			// Calculate the height of all the neighbouring cells around the droplet position
			const float heightXY{ heights.GetHeight(gridPosX, gridPosY) };
			const float heightXPlusY{ heights.GetHeight(gridPosX + 1, gridPosY) };
			const float heightXYPlus{ heights.GetHeight(gridPosX, gridPosY + 1) };
			const float heightXPlusYPlus{ heights.GetHeight(gridPosX + 1, gridPosY + 1) };

			// Calculate the gradient at the droplet position
			const glm::vec2 gradient
			{
				(heightXPlusY - heightXY) * (1.0f - cellPosY) + (heightXPlusYPlus - heightXYPlus) * cellPosY,
				(heightXYPlus - heightXY) * (1.0f - cellPosX) + (heightXPlusYPlus - heightXPlusY) * cellPosX
			};
			// Calculate the height at the droplet position
			const float height{ (1.0f - cellPosY) * ((1.0f - cellPosX) * heightXY + cellPosX * heightXPlusY) + cellPosY * ((1.0f - cellPosX) * heightXYPlus + cellPosX * heightXPlusYPlus) };

			// Calculate the new direction and position of the droplet
			droplet.direction = droplet.direction * m_Inertia - gradient * (1.0f - m_Inertia);
			if (glm::dot(droplet.direction, droplet.direction) < FLT_EPSILON) break;
			else droplet.direction = glm::normalize(droplet.direction);
			droplet.position = droplet.position + droplet.direction;

			// Calculate grid position and position inside cell of the new droplet position
			const int newGridPosX{ static_cast<int>(droplet.position.x) };
			const int newGridPosY{ static_cast<int>(droplet.position.y) };
			const float newCellPosX{ droplet.position.x - newGridPosX };
			const float newCellPosY{ droplet.position.y - newGridPosY };

			// Calculate the height of all the neighbouring cells around the new droplet position
			const float newHeightXY{ heights.GetHeight(newGridPosX, newGridPosY) };
			const float newHeightXPlusY{ heights.GetHeight(newGridPosX + 1, newGridPosY) };
			const float newHeightXYPlus{ heights.GetHeight(newGridPosX, newGridPosY + 1) };
			const float newHeightXPlusYPlus{ heights.GetHeight(newGridPosX + 1, newGridPosY + 1) };

			// Calculate the height at the new droplet position
			const float newHeight{ (1.0f - newCellPosY) * ((1.0f - newCellPosX) * newHeightXY + newCellPosX * newHeightXPlusY) + newCellPosY * ((1.0f - newCellPosX) * newHeightXYPlus + newCellPosX * newHeightXPlusYPlus) };

			// Calculate the height difference between the start and end position of the droplet
			const float heightDiff{ newHeight - height };

			// Calculate the capacity of the droplet at its current state
			const float curCapacity{ std::max(-heightDiff, m_MinSlope) * droplet.speed * droplet.amountWater * m_Capacity };

			// If the droplet has more sediment then its capacity, or the droplet is moving upwards, drop sediment on the ground
			// Otherwise let the droplet take up sediment from the ground
			if (droplet.amountSediment > curCapacity || heightDiff > 0.0f)
			{
				// Calculate the dropped amount of sediment
				const float droppedSediment{ heightDiff > 0.0f ? std::min(heightDiff, droplet.amountSediment) : (droplet.amountSediment - curCapacity) * m_Deposition };

				// Add the sediment at the four grid positions around the droplets position
				heights.GetHeight(gridPosX, gridPosY) += droppedSediment * (1.0f - cellPosX) * (1.0f - cellPosY);
				heights.GetHeight(gridPosX + 1, gridPosY) += droppedSediment * cellPosX * (1.0f - cellPosY);
				heights.GetHeight(gridPosX, gridPosY + 1) += droppedSediment * (1.0f - cellPosX) * cellPosY;
				heights.GetHeight(gridPosX + 1, gridPosY + 1) += droppedSediment * cellPosX * cellPosY;
				
				// Update the droplets sediment amount
				droplet.amountSediment -= droppedSediment;
			}
			else
			{
				// Calculate the taken amount of sediment
				const float takenSediment{ std::min((curCapacity - droplet.amountSediment) * m_Erosion, -heightDiff) };

				// Calculate the current position of the droplet
				const float dropletPosX{ gridPosX + cellPosX };
				const float dropletPosY{ gridPosY + cellPosY };

				float totalWeight{};
				float smallestDistance{ FLT_MAX };
				float highestDistance{};

				// Calculate the weights of all the grid points near the droplet and their combined total weight
				const int halfErosionRadius{ m_ErosionRadius / 2 };
				for (int radX{}; radX < m_ErosionRadius; ++radX)
				{
					for (int radY{}; radY < m_ErosionRadius; ++radY)
					{
						const int xPos = gridPosX + radX - halfErosionRadius;
						const int yPos = gridPosY + radY - halfErosionRadius;

						const float dX{ dropletPosX - xPos };
						const float dY{ dropletPosY - yPos };

						const float distance{ sqrtf(static_cast<float>(dX * dX + dY * dY)) };
						const int radiusIdx{ radX + radY * m_ErosionRadius };
						radiusWeights[radiusIdx] = distance;

						if (smallestDistance > distance) smallestDistance = distance;
						if (highestDistance < distance) highestDistance = distance;
					}
				}

				// Reverse the weights
				for (float& weight : radiusWeights)
				{
					weight = 1.0f - (weight - smallestDistance) / (highestDistance - smallestDistance);
					totalWeight += weight;
				}

				// Normalize the weights
				for (float& weight : radiusWeights)
				{
					weight /= totalWeight;
				}

				// Remove the sediment from all the grid positions in the radius of the droplet
				for (int radX{}; radX < m_ErosionRadius; ++radX)
				{
					for (int radY{}; radY < m_ErosionRadius; ++radY)
					{
						const int xPos = radX - halfErosionRadius;
						const int yPos = radY - halfErosionRadius;

						const int radiusIdx{ radX + radY * m_ErosionRadius };
						heights.GetHeight(gridPosX + xPos, gridPosY + yPos) -= takenSediment * radiusWeights[radiusIdx];
					}
				}

				// Update the droplets sediment amount
				droplet.amountSediment += takenSediment;
			}

			// Update the speed of the droplet
			const float sqrtSpeed{ droplet.speed * droplet.speed + heightDiff * m_Gravity };
			droplet.speed = sqrtSpeed < 0.0f ? 0.0f : sqrtf(sqrtSpeed);

			// If the droplet has lost all its speed, disable the droplet
			if (droplet.speed <= 0) break;

			// Update the water amount of the droplet
			droplet.amountWater = droplet.amountWater * (1.0f - m_Evaporation);

			// If the droplet has no more water, disable the droplet
			if (droplet.amountWater <= 0) break;

			// Increase the life time of the droplet, if it exceeds the max, disable the droplet
			if (++droplet.pathLength >= m_MaxPathLength) break;
		}
	}

	std::vector<float> output(heights.size());
	int kernelSize{ 5 };
	float kernelSum{};
	float sigma{ 1.0f };
	std::vector<float> kernel;
	for (int x = -kernelSize / 2; x <= kernelSize / 2; ++x) {
		for (int y = -kernelSize / 2; y <= kernelSize / 2; ++y) {
			float value = std::exp(-(x * x + y * y) / (2.0f * sigma * sigma));
			kernel.push_back(value);
			kernelSum += value;
		}
	}

	// Normalize the kernel
	for (float& value : kernel) {
		value /= kernelSum;
	}
	// Apply the convolution operation
	for (int y = kernelSize / 2; y < terrainSize - kernelSize / 2; ++y) {
		for (int x = kernelSize / 2; x < terrainSize - kernelSize / 2; ++x) {
			float sum = 0.0f;
			for (int ky = -kernelSize / 2; ky <= kernelSize / 2; ++ky) {
				for (int kx = -kernelSize / 2; kx <= kernelSize / 2; ++kx) {
					int index = (y + ky) * terrainSize + (x + kx);
					sum += heights[index] * kernel[(ky + kernelSize / 2) * kernelSize + (kx + kernelSize / 2)];
				}
			}
			output[y * terrainSize + x] = sum;
		}
	}
	heights = output;

}

void Erosion::HansBeyer::OnGUI()
{
	ImGui::Spacing();
	ImGui::Text("Hans Beyer Settings");
	ImGui::SliderInt("Nr Cycles", &m_Cycles, 0, 1'000'000);
	ImGui::SliderInt("Erosion Radius", &m_ErosionRadius, 1, 30);
	ImGui::SliderInt("Max Path Length", &m_MaxPathLength, 1, 500);
	ImGui::SliderFloat("Inertia", &m_Inertia, 0.0f, 1.0f);
	ImGui::SliderFloat("Min Slope", &m_MinSlope, 0.0f, 1.0f);
	ImGui::SliderFloat("Capacity", &m_Capacity, 0.0f, 128.0f);
	ImGui::SliderFloat("Gravity", &m_Gravity, -50.0f, 50.0f);
	ImGui::SliderFloat("Evaporation", &m_Evaporation, 0.0f, 1.0f);
	ImGui::SliderFloat("Deposition", &m_Deposition, 0.0f, 1.0f);
	ImGui::SliderFloat("Erosion", &m_Erosion, 0.0f, 1.0f);
}
