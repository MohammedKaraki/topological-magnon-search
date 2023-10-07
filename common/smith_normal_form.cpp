#include "common/smith_normal_form.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <tuple>

namespace magnon::common {

namespace {

using Matrix = MatrixXl;

// Returns the tuple {gcd, sigma, tau}, where gcd is the greatest common divisor of
// x and y, and sigma and tau satisfy sigma * x + tau * y == gcd.
//
// This is an implementation of the algorithm described in the following wiki page.
// https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
//
std::tuple<long, long, long> compute_bezout_coefficients(const long x, const long y) {
    long s = 0, prev_s = 1;
    long r = y, prev_r = x;

    while (r != 0) {
        const long quotient = prev_r / r;
        std::tie(prev_r, r) = std::tuple{r, prev_r - quotient * r};
        std::tie(prev_s, s) = std::tuple{s, prev_s - quotient * s};
    }

    long tau = y == 0 ? 0 : (prev_r - prev_s * x) / y;
    return {prev_r, prev_s, tau};
}

void fix_nth_row_and_column(auto &s, auto &a, auto &t, const auto n) {
    const auto fix_column_entry = [n](auto &&s, auto &&a, auto entry_idx) {
        assert(entry_idx > n);
        const auto pivot = a(n, n);
        const auto entry = a(entry_idx, n);
        if (entry == 0) {
            return;
        }
        assert(pivot != 0);

        const auto [beta, sigma, tau] = compute_bezout_coefficients(pivot, entry);
        const auto pivot_prime = pivot / beta;
        const auto entry_prime = entry / beta;

        const auto num_rows = a.rows();
        Matrix op = Matrix::Identity(num_rows, num_rows);
        op(n, n) = sigma;
        op(n, entry_idx) = tau;
        op(entry_idx, n) = -entry_prime;
        op(entry_idx, entry_idx) = pivot_prime;
        s = op * s;
        a = op * a;
    };

    // Returns true if all values below pivot are zero.
    const auto is_column_done = [n](const auto &a) {
        if (a.rows() <= n + 1) {
            return true;
        }
        return a.col(n).tail(a.rows() - n - 1).isZero();
    };

    while (!is_column_done(a) || !is_column_done(a.transpose())) {
        for (int i = n + 1; i < a.cols(); ++i) {
            fix_column_entry(t.transpose(), a.transpose(), i);
        }
        for (int i = n + 1; i < a.rows(); ++i) {
            fix_column_entry(s, a, i);
        }
    }
}

}  // namespace

// This implementation is based on the description of the algorithm in the following wiki page.
// https://en.wikipedia.org/wiki/Smith_normal_form
//
std::tuple<Matrix, Matrix, Matrix> compute_smith_normal_form(const Matrix &matrix) {
    const auto num_rows = matrix.rows();
    const auto num_columns = matrix.cols();

    Matrix s = Matrix::Identity(num_rows, num_rows);
    Matrix a = matrix;
    Matrix t = Matrix::Identity(num_columns, num_columns);

    const auto is_correct_diagnoal_entry_pair_at = [&a](const int i) {
        const auto first = a(i, i);
        const auto second = a(i + 1, i + 1);
        return std::gcd(first, second) == first;
    };

    const auto min_dimension = std::min(a.rows(), a.cols());

    for (auto n = 0; n < min_dimension; ++n) {
        if (a(n, n) == 0) {
            for (int m = n + 1; m < min_dimension; ++m) {
                if (a(m, n) != 0) {
                    s.row(n).swap(s.row(m));
                    a.row(n).swap(a.row(m));
                    break;
                }
                if (a(n, m) != 0) {
                    a.col(n).swap(a.col(m));
                    t.col(n).swap(t.col(m));
                    break;
                }
            }
        }
        fix_nth_row_and_column(s, a, t, n);
    }

    const auto fix_diagonal_entry_pair_at = [&s, &a, &t, num_rows, num_columns](const int n) {
        Matrix row_op = Matrix::Identity(num_rows, num_rows);
        Matrix col_op = Matrix::Identity(num_columns, num_columns);
        row_op(n + 1, n) = 1;
        if (a(n, n) < 0) {
            row_op(n, n) *= -1;
            row_op(n + 1, n) *= -1;
        }
        if (a(n + 1, n + 1) < 0) {
            row_op(n + 1, n + 1) *= -1;
        }
        s = row_op * s;
        a = row_op * a * col_op;
        t = t * col_op;
        fix_nth_row_and_column(s, a, t, n);
    };

    bool done{false};
    while (!done) {
        done = true;
        for (auto n = 0; n < min_dimension; ++n) {
            if (a(n, n) < 0) {
                a.col(n) *= -1;
                t.col(n) *= -1;
            }
            if (n + 1 < min_dimension && !is_correct_diagnoal_entry_pair_at(n)) {
                done = false;
                fix_diagonal_entry_pair_at(n);
            }
        }
    }
    assert((s * matrix * t - a).isZero());
    return std::tuple{s, a, t};
}

}  // namespace magnon::common
