#include "boost/program_options.hpp"
#include "fmt/core.h"

#include "common/proto_text_format.hpp"
#include "diagnose2/perturbed_band_structure.pb.h"
#include "diagnose2/spectrum_data.hpp"
#include "summary/visualizer.hpp"

namespace po = boost::program_options;

int main(const int argc, const char *const argv[]) {
    using namespace magnon;

    po::options_description desc{};
    desc.add_options()(
        "perturbation_data", po::value<std::string>(), "Filename of perturbed spectrum result");

    po::variables_map vm{};
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    assert(vm.count("perturbation_data") == 1);
    const auto perturbed_spectrum_filename = vm["perturbation_data"].as<std::string>();
    const auto perturbation_data = [&]() {
        magnon::diagnose2::PerturbedBandStructure result{};
        magnon::common::proto::read_from_text_file(perturbed_spectrum_filename, result);
        return result;
    }();

    const diagnose2::SpectrumData spectrum_data(perturbation_data);
    const diagnose2::Superband superband(spectrum_data.pos_neg_magnonirreps.first, spectrum_data);
    const diagnose2::Subband subband = superband.make_subband();

    Visualizer({0, 1, 2, 3, 4, 5, 6, 7}, superband, subband, spectrum_data, {}, {})
        .dump("/home/mohammed/magnon/tmp/out.tex");
}
