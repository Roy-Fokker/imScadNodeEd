module;

export module nodes:constants;

import std;
import helper;

export namespace project::nodes::constants
{
	template <float value, StaticString name>
	class constant_node : public ImFlow::BaseNode
	{
		constexpr static float val = value;

	public:
		constant_node()
		{
			setTitle(std::format("{}: {}", name.str.data(), val));
			setStyle(ImFlow::NodeStyle::cyan());

			using ImFlow::BaseNode;
			auto out_pin = addOUT<float>("Out", nullptr);
			out_pin->behaviour([this] -> float {
				return val;
			});
		}
	};

	using pi_node  = constant_node<std::numbers::pi_v<float>, "Pi">;
	using phi_node = constant_node<std::numbers::phi_v<float>, "Phi">;
	using exp_node = constant_node<std::numbers::e_v<float>, "e">;
}