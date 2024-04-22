#include "formula/parse.hpp"

#include <utility>

#include "boost/fusion/adapted/std_pair.hpp"
#include "boost/spirit/home/x3.hpp"

namespace magnon::formula {
namespace parser {
namespace x3 = boost::spirit::x3;
using x3::char_;
using x3::int_;
using x3::lexeme;
using x3::rule;

using Basis = std::string;
using Factor = std::pair<std::optional<char>, std::optional<int>>;
using Term = std::pair<Factor, Basis>;
using Terms = std::vector<Term>;

const auto plus_or_minus = rule<class factor, char>{} = char_('+') | char_('-');
const auto factor = rule<class factor, Factor>{} = -plus_or_minus >> -int_;

const auto basis = rule<class basis, Basis>{} = lexeme[+char_("-+_^{}a-zA-Z0-9")];
const auto term = rule<class term, Term>{} = factor >> 'n' >> '(' >> basis >> ')';
const auto terms = rule<class terms, Terms>{} = term >> *(&plus_or_minus >> term);
const auto parser = x3::skip(x3::space)[terms];

}  // namespace parser

namespace {

int to_int(const parser::Factor &factor) {
    const char sign = factor.first.value_or('+');
    assert(sign == '-' || sign == '+');

    const int coeff = factor.second.value_or(1);
    return sign == '+' ? coeff : -coeff;
}

Term to_term(const parser::Term &term) {
    return Term{.factor = to_int(term.first), .basis{term.second}};
}

}  // namespace

std::optional<Terms> parse_formula(const std::string &formula) {
    auto f = formula.cbegin();
    const auto l = formula.cend();
    parser::Terms terms;
    if (!parse(f, l, parser::parser, terms) || (f != l)) {
        return std::nullopt;
    }

    Terms result{};
    for (const auto &term : terms) {
        result.push_back(to_term(term));
    };
    return result;
}

}  // namespace magnon::formula
