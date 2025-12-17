// derived from something ritten (eventually) by CoPilot.
//

#pragma once

#include <vector>

using constraint_t = std::vector<std::vector<double>>;
using target_t = std::vector<double>;
using objective_t = std::vector<double>;

std::pair<std::vector<double>, double> solve_equal_min(constraint_t const& constraints,
														target_t const& b,
														objective_t const& c);