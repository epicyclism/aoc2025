#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

using vertex_t = uint16_t;

constexpr vertex_t encode(std::string_view n)
{
	return n[0] - 'a' | (n[1] - 'a') << 5 | (n[2] - 'a') << 10;
}

constexpr std::string_view decode(vertex_t v)
{
	static char buf[4];
	buf[0] = (v & 0x01f) + 'a';
	buf[1] = ((v >> 5) & 0x01f) + 'a';
	buf[2] = ((v >> 10) & 0x01f) + 'a';
	buf[3] = 0;
	return std::string_view(buf, 3);
}

using visited_t = std::bitset<32768>;
using graph_t = std::vector<std::vector<vertex_t>>;

auto get_input()
{
	graph_t g(encode("zzz"));
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(auto[m, f, t] = ctre::match<"(\\w+): (.+)">(ln); m)
		{
			auto n = encode(f);
			for(auto m : ctre::search_all<"(\\w+)">(t))
				g[n].emplace_back(encode(m));
		}
		else
			fmt::println("parse fail {}", ln);
	}
	return g;
}

void dfs(graph_t const& g, vertex_t f, vertex_t t, int& cnt)
{
	for(auto n: g[f])
	{
		if(n == t)
			++cnt;
		else
			dfs(g, n, t, cnt);
	}
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	int pt1 = 0;
	dfs(in, encode("you"), encode("out"), pt1);
	return pt1;
}

void dfs2(graph_t const& g, vertex_t f, vertex_t t, bool dac, bool fft, visited_t& v, int& cnt)
{
	for(auto n: g[f])
	{
		if(!v[n])
		{
			v.set(n);
			if(n == t)
			{
				if(dac && fft)
				{
					++cnt;
					fmt::println("{}", cnt);
				}
			}
			else
			{
#if 1
				if(n == encode("dac"))
				{
					if(dac)
						return;
					dac = true;
				}
				if(n == encode("fft"))
				{
					if(fft)
						return;
					fft = true;
				}
#endif
				dfs2(g, n, t, dac, fft, v, cnt);
			}
		}
		else
			fmt::println("loop");
		v.reset(n);
	}
	fmt::println("ret from {}", decode(f));
}

int64_t pt2a(auto const& in)
{
	timer t("p2");
	int pt2= 0;
	bool dac = false;
	bool fft = false;
	dfs(in, encode("svr"), encode("out"), pt2);
	return pt2;
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	int pt2= 0;
	bool dac = false;
	bool fft = false;
	visited_t v;
	dfs2(in, encode("svr"), encode("out"), dac, fft, v, pt2);
	return pt2;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
