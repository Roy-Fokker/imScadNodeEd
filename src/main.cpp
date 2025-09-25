import std;

import sdl;
import imgui;
import application;
import nodes;

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
	inline constexpr auto WND_TITLE = "OpenSCAD Node Editor"sv; // Window Title text
}

auto main() -> int
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

	auto app = project::application(wnd.get(), gpu.get());

	app.add_layer<project::fps_layer>();
	app.add_layer<project::nodes_layer>();

	app.run();

	return 0;
}
