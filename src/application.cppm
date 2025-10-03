module;

export module application;

import std;
import sdl;
import imgui;

export namespace project
{
	// Abstract class for each UI layer
	class app_layer
	{
	protected:
		ImGuiIO *const io = &ImGui::GetIO();

	public:
		virtual ~app_layer() = default;

		virtual void update() = 0;
	};

	template <typename T_layer>
	concept app_layer_base = std::is_base_of_v<app_layer, T_layer>;

	// Frame Rate Stats class
	class fps_layer : public app_layer
	{
	public:
		void update() override
		{
			ImGui::SetNextWindowFocus();

			constexpr auto msec = 1000.0f;
			ImGui::Begin("Frame Rate");
			ImGui::Text("%.3f ms/frame (%.1f FPS)", msec / io->Framerate, io->Framerate);
			ImGui::End();
		}
	};

	class application
	{
	public:
		~application() = default;
		application(SDL_Window *wnd, SDL_GPUDevice *gpu)
			: wnd(wnd), gpu(gpu)
		{
		}

		template <typename T_layer>
			requires(std::is_base_of_v<app_layer, T_layer>)
		void add_layer()
		{
			layer_stack.push_back(std::make_unique<T_layer>());
		}

		void run()
		{
			while (not quit)
			{
				process_sdl_events();
				update();
				draw();
			}
		}

	private:
		void process_sdl_events()
		{
			while (SDL_PollEvent(&event))
			{
				ImGui_ImplSDL3_ProcessEvent(&event);
				switch (event.type)
				{
				case SDL_EVENT_QUIT:
					quit = true;
					break;
				default:
					break;
				}
			}
		}

		void update()
		{
			gui.update();

			for (auto &&lyr : layer_stack)
			{
				lyr->update();
			}
		}

		void draw()
		{
			auto cmd_buf = SDL_AcquireGPUCommandBuffer(gpu);
			assert(cmd_buf != nullptr and "Failed to acquire command buffer");

			color_target.texture = sdl::next_swapchain_image(wnd, cmd_buf);

			gui.draw(cmd_buf, &color_target);

			SDL_SubmitGPUCommandBuffer(cmd_buf);
		}

		SDL_Window *wnd    = nullptr;
		SDL_GPUDevice *gpu = nullptr;

		imgui::imgui gui = imgui::imgui(wnd, gpu);

		bool quit              = false;
		SDL_FColor clear_color = { 0.2f, 0.2f, 0.4f, 1.0f };
		SDL_Event event        = {};

		SDL_GPUColorTargetInfo color_target = {
			.clear_color = clear_color,
			.load_op     = SDL_GPU_LOADOP_CLEAR,
			.store_op    = SDL_GPU_STOREOP_STORE,
		};

		using app_layer_ptr                    = std::unique_ptr<app_layer>;
		std::vector<app_layer_ptr> layer_stack = {};
	};
}