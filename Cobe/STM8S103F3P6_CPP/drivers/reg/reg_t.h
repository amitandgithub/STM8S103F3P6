#pragma once

#include"generate_mask_t.h"
#include"access_policy_t.h"

/**
 * Model an MMIO register.
 *
 * Allows policy-based configuration of hardware registers
 * to give compile-time assurances.  For example, your
 * policy may not implement read() if it wants to prevent
 * read access.
 *
 * Enhances correctness by constraining register access to
 * the specific subregisters it contains. Compile time
 * calculation constrains the all access to within the
 * subregister.
 *
 * Performance of register access is exactly the same as
 * traditional *reg = *reg | (1 << offset) style access due
 * to the compiler's ability to inline static function calls
 * which depend only on compile time values. As such, use
 * this with compiler optimization enabled.
 *
 * @tparam mutability_policy_t a duck-typed static monostate
 * which implements the actual mechanics of register access.
 * @tparam address the memory location of the register
 * @tparam offset the bit position of the least significant
 * bit within the register for the subregister
 * @tparam width the width of the subregister in bits
 */
template < typename mutability_policy_t, uint16_t address, uint8_t offset, uint8_t width >
struct reg_t
{
   static_assert(width > 0, "invalid field of zero width");
   //static_assert(width + offset <= std::numeric_limits<uint8_t>::digits, "register width overflow");

   /**
    * Read the subregister.
    * @return the value
    */
   static uint8_t read()
   {
      return mutability_policy_t::read(reinterpret_cast<volatile uint8_t *>(address),offset,generate_mask_t<offset, width>::value);
   }

   /**
    * Write a subregister.
    * @param value the new value
    */
   static void write(uint8_t value)
   {
      mutability_policy_t::write( reinterpret_cast<volatile uint8_t *>(address), offset, generate_mask_t<offset, width>::value,value );
   }

   /**
    * Set all bits in the subregister to one.
    */
   static void set()
   {
      mutability_policy_t::set( reinterpret_cast<volatile uint8_t *>(address), generate_mask_t<offset, width>::value );
   }

   /**
    * Clear all bits in the subregister to zero.
    */
   static void clear()
   {
      mutability_policy_t::clear( reinterpret_cast<volatile uint8_t *>(address), generate_mask_t<offset, width>::value );
   }
};
