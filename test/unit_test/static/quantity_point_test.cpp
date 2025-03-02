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
#include <units/bits/common_type.h>
#include <units/bits/external/type_traits.h>
#include <units/chrono.h>
#include <units/isq/si/length.h>
#include <units/isq/si/speed.h>
#include <units/isq/si/uscs/length.h>
#include <units/isq/si/volume.h>
#include <units/quantity_point.h>
#include <limits>
#include <type_traits>
#include <utility>

namespace {

using namespace units;
using namespace isq::si;
using namespace references;
using namespace std::chrono_literals;
using sys_seconds = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;

struct sea_level_origin : point_origin<dim_length> {};

// class invariants

template<typename DimLength>
concept invalid_types = requires {
  // unit of a different dimension:
  requires !requires { typename quantity_point<dynamic_origin<DimLength>, second, int>; };
  // quantity used as Rep:
  requires !requires { typename quantity_point<dynamic_origin<DimLength>, metre, quantity<DimLength, metre, int>>; };
  // quantity point used as Rep:
  requires !requires {
    typename quantity_point<dynamic_origin<DimLength>, metre, quantity_point<dynamic_origin<DimLength>, metre, int>>;
  };
  // reordered arguments:
  requires !requires { typename quantity_point<metre, dynamic_origin<DimLength>, double>; };
  // dimension used as origin:
  requires !requires { typename quantity_point<DimLength, second, int>; };
};

static_assert(invalid_types<dim_length>);

// member types

static_assert(is_same_v<quantity_point<dynamic_origin<dim_length>, metre, int>::rep, int>);
static_assert(is_same_v<quantity_point<dynamic_origin<dim_length>, metre, double>::rep, double>);
static_assert(is_same_v<quantity_point<dynamic_origin<dim_length>, metre, int>::unit, metre>);
static_assert(is_same_v<quantity_point<dynamic_origin<dim_length>, kilometre, int>::unit, kilometre>);
static_assert(is_same_v<quantity_point<dynamic_origin<dim_length>, metre, int>::dimension, dim_length>);
static_assert(is_same_v<quantity_point<dynamic_origin<dim_length>, metre, int>::origin, dynamic_origin<dim_length>>);
static_assert(
  is_same_v<quantity_point<dynamic_origin<dim_length>, metre, int>::quantity_type, quantity<dim_length, metre, int>>);

// constructors

static_assert(quantity_point(1).relative() == quantity(1));
static_assert(!std::is_convertible_v<int, quantity_point<dynamic_origin<dim_one>, one, int>>);

static_assert(quantity_point(42s).relative() == 42 * s);
static_assert(quantity_point(sys_seconds{42s}).relative() == 42 * s);
static_assert(!std::is_convertible_v<std::chrono::seconds,
                                     quantity_point<dynamic_origin<dim_time>, second, std::chrono::seconds::rep>>);
static_assert(
  !std::is_convertible_v<std::chrono::seconds,
                         quantity_point<clock_origin<std::chrono::system_clock>, second, std::chrono::seconds::rep>>);
static_assert(!std::is_convertible_v<sys_seconds, quantity_point<dynamic_origin<dim_time>, second, sys_seconds::rep>>);
static_assert(!std::is_convertible_v<
              sys_seconds, quantity_point<clock_origin<std::chrono::system_clock>, second, sys_seconds::rep>>);

static_assert(quantity_point<dynamic_origin<dim_length>, metre, int>().relative() == 0_q_m);
constexpr quantity_point<dynamic_origin<dim_length>, metre, int> km{1000_q_m};
static_assert(km.relative() == 1000_q_m);
static_assert(quantity_point<dynamic_origin<dim_length>, metre, int>(km).relative() == km.relative());

static_assert(quantity_point<dynamic_origin<dim_length>, metre, int>(1_q_m).relative() == 1_q_m);
static_assert(
  !std::is_constructible_v<quantity_point<dynamic_origin<dim_length>, metre, int>, double>);  // truncating conversion
static_assert(quantity_point<dynamic_origin<dim_length>, metre, double>(1.0_q_m).relative() == 1.0_q_m);
static_assert(quantity_point<dynamic_origin<dim_length>, metre, double>(1_q_m).relative() == 1_q_m);
static_assert(quantity_point<dynamic_origin<dim_length>, metre, long double>(3.14_q_m).relative() == 3.14_q_m);

static_assert(quantity_point<dynamic_origin<dim_length>, metre, int>(km).relative() == 1000_q_m);
static_assert(
  !std::is_constructible_v<quantity_point<dynamic_origin<dim_length>, metre, int>,
                           quantity_point<dynamic_origin<dim_length>, metre, double>>);  // truncating conversion
static_assert(quantity_point<dynamic_origin<dim_length>, metre, double>(quantity_point(1000.0_q_m)).relative() ==
              1000.0_q_m);
static_assert(quantity_point<dynamic_origin<dim_length>, metre, double>(km).relative() == 1000.0_q_m);
static_assert(quantity_point<dynamic_origin<dim_length>, metre, int>(quantity_point(1_q_km)).relative() == 1000_q_m);
static_assert(!std::is_constructible_v<quantity_point<dynamic_origin<dim_length>, metre, int>,
                                       quantity_point<dynamic_origin<dim_time>, second, int>>);  // different dimensions
static_assert(
  !std::is_constructible_v<quantity_point<dynamic_origin<dim_length>, kilometre, int>,
                           quantity_point<dynamic_origin<dim_length>, metre, int>>);  // truncating conversion

static_assert(!std::is_constructible_v<quantity_point<sea_level_origin, kilometre, int>,
                                       quantity_point<dynamic_origin<dim_length>, metre, int>>,
              "non-equivalent origins");
static_assert(!std::is_constructible_v<quantity_point<dynamic_origin<dim_time>, second, int>, sys_seconds>,
              "non-equivalent origins, no implicit conversion from `clock_origin`");

// assignment operator

static_assert(([]() {
                quantity_point<dynamic_origin<dim_length>, metre, int> l1(1_q_m), l2{};
                return l2 = l1;
              }())
                .relative() == 1_q_m);

// static member functions

static_assert(quantity_point<dynamic_origin<dim_length>, metre, int>::min().relative().number() ==
              std::numeric_limits<int>::lowest());
static_assert(quantity_point<dynamic_origin<dim_length>, metre, int>::max().relative().number() ==
              std::numeric_limits<int>::max());
static_assert(quantity_point<dynamic_origin<dim_length>, metre, double>::min().relative().number() ==
              std::numeric_limits<double>::lowest());
static_assert(quantity_point<dynamic_origin<dim_length>, metre, double>::max().relative().number() ==
              std::numeric_limits<double>::max());

// unary member operators

static_assert([](auto v) {
  auto vv = v++;
  return std::pair(v, vv);
}(km) == std::pair(quantity_point<dynamic_origin<dim_length>, metre, int>(1001_q_m),
                   quantity_point<dynamic_origin<dim_length>, metre, int>(1000_q_m)));
static_assert([](auto v) {
  auto vv = ++v;
  return std::pair(v, vv);
}(km) == std::pair(quantity_point<dynamic_origin<dim_length>, metre, int>(1001_q_m),
                   quantity_point<dynamic_origin<dim_length>, metre, int>(1001_q_m)));
static_assert([](auto v) {
  auto vv = v--;
  return std::pair(v, vv);
}(km) == std::pair(quantity_point<dynamic_origin<dim_length>, metre, int>(999_q_m),
                   quantity_point<dynamic_origin<dim_length>, metre, int>(1000_q_m)));
static_assert([](auto v) {
  auto vv = --v;
  return std::pair(v, vv);
}(km) == std::pair(quantity_point<dynamic_origin<dim_length>, metre, int>(999_q_m),
                   quantity_point<dynamic_origin<dim_length>, metre, int>(999_q_m)));

// compound assignment

static_assert((quantity_point(1_q_m) += 1_q_m).relative().number() == 2);
static_assert((quantity_point(2_q_m) -= 1_q_m).relative().number() == 1);

// non-member arithmetic operators

static_assert(compare<decltype(quantity_point<dynamic_origin<dim_length>, metre, int>() + length<metre, double>()),
                      quantity_point<dynamic_origin<dim_length>, metre, double>>);
static_assert(compare<decltype(length<metre, int>() + quantity_point<dynamic_origin<dim_length>, metre, double>()),
                      quantity_point<dynamic_origin<dim_length>, metre, double>>);
static_assert(compare<decltype(quantity_point<dynamic_origin<dim_length>, kilometre, int>() + length<metre, double>()),
                      quantity_point<dynamic_origin<dim_length>, metre, double>>);
static_assert(compare<decltype(length<kilometre, int>() + quantity_point<dynamic_origin<dim_length>, metre, double>()),
                      quantity_point<dynamic_origin<dim_length>, metre, double>>);
static_assert(compare<decltype(quantity_point<dynamic_origin<dim_length>, metre, double>() - length<metre, int>()),
                      quantity_point<dynamic_origin<dim_length>, metre, double>>);
static_assert(compare<decltype(quantity_point<dynamic_origin<dim_length>, kilometre, double>() - length<metre, int>()),
                      quantity_point<dynamic_origin<dim_length>, metre, double>>);
static_assert(compare<decltype(quantity_point<dynamic_origin<dim_length>, metre, double>() -
                               quantity_point<dynamic_origin<dim_length>, metre, int>()),
                      length<metre, double>>);
static_assert(compare<decltype(quantity_point<dynamic_origin<dim_length>, kilometre, double>() -
                               quantity_point<dynamic_origin<dim_length>, metre, int>()),
                      length<metre, double>>);

static_assert((1_q_m + km).relative().number() == 1001);
static_assert((quantity_point(1_q_m) + 1_q_km).relative().number() == 1001);
static_assert((km - 1_q_m).relative().number() == 999);
static_assert((quantity_point(1_q_km) - quantity_point(1_q_m)).number() == 999);

template<typename Int>
concept invalid_subtraction =
  requires(quantity_point<dynamic_origin<dim_length>, metre, Int> lhs,
           quantity_point<sea_level_origin, metre, Int> rhs) { requires !requires { rhs - lhs; }; };
static_assert(invalid_subtraction<int>);

// comparators

static_assert(quantity_point(2_q_m) + 1_q_m == quantity_point(3_q_m));
static_assert(!(2_q_m + quantity_point(2_q_m) == quantity_point(3_q_m)));
static_assert(quantity_point(2_q_m) + 2_q_m != quantity_point(3_q_m));
static_assert(!(2_q_m + quantity_point(2_q_m) != quantity_point(4_q_m)));
static_assert(quantity_point(2_q_m) > quantity_point(1_q_m));
static_assert(!(quantity_point(1_q_m) > quantity_point(1_q_m)));
static_assert(quantity_point(1_q_m) < quantity_point(2_q_m));
static_assert(!(quantity_point(2_q_m) < quantity_point(2_q_m)));
static_assert(quantity_point(2_q_m) >= quantity_point(1_q_m));
static_assert(quantity_point(2_q_m) >= quantity_point(2_q_m));
static_assert(!(quantity_point(2_q_m) >= quantity_point(3_q_m)));
static_assert(quantity_point(1_q_m) <= quantity_point(2_q_m));
static_assert(quantity_point(2_q_m) <= quantity_point(2_q_m));
static_assert(!(quantity_point(3_q_m) <= quantity_point(2_q_m)));

static_assert(quantity_point(3_q_m) == quantity_point(3.0_q_m));
static_assert(quantity_point(3_q_m) != quantity_point(3.14_q_m));
static_assert(quantity_point(2_q_m) > quantity_point(1.0_q_m));
static_assert(quantity_point(1.0_q_m) < quantity_point(2_q_m));
static_assert(quantity_point(2.0_q_m) >= quantity_point(1_q_m));
static_assert(quantity_point(1_q_m) <= quantity_point(2.0_q_m));

static_assert(quantity_point(1000_q_m) == quantity_point(1_q_km));
static_assert(quantity_point(1001_q_m) != quantity_point(1_q_km));
static_assert(quantity_point(1001_q_m) > quantity_point(1_q_km));
static_assert(quantity_point(999_q_m) < quantity_point(1_q_km));
static_assert(quantity_point(1000_q_m) >= quantity_point(1_q_km));
static_assert(quantity_point(1000_q_m) <= quantity_point(1_q_km));

template<typename Int>
concept invalid_comparisons = requires(quantity_point<dynamic_origin<dim_length>, metre, Int> lhs,
                                       quantity_point<sea_level_origin, metre, Int> rhs) {
  requires !requires { lhs == rhs; };
  requires !requires { lhs < rhs; };
};
static_assert(invalid_comparisons<int>);

// alias units

static_assert(quantity_point(2_q_l) + 2_q_ml == quantity_point(2002_q_ml));
static_assert(2_q_l + quantity_point(2_q_ml) == quantity_point(2002_q_cm3));
static_assert(quantity_point(2_q_l) + 2_q_cm3 == quantity_point(2002_q_ml));
static_assert(2_q_dm3 + quantity_point(2_q_cm3) == quantity_point(2002_q_ml));

// is_quantity_point

static_assert(QuantityPoint<quantity_point<dynamic_origin<dim_length>, millimetre, int>>);

// common_type

static_assert(compare<std::common_type_t<quantity_point<dynamic_origin<dim_length>, metre, int>,
                                         quantity_point<dynamic_origin<dim_length>, kilometre, int>>,
                      quantity_point<dynamic_origin<dim_length>, metre, int>>);
static_assert(compare<std::common_type_t<quantity_point<dynamic_origin<dim_length>, kilometre, long long>,
                                         quantity_point<dynamic_origin<dim_length>, metre, int>>,
                      quantity_point<dynamic_origin<dim_length>, metre, long long>>);
static_assert(compare<std::common_type_t<quantity_point<dynamic_origin<dim_length>, kilometre, long long>,
                                         quantity_point<dynamic_origin<dim_length>, millimetre, double>>,
                      quantity_point<dynamic_origin<dim_length>, millimetre, double>>);

// common_type

using namespace units::isq::si::uscs::literals;

static_assert(std::equality_comparable<decltype(quantity_point(1_q_m))>);
static_assert(std::equality_comparable_with<decltype(quantity_point(1_q_m)), decltype(quantity_point(1_q_km))>);
static_assert(quantity_point(0_q_m) == quantity_point(0_q_ft_us));
static_assert(std::equality_comparable_with<decltype(quantity_point(1_q_m)), decltype(quantity_point(1_q_ft_us))>);

// quantity_cast

static_assert(quantity_point_cast<quantity_point<dynamic_origin<dim_length>, metre, int>>(quantity_point(2_q_km))
                .relative()
                .number() == 2000);
static_assert(quantity_point_cast<quantity_point<dynamic_origin<dim_length>, kilometre, int>>(quantity_point(2000_q_m))
                .relative()
                .number() == 2);
static_assert(quantity_point_cast<quantity_point<dynamic_origin<dim_length>, metre, int>>(quantity_point(1.23_q_m))
                .relative()
                .number() == 1);
static_assert(quantity_point_cast<length<metre, int>>(quantity_point(2_q_km)).relative().number() == 2000);
static_assert(quantity_point_cast<length<kilometre, int>>(quantity_point(2000_q_m)).relative().number() == 2);
static_assert(quantity_point_cast<length<metre, int>>(quantity_point(1.23_q_m)).relative().number() == 1);
static_assert(quantity_point_cast<metre>(quantity_point(2_q_km)).relative().number() == 2000);
static_assert(quantity_point_cast<kilometre>(quantity_point(2000_q_m)).relative().number() == 2);
static_assert(quantity_point_cast<int>(quantity_point(1.23_q_m)).relative().number() == 1);
static_assert(
  quantity_point_cast<dim_speed, kilometre_per_hour>(quantity_point(2000.0_q_m / 3600.0_q_s)).relative().number() == 2);

template<typename Int>
concept invalid_cast = requires(Int i) {
  requires !requires {
    quantity_point_cast<quantity_point<dynamic_origin<dim_time>, second, Int>>(quantity_point(i * m));
  };
  requires !requires {
    quantity_point_cast<quantity_point<dynamic_origin<dim_length>, metre, Int>>(
      quantity_point<sea_level_origin, metre, Int>(i * m));
  };
};
static_assert(invalid_cast<int>);

// time

static_assert(quantity_point{1_q_h} == quantity_point{3600_q_s});

template<typename Metre>
concept no_crossdimensional_equality = !requires { quantity_point(1_q_s) == quantity_point(length<Metre, int>(1)); };

static_assert(no_crossdimensional_equality<metre>);

// length

static_assert(quantity_point(1_q_km) != quantity_point(1_q_m));
static_assert(quantity_point(1_q_km) == quantity_point(1000_q_m));
static_assert(quantity_point(1_q_km) + 1_q_m == quantity_point(1001_q_m));
static_assert(1_q_km + quantity_point(1_q_m) == quantity_point(1001_q_m));

template<class T>
concept dimensional_analysis = requires(T t) { pow<2>(t); };

static_assert(!dimensional_analysis<quantity_point<dynamic_origin<dim_length>, metre, int>>);

}  // namespace
