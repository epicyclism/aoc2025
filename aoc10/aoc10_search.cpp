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

constexpr int dim = 16;
using j_t = std::vector<int>;

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
	for(auto j : ctre::search_all<"(\\d+)">(v))
		r.emplace_back(j.to_number<int>());
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

using mat1 = std::array<int, dim>;
using mat2 = std::array<mat1, dim>;

mat2 mm()
{
	mat2 m;
	for(int n = 0; n < dim; ++n)
		m[n].fill(0);
	return m;
}

void dmp(mat2 const& mm)
{
	for(int r = 0; r < dim; ++r)
		fmt::println("{}", mm[r]);
}

mat1 build_tgt(b_t const& bt)
{
	mat1 out;
	out.fill(0);
	std::ranges::copy(bt.jolts_, std::ranges::begin(out));
	return out;
}

mat2 build_coeff(b_t const& bt)
{
	mat2 out = mm();
	int col = 0;
	for(auto p: bt.press_)
	{
		uint32_t b = 1;
		for(int r = 0; r < dim; ++r)
		{
			if(p & b)
				out[r][col] = 1;
			b <<= 1;
		}
		++col;
	}
	return out;
}

mat1 build_constraints(b_t const& bt)
{
	mat1 m;
	m.fill(std::numeric_limits<int>::max());

	for(auto p = 0; p < bt.press_.size(); ++p)
	{
		uint32_t b = 1;
		for(auto s = 0; s < 8; ++s )
		{
			if( b & bt.press_[p])
				m[p] = std::min(m[p], bt.jolts_[s]);
			b <<= 1;
		}
	}
	return m;
}

bool non_zero(auto const& v)
{
	return std::ranges::none_of(v, [](auto x){ return x == 0;});
}

bool all_zero(auto const& v)
{
	return std::ranges::all_of(v, [](auto x){ return x == 0;});
}

void scale(auto& v, int s)
{
	std::ranges::for_each(v, [s](int64_t& i) { i *= s; });
}

struct Counters
{
    std::vector<int16_t> TargetJolts;
    std::vector<std::vector<int8_t>> Buttons;
};

Counters make_counters(b_t const& b)
{
	Counters c;
	c.TargetJolts.resize(b.jolts_.size());
	std::ranges::copy(b.jolts_, c.TargetJolts.begin());
	c.Buttons.resize(b.press_.size());
	auto i = c.Buttons.begin();
	for(auto& p: b.press_)
	{
		unsigned u = 1;
		for(int8_t n = 0; n < 8; ++n)
		{
			if(p & u)
				(*i).emplace_back(n);
			u <<= 1;
		}
		++i;
	}
	return c;
}

void Scale(auto& v, auto s)
{
	std::ranges::for_each(v, [s](auto& i) { i *= s; });
}

auto CountersToAugmentedMatrix(const Counters& c)
{
	std::vector<std::vector<int>> m(c.TargetJolts.size(), std::vector<int>(c.Buttons.size() + 1));

	// Each button is a column
	for (auto column = 0; column < c.Buttons.size(); column++)
	{
		for (int8_t row : c.Buttons[column])
		{
			m[row][column] = 1;
		}
	}

	for (auto i = 0; i < c.TargetJolts.size(); i++)
	{
		m[i].back() = c.TargetJolts[i];
	}

	return m;
}

auto CountersToConstraints(const Counters& c)
{
	std::vector<int> constraints(c.Buttons.size(), std::numeric_limits<int>::max());
	for (auto button = 0; button < c.Buttons.size(); button++)
	{
		for (int8_t counter : c.Buttons[button])
		{
			constraints[button] = std::min<int>(constraints[button], c.TargetJolts[counter]);
		}
	}
//    fmt::println("{}", constraints);
	return constraints;
}

bool IsNonZeroRow(std::vector<int> const& v)
{
	return std::ranges::count(v, 0) != v.size();
}

bool IsZeroRow(std::vector<int> const& v)
{
	return std::ranges::count(v, 0) == v.size();
}

auto Reduce(std::vector<int> rowToReduce, std::vector<int> reducingRow, int reducingColumn)
{
	if (rowToReduce[reducingColumn] == 0)
	{
		// Nothing to do
		return rowToReduce;
	}

	if (rowToReduce[reducingColumn] < 0)
	{
		Scale(rowToReduce, -1);
	}

	auto scaleTo = std::lcm(rowToReduce[reducingColumn], reducingRow[reducingColumn]);
	Scale(rowToReduce, scaleTo / rowToReduce[reducingColumn]);
	Scale(reducingRow, scaleTo / reducingRow[reducingColumn]);

	for (auto i = 0; i < rowToReduce.size(); i++)
	{
		rowToReduce[i] -= reducingRow[i];
	}

	return rowToReduce;
}

void ReduceAndTrim(std::vector<std::vector<int>>& m)
{
	auto diagonalEnd = std::min(m.size(), m.front().size() - 1);
	for (auto diagonal = 0; diagonal < diagonalEnd; diagonal++)
	{
		// Put empty rows at the bottom
		std::partition(m.begin() + diagonal, m.end(), IsNonZeroRow);

		// Find a row with a non-zero element in the column
		for (auto reducingRow = diagonal; reducingRow < m.size(); reducingRow++)
		{
			if (m[reducingRow][diagonal] != 0)
			{
				std::swap(m[diagonal], m[reducingRow]);
				break;
			}
		}

		if (m[diagonal][diagonal] < 0)
		{
			Scale(m[diagonal], -1);
		}

		for (auto rowToReduce = diagonal + 1; rowToReduce < m.size(); rowToReduce++)
		{
			m[rowToReduce] = Reduce(m[rowToReduce], m[diagonal], diagonal);
		}
	}

	// Get rid of the empty rows
	m.erase(std::remove_if(m.begin(), m.end(), IsZeroRow), m.end());
}

void TrySolvedConstrained(const Counters& counters,
	const std::vector<std::vector<int>>& m,
	int rowToSolve,
	int nextUnknown,
	const std::vector<int>& constraints,
	std::vector<int>& solution,
	int& minimumPresses)
{
	if (nextUnknown == -1)
	{
		std::vector<int16_t> accumulatedJolts(counters.TargetJolts.size(), 0);
		for (auto button = 0; button < counters.Buttons.size(); button++)
		{
			for (int8_t counter : counters.Buttons[button])
			{
				accumulatedJolts[counter] += (int16_t)solution[button];
			}
		}

		if (accumulatedJolts == counters.TargetJolts)
		{
			minimumPresses = std::min(minimumPresses, std::ranges::fold_left(solution, 0, std::plus{}));
		}

		return;
	}

	// If the matrix isn't big enough we're going to need to guess
	if (nextUnknown > rowToSolve)
	{
		for (auto guess = 0; guess <= constraints[nextUnknown]; guess++)
		{
			solution[nextUnknown] = guess;
			TrySolvedConstrained(counters, m, rowToSolve, nextUnknown - 1, constraints, solution, minimumPresses);
		}
		return;
	}

	if (m[rowToSolve][nextUnknown] == 0)
	{
		// We're not able to solve directly so we need to guess
		for (auto guess = 0; guess <= constraints[nextUnknown]; guess++)
		{
			solution[nextUnknown] = guess;
			TrySolvedConstrained(counters, m, rowToSolve - 1, nextUnknown - 1, constraints, solution, minimumPresses);
		}
		return;
	}

	auto rowTargetSum = m[rowToSolve].back();

	// Eliminate everything we already know about
	for (auto known = nextUnknown + 1; known < solution.size(); known++)
	{
		rowTargetSum -= m[rowToSolve][known] * solution[known];
		//m[rowToSolve][known] = 0; // For debugging sanity
	}

	// Do we have a valid integer solution?
	if ((rowTargetSum % m[rowToSolve][nextUnknown]) != 0)
	{
		// We don't have a valid integer solution, probably an incorrect guess from earlier, so we should bail out
		return;
	}

	auto tentativeSolution = rowTargetSum / m[rowToSolve][nextUnknown];
	if (tentativeSolution < 0)
	{
		// We're only looking for positive solutions
		return;
	}

	solution[nextUnknown] = tentativeSolution;

	TrySolvedConstrained(counters, m, rowToSolve - 1, nextUnknown - 1, constraints, solution, minimumPresses);
}

void SolveConstrained(const Counters& counters, const std::vector<std::vector<int>>& m, const std::vector<int>& constraints, int& minimumPresses)
{
	std::vector<int> solution(constraints.size(), -1);
	TrySolvedConstrained(counters, m, std::min(solution.size() - 1, m.size() - 1), solution.size() - 1, constraints, solution, minimumPresses);
}

void test(b_t const& bt, mat1 const& a)
{
	auto c = build_constraints(bt);
	auto tgt = build_tgt(bt);
	auto cf = build_coeff(bt);
	//dmp(cf);
	fmt::println("constraints = {}", c);
	fmt::println(" * {}", a);
	fmt::println(" == {}", tgt);
}

int shortest3(b_t const& bt)
{
	auto counters = make_counters(bt);
	auto c = CountersToConstraints(counters);
	auto cf = CountersToAugmentedMatrix(counters);
	int minimumPresses = std::numeric_limits<int>::max();
	ReduceAndTrim(cf);
	SolveConstrained(counters, cf, c, minimumPresses);
	return minimumPresses;
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
//	test(in[0], {1, 3, 0, 3, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
