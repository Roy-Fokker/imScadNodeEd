import std;

import sdl;

using namespace std::literals;

namespace
{
	constexpr auto WND_WIDTH     = 800u;
	constexpr auto ASPECT_RATIO  = 16.f / 9.f;
	constexpr auto WND_HEIGHT    = static_cast<int>(WND_WIDTH / ASPECT_RATIO);
	constexpr auto SHADER_FORMAT = SDL_GPUShaderFormat{
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
	auto sdl_o = sdl::sdl();

	auto wnd = sdl::make_window({
		.width  = WND_WIDTH,
		.height = WND_HEIGHT,
		.title  = WND_TITLE,
	});

	auto gpu = sdl::make_gpu(wnd.get(),
	                         {
								 .preferred_shader_format = SHADER_FORMAT,
							 });

	auto clear_color = SDL_FColor{ 0.2f, 0.2f, 0.4f, 1.0f };
	auto quit        = false;
	auto evt         = SDL_Event{};

	auto pgpu = gpu.get();
	auto pwnd = wnd.get();

	while (not quit)
	{
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

		// Draw using GPU
		auto cmd_buf = SDL_AcquireGPUCommandBuffer(pgpu);
		assert(cmd_buf != nullptr and "Failed to acquire command buffer.");

		auto sc_img = sdl::next_swapchain_image(pwnd, cmd_buf);

		auto color_target = SDL_GPUColorTargetInfo{
			.texture     = sc_img,
			.clear_color = clear_color,
			.load_op     = SDL_GPU_LOADOP_CLEAR,
			.store_op    = SDL_GPU_STOREOP_STORE,
		};

		auto render_pass = SDL_BeginGPURenderPass(cmd_buf, &color_target, 1, nullptr);
		// draw call stuff here
		SDL_EndGPURenderPass(render_pass);
		SDL_SubmitGPUCommandBuffer(cmd_buf);
	}

	return 0;
}