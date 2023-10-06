#pragma once

#include <complex>

#include "common/complex_number.pb.h"

namespace magnon::common {

std::complex<double> from_proto(const ComplexNumber &complex);

ComplexNumber to_proto(const std::complex<double> &complex);

}  // namespace magnon::common
