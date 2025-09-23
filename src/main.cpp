import std;

import sdl;
import imgui;

using namespace std::literals;

namespace
{
	constexpr auto WND_WIDTH     = 800u;                                       // Window Width
	constexpr auto ASPECT_RATIO  = 16.f / 9.f;                                 // Window's Aspect Ratio
	constexpr auto WND_HEIGHT    = static_cast<int>(WND_WIDTH / ASPECT_RATIO); // Window Height based on Width and Aspect Ratio
	constexpr auto SHADER_FORMAT = SDL_GPUShaderFormat{                        // Shader's will be using this format
#ifdef SPIRV
		                                                SDL_GPU_SHADERFORMAT_SPIRV
#elifdef DXIL
		                                                SDL_GPU_SHADERFORMAT_DXIL
#endif
	};

	// TODO: why does it need 'inline' keyword???
	inline constexpr auto WND_TITLE = "OpenSCAD Node Editor"sv;
}

int main()
{
	// object to init and quit SDL 3
	auto sdl_o = sdl::sdl();

	// get a smart pointer wrapped SDL_Window
	auto wnd = sdl::make_window({
		.width  = WND_WIDTH,
		.height = WND_HEIGHT,
		.title  = WND_TITLE,
	});

	// get a smart pointer wrapped SDL_GPUDevice
	auto gpu = sdl::make_gpu(wnd.get(),
	                         {
								 .preferred_shader_format = SHADER_FORMAT,
							 });

	// Basic state management variables
	auto clear_color = SDL_FColor{ 0.2f, 0.2f, 0.4f, 1.0f };
	auto quit        = false;

	// So i don't have to type .get() everywhere raw pointer is needed
	auto pgpu = gpu.get();
	auto pwnd = wnd.get();

	auto gui = imgui::imgui(pwnd, pgpu);

	// variable to be populated by SDL_PollEvent in event loop
	auto evt = SDL_Event{};
	while (not quit)
	{
		// Event Loop, see all SDL Events and act as necessary
		while (SDL_PollEvent(&evt))
		{
			switch (evt.type)
			{
			case SDL_EVENT_QUIT:
				quit = true;
				break;

			default:
				break;
			}
		}

		// Update imgui
		gui.update();

		// Draw using GPU
		// Get a command buffer for this frame
		auto cmd_buf = SDL_AcquireGPUCommandBuffer(pgpu);
		assert(cmd_buf != nullptr and "Failed to acquire command buffer.");

		// Get image to render to from swapchain
		auto sc_img = sdl::next_swapchain_image(pwnd, cmd_buf);

		// Color target properties
		auto color_target = SDL_GPUColorTargetInfo{
			.texture     = sc_img,
			.clear_color = clear_color,
			.load_op     = SDL_GPU_LOADOP_CLEAR,
			.store_op    = SDL_GPU_STOREOP_STORE,
		};

		/* not needed as we are only using imgui
		auto render_pass = SDL_BeginGPURenderPass(cmd_buf, &color_target, 1, nullptr);
		// draw call stuff here
		SDL_EndGPURenderPass(render_pass);
		*/

		// draw using imgui
		gui.draw(cmd_buf, &color_target);

		// Submit the command buffer to gpu
		SDL_SubmitGPUCommandBuffer(cmd_buf);
	}

	return 0;
}