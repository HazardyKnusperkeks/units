// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "test_tools.h"
#include <units/bits/dimension_op.h>
#include <units/derived_dimension.h>
#include <units/isq/si/frequency.h>
#include <units/isq/si/mass.h>
#include <units/isq/si/time.h>
#include <units/isq/si/voltage.h>
#include <units/math.h>  // IWYU pragma: keep
#include <units/unit.h>

namespace {

using namespace units;
using namespace units::isq::si;

// power spectral density
struct sq_volt_per_hertz : derived_unit<sq_volt_per_hertz> {};
struct dim_power_spectral_density :
    derived_dimension<dim_power_spectral_density, sq_volt_per_hertz, units::exponent<dim_voltage, 2>,
                      units::exponent<dim_frequency, -1>> {};

template<UnitOf<dim_power_spectral_density> U, Representation Rep = double>
using power_spectral_density = quantity<dim_power_spectral_density, U, Rep>;

// amplitude spectral density
struct volt_per_sqrt_hertz : derived_unit<volt_per_sqrt_hertz> {};
struct dim_amplitude_spectral_density :
    derived_dimension<dim_amplitude_spectral_density, volt_per_sqrt_hertz, units::exponent<dim_voltage, 1>,
                      units::exponent<dim_frequency, -1, 2>> {};

template<UnitOf<dim_amplitude_spectral_density> U, Representation Rep = double>
using amplitude_spectral_density = quantity<dim_amplitude_spectral_density, U, Rep>;

}  // namespace

namespace {

static_assert(compare<dimension_sqrt<dim_power_spectral_density>, dim_amplitude_spectral_density>);
static_assert(compare<dimension_pow<dim_amplitude_spectral_density, 2>, dim_power_spectral_density>);

static_assert(compare<decltype(pow<2>(amplitude_spectral_density<volt_per_sqrt_hertz>(4))),
                      decltype(power_spectral_density<sq_volt_per_hertz>(16))>);
static_assert(compare<decltype(sqrt(power_spectral_density<sq_volt_per_hertz>(16))),
                      decltype(amplitude_spectral_density<volt_per_sqrt_hertz>(4))>);

}  // namespace

namespace {

struct kilogram_per_second : derived_unit<kilogram_per_second> {};
struct dim_mass_rate :
    derived_dimension<dim_mass_rate, kilogram_per_second, units::exponent<dim_mass, 1>, units::exponent<dim_time, -1>> {
};
struct kilogram_per_hour : derived_scaled_unit<kilogram_per_hour, dim_mass_rate, kilogram, hour> {};
[[maybe_unused]] constexpr auto a = 1_q_kg / 1_q_h;

}  // namespace
