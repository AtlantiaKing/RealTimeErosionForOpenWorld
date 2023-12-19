#include "HansBeyer.h"

#include <vec2.hpp>
#include <ext/scalar_constants.hpp>
#include <geometric.hpp>

#include <ImGui/imgui.h>

void Erosion::HansBeyer::GetHeights(std::vector<float>& heights)
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
	const int terrainSize{ static_cast<int>(sqrtf(static_cast<float>(heights.size()))) };

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
			glm::vec2{ Random01() * (terrainSize - 1), Random01() * (terrainSize - 1) },
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
			const float heightXY{ heights[gridPosX + gridPosY * terrainSize] };
			const float heightXPlusY{ gridPosX + 1 >= terrainSize ? heightXY : heights[gridPosX + 1 + gridPosY * terrainSize] };
			const float heightXYPlus{ gridPosY + 1 >= terrainSize ? heightXY : heights[gridPosX + (gridPosY + 1) * terrainSize] };
			const float heightXPlusYPlus{ gridPosX + 1 >= terrainSize || gridPosY + 1 >= terrainSize ? heightXY : heights[gridPosX + 1 + (gridPosY + 1) * terrainSize] };

			// Calculate the gradient at the droplet position
			const glm::vec2 gradient
			{
				(heightXPlusY - heightXY)* (1.0f - cellPosY) + (heightXPlusYPlus - heightXYPlus) * cellPosY,
				(heightXYPlus - heightXY)* (1.0f - cellPosX) + (heightXPlusYPlus - heightXPlusY) * cellPosX
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

			// If the droplet has fallen outside the map, disable the droplet
			if (newGridPosX < 0 || newGridPosY < 0 || newGridPosX >= terrainSize || newGridPosY >= terrainSize)
			{
				break;
			}

			// Calculate the height of all the neighbouring cells around the new droplet position
			const float newHeightXY{ heights[newGridPosX + newGridPosY * terrainSize] };
			const float newHeightXPlusY{ newGridPosX + 1 >= terrainSize ? heightXY : heights[newGridPosX + 1 + newGridPosY * terrainSize] };
			const float newHeightXYPlus{ newGridPosY + 1 >= terrainSize ? heightXY : heights[newGridPosX + (newGridPosY + 1) * terrainSize] };
			const float newHeightXPlusYPlus{ newGridPosX + 1 >= terrainSize || newGridPosY + 1 >= terrainSize ? heightXY : heights[newGridPosX + 1 + (newGridPosY + 1) * terrainSize] };

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
				heights[gridPosX + gridPosY * terrainSize] += droppedSediment * (1.0f - cellPosX) * (1.0f - cellPosY);
				if(gridPosX + 1 < terrainSize) heights[gridPosX + 1 + gridPosY * terrainSize] += droppedSediment * cellPosX * (1.0f - cellPosY);
				if (gridPosY + 1 < terrainSize) heights[gridPosX + (gridPosY + 1) * terrainSize] += droppedSediment * (1.0f - cellPosX) * cellPosY;
				if (gridPosX + 1 < terrainSize && gridPosY + 1 < terrainSize) heights[gridPosX + 1 + (gridPosY + 1) * terrainSize] += droppedSediment * cellPosX * cellPosY;
				
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

						if (xPos < 0 || yPos < 0 || xPos >= terrainSize || yPos >= terrainSize) continue;

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

						if (gridPosX + xPos < 0 || gridPosY + yPos < 0 || gridPosX + xPos >= terrainSize || gridPosY + yPos >= terrainSize) continue;

						const int radiusIdx{ radX + radY * m_ErosionRadius };
						heights[gridPosX + xPos + (gridPosY + yPos) * terrainSize] -= takenSediment * radiusWeights[radiusIdx];
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