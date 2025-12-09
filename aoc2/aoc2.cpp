#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <charconv>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

auto parse_range(std::string_view s)
{
	auto p = s.find('-');
	auto f = sv_to_t<int64_t>(std::string_view(s.data(), p));
	auto t = sv_to_t<int64_t>(std::string_view(s.data() + p + 1, s.size() - p));
	return std::make_pair(f, t);
}

auto get_input()
{
	std::vector<std::pair<int64_t, int64_t>> rv;
	std::string ln;
	std::getline(std::cin, ln);
	for(auto const v : std::views::split(ln, ','))
		rv.emplace_back(parse_range(std::string_view(v)));
	return rv;
}

bool invalid1(std::string_view sv)
{
	if(sv.size() & 1)
		return false;
	return sv.substr(0, sv.size() / 2) == sv.substr(sv.size() / 2);
}

inline bool my_test(std::string_view sv, int n)
{
	auto component = sv.substr(0, n);	
	for(int o = n; o < sv.size(); o += n)
		if(sv.substr(o, n) != component)
			return false;
	return true;
}

bool invalid2(std::string_view sv)
{
	for(int n = 1; n <= sv.size() / 2; ++n)
		if(sv.length() % n == 0 && my_test(sv, n))
			return true;
	return false;
}

auto pt12(auto const& in)
{
	timer t("p12");
	int64_t p1 = 0;
	int64_t p2 = 0;
	const size_t sz = 32;
	char buf[sz];
	for(auto[f, t] : in)
	{
		for(; f <= t; ++f)
		{
			auto cr = std::to_chars(buf, buf + sz, f);
			std::string_view sv(buf, cr.ptr - buf);
			if(invalid1(sv))
				p1 += f;
			if(invalid2(sv))
				p2 += f;
		}
	}
	return std::make_pair(p1, p2);
}

constexpr std::array magnitudes = {1LL, 10LL, 100LL, 1000LL, 10000LL, 100000LL, 1000000LL, 10000000LL, 100000000LL, 1000000000LL, 10000000000LL};
constexpr std::array factors1   = { 1LL, 11LL, 101LL, 1001LL, 10001LL, 100001LL, 1000001LL, 10000001LL};
constexpr std::array factors2   = { 1LL, 101LL, 10101LL, 1010101LL, 101010101LL, 10101010101LL,
										1001LL, 1001001001LL,
									10001LL, 100010001LL };

int mag(int64_t v)
{
	return std::ranges::distance(std::ranges::begin(magnitudes), std::ranges::lower_bound(magnitudes, v));
}

auto pt12_2(auto const& in)
{
	timer t("p12_2");
	int64_t p1 = 0;
	int64_t p2 = 0;

	for(auto[f, t] : in)
	{
		for(; f <= t; ++f)
		{
			auto m = mag(f);
			if((m & 1) == 0)
			{
				if(f%factors1[m/2] == 0)
					p1 += f;
			}
		}
	}
	return std::make_pair(p1, p2);
}

int main()
{
	auto in = get_input();
	auto[p1, p2] = pt12(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
