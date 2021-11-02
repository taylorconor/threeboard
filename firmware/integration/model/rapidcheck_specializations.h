#include "rapidcheck/include/rapidcheck/gen/Arbitrary.h"
#include "rapidcheck/include/rapidcheck/gen/Numeric.h"
#include "rapidcheck/include/rapidcheck/gen/Transform.h"
#include "simulator/simulator_state.h"
#include "src/keypress.h"

namespace rc {

template <>
struct Arbitrary<threeboard::Keypress> {
  static Gen<threeboard::Keypress> arbitrary() {
    return gen::cast<threeboard::Keypress>(gen::inRange(1, 8));
  }
};

template <>
void show(const threeboard::Keypress &keypress, std::ostream &os) {
  if (keypress == threeboard::Keypress::X) {
    os << "X";
  } else if (keypress == threeboard::Keypress::Y) {
    os << "Y";
  } else if (keypress == threeboard::Keypress::Z) {
    os << "Z";
  } else if (keypress == threeboard::Keypress::XY) {
    os << "XY";
  } else if (keypress == threeboard::Keypress::XZ) {
    os << "XZ";
  } else if (keypress == threeboard::Keypress::YZ) {
    os << "YZ";
  } else if (keypress == threeboard::Keypress::XYZ) {
    os << "XYZ";
  } else if (keypress == threeboard::Keypress::INACTIVE) {
    os << "INACTIVE";
  }
}

template <>
void show(const threeboard::simulator::DeviceState &state, std::ostream &os) {
  os << state;
}
}  // namespace rc