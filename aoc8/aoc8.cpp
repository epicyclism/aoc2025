#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

struct pt_t
{
	int64_t x_;
	int64_t y_;
	int64_t z_;
};

int64_t d2(pt_t a, pt_t b)
{
	auto dx = a.x_ - b.x_;
	auto dy = a.y_ - b.y_;
	auto dz = a.z_ - b.z_;
	return dx * dx + dy * dy + dz * dz;
}

auto get_input()
{
	std::vector<pt_t> v;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		auto[m, x, y, z] = ctre::match<"(\\d+),(\\d+),(\\d+)">(ln);
		v.emplace_back(x.to_number<int64_t>(), y.to_number<int64_t>(), z.to_number<int64_t>());
	}
	return v;
}

constexpr int64_t kv = 1024;

auto xy_k(auto x, auto y)
{
	return y * kv + x;
}

auto k_xy(auto k)
{
	return std::make_pair(k % kv, k / kv );
}

auto compute_distances(std::vector<pt_t> const& p)
{
	timer t("compute distances");
	const auto st = p.size();
	std::vector<std::pair<int64_t, int64_t>> r;
	r.reserve(st * st);
	for(auto y = 0; y < p.size() - 1; ++y)
		for(auto x = y + 1; x < p.size(); ++x)
		{
			auto d = d2(p[x], p[y]);
			r.emplace_back(d, xy_k(x, y));
		}
	std::ranges::sort(r, std::less(), &std::pair<int64_t, int64_t>::first);
	return r;
}

int64_t pt1(auto const& in, auto const& d)
{
	timer t("p1");
//	auto d = compute_distances(in);
	std::vector<std::unordered_set<int64_t>> ss;
	const auto nd = (in.size() < 1000 ? 10 : 1000);
	for(int n = 0; n < nd; ++n)
	{
		auto[a, b] = k_xy(d[n].second);
		auto ai = ss.end();
		auto bi = ss.end();
		for(auto is = ss.begin(); is != ss.end(); ++is)
		{
			if((*is).contains(a))
				ai = is ;
			if((*is).contains(b))
				bi = is;
		}
		if(ai == ss.end() && bi == ss.end())
		{
			std::unordered_set<int64_t> ns;
			ns.insert(a);
			ns.insert(b);
			ss.emplace_back(std::move(ns));
		}
		else if(ai == ss.end())
		{
			(*bi).insert(a);
		}
		else if(bi == ss.end())
		{
			(*ai).insert(b);
		}
		else
		{
			if(ai != bi)
			{
				(*ai).insert_range(*bi);
				ss.erase(bi);	
			}		
		}
	}
	std::ranges::sort(ss, [](auto& l, auto& r){ return l.size() > r.size();});
	int64_t p1 = 1;
	for(int n = 0; n < 3; ++n)
		p1 *= ss[n].size();
	return p1;
}

int64_t pt2(auto const& in, auto const& d)
{
	timer t("p2");
//	auto d = compute_distances(in);
	std::vector<std::unordered_set<int64_t>> ss;
	int64_t p2 = 0;
	for(auto n = 0; n < d.size(); ++n)
	{
		auto[a, b] = k_xy(d[n].second);
		auto ai = ss.end();
		auto bi = ss.end();
		for(auto is = ss.begin(); is != ss.end(); ++is)
		{
			if((*is).contains(a))
				ai = is ;
			if((*is).contains(b))
				bi = is;
		}
		if(ai == ss.end() && bi == ss.end())
		{
			std::unordered_set<int64_t> ns;
			ns.insert(a);
			ns.insert(b);
			ss.emplace_back(std::move(ns));
		}
		else if(ai == ss.end())
		{
			(*bi).insert(a);
		}
		else if(bi == ss.end())
		{
			(*ai).insert(b);
		}
		else
		{
			if(ai != bi)
			{
				(*ai).insert_range(*bi);
				ss.erase(bi);	
			}		
		}
		if(ss[0].size() == in.size())
		{
			p2 = in[a].x_ * in[b].x_;
			break;
		}
	}
	return p2;
}

int main()
{
	auto in = get_input();
	auto d = compute_distances(in);
	auto p1 = pt1(in, d);
	auto p2 = pt2(in, d);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
