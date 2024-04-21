#pragma once

#include <complex>

#include "utils/complex_number.pb.h"

namespace magnon::utils {

std::complex<double> from_proto(const ComplexNumber &complex);

ComplexNumber to_proto(const std::complex<double> &complex);

}  // namespace magnon::utils
