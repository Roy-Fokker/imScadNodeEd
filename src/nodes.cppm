module;

export module nodes;

import std;
import sdl;
import application;

export import :constants;

export namespace project
{
	class simple_sum : public ImFlow::BaseNode
	{
		int val_b = 0;

	public:
		simple_sum()
		{
			setTitle("Simple Sum");
			setStyle(ImFlow::NodeStyle::green());

			using ImFlow::BaseNode;
			addIN<int>("In", 0, ImFlow::ConnectionFilter::SameType());
			addOUT<int>("Out", nullptr)->behaviour([this]() {
				return getInVal<int>("In") + val_b;
			});
		}

		void draw() override
		{
			ImGui::SetNextItemWidth(100.f);
			ImGui::InputInt("##ValB", &val_b);
		}
	};

	class nodes_layer : public app_layer
	{
		ImFlow::ImNodeFlow editor_surface;

	public:
		nodes_layer()
		{
			using namespace nodes;

			editor_surface.addNode<constants::pi_node>({ 20, 10 });
			editor_surface.addNode<constants::phi_node>({ 20, 60 });
			editor_surface.addNode<constants::exp_node>({ 20, 110 });
		}

		void update() override
		{
			auto display_size = io->DisplaySize - ImVec2{ 1.f, 1.f };

			editor_surface.setSize(display_size - ImVec2{ 16.f, 16.f });

			ImGui::SetNextWindowPos({ 1.f, 1.f });
			ImGui::SetNextWindowSize(display_size);
			ImGui::Begin("Node Editor", nullptr,
			             ImGuiWindowFlags_NoTitleBar
			                 | ImGuiWindowFlags_NoResize
			                 | ImGuiWindowFlags_NoMove
			                 | ImGuiWindowFlags_NoCollapse
			                 | ImGuiWindowFlags_NoScrollbar);
			editor_surface.update();
			ImGui::End();
		}
	};
}