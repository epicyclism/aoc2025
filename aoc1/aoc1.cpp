#include <iostream>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

auto get_input()
{
	std::vector<int> v;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		std::string_view s(ln);
		s.remove_prefix(1);
		auto tmp = sv_to_t<int>(s);
		v.emplace_back(ln[0] == 'L' ? tmp * -1 : tmp);
	}
	return v;
}

auto pt12(auto const& in)
{
	timer t("p12");
	int p = 50;
	int p1 = 0;
	int p2 = 0;
	for(auto t: in)
	{
		if(t < 0)
		{
			p2 -= t / 100;
			t %= 100;
			for(int n = t; n < 0; ++n)
			{
				--p;
				p2 += p == 0;
				if(p < 0)
					p = 99;
			}
		}
		else
		{
			p2 += t / 100;
			t %= 100;
			for(int n = 0; n < t; ++n)
			{
				++p;
				if(p == 100)
				{
					++p2;
					p = 0;
				}
			}
		}
		p1 += p == 0;
	}
	return std::make_pair(p1, p2);
}

int main()
{
	auto in = get_input();
	auto[p1, p2] = pt12(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
