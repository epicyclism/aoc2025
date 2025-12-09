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
	for(auto y = 0; y < in.size() - 1; ++y)
		for(auto x = y + 1; x < in.size(); ++x)
		{
			auto ar = area(in[x], in[y]);
//			fmt::println("{} {} - {} {} = {}", in[x].first, in[x].second, in[y].first, in[y].second, ar);
			p1 = ar > p1 ? ar : p1;
		}
	return p1;
}
/*
def point_in_polygon(point, polygon):
x, y = point
num_vertices = len(polygon)
inside = False

p1x, p1y = polygon[0]
for i in range(num_vertices + 1):
	p2x, p2y = polygon[i % num_vertices]
	if y > min(p1y, p2y):
		if y <= max(p1y, p2y):
			if x <= max(p1x, p2x):
				if p1y != p2y:
					x_intersection = (y - p1y) * (p2x - p1x) / (p2y - p1y) + p1x
				if p1x == p2x or x <= x_intersection:
					inside = not inside
	p1x, p1y = p2x, p2y

return inside
*/
bool inside(auto p, auto const& vp)
{
	bool in = false;
	auto i1 = vp.begin();
	auto i2 = i1 + 1;
	while(i2 != vp.end())
	{
		if(p.second > std::min((*i1).second, (*i2).second))
			if(p.second <= std::max((*i1).second, (*i2).second))
				if(p.first <= std::max((*i1).first, (*i2).first))
				{
					double xi = -1;
					if((*i1).second != (*i2).second)
						xi = double(p.second - (*i1).second) * ((*i2).first - (*i1).first) / ((*i2).second - (*i1).second) + (*i1).first;
					if((*i1).first == (*i2).first || p.first <= xi)
						in = !in;
				}
		++i1;
		++i2;
	}

	return in;
}

bool all_inside(auto a, auto b, auto const& vp)
{
	auto ul = a;
	auto br = b;
	if(a.first > b.first)
	{
		ul.first = b.first;
		br.first = a.first;
	}
	if(a.second > b.second)
	{
		ul.second = b.second;
		br.second = a.second;
	}
	--br.first;
	--br.second;
	
	auto ra = inside(std::make_pair(br.first, ul.second), vp);
	auto rb = 
			inside(std::make_pair(ul.first, br.second), vp);
//	fmt::println("{},{} - {},{} {},{} - {},{}: {} {}", a.first, a.second, b.first, b.second, b.first, a.second, a.first, b.second, ra, rb);
	return ra && rb;
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	auto area = [](auto a, auto b){ return (std::abs(a.first - b.first) + 1) * (std::abs(a.second - b.second) + 1);};
	int64_t p2 = 0;
	for(auto y = 0; y < in.size() - 1; ++y)
		for(auto x = y + 1; x < in.size(); ++x)
		{
			auto ar = area(in[x], in[y]);
			if(ar > p2 && all_inside(in[x], in[y], in))
				p2 = ar;
		}
	return p2;
}
// 4620005060 too high
//

void test(auto const& in)
{
	fmt::println("2, 3 {}", inside(std::make_pair(2, 3), in));
	fmt::println("9, 5 {}", inside(std::make_pair(9, 5), in));
	fmt::println("9, 3 {}", inside(std::make_pair(9, 3), in));
	fmt::println("2, 5 {}", inside(std::make_pair(2, 5), in));
}
int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	test(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
