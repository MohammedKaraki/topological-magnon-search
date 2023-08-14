#ifndef SISUMMARY_HPP
#define SISUMMARY_HPP

#include <iostream>
#include <map>

#include "spectrum_data.hpp"

namespace TopoMagnon {

struct IntMatrixComp {
    bool operator()(const IntMatrix &a, const IntMatrix &b) const {
        return std::lexicographical_compare(
            a.data(), a.data() + a.size(), b.data(), b.data() + b.size());
    }
};

class SiSummary {
 private:
    int gapless_count, trivialorgapless_count;
    std::map<IntMatrix, int, IntMatrixComp> si_to_count;

    void increment_trivialorgapless() { ++trivialorgapless_count; }

 public:
    SiSummary() : gapless_count{0}, trivialorgapless_count{0} {}

    int get_trivialorgapless_count() const { return trivialorgapless_count; }

    void increment_si(const IntMatrix &si) {
        ++si_to_count[si];

        if (si.isZero()) {
            increment_trivialorgapless();
        }
    }

    void increment_gapless() {
        ++gapless_count;
        increment_trivialorgapless();
    }

    SiSummary &operator+=(const SiSummary &rhs) {
        gapless_count += rhs.gapless_count;
        trivialorgapless_count += rhs.trivialorgapless_count;
        for (const auto &[si, count] : rhs.si_to_count) {
            si_to_count[si] += count;
        }

        return *this;
    }

    void print(std::ostream &out) {
        out << "-----------------\n";
        for (const auto &[si, count] : si_to_count) {
            out << si.transpose() << ":\t" << count << '\n';
        }
        out << "gapless:\t" << gapless_count << '\n';
        out << "trivial or gapless:\t" << trivialorgapless_count << '\n';
        out << "-----------------\n";
    }

    static SiSummary lower_bound(const SiSummary &l, const SiSummary &r) {
        SiSummary result = l;

        result.gapless_count = std::min(result.gapless_count, r.gapless_count);
        result.trivialorgapless_count =
            std::min(result.trivialorgapless_count, r.trivialorgapless_count);

        for (const auto &[si, count] : l.si_to_count) {
            if (!r.si_to_count.contains(si)) {
                result.si_to_count[si] = 0;
            }
        }

        for (const auto &[si, count] : r.si_to_count) {
            auto &result_count = result.si_to_count[si];

            if (!l.si_to_count.contains(si)) {
                result_count = 0;
            }

            result_count = std::min(result_count, count);
        }

        return result;
    }

    static SiSummary upper_bound(const SiSummary &l, const SiSummary &r) {
        SiSummary result = l;

        result.gapless_count = std::max(result.gapless_count, r.gapless_count);
        result.trivialorgapless_count =
            std::max(result.trivialorgapless_count, r.trivialorgapless_count);

        for (const auto &[si, count] : r.si_to_count) {
            auto &result_count = result.si_to_count[si];
            result_count = std::max(result_count, count);
        }

        return result;
    }
};

}  // namespace TopoMagnon

#endif  // SISUMMARY_HPP
