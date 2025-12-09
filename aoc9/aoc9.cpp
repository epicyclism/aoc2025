#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

auto get_input()
{
	std::vector<std::pair<int64_t, int64_t>> v;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		auto[m, a, b] = ctre::match<"(\\d+),(\\d+)">(ln);
		v.emplace_back(a.to_number<int64_t>(), b.to_number<int64_t>());
	}
	// close the loop
	v.emplace_back(v.front());
	return v;
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	auto area = [](auto a, auto b){ return (std::abs(a.first - b.first) + 1) * (std::abs(a.second - b.second) + 1);};
	int64_t p1 = 0;
	for (auto y = 0; y < in.size() - 1; ++y)
		for (auto x = y + 1; x < in.size(); ++x)
			p1 = std::max(p1, area(in[x], in[y]));

	return p1;
}

template<typename T> bool between(T t, T a, T b)
{
	return (t >= a && t <= b) || (t <= a && t >= b);
}

// after a Stack Overflow point in polygon answer...
bool inside(auto p, auto const& vp)
{
	bool in = false;

	auto i1 = vp.begin();
	auto i2 = i1 + 1;
	for(; i2 != vp.end(); ++i1, ++i2)
	{
		auto A = *i1;
		auto B = *i2;
		if (p == A || p == B)
			return true;
		if (A.second == B.second && p.second == A.second && between(p.first, A.first, B.first))
			return true;
		if (between(p.second, A.second, B.second))
		{
			if (p.second == A.second && B.second >= A.second || p.second == B.second && A.second >= B.second)
				continue;
			auto c = (A.first - p.first) * (B.second - p.second) - (B.first - p.first) * (A.second - p.second);
			if (c == 0)
				return true;
			if ((A.second < B.second) == c > 0)
				in = !in;
		}
	}
	return in;
}

bool all_inside(auto a, auto b, auto const& vp, auto const& xx, auto const& yy)
{
	auto A = std::make_pair(a.first, b.second);
	auto B = std::make_pair(b.first, a.second);
	if (inside(A, vp) && inside(B, vp))
	{
		for (auto y : yy)
		{
			if (between(y, a.second, b.second))
			{
				if(!inside(std::make_pair(a.first, y), vp))
					return false;
				if(!inside(std::make_pair(b.first, y), vp))
					return false;
			}
		}		
		for (auto x : xx)
		{
			if (between(x, a.first, b.first))
			{
				if (!inside(std::make_pair(x, a.second), vp))
					return false;
				if (!inside(std::make_pair(x, b.second), vp))
					return false;
			}
		}
		return true;
	}
	return false;
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	auto area = [](auto a, auto b){ return (std::abs(a.first - b.first) + 1) * (std::abs(a.second - b.second) + 1);};

	std::vector<int64_t> xx;
	std::vector<int64_t> yy;
	for (auto p : in)
	{
		xx.emplace_back(p.first);
		yy.emplace_back(p.second);
	}
	std::ranges::sort(xx);
	std::ranges::sort(yy);
	auto [xe, xl] = std::ranges::unique(xx);
	xx.erase(xe, xl);
	auto[ye, yl] = std::ranges::unique(yy);
	yy.erase(ye, yl);
	int64_t p2 = 0;
	for(auto y = 0; y < in.size() - 1; ++y)
		for(auto x = y + 1; x < in.size(); ++x)
		{
			auto ar = area(in[x], in[y]);
			if(ar > p2 && all_inside(in[x], in[y], in, xx, yy))
				p2 = ar;
		}
	return p2;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}