#pragma once

#include "../Component.h"

#include "../../Graphics/Mesh.h"
#include "../../Graphics/Material.h"
#include "../../Graphics/Texture.h"

#include <vector>
#include <unordered_map>

namespace leap
{
	namespace graphics
	{
		class IMeshRenderer;
	}

	class TerrainComponent final : public Component
	{
	public:
		TerrainComponent();
		virtual ~TerrainComponent();

		TerrainComponent(const TerrainComponent& other) = delete;
		TerrainComponent(TerrainComponent&& other) = delete;
		TerrainComponent& operator=(const TerrainComponent& other) = delete;
		TerrainComponent& operator=(TerrainComponent&& other) = delete;

		void SetSize(unsigned int size, unsigned int quadsPerMeter = 1);
		void LoadHeightmap(const std::string& path, bool isShared = false, unsigned int quadsPerMeter = 1);

		std::vector<float> GetHeights() const;
		void SetHeights(const std::vector<float>& heights);

	private:
		struct TerrainMesh final
		{
			unsigned int useCounter{ 1 };
			Mesh mesh{};
		};
		inline static std::unordered_map<unsigned int, TerrainMesh> m_Meshes{};
		std::unique_ptr<Material> m_pMaterial{};

		virtual void Awake() override;

		void ApplySizeTexture();
		void ApplySizeMesh(unsigned int prevSize);
		void CreateMesh();

		unsigned int m_Size{};
		unsigned int m_QuadsPerMeter{};
		graphics::IMeshRenderer* m_pRenderer{};
		Texture m_Texture{};
	};
}