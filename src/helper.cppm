module;

export module helper;

import std;

export namespace project
{
	template <size_t N>
	struct StaticString
	{
		std::array<char, N> str = {};

		constexpr StaticString(const char (&input)[N])
		{
			std::ranges::copy_n(input, N, str.begin());
		}
	};
}