#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <unordered_set>
#include <set>
#include <queue>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "ctre_inc.h"
#include "timer.h"

using j_t = std::array<uint8_t, 10>;

struct b_t
{
	uint32_t tgt_;
	std::vector<uint32_t> press_;
	j_t jolts_;
};

uint32_t to_tgt(std::string_view v)
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

std::vector<uint32_t> to_press(std::string_view v)
{
	std::vector<uint32_t> r;

	for(auto bs : ctre::search_all<"\\([\\d,]+\\)">(v))
	{
		uint32_t p = 0;
		for(auto b : ctre::search_all<"(\\d+)">(bs))
			p |= 1 << b.to_number<int>();
		r.emplace_back(p);
	}

	return r;
}

j_t to_jolts(std::string_view v)
{
	j_t r;
	r.fill(0);
	int n = 0;

	for(auto j : ctre::search_all<"(\\d+)">(v))
	{
		r[n] = j.to_number<uint8_t>();
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
//			fmt::println("{} {} {}", b.view(), p.view(), j.view());
			bt.tgt_ = to_tgt(b);
			bt.press_ = to_press(p);
			bt.jolts_ = to_jolts(j);
//			fmt::println("{:x} {} {}", bt.tgt_, bt.press_, bt.jolts_);
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

using mat10   = std::array<int, 10>;
using mat1010 = std::array<mat10, 10>;
mat1010 mm()
{
	mat1010 m;
	for(int n = 0; n < 10; ++n)
		m[n].fill(0);
	return m;
}

void dmp(mat1010 const& mm)
{
	for(int r = 0; r < 10; ++r)
		fmt::println("{}", mm[r]);
}

mat10 build_tgt(b_t const& bt)
{
	mat10 out;
	std::ranges::copy(bt.jolts_, std::ranges::begin(out));
	return out;
}

mat1010 build_coeff(b_t const& bt)
{
	mat1010 out = mm();
	int col = 0;
	for(auto p: bt.press_)
	{
		uint32_t b = 1;
		for(int r = 0; r < 10; ++r)
		{
			if(p & b)
				out[r][col] = 1;
			b <<= 1;
		}
		++col;
	}
	return out;
}

void swap(mat10& l, mat10& r)
{
	for(int n = 0; n < 10; ++n)
		std::swap(l[n], r[n]);
}

mat10 times(mat1010 const& mm, mat10 const& m)
{
	mat10 out;
	for(int row = 0; row < 10; ++row)
	{
		out[row] = std::inner_product(mm[row].begin(), mm[row].end(), m.begin(), 0);
	}
	return out;
}

double determinant(mat1010 const& m)
{
	double det = 0.0;
	for (int i = 0; i < 10; i++)
	{
		det += m[0][i] * (m[1][(i + 1) % 10] * m[2][(i + 2) % 10] - m[1][(i + 2) % 10] * m[2][(i + 1) % 10]);
	}
	return det;
}

mat1010 invert(mat1010 m)
{
	fmt::println("det = {}", determinant(m));
	return m;
}

void test(b_t const& bt, mat10 const& a)
{
	auto tgt = build_tgt(bt);
	auto cf = build_coeff(bt);
	dmp(cf);
	auto rf = times(cf, a);
	fmt::println(" * {}", a);
	fmt::println(" = {}", rf);
	fmt::println(" == {}", tgt);
}

int shortest3(b_t const& bt)
{
	auto tgt = build_tgt(bt);
	auto cf = build_coeff(bt);
	auto icf = invert(cf);
	auto bp = times(icf, tgt);
//	return std::ranges::fold_left(bp, 0);
	return 0;
}

int pt2(auto const& in)
{
	timer t("p2");
	return std::ranges::fold_left(in, 0, [](auto sm, auto& b){ return sm + shortest3(b);});
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	test(in[0], {1, 3, 0, 3, 1, 2, 0, 0, 0, 0});
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
