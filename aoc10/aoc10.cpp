#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <numeric>
#include <ranges>

#include "simplex.h"

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "ctre_inc.h"
#include "timer.h"

constexpr int jolt_sz = 10;
using j_t = std::array<int, jolt_sz>;
using p_t = std::vector<uint32_t>;

struct b_t
{
	uint32_t tgt_;
	p_t press_;
	j_t jolts_;
};

auto to_tgt(std::string_view v)
{
	uint32_t b = 1;
	uint32_t u = 0;
	for(auto c: v)
	{
		if( c == '#')
			u |= b;
		b <<= 1;
	}
	return u;
}

auto to_press(std::string_view v)
{
	p_t r;

	for(auto bs : ctre::search_all<"\\([\\d,]+\\)">(v))
	{
		uint32_t p = 0;
		for(auto b : ctre::search_all<"(\\d+)">(bs))
			p |= 1 << b.to_number<int>();
		r.emplace_back(p);
	}

	return r;
}

auto to_jolts(std::string_view v)
{
	j_t r;
	r.fill(0);
	int n = 0;

	for(auto j : ctre::search_all<"(\\d+)">(v))
	{
		r[n] = j.to_number<int>();
		++n;
	}
	return r;
}

auto get_input()
{
	std::vector<b_t> vb;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		b_t bt;
		if(auto[m, b, p, j] = ctre::match<"\\[([.#]+)\\] ([^\\{}]+) \\{([^\\}]+)\\}">(ln); m)
		{
			bt.tgt_ = to_tgt(b);
			bt.press_ = to_press(p);
			bt.jolts_ = to_jolts(j);
			vb.emplace_back(std::move(bt));
		}
		else
			fmt::println("parse fail {}", ln);
	}
	return vb;
}

int shortest1(b_t const& bt)
{
	std::unordered_set<uint32_t> cache;
	std::queue<std::pair<uint32_t, int>> q;
	q.push({0, 0});
	while(!q.empty())
	{
		auto p = q.front();
		q.pop();
		if(p.first == bt.tgt_)
			return p.second;
		for(auto pr: bt.press_)
		{
			if(!cache.contains(p.first ^ pr))
			{
				q.push({p.first ^ pr, p.second + 1});
				cache.insert(p.first ^ pr);
			}
		}
	}
	//NOT REACHED
	return -1;
}

int pt1(auto const& in)
{
	timer t("p1");
	return std::ranges::fold_left(in, 0, [](auto sm, auto& b){ return sm + shortest1(b);});
}


constraint_t mm(auto press_count)
{
	constraint_t m(10);
	for(int n = 0; n < m.size(); ++n)
		m[n].resize(press_count, 0);
	return m;
}

auto build_coeff(b_t const& bt)
{
	auto out = mm(bt.press_.size());
	size_t col = 0;
	for(auto p: bt.press_)
	{
		uint32_t b = 1;
		for(auto r = 0; r < out.size(); ++r)
		{
			if(p & b)
				out[r][col] = 1;
			b <<= 1;
		}
		++col;
	}
	return out;
}

int shortest2(b_t const& bt)
{
	auto A = build_coeff(bt);
	target_t b = target_t(bt.jolts_.begin(), bt.jolts_.begin() + A.size());
	objective_t c = objective_t(A[0].size(), 1);

	auto [_, m] = solve_equal_min(A, b, c);
	return m;
}

int pt2(auto const& in)
{
	timer t("p2");
	return std::ranges::fold_left(in, 0, [](auto sm, auto& b){ return sm + shortest2(b);});
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}