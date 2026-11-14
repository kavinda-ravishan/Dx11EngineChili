#pragma once
#include <optional>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <DX3D/Window/ImGUI/imgui.h>

#include <DX3D/Graphics/Bindable/BindableCommon.hpp>
#include <DX3D/Graphics/Drawable/Drawable.hpp>
#include <DX3D/Graphics/Drawable/Vertex.hpp>

namespace dx3d {

	class Mesh final : public Drawable
	{
	public:
		Mesh(const DrawableDesc& desc, Graphics& graphics, std::vector<std::shared_ptr<Bindable>> bindables);

		void Draw(Graphics& graphics, DirectX::FXMMATRIX accumulated_transform) const noxnd;

		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	private:
		mutable DirectX::XMFLOAT4X4 _transform;
	};

	class Node final
	{
		friend class Model;

	public:
		struct PSMaterialConstantFullmonte
		{
			BOOL normal_map_enabled{ TRUE };
			BOOL specular_map_enabled{ TRUE };
			BOOL has_gloss_map{};
			float specular_power{ 3.1f };
			DirectX::XMFLOAT3 specular_color{ 0.75f, 0.75f, 0.75f };
			float specular_map_weight{ 0.671f };
		};

		struct PSMaterialConstantNotex
		{
			DirectX::XMFLOAT4 material_color{};
			DirectX::XMFLOAT4 specular_color{};
			float specular_power{};
			float padding[3]{};
		};

	public:
		Node(const int id, const std::string& name, std::vector<Mesh*> mesh_ptrs, const DirectX::XMMATRIX& transform) noxnd;

		void Draw(Graphics& graphics, DirectX::FXMMATRIX accumulated_transform) const noxnd;
		void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
		const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
		void ShowTree(Node*& selected_node_ptr) const noexcept;
		int GetID() const noexcept;

		template<class T>
		bool MaterialConstantControl(Graphics& graphics, T& c)
		{
			if (_mesh_ptrs.empty())
			{
				return false;
			}

			if constexpr (std::is_same<T, PSMaterialConstantFullmonte>::value)
			{
				if (auto pcb = _mesh_ptrs.front()->QueryBindable<BPixelConstantBuffer<T>>())
				{
					ImGui::Text("Material");

					bool normal_map_enabled = (bool)c.normal_map_enabled;
					ImGui::Checkbox("Norm Map", &normal_map_enabled);
					c.normal_map_enabled = normal_map_enabled ? TRUE : FALSE;

					bool specular_map_enabled = (bool)c.specular_map_enabled;
					ImGui::Checkbox("Spec Map", &specular_map_enabled);
					c.specular_map_enabled = specular_map_enabled ? TRUE : FALSE;

					bool has_gloss_map = (bool)c.has_gloss_map;
					ImGui::Checkbox("Gloss Alpha", &has_gloss_map);
					c.has_gloss_map = has_gloss_map ? TRUE : FALSE;

					ImGui::SliderFloat("Spec Weight", &c.specular_map_weight, 0.0f, 2.0f);

					ImGui::SliderFloat("Spec Pow", &c.specular_power, 0.0f, 1000.0f, "%f", ImGuiSliderFlags_Logarithmic);

					ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&c.specular_color));

					pcb->Update(graphics, c);
					return true;
				}
			}
			else if constexpr (std::is_same<T, struct PSMaterialConstantNotex>::value)
			{
				if (auto pcb = _mesh_ptrs.front()->QueryBindable<BPixelConstantBuffer<T>>())
				{
					ImGui::Text("Material");

					ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&c.specular_color));

					ImGui::SliderFloat("Spec Pow", &c.specular_power, 0.0f, 1000.0f, "%f", ImGuiSliderFlags_Logarithmic);

					ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&c.material_color));

					pcb->Update(graphics, c);
					return true;
				}
			}
			return false;
		}

	private:
		void AddChild(std::unique_ptr<Node> child) noxnd;

	private:
		const int _id{};
		const std::string _name{};
		std::vector<std::unique_ptr<Node>> _child_ptrs{};
		std::vector<Mesh*> _mesh_ptrs{};
		DirectX::XMFLOAT4X4 _transform{};
		DirectX::XMFLOAT4X4 _applied_transform{};
	};

	class Model final : public Base
	{
	public:
		Model(const BaseDesc& desc, Graphics& graphics, const std::filesystem::path file_path, const float scale = 1.0f);
		~Model() noexcept;
		
		void Draw(Graphics& graphics) const;
		void ShowWindow(Graphics& graphics, const char* window_name = nullptr) noexcept;
		void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;

	private:
		static std::unique_ptr<Mesh> ParseMesh(
			const BaseDesc& desc, 
			Graphics& graphics, 
			const std::filesystem::path& file_path, 
			const aiMesh& ai_mesh, 
			const aiMaterial* const* ai_material, 
			const float scale
		);
		std::unique_ptr<Node> ParseNode(int& next_id, const aiNode& node);

	private:
		std::unique_ptr<Node> _root{};
		std::vector<std::unique_ptr<Mesh>> _mesh_ptrs{};
		std::unique_ptr<class ModelWindow> _window{};
	};
} // namespace dx3d
