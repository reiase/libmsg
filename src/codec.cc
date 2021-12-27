#include "codec.hpp"

namespace msg {
Codec *Codec::create(Wire &wire) { return new DebugCodec(wire); }
}  // namespace msg