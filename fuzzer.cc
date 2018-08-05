#include <algorithm>
#include <vector>

#include "result.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size < sizeof(int))
    return 0;

  int looking_for = *(reinterpret_cast<const int*>(data));
  data += sizeof(int);
  size -= sizeof(int);

  const int* data_f = reinterpret_cast<const int*>(data);
  const int* data_l = data_f + size / sizeof(int);

  std::vector<int> as_v(data_f, data_l);
  std::sort(as_v.begin(), as_v.end());

  auto expected = std::lower_bound(as_v.begin(), as_v.end(), looking_for);
  auto actual = srt::lower_bound_biased(as_v.begin(), as_v.end(), looking_for);
  assert(expected == actual);

  return 0;  // Non-zero return values are reserved for future use.
}