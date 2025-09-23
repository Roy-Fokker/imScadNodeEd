import std;

import sdl;
import imgui;
import application;

using namespace std::literals;

namespace
{
	constexpr auto WND_WIDTH     = 800u;                                       // Window Width
	constexpr auto ASPECT_RATIO  = 16.f / 9.f;                                 // Window's Aspect Ratio
	constexpr auto WND_HEIGHT    = static_cast<int>(WND_WIDTH / ASPECT_RATIO); // Window Height based on Width and Aspect Ratio
	constexpr auto SHADER_FORMAT = SDL_GPUShaderFormat{                        // Shaders will be using this format
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
	auto gpu = sdl::make_gpu(wnd.get(), { SHADER_FORMAT });

	// So i don't have to type .get() everywhere raw pointer is needed
	auto pgpu = gpu.get();
	auto pwnd = wnd.get();

	auto app = project::application(pwnd, pgpu);

	app.add_layer<project::test_layer>();

	app.run();

	return 0;
}