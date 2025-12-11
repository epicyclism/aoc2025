#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#if defined (USE_Z3)
#include <z3++.h>
#endif

#include "ctre_inc.h"
#include "timer.h"

using j_t = std::array<int, 10>;

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

constexpr int matsz = 16;
using mat1  = std::array<int, matsz>;
using mat11 = std::array<mat1, matsz>;

mat11 mm()
{
	mat11 m;
	for(int n = 0; n < matsz; ++n)
		m[n].fill(0);
	return m;
}

mat1 build_tgt(b_t const& bt)
{
	mat1 out;
    out.fill(0);
	std::ranges::copy(bt.jolts_, std::ranges::begin(out));
	return out;
}

mat11 build_coeff(b_t const& bt)
{
	mat11 out = mm();
	int col = 0;
	for(auto p: bt.press_)
	{
		uint32_t b = 1;
		for(int r = 0; r < matsz; ++r)
		{
			if(p & b)
				out[r][col] = 1;
			b <<= 1;
		}
		++col;
	}
	return out;
}

#if defined (USE_Z3)
int shortest3(b_t const& bt)
{
	auto tgt = build_tgt(bt);
	auto cf = build_coeff(bt);
	z3::context c;
    z3::expr t[] = {c.int_const("a"), c.int_const("b"), c.int_const("c"), c.int_const("d"),
					c.int_const("e"), c.int_const("f"), c.int_const("g"), c.int_const("h"),
					c.int_const("i"), c.int_const("j"), c.int_const("k"), c.int_const("l"),
				    c.int_const("m"), c.int_const("n"),c.int_const("o"),c.int_const("p")};
    z3::optimize o(c);
    for(int r = 0; r < 16; ++r)
    {
        o.add(c.int_val(cf[r][0]) * t[0] + c.int_val(cf[r][1]) * t[1] + c.int_val(cf[r][2]) * t[2] 
            + c.int_val(cf[r][3]) * t[3] + c.int_val(cf[r][4]) * t[4] + c.int_val(cf[r][5]) * t[5] 
            + c.int_val(cf[r][6]) * t[6] + c.int_val(cf[r][7]) * t[7] + c.int_val(cf[r][8]) * t[8]
            + c.int_val(cf[r][9]) * t[9] + c.int_val(cf[r][10]) * t[10] + c.int_val(cf[r][11]) * t[11]
            + c.int_val(cf[r][12]) * t[12] + c.int_val(cf[r][13]) * t[13] + c.int_val(cf[r][14]) * t[14]
            + c.int_val(cf[r][15]) * t[15] == c.int_val(tgt[r]));
    }
    for(int n = 0; n < 16; ++n)
        o.add(t[n] >= 0);
    o.minimize(t[0] + t[1] + t[2] + t[3] + t[4] + t[5] + t[6] + t[7] + t[8]+ t[9] + t[10] + t[11] + t[12] + t[13] + t[14] + t[15]);
    if(o.check() != z3::sat)
    {
        fmt::println("unsat...");
        return 0;
    }
    return o.get_model().eval(t[0] + t[1] + t[2] + t[3] + t[4] + t[5] + t[6] + t[7] + t[8]
								+ t[9] + t[10] + t[11] + t[12] + t[13]+ t[14] + t[15]).as_int64();
}

int pt2(auto const& in)
{
	timer t("p2");
	return std::ranges::fold_left(in, 0, [](auto sm, auto& b){ return sm + shortest3(b);});
}
#else
int pt2(auto const& in)
{
	fmt::println("Z3 needed for pt2, for now");
	return 0;
}
#endif

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}