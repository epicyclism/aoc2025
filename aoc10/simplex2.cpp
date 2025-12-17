#include <fmt/format.h>
#include <fmt/ranges.h>

#include "simplex.h"

void t1()
{
    constraint_t constraints = {{0, 0, 0, 0, 1, 1},
                                {0, 1, 0, 0, 0, 1},
                                {0, 0, 1, 1, 1, 0},
                                {1, 1, 0, 1, 0, 0}};
    target_t    b = { 3, 5, 4, 7 };
    objective_t c = { 1, 1, 1, 1, 1, 1 };

    auto[t, mn] = solve_equal_min(constraints, b, c);

    fmt::println("solution 1 = {}, opt min {}", t, mn);
}

void t2()
{
    constraint_t constraints = {{ 1, 0, 1, 1, 0 },
                                { 0, 0, 0, 1, 1 },
                                { 1, 1, 0, 1, 1 },
                                { 1, 1, 0, 0, 1 },
                                { 1, 0, 1, 0, 1 }};
    target_t b = { 7, 5, 12, 7, 2 };
    objective_t c = { 1, 1, 1, 1, 1 };

    auto [t, mn] = solve_equal_min(constraints, b, c);

    fmt::println("solution 2 = {}, opt min {}", t, mn);
}

void t3()
{
    constraint_t constraints = {{1, 1, 1, 0},
                                {1, 0, 1, 1 },
                                {1, 0, 1, 1},
                                {1, 1, 0, 0},
                                {1, 1, 1, 0},
                                {0, 0, 1, 0}};

    target_t b = { 10, 11, 11, 5, 10, 5 };
    objective_t c = { 1, 1, 1, 1 }; // output

    auto [t, mn] = solve_equal_min(constraints, b, c);

    fmt::println("solution 3 = {}, opt min {}", t, mn);
}

void t4()
{
    constraint_t constraints = { {1, 1, 1, 0, 0},
                                {1, 0, 1, 1, 0 },
                                {1, 0, 1, 1, 0},
                                {1, 1, 0, 0, 0},
                                {1, 1, 1, 0, 0},
                                {0, 0, 1, 0, 0},
                                {0, 0, 0, 0, 0}};

    target_t b = { 10, 11, 11, 5, 10, 5, 0 };
    objective_t c = { 1, 1, 1, 1, 1 }; // output

    auto [t, mn] = solve_equal_min(constraints, b, c);

    fmt::println("solution 4 = {}, opt min {}", t, mn);
}

int main()
{
    t1();
    t2();
    t3();
    t4();
}
