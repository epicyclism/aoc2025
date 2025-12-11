#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
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

using graph_t = std::vector<std::vector<vertex_t>>;
using cache_t = std::unordered_map<vertex_t, int64_t>;

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

int64_t dfs(graph_t const& g, vertex_t f, vertex_t t)
{
	if (f == t)
		return 1;
	return std::ranges::fold_left(g[f], 0LL, [&](auto s, auto v) { return s + dfs(g, v, t); });
}

int64_t dfs(graph_t const& g, vertex_t f, vertex_t t, cache_t& cache)
{
	if (f == t)
		return 1;
	if (cache.contains(f))
		return cache[f];
	auto sm = std::ranges::fold_left(g[f], 0LL, [&](auto s, auto v) { return s + dfs(g, v, t, cache); });
	cache[f] = sm;
	return sm;
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	return dfs(in, encode("you"), encode("out"));
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	cache_t cache;
	// cache has to be cleared since it stores distance to the end,
	// and the end is changing...
	auto a = dfs(in, encode("svr"), encode("fft"), cache);
	cache.clear();
	auto b = dfs(in, encode("fft"), encode("dac"), cache);
	cache.clear();
//	auto c = dfs(in, encode("dac"), encode("fft"), cache);
//	cache.clear();
	auto d = dfs(in, encode("dac"), encode("out"), cache);
//	if (c != 0) // not in either data set I've seen
//		fmt::println("tilt - dac-fft != 0");
	return a * b * d;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
