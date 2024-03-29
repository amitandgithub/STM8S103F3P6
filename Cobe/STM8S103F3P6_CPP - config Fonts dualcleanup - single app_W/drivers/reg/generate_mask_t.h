#pragma once

#include "generate_unshifted_mask_t.h"

/**
 * Generates a bit mask of the given width left shifted
 * offset bits from the least significant bit position of
 * the word.
 * @tparam offset the number of zero bits on the least
 * significant side of the mask
 * @tparam width the number of consecutive one bits in the
 * mask
 */
template <uint8_t offset, uint8_t width>
struct generate_mask_t
{
   enum { value = generate_unshifted_mask_t<width>::value << offset };
};
