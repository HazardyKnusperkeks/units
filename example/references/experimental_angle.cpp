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

#include <units/generic/angle.h>

UNITS_DIAGNOSTIC_PUSH
UNITS_DIAGNOSTIC_IGNORE_SHADOW
#include <units/isq/si/force.h>  // 'N' (Newton) shadows a template parameter traditionally used as a size of the array
UNITS_DIAGNOSTIC_POP

#include <units/isq/si/length.h>
#include <units/isq/si/torque.h>
#include <units/math.h>
#include <units/quantity_io.h>
#include <iostream>

int main()
{
  using namespace units;
  using namespace units::isq;
  using namespace units::isq::si::references;
  using namespace units::references;

  const Length auto lever = 20 * cm;
  const Force auto force = 500 * N;
  const Angle auto angle = 90. * deg;
  const Torque auto torque = lever * force * sin(angle) / cotes_angle<>;

  std::cout << "Applying a perpendicular force of " << force << " to a " << lever << " long lever results in "
            << quantity_cast<si::newton_metre_per_radian>(torque) << " of torque.\n";
}
