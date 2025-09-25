module;

export module nodes;

import std;
import sdl;
import application;

export namespace project
{
	class nodes_layer : public app_layer
	{
		ImFlow::ImNodeFlow editor_surface;

	public:
		void update() override
		{
			auto display_size = io->DisplaySize;
			display_size.x    = std::clamp(display_size.x, 100.f, display_size.x - 50.f);
			display_size.y    = std::clamp(display_size.y, 100.f, display_size.y - 50.f);

			ImGui::SetNextWindowSize(display_size);
			ImGui::Begin("Node Editor");
			editor_surface.update();
			ImGui::End();
		}
	};
}