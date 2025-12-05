#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

using interval = std::pair<int64_t, int64_t>;

bool contains(interval const& i, int64_t p)
{
	return p >= i.first && p <= i.second;
}

int64_t size(interval const& i)
{
	return i.second - i.first + 1;
}

bool intersect(interval const& l, interval const& r)
{
	return contains(l, r.first);
}

interval intersection(interval const& l, interval const& r)
{
	return {std::min(l.first, r.first), std::max(l.second, r.second)};
}

std::vector<std::pair<int64_t, int64_t>> condense(std::vector<std::pair<int64_t, int64_t>>& v)
{
	std::vector<std::pair<int64_t, int64_t>> rv;
	interval tmp = v.front();
	auto i = v.begin();
	++i;
	for(auto i = std::ranges::begin(v) + 1; i != std::ranges::end(v); ++i)
	{
		if(intersect(tmp, *i))
			tmp = intersection(tmp, *i);
		else
		{
			rv.emplace_back(tmp);
			tmp = *i;
		}
	}
	rv.emplace_back(tmp);
	return rv;
}

auto get_input()
{
	std::vector<interval> fresh;
	std::vector<int64_t> ing;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(ln.empty())
			break;
		auto[m,f,t] = ctre::match<"(\\d+)-(\\d+)">(ln);
			fresh.emplace_back(f.to_number<int64_t>(), t.to_number<int64_t>());
	}
	while(std::getline(std::cin, ln))
		ing.emplace_back(sv_to_t<int64_t>(ln));
	std::ranges::sort(fresh, std::ranges::less(), &std::pair<int64_t, int64_t>::first);
	return std::make_pair(condense(fresh), ing);
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	int p1 = 0;
	for(auto i: in.second)
		for(auto& r: in.first)
		{
			if(contains(r, i))
			{
				++p1;
				break;
			}
		}
	return p1;
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	return std::ranges::fold_left(in.first, 0LL, [](auto s, auto const&i ){ return s + size(i);});
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
