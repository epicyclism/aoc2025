#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

using shape_t = std::array<char, 9>;
struct tgt_t
{
	int x_;
	int y_;
	std::array<int, 6> qts_;
};

auto get_input()
{
	std::vector<shape_t> shapes;
	std::vector<tgt_t> tgts;
	std::string ln;
	int off = 0;
	while(std::getline(std::cin, ln))
	{
		if(ln.empty())
			continue;
		if(auto[m, x, y, s] = ctre::match<"(\\d+)x(\\d+): ([\\d ]+)">(ln); m)
		{
			tgts.emplace_back(x.to_number<int>(), y.to_number<int>());
			int n = 0;
			for(auto c: ctre::search_all<"(\\d+)">(s))
			{
				tgts.back().qts_[n] = c.to_number<int>();
				++n;
			}
		}
		else
		if(auto[m] = ctre::match<"\\d:">(ln); m)
		{
			shapes.emplace_back();
			off = 0;
		}
		else
		{
			std::copy(ln.begin(), ln.end(), shapes.back().begin() + off);
			off += ln.size();
		}
	}
	return std::make_pair(shapes, tgts);
}

// all the areas are 7.
// this puzzle reduces to whether the target area is greater/equal the sum of
// present counts times 7.
//
#define FAST

int pt1(auto const& in)
{
	timer t("p1");
#if !defined (FAST)
	// check areas
	std::array<int, 6> a;
	// these are all 7...
	std::ranges::transform(in.first, std::ranges::begin(a), [](auto& sh){ return std::ranges::count(sh, '#');});
//	a.fill(7);
	// could the parcels fit? If an arrangement were possible.
	auto test0 = [&](auto& tgt)
	{
		auto ar = tgt.x_ * tgt.y_;
		auto tar = 0;
		for(int n = 0; n < a.size(); ++n)
			tar += a[n] * tgt.qts_[n];
		return tar <= ar;
	};
	// can the parcels be laid out adjacent without any interlocking?
	auto test = [&](auto& tgt)
	{
		auto ar = (tgt.x_ / 3)  * (tgt.y_ / 3);
		return test0(tgt) && ar >= std::ranges::fold_left(tgt.qts_, 0, std::plus());
	};
	// it seems that if test0 then also test, so no need to ever dive deeper.
#else
	auto test = [&](auto& tgt)
	{
		auto ar = (tgt.x_ / 3)  * (tgt.y_ / 3);
		return ar >= std::ranges::fold_left(tgt.qts_, 0, std::plus());
	};
#endif
	return std::ranges::count_if(in.second, test);
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	fmt::println("pt1 = {}", p1);
}