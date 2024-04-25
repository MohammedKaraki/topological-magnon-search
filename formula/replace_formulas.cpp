#include "Eigen/Core"

#include "formula/parse.hpp"
#include "formula/replace_formulas.hpp"
#include "utils/matrix_converter.hpp"
#include "utils/proto_text_format.hpp"

namespace magnon::formula {

constexpr char ALTERNATIVE_FORMULAS_FILENAME[] = "formula/alternative_formulas.pb.txt";

using VecX = Eigen::VectorXi;

VecX vector_from_expression(const Terms &expression,
                            const std::map<std::string, int> &basis_to_index) {
    const int dim = basis_to_index.size();
    VecX result = VecX::Zero(dim);
    for (const auto &term : expression) {
        const int index = basis_to_index.at(term.basis);
        if (index < 0 || index >= dim) {
            throw std::runtime_error("Basis index out of range!");
        }
        result(index) += term.factor;
    }
    return result;
}

void replace_formula(groups::MagneticSpaceGroup &group, const Formulas &formulas) {
    const std::size_t num_formulas = formulas.formula().size();
    if (num_formulas == 0) {
        throw std::runtime_error("replace_formula called with 0 formulas!");
    }
    if (!group.has_symmetry_indicator_matrix()) {
        throw std::runtime_error(
            "replace_formula called with group missing symmetry indicator matrix!");
    }
    if (group.symmetry_indicator_matrix().num_rows() != num_formulas) {
        throw std::runtime_error(
            "Number of formulas provided doesn't match the number of nontrivial SI formulas int he "
            "groups!");
    }
    if (group.irrep_label_to_matrix_column_index().size() == 0) {
        throw std::runtime_error(
            "replace_formula called with group missing irrep label to index map!");
    }
    const std::map<std::string, int> label_to_index(
        group.irrep_label_to_matrix_column_index().begin(),
        group.irrep_label_to_matrix_column_index().end());
    Eigen::MatrixXi si_matrix{from_proto(group.symmetry_indicator_matrix())};
    for (int row = 0; row < si_matrix.rows(); ++row) {
        si_matrix.row(row) =
            vector_from_expression(parse_formula(formulas.formula(row)).value(), label_to_index);
    }
    *group.mutable_symmetry_indicator_matrix() = utils::to_proto(si_matrix);
}
bool maybe_replace_formula(groups::MagneticSpaceGroup &group,
                           const MsgToFormulas &msg_to_formulas) {
    if (!group.has_number()) {
        throw std::runtime_error("Missing group number!");
    }
    if (!msg_to_formulas.msg_to_formulas().contains(group.number())) {
        return false;
    }
    replace_formula(group, msg_to_formulas.msg_to_formulas().at(group.number()));
    return true;
}

bool maybe_replace_formula(groups::MagneticSpaceGroup &group) {
    MsgToFormulas msg_to_formulas{};
    if (!utils::proto::read_from_text_file(ALTERNATIVE_FORMULAS_FILENAME, msg_to_formulas)) {
        throw std::runtime_error("Couldn't read formulas file!");
    }
    return maybe_replace_formula(group, msg_to_formulas);
}

groups::MagneticSpaceGroup maybe_with_alternative_si_formulas(groups::MagneticSpaceGroup group) {
    maybe_replace_formula(group);
    return group;
}

diagnose2::PerturbedBandStructure maybe_with_alternative_si_formulas(
    diagnose2::PerturbedBandStructure perturbation) {
    maybe_replace_formula(*perturbation.mutable_subgroup());
    return perturbation;
}

}  // namespace magnon::formula
