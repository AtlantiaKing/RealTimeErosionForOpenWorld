#include "VelocityField.h"

#include <ImGui/imgui.h>
#include <Debug.h>

void Erosion::VelocityField::GetHeights(std::vector<float>& heights)
{
	// Create a grid of cells
	std::vector<Cell> cells(heights.size());
	for (size_t i{}; i < heights.size(); ++i)
	{
		cells[i] = Cell { &heights[i] };
	}

	// Erosion radius data
	std::vector<float> radiusWeights{};
	radiusWeights.resize(m_ErosionRadius * m_ErosionRadius);

	// Retrieve the terrain size
	const int terrainSize{ static_cast<int>(sqrtf(static_cast<float>(heights.size()))) };

	for (int i{}; i < m_Cycles; ++i)
	{
		// Spawn droplets
		for (int dropletIdx{}; dropletIdx < m_DropletsPerCycle; ++dropletIdx)
		{
			const int waterCellIdx{ (rand() % terrainSize) + (rand() % terrainSize) * terrainSize };
			Cell& waterCell{ cells[waterCellIdx] };
			waterCell.water += m_WaterIncrement * m_TimeStep;
		}

		// Calculate flux
		for (int x{}; x < terrainSize; ++x)
		{
			for (int z{}; z < terrainSize; ++z)
			{
				// Get current cell
				const int cellIdx{ x + z * terrainSize };
				Cell& cell{ cells[cellIdx] };

				// Get height difference if the neighbouring cell exists
				float heightDiffL{};
				if (x - 1 >= 0)
				{
					const Cell& leftCell{ cells[x - 1 + z * terrainSize] };
					heightDiffL = *cell.height + cell.water - *leftCell.height - leftCell.water;
				}
				// Calculate the flux
				float fluxL{ std::max(0.0f, cell.leftFlux + m_TimeStep * m_PipeArea * (m_Gravity * heightDiffL) / m_PipeLength) };
				// Fix floating point errors
				fluxL = fluxL < m_FluxEpsilon ? 0.0f : fluxL;

				// Get height difference if the neighbouring cell exists
				float heightDiffR{};
				if (x + 1 < terrainSize)
				{
					const Cell& rightCell{ cells[x + 1 + z * terrainSize] };
					heightDiffR = *cell.height + cell.water - *rightCell.height - rightCell.water;
				}
				// Calculate the flux
				float fluxR{ std::max(0.0f, cell.rightFlux + m_TimeStep * m_PipeArea * (m_Gravity * heightDiffR) / m_PipeLength) };
				// Fix floating point errors
				fluxR = fluxR < m_FluxEpsilon ? 0.0f : fluxR;

				// Get height difference if the neighbouring cell exists
				float heightDiffT{};
				if (z + 1 < terrainSize)
				{
					const Cell& topCell{ cells[x + (z + 1) * terrainSize] };
					heightDiffT = *cell.height + cell.water - *topCell.height - topCell.water;
				}
				// Calculate the flux
				float fluxT{ std::max(0.0f, cell.topFlux + m_TimeStep * m_PipeArea * (m_Gravity * heightDiffT) / m_PipeLength) };
				// Fix floating point errors
				fluxT = fluxT < m_FluxEpsilon ? 0.0f : fluxT;

				// Get height difference if the neighbouring cell exists
				float heightDiffB{};
				if (z - 1 >= 0)
				{
					const Cell& bottomCell{ cells[x + (z - 1) * terrainSize] };
					heightDiffB = *cell.height + cell.water - *bottomCell.height - bottomCell.water;
				}
				// Calculate the flux
				float fluxB{ std::max(0.0f, cell.bottomFlux + m_TimeStep * m_PipeArea * (m_Gravity * heightDiffB) / m_PipeLength) };
				// Fix floating point errors
				fluxB = fluxB < m_FluxEpsilon ? 0.0f : fluxB;

				// Calculate the total flux sum
				const float fluxSum{ fluxL + fluxR + fluxT + fluxB };
				// Calculate the limit multiplier so water in the current cell won't go negative
				const float k{ fluxSum < FLT_EPSILON ? 0.0f : std::min(1.0f, cell.water / fluxSum) };

				// Calculate the final flux values by multiplying the calculated flux values with the limit multiplier
				cell.leftFlux = fluxL * k;
				cell.rightFlux = fluxR * k;
				cell.topFlux = fluxT * k;
				cell.bottomFlux = fluxB * k;
			}
		}

		// Calculate velocity and sediment
		for (int x{}; x < terrainSize; ++x)
		{
			for (int z{}; z < terrainSize; ++z)
			{
				// Get current cell
				const int cellIdx{ x + z * terrainSize };
				Cell& cell{ cells[cellIdx] };

				// Retrieve incoming flux values from neighbouring cells
				const float incomingFluxL{ x - 1 >= 0 ? cells[x - 1 + z * terrainSize].rightFlux : 0.0f };
				const float incomingFluxR{ x + 1 < terrainSize ? cells[x + 1 + z * terrainSize].leftFlux : 0.0f };
				const float incomingFluxT{ z + 1 < terrainSize ? cells[x + (z + 1) * terrainSize].bottomFlux : 0.0f };
				const float incomingFluxB{ z - 1 >= 0 ? cells[x + (z - 1) * terrainSize].topFlux : 0.0f };

				// Calculate the volume change by adding incoming flux and substracting outgoing flux from this cell
				const float volumeChange
				{
					incomingFluxL + incomingFluxR + incomingFluxT + incomingFluxB
					- cell.leftFlux - cell.rightFlux - cell.topFlux - cell.bottomFlux
				};

				// Calculate new water height and average water height over the current cycle
				const float averageWaterHeight{ cell.water + volumeChange / 2.0f };
				cell.water += volumeChange;

				// Calculate water movement per direction
				const float waterMovementX{ (incomingFluxL + cell.rightFlux - incomingFluxR - cell.leftFlux) / 2.0f };
				const float waterMovementY{ (incomingFluxB + cell.topFlux - incomingFluxT - cell.bottomFlux) / 2.0f };

				// Calculate velocity depending on the water movement and water height
				if (abs(averageWaterHeight) < FLT_EPSILON)
				{
					cell.velocityX = 0.0f;
					cell.velocityY = 0.0f;
				}
				else
				{
					cell.velocityX = waterMovementX / averageWaterHeight;
					cell.velocityY = waterMovementY / averageWaterHeight;
				}

				// Retrieve heights of neighbouring cells
				const float heightL{ x - 1 >= 0 ? *cells[x - 1 + z * terrainSize].height : 0.0f };
				const float heightR{ x + 1 < terrainSize ? *cells[x + 1 + z * terrainSize].height : 0.0f };
				const float heightT{ z + 1 < terrainSize ? *cells[x + (z + 1) * terrainSize].height : 0.0f };
				const float heightB{ z - 1 >= 0 ? *cells[x + (z - 1) * terrainSize].height : 0.0f };

				// Calculate height difference from neighbouring cells
				const float heightDiffXSqr{ (heightR - heightL) * (heightR - heightL) };
				const float heightDiffYSqr{ (heightT - heightB) * (heightT - heightB) };

				// Calculate the sin of the local tilt angle of the current cell
				const float sinTiltAngle{ sqrtf(heightDiffXSqr + heightDiffYSqr) / sqrtf(heightDiffXSqr + heightDiffYSqr + 1) };
				// Calcualte the magnitude of the velocity in the current cell
				const float velocityMagnitude{ sqrtf(cell.velocityX * cell.velocityX + cell.velocityY * cell.velocityY) };
				// Calculate the allowed capacity for the current velocity and tilt angle
				const float curCapacity{ m_Capacity * sinTiltAngle * velocityMagnitude };

				// Remove or add sediment to the current cell depending on the capacity
				if (curCapacity > cell.sediment)
				{
					const float removedSediment{ m_Erosion * (m_Capacity - cell.sediment) };


					float totalWeight{};
					float smallestDistance{ FLT_MAX };
					float highestDistance{};

					// Calculate the weights of all the grid points near the droplet and their combined total weight
					const int halfErosionRadius{ m_ErosionRadius / 2 };
					for (int radX{}; radX < m_ErosionRadius; ++radX)
					{
						for (int radZ{}; radZ < m_ErosionRadius; ++radZ)
						{
							const int xDisplacement{ radX - halfErosionRadius };
							const int zDisplacement{ radZ - halfErosionRadius };

							const int xPos = x + xDisplacement;
							const int zPos = z + zDisplacement;

							if (xPos < 0 || zPos < 0 || xPos >= terrainSize || zPos >= terrainSize) continue;

							const float distance{ sqrtf(static_cast<float>(xDisplacement * xDisplacement + zDisplacement * zDisplacement)) };
							const int radiusIdx{ radX + radZ * m_ErosionRadius };
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
						for (int radZ{}; radZ < m_ErosionRadius; ++radZ)
						{
							const int xPos = radX - halfErosionRadius;
							const int zPos = radZ - halfErosionRadius;

							if (x + xPos < 0 || z + zPos < 0 || x + xPos >= terrainSize || z + zPos >= terrainSize) continue;

							const int radiusIdx{ radX + radZ * m_ErosionRadius };
							*cells[x + xPos + (z + zPos) * terrainSize].height -= removedSediment * radiusWeights[radiusIdx];
						}
					}

					cell.sediment += removedSediment;
				}
				else
				{
					const float addedSediment{ m_Deposition * (cell.sediment - curCapacity) };

					*cell.height += addedSediment;
					cell.sediment -= addedSediment;
				}
			}
		}

		// Move sediment with the water flow and evaporate water
		for (int x{}; x < terrainSize; ++x)
		{
			for (int y{}; y < terrainSize; ++y)
			{
				// Get the current cell
				const int cellIdx{ x + y * terrainSize };
				Cell& cell{ cells[cellIdx] };

				// Find the previous cell depending on the velocity (e.g. if moving forward, get the back cell)
				const int incomingSedimentX{ static_cast<int>(x + 0.5f - cell.velocityX * m_TimeStep) };
				const int incomingSedimentY{ static_cast<int>(y + 0.5f - cell.velocityY * m_TimeStep) };

				if (abs(cell.velocityX) < 0.5f && abs(cell.velocityY) < 0.5f)
				{
					int test{};
					test;
				}

				if (incomingSedimentX < 0 || incomingSedimentX >= terrainSize || incomingSedimentY < 0 || incomingSedimentY >= terrainSize)
				{
					// If the cell doesn't exist, find the four closest neighbouring cells and take the average of their sediment values
					int closestX[4]{};
					int closestY[4]{};
					float closestSediments[4]{};
					int closestDistances[4]{ INT_MAX, INT_MAX, INT_MAX, INT_MAX };

					// Find four closest neighbours
					for (int otherX{ -2 }; otherX <= 2; ++otherX)
					{
						if (incomingSedimentX + otherX < 0 || incomingSedimentX + otherX >= terrainSize) continue;

						for (int otherY{ -2 }; otherY <= 2; ++otherY)
						{
							if (incomingSedimentY + otherY < 0 || incomingSedimentY + otherY >= terrainSize) continue;

							const int distance{ otherX * otherX + otherY * otherY };

							for (int neighbourIdx{}; neighbourIdx < 4; ++neighbourIdx)
							{
								int& neighbourDistance{ closestDistances[neighbourIdx] };
								if (neighbourDistance < distance) continue;

								neighbourDistance = distance;
								closestX[neighbourIdx] = incomingSedimentX + otherX;
								closestY[neighbourIdx] = incomingSedimentY + otherY;
								const int neighbourCellIdx{ static_cast<int>(incomingSedimentX + otherX) + static_cast<int>(incomingSedimentY + otherY) * terrainSize };
								closestSediments[neighbourIdx] = cells[neighbourCellIdx].sediment;
							}
						}
					}

					// Calculate average of their sediment values
					cell.sediment = (closestSediments[0] + closestSediments[1] + closestSediments[2] + closestSediments[3]) / 4.0f;
				}
				else
				{
					// Move the sediment from the previous cell to the next one
					const int incomingSedimentIdx{ incomingSedimentX + incomingSedimentY * terrainSize };

					cell.sediment = cells[incomingSedimentIdx].sediment;
				}

				// Evaporate water
				cell.water *= (1.0f - m_Evaporation);
			}
		}
	}
}

void Erosion::VelocityField::OnGUI()
{
	ImGui::Spacing();
	ImGui::Text("Velocity Field Settings");
	ImGui::SliderInt("Nr Cycles", &m_Cycles, 0, 1000);
	ImGui::SliderInt("Drops per Cycle", &m_DropletsPerCycle, 0, 100);
	ImGui::SliderFloat("Water Increment", &m_WaterIncrement, 0.0f, 0.5f, "%0.10f");
	ImGui::SliderFloat("Gravity", &m_Gravity, -10.0f, 10.0f);
	ImGui::SliderFloat("Capacity", &m_Capacity, 0.0f, 0.1f, "%0.10f");
	ImGui::SliderFloat("Erosion", &m_Erosion, 0.0f, 1.0f);
	ImGui::SliderFloat("Deposition", &m_Deposition, 0.0f, 1.0f);
	ImGui::SliderFloat("Evaporation", &m_Evaporation, 0.0f, 1.0f);
	ImGui::SliderFloat("Time step", &m_TimeStep, 0.0f, 5.0f);
	ImGui::SliderFloat("Pipe length", &m_PipeLength, 0.0f, 5.0f);
	ImGui::SliderFloat("Pipe area", &m_PipeArea, 0.0f, 5.0f);
}
