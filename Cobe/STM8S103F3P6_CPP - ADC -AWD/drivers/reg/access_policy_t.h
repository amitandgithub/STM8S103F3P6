#pragma once


/**
 * A write-only mutability policy for use with reg_t.
 */
struct wo_t
{
   static void write( volatile uint8_t * device, uint8_t offset, uint8_t mask, uint8_t value )
   { 
     *device = ((value << offset) & mask); 
   }

   static void set(volatile uint8_t * device,uint8_t mask )
   { 
     *device = mask;
   }
};


/**
 * A read-only mutability policy for use with reg_t.
 */
struct ro_t
{
   static uint8_t read( volatile uint8_t * device, uint8_t offset, uint8_t mask )
   { 
     return (*device & mask) >> offset;
   } 
};


/**
 * A read-write mutability policy for use with reg_t.
 */
struct rw_t : ro_t
{
   static void write( volatile uint8_t * device,uint8_t offset,uint8_t mask,uint8_t value )
   { 
     *device = (*device & ~mask) | ((value << offset) & mask);
   }

   static void set( volatile uint8_t * device,uint8_t mask)
   { 
     *device |= mask; 
   }

   static void clear(volatile uint8_t * device,uint8_t mask)
   { 
     *device &= ~mask;
   }
};



