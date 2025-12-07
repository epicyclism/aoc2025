#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

struct sum_t
{
	std::vector<int64_t> vars_;
	bool prod_ = false;
};
using sums = std::vector<sum_t>;

auto get_input()
{
	std::vector<std::string> r;
	std::string ln;
	while(std::getline(std::cin, ln))
		r.emplace_back(std::move(ln));
	return r;
}

sums proc_pt1(std::vector<std::string> const& ss)
{
	sums sm;
	for(auto v : ctre::search_all<"([+*])">(ss.back()))
	{
		sm.emplace_back();
		sm.back().prod_ = v.view()[0] == '*';
	}
	for(auto const& s: ss)
	{
		auto it = sm.begin();
		for(auto v : ctre::search_all<"(\\d+)">(s))
		{
			(*it).vars_.push_back(v.to_number<int64_t>());
			++it;
		}
	}

	return sm;
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	return std::ranges::fold_left(proc_pt1(in), 0LL, 
		[](auto t, auto& sm)
		{
			if(sm.prod_)
				return t + std::ranges::fold_left(sm.vars_, 1LL, std::multiplies());
			 return t + std::ranges::fold_left(sm.vars_, 0LL, std::plus());
		});
}

sums proc_pt2(std::vector<std::string> const& ss)
{
	sums sm;
	for(auto v : ctre::search_all<"([+*])">(ss.back()))
	{
		sm.emplace_back();
		sm.back().prod_ = v.view()[0] == '*';
	}
	auto its = sm.begin();
	int64_t v = 0;
	for(size_t pt = 0; pt < ss.front().size(); ++pt)
	{
		bool all_space = true;
		v = 0;
		for(size_t sn = 0; sn < ss.size() - 1; ++sn)
		{
			if( ss[sn][pt] != ' ')
			{
				all_space = false;
				v *= 10;
				v += ss[sn][pt] - '0';
			}
		}
		if(v != 0)
			(*its).vars_.push_back(v);
		if(all_space)
			++its;
	}
	return sm;
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	return std::ranges::fold_left(proc_pt2(in), 0LL, 
		[](auto t, auto& sm)
		{
			if(sm.prod_)
				return t + std::ranges::fold_left(sm.vars_, 1LL, std::multiplies());
			 return t + std::ranges::fold_left(sm.vars_, 0LL, std::plus());
		});
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}