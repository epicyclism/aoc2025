#include <iostream>
#include <string>
#include <vector>
#include <span>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "ctre_inc.h"
#include "timer.h"

auto get_input()
{
	std::vector<char> v;
	size_t stride = 0;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(stride == 0)
			stride = ln.size();
		v.append_range(ln);
	}
	return std::make_pair(stride, v);
}

void op(auto const& a, auto const& b, auto& out)
{
	int r = 0;
	auto ita = a.begin();
	auto itb = b.begin();
	auto ito = out.begin();
	for( ; ita != a.end(); ++ita, ++itb, ++ito)
	{
		if(*ita == 'S')
			*itb = '|';
		if(*ita == '|')
		{
			if(*itb == '^')
			{
				*ito = '+';
				*(itb - 1) = '|';
				*(itb + 1) = '|';
				r += 2;
			}
			else
			{
				*itb = '|';
			}
		}
	}
}

auto pt1(auto const& in)
{
	timer t("p1");
	auto[stride, grid] = in;
	auto ita = grid.begin();
	auto itb = grid.begin() + stride;
	int pt1 = 0;
	int pt2 = 0;
	std::vector<char> tmp(stride, '.');
	while(itb != grid.end())
	{
		std::ranges::fill(tmp, '.');
		std::span a(ita, stride);
		std::span b(itb, stride);
		op(a, b, tmp);
		pt1 += std::ranges::count(tmp, '+');
		ita = itb;
		itb += stride;
	}
	return pt1;
}

// 3406 too low
//

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	fmt::println("pt1 = {}", p1);
}
