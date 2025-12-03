#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "timer.h"

auto get_input()
{
	std::vector<std::string> v;
	std::string ln;
	while(std::getline(std::cin, ln))
		v.emplace_back(std::move(ln));
	return v;
}

constexpr std::array magnitudes = { 1LL, 10LL, 100LL, 1000LL, 10000LL, 100000LL, 1000000LL, 10000000LL, 100000000LL,
									 1000000000LL, 10000000000LL, 100000000000LL, 1000000000000LL};
constexpr int64_t n_digits = 12;

// nth digit starting at offset start
//
template<int64_t ND> std::pair<int64_t, int64_t> jolt_wkr(std::string_view s, size_t start, int nth)
{
	const auto in = ND - nth - 1;
	auto me = std::ranges::max_element(s.substr(start, s.length() - in - start)) ;
	return std::make_pair((*me - '0') * magnitudes[in], std::ranges::distance(std::ranges::begin(s), me + 1));
}

int64_t jolt(std::string_view s)
{
	int64_t start = 0;
	int64_t res = 0;
	for(size_t nth = 0; nth < 2; ++nth)
	{
		auto[val, st] = jolt_wkr<2>(s, start, nth);
		res += val;
		start = st;
	}
	return res;
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	return std::ranges::fold_left(in, 0, [](auto s, auto& v){ return s + jolt(v);});
}

int64_t jolt2(std::string_view s)
{
	int64_t start = 0;
	int64_t res = 0;
	for(size_t nth = 0; nth < n_digits; ++nth)
	{
		auto[val, st] = jolt_wkr<12>(s, start, nth);
		res += val;
		start = st;
	}
	return res;
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	return std::ranges::fold_left(in, 0, [](auto s, auto& v){ return s + jolt2(v);});
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
