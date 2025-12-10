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

// scraped from Matthew's solution. 1/5 the execution time...
//
int64_t cross_product(const auto& a, const auto& b, const auto& c) {
    return (b.first - a.first) * (c.second - a.second) - (b.second - a.second) * (c.first - a.first);
}

int64_t dist_sq(const auto& a, const auto& b) {
    return (a.first - b.first)*(a.first - b.first) + (a.second - b.second)*(a.second - b.second);
}

// Grahams scan algorithm to find convex hull
auto convex_hull(std::vector<std::pair<int64_t, int64_t>>& points) {

    if(points.size() < 3) {
        return points;
    }

    // Move bottom most point to the front
    auto it = std::min_element(points.begin(), points.end(), [](const auto& a, const auto& b) {
        return (a.second < b.second) || (a.second == b.second && a.first < b.first);
    });
    std::iter_swap(points.begin(), it);
    auto p0 = points[0];

    // Sort the rest of the points based on polar angle relative to p0
    std::sort(points.begin() + 1, points.end(), [&](const auto& a, const auto& b) {
        int64_t cp = cross_product(p0, a, b);
        if (cp == 0) {
            // If collinear, put the one CLOSER to p0 first
            return dist_sq(p0, a) < dist_sq(p0, b); 
        }
        return cp > 0;
    });

    auto stack = std::vector<std::pair<int64_t, int64_t>>{p0, points[1]};

    for(const auto& p : points | std::views::drop(1)) {
        while (stack.size() > 1){
            auto top = stack.back();
            auto next_to_top = stack[stack.size() - 2];

            if(cross_product(next_to_top, top, p) <= 0) {
                stack.pop_back();
            } else {
                break;
            }
        }
        stack.push_back(p);
    }
    return stack;
}

int64_t pt1(auto in)
{
	timer t("p1");
	auto area = [](auto a, auto b){ return (std::abs(a.first - b.first) + 1) * (std::abs(a.second - b.second) + 1);};
	int64_t p1 = 0;
	auto pp = convex_hull(in);
	for (auto y = 0; y < pp.size() - 1; ++y)
		for (auto x = y + 1; x < pp.size(); ++x)
			p1 = std::max(p1, area(pp[x], pp[y]));

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