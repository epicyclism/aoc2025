#include <vector>
#include <algorithm>
#include <numeric>
#include <ranges>

#include "fmt/format.h"
#include "fmt/ranges.h"

using namespace std;

struct Counters
{
    vector<int16_t> TargetJolts;
    vector<vector<int8_t>> Buttons;
};

void Scale(vector<int64_t>* v, int64_t s)
{
	ranges::for_each(*v, [s](int64_t& i) { i *= s; });
}

vector<vector<int64_t>> CountersToAugmentedMatrix(const Counters& c)
{
	vector<vector<int64_t>> m(c.TargetJolts.size(), vector<int64_t>(c.Buttons.size() + 1));

	// Each button is a column
	for (int64_t column = 0; column < (int64_t)c.Buttons.size(); column++)
	{
		for (int8_t row : c.Buttons[column])
		{
			m[row][column] = 1;
		}
	}

	for (int64_t i = 0; i < (int64_t)c.TargetJolts.size(); i++)
	{
		m[i].back() = c.TargetJolts[i];
	}

	return m;
}

vector<int64_t> CountersToConstraints(const Counters& c)
{
	vector<int64_t> constraints(c.Buttons.size(), numeric_limits<int64_t>::max());
	for (int64_t button = 0; button < (int64_t)c.Buttons.size(); button++)
	{
		for (int8_t counter : c.Buttons[button])
		{
			constraints[button] = min<int64_t>(constraints[button], c.TargetJolts[counter]);
		}
	}
    fmt::println("{}", constraints);
	return constraints;
}

bool IsNonZeroRow(const vector<int64_t>& v)
{
	return ranges::count(v, 0) != (int64_t)v.size();
}

bool IsZeroRow(const vector<int64_t>& v)
{
	return ranges::count(v, 0) == (int64_t)v.size();
}

vector<int64_t> Reduce(vector<int64_t> rowToReduce, vector<int64_t> reducingRow, int64_t reducingColumn)
{
	if (rowToReduce[reducingColumn] == 0)
	{
		// Nothing to do
		return rowToReduce;
	}

	if (rowToReduce[reducingColumn] < 0)
	{
		Scale(&rowToReduce, -1);
	}

	int64_t scaleTo = lcm(rowToReduce[reducingColumn], reducingRow[reducingColumn]);
	Scale(&rowToReduce, scaleTo / rowToReduce[reducingColumn]);
	Scale(&reducingRow, scaleTo / reducingRow[reducingColumn]);

	for (int i = 0; i < (int64_t)rowToReduce.size(); i++)
	{
		rowToReduce[i] -= reducingRow[i];
	}

	return rowToReduce;
}

void ReduceAndTrim(vector<vector<int64_t>>* pm)
{
	vector<vector<int64_t>>& m = *pm;
	int64_t diagonalEnd = min<int64_t>(m.size(), m.front().size() - 1);
	for (int64_t diagonal = 0; diagonal < diagonalEnd; diagonal++)
	{
		// Put empty rows at the bottom
		partition(m.begin() + diagonal, m.end(), IsNonZeroRow);

		// Find a row with a non-zero element in the column
		for (int64_t reducingRow = diagonal; reducingRow < (int64_t)m.size(); reducingRow++)
		{
			if (m[reducingRow][diagonal] != 0)
			{
				swap(m[diagonal], m[reducingRow]);
				break;
			}
		}

		if (m[diagonal][diagonal] < 0)
		{
			Scale(&m[diagonal], -1);
		}

		for (int64_t rowToReduce = diagonal + 1; rowToReduce < (int64_t)m.size(); rowToReduce++)
		{
			m[rowToReduce] = Reduce(m[rowToReduce], m[diagonal], diagonal);
		}
	}

	// Get rid of the empty rows
	m.erase(remove_if(m.begin(), m.end(), IsZeroRow), m.end());
}

void TrySolvedConstrained(const Counters& counters,
	const vector<vector<int64_t>>& m,
	int64_t rowToSolve,
	int64_t nextUnknown,
	const vector<int64_t>& constraints,
	vector<int64_t>* alreadyAssigned,
	int64_t *minimumPresses)
{
	vector<int64_t>& solution = *alreadyAssigned;

	if (nextUnknown == -1)
	{
		vector<int16_t> accumulatedJolts(counters.TargetJolts.size(), 0);
		for (int64_t button = 0; button < (int64_t)counters.Buttons.size(); button++)
		{
			for (int8_t counter : counters.Buttons[button])
			{
				accumulatedJolts[counter] += (int16_t)solution[button];
			}
		}

		if (accumulatedJolts == counters.TargetJolts)
		{
			*minimumPresses = min(*minimumPresses, ranges::fold_left(solution, 0, plus{}));
		}

		return;
	}

	// If the matrix isn't big enough we're going to need to guess
	if (nextUnknown > rowToSolve)
	{
		for (int64_t guess = 0; guess <= constraints[nextUnknown]; guess++)
		{
			solution[nextUnknown] = guess;
			TrySolvedConstrained(counters, m, rowToSolve, nextUnknown - 1, constraints, alreadyAssigned, minimumPresses);
		}
		return;
	}

	if (m[rowToSolve][nextUnknown] == 0)
	{
		// We're not able to solve directly so we need to guess
		for (int64_t guess = 0; guess <= constraints[nextUnknown]; guess++)
		{
			solution[nextUnknown] = guess;
			TrySolvedConstrained(counters, m, rowToSolve - 1, nextUnknown - 1, constraints, alreadyAssigned, minimumPresses);
		}
		return;
	}

	int64_t rowTargetSum = m[rowToSolve].back();

	// Eliminate everything we already know about
	for (int64_t known = nextUnknown + 1; known < (int64_t)solution.size(); known++)
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

	int64_t tentativeSolution = rowTargetSum / m[rowToSolve][nextUnknown];
	if (tentativeSolution < 0)
	{
		// We're only looking for positive solutions
		return;
	}

	solution[nextUnknown] = tentativeSolution;

	TrySolvedConstrained(counters, m, rowToSolve - 1, nextUnknown - 1, constraints, alreadyAssigned, minimumPresses);
}

void SolveConstrained(const Counters& counters, const vector<vector<int64_t>>& m, const vector<int64_t>& constraints, int64_t *minimumPresses)
{
	vector<int64_t> solution(constraints.size(), -1);
	TrySolvedConstrained(counters, m, min(solution.size() - 1, m.size() - 1), solution.size() - 1, constraints, &solution, minimumPresses);
}

int main()
{
	int64_t answer = 0;

		Counters counters = {{3, 5, 4, 7}, {{3}, {1, 3}, {2}, {2, 3}, {0, 2}, {0,1}}};

		auto matrix = CountersToAugmentedMatrix(counters);
		auto constraints = CountersToConstraints(counters);

		int64_t minimumPresses = numeric_limits<int64_t>::max();

		ReduceAndTrim(&matrix);
		SolveConstrained(counters, matrix, constraints, &minimumPresses);

		answer += minimumPresses;

	fmt::println("{}", answer);
}