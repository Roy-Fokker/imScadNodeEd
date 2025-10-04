module;

export module nodes:operators;

import std;
import helper;

export namespace project::nodes::operators
{
	template <auto Op, StaticString name>
	class operator_node : public ImFlow::BaseNode
	{
	public:
		operator_node()
		{
			setTitle(std::format("{}", name.str.data()));
			setStyle(ImFlow::NodeStyle::green());

			using ImFlow::BaseNode;
			addIN<float>("A", 0.f, ImFlow::ConnectionFilter::Numbers());
			addIN<float>("B", 0.f, ImFlow::ConnectionFilter::Numbers());

			auto out_pin = addOUT<float>("Result", nullptr);
			out_pin->behaviour([this] -> float {
				return Op(getInVal<float>("A"), getInVal<float>("B"));
			});
		}

		void draw() override
		{
			auto result = Op(getInVal<float>("A"), getInVal<float>("B"));

			ImGui::SetNextItemWidth(150.f);
			ImGui::Text("%f", result);
		}
	};

	using addition       = operator_node<std::plus{}, "Add">;
	using subtraction    = operator_node<std::minus{}, "Minus">;
	using multiplication = operator_node<std::multiplies{}, "Multiply">;
	using division       = operator_node<std::divides{}, "Divide">;
	using modulo         = operator_node<std::fmodf, "Modulo">;
	using exponentiation = operator_node<std::powf, "Exponent">;
}