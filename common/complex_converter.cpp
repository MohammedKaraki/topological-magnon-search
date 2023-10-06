#include "common/complex_converter.hpp"

#include <cstddef>

namespace magnon::common {

std::complex<double> from_proto(const ComplexNumber &complex) {
    assert(complex.has_real());
    return {complex.real(), complex.imaginary()};
}

ComplexNumber to_proto(const std::complex<double> &complex) {
    ComplexNumber result;
    result.set_real(complex.real());
    result.set_imaginary(complex.imag());
    return result;
}

}  // namespace magnon::common
