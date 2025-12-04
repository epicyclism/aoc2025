#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "timer.h"

constexpr std::array<int, 8> eight_round (int stride)
{
	return { -(stride + 1), -stride, -(stride - 1), -1, 1, stride - 1, stride, stride + 1};
}

auto get_input()
{
	std::vector<char> v;
	size_t stride = 0;
	std::string ln;
	if(std::getline(std::cin, ln))
	{
		stride = ln.size() + 2;
		v.insert(v.end(), stride, '.');
		v.emplace_back('.');
		v.insert(v.end(), ln.begin(), ln.end());
		v.emplace_back('.');
		while(std::getline(std::cin, ln))
		{
			v.emplace_back('.');
			v.insert(v.end(), ln.begin(), ln.end());
			v.emplace_back('.');			
		}
		v.insert(v.end(), stride, '.');
	}

	return std::make_pair(v, stride);
}

int cnt_rnd(auto const& g, size_t st, size_t pt)
{
	auto mk8 = eight_round(st);
	int r = 0;
	for(auto o: mk8)
		r += g[pt + o] == '@';
	return r;
}

auto accessible(auto const& g, size_t stride)
{
	std::vector<size_t> r;
	for(int x = stride; x < g.size() - stride;x += stride)
	{
		for(int p = 1; p < stride - 1; ++p)
		{
			auto pt = x + p;
			if(g[pt] == '@' && cnt_rnd(g, stride, pt) < 4)
				r.emplace_back(pt);
		}
	}
	return r;
}

auto pt12(auto in)
{
	timer t("p12");
	auto [g, stride] = in;
	int p1 = 0;
	int p2 = 0;
	auto rr = accessible(g, stride);
	p1 = rr.size();
	while(!rr.empty())
	{
		p2 += rr.size();
		for(auto p: rr)
			g[p] = '.';
		rr = accessible(g, stride);
	}
	return std::make_pair(p1, p2);
}

int main()
{
	auto in = get_input();
	auto [p1, p2] = pt12(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
