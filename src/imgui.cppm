module;

export module imgui;

import std;

export namespace imgui
{
	class imgui
	{
		ImGuiContext *ctx = nullptr;
		ImGuiIO *io       = nullptr;

	public:
		imgui(SDL_Window *wnd, SDL_GPUDevice *gpu)
		{
			IMGUI_CHECKVERSION();
			ctx = ImGui::CreateContext();

			io = &ImGui::GetIO();
			io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

			ImGui::StyleColorsDark();

			ImGui_ImplSDL3_InitForSDLGPU(wnd);
			auto init_info = ImGui_ImplSDLGPU3_InitInfo{
				.Device            = gpu,
				.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpu, wnd),
				.MSAASamples       = SDL_GPU_SAMPLECOUNT_1,
			};
			ImGui_ImplSDLGPU3_Init(&init_info);
		}

		~imgui()
		{
			ImGui_ImplSDLGPU3_Shutdown();
			ImGui_ImplSDL3_Shutdown();
			ImGui::DestroyContext(ctx);
		}

		void update()
		{
			ImGui_ImplSDLGPU3_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();
		}

		void draw(SDL_GPUCommandBuffer *cmd_buf, const SDL_GPUColorTargetInfo *color_target)
		{
			ImGui::Render();

			auto draw_data = ImGui::GetDrawData();
			ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, cmd_buf);

			auto render_pass = SDL_BeginGPURenderPass(cmd_buf, color_target, 1, nullptr);
			ImGui_ImplSDLGPU3_RenderDrawData(draw_data, cmd_buf, render_pass);
			SDL_EndGPURenderPass(render_pass);
		}
	};
}