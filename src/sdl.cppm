module;

export module sdl;

import std;

export namespace sdl
{
	// If we are building in DEBUG mode, use this constant to enable extra messages from SDL
	constexpr auto IS_DEBUG = bool{
#ifdef DEBUG
		true
#endif
	};

	// Deleter template, for use with SDL objects.
	// Allows use of SDL Objects with C++'s smart pointers, using SDL's destroy function
	template <auto fn>
	struct sdl_deleter
	{
		constexpr void operator()(auto *arg)
		{
			fn(arg);
		}
	};
	// Define SDL type with std::unique_ptr and custom deleter
	using window_ptr = std::unique_ptr<SDL_Window, sdl_deleter<SDL_DestroyWindow>>;

	// Special deleter for gpu.
	// it will release window on destruction
	struct gpu_window_deleter
	{
		SDL_Window *window = nullptr;
		constexpr void operator()(auto *gpu)
		{
			assert(window != nullptr and "Window pointer for GPU is null.");

			SDL_ReleaseWindowFromGPUDevice(gpu, window);
			SDL_DestroyGPUDevice(gpu);
		}
	};
	// Define GPU type with std::unique_ptr and custom deleter
	using gpu_ptr = std::unique_ptr<SDL_GPUDevice, gpu_window_deleter>;

	// Deleter for all gpu objects in SDL
	template <auto fn>
	struct gpu_deleter
	{
		SDL_GPUDevice *gpu = nullptr;
		constexpr void operator()(auto *arg)
		{
			fn(gpu, arg);
		}
	};
	// Define SDL GPU types with std::unique_ptr and custom deleter
	using free_gfx_pipeline  = gpu_deleter<SDL_ReleaseGPUGraphicsPipeline>;
	using gfx_pipeline_ptr   = std::unique_ptr<SDL_GPUGraphicsPipeline, free_gfx_pipeline>;
	using free_comp_pipeline = gpu_deleter<SDL_ReleaseGPUComputePipeline>;
	using comp_pipeline_ptr  = std::unique_ptr<SDL_GPUComputePipeline, free_comp_pipeline>;
	using free_gfx_shader    = gpu_deleter<SDL_ReleaseGPUShader>;
	using gfx_shader_ptr     = std::unique_ptr<SDL_GPUShader, free_gfx_shader>;
	using free_gpu_buffer    = gpu_deleter<SDL_ReleaseGPUBuffer>;
	using gpu_buffer_ptr     = std::unique_ptr<SDL_GPUBuffer, free_gpu_buffer>;
	using free_gpu_texture   = gpu_deleter<SDL_ReleaseGPUTexture>;
	using gpu_texture_ptr    = std::unique_ptr<SDL_GPUTexture, free_gpu_texture>;
	using free_gpu_sampler   = gpu_deleter<SDL_ReleaseGPUSampler>;
	using gfx_sampler_ptr    = std::unique_ptr<SDL_GPUSampler, free_gpu_sampler>;

	// Simple SDL class which will manage initialization and clean up of SDL and it's related libraries.
	class sdl
	{
	public:
		sdl()
		{
			auto result = SDL_Init(SDL_INIT_VIDEO);
			assert(result and "SDL could not be initialized.");
		}

		~sdl()
		{
			SDL_Quit();
		}
	};

	// Used to create SDL_Window using make_window function
	struct window_desc_t
	{
		uint32_t width;
		uint32_t height;
		std::string_view title;
		SDL_WindowFlags flags;
	};

	// Create SDL Window with described parameters
	auto make_window(const window_desc_t &desc = {}) -> window_ptr
	{
		auto window = SDL_CreateWindow(desc.title.data(), static_cast<int>(desc.width), static_cast<int>(desc.height), desc.flags);
		assert(window != nullptr and "Window could not be created.");

		return window_ptr{ window };
	}

	// Wrap SDL's C-style enums into C++, totally unnecessary
	enum class swapchain_mode : uint8_t
	{
		vsync     = SDL_GPU_PRESENTMODE_VSYNC,
		immediate = SDL_GPU_PRESENTMODE_IMMEDIATE,
		mailbox   = SDL_GPU_PRESENTMODE_MAILBOX,
	};

	auto to_sdl(swapchain_mode mode) -> SDL_GPUPresentMode
	{
		return static_cast<SDL_GPUPresentMode>(mode);
	}

	// Wrap SDL's C-style enums into C++, totally unnecessary
	enum class swapchain_composition : uint8_t
	{
		sdr                 = SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
		sdr_linear          = SDL_GPU_SWAPCHAINCOMPOSITION_SDR_LINEAR,
		hdr_extended_linear = SDL_GPU_SWAPCHAINCOMPOSITION_HDR_EXTENDED_LINEAR,
		hdr10_st2048        = SDL_GPU_SWAPCHAINCOMPOSITION_HDR10_ST2084
	};

	auto to_sdl(swapchain_composition composition) -> SDL_GPUSwapchainComposition
	{
		return static_cast<SDL_GPUSwapchainComposition>(composition);
	}

	// Used to create SDL_GPUDevice using make_gpu function
	struct gpu_desc_t
	{
		SDL_GPUShaderFormat preferred_shader_format;
		swapchain_mode mode               = swapchain_mode::vsync;
		swapchain_composition composition = swapchain_composition::sdr;
	};

	// make_gpu does not take ownership of *wnd. *wnd must stay alive for duration of gpu_ptr.
	auto make_gpu(SDL_Window *wnd, const gpu_desc_t &desc = {}) -> gpu_ptr
	{
		auto gpu = SDL_CreateGPUDevice(desc.preferred_shader_format, IS_DEBUG, NULL);
		assert(gpu != nullptr and "GPU device could not be created.");

		std::println("GPU Driver API: {}", SDL_GetGPUDeviceDriver(gpu));

		auto result = SDL_ClaimWindowForGPUDevice(gpu, wnd);
		assert(result == true and "Could not claim window for GPU.");

		result = SDL_WindowSupportsGPUPresentMode(gpu, wnd, to_sdl(desc.mode));
		assert(result == true and "GPU swapchain does not support swapchain mode.");

		result = SDL_WindowSupportsGPUSwapchainComposition(gpu, wnd, to_sdl(desc.composition));
		assert(result == true and "GPU swapchain does not support swapchain composition.");

		result = SDL_SetGPUSwapchainParameters(gpu, wnd, to_sdl(desc.composition), to_sdl(desc.mode));
		assert(result == true and "Unable to set GPU Swapchain Parameters");

		return gpu_ptr{ gpu, { wnd } };
	}

	// Get the next image in swapchain queue
	auto next_swapchain_image(SDL_Window *wnd, SDL_GPUCommandBuffer *cmd_buf) -> SDL_GPUTexture *
	{
		auto sc_tex = (SDL_GPUTexture *)nullptr;

		auto res = SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buf, wnd, &sc_tex, NULL, NULL);
		assert(res == true and "Wait and acquire GPU swapchain texture failed.");
		assert(sc_tex != nullptr and "Swapchain texture is null. Is window minimized?");

		return sc_tex;
	}
}