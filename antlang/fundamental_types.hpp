#include <cstddef>
#include <cstdint>

namespace ant
{

using std::size_t;

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

using flt32_t = float;
static_assert(sizeof(flt32_t) == 32/8);

using flt64_t = double;
static_assert(sizeof(flt64_t) == 64/8);

} // namespace ant
