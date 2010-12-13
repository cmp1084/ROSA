
/*! \name Bit Reversing
 */
//! @{

/*! \brief Reverses the bits of \a u8.
 *
 * \param u8  U8 of which to reverse the bits.
 *
 * \return Value resulting from \a u8 with reversed bits.
 */
#define bit_reverse8(u8)    ((U8)(bit_reverse32((U8)(u8)) >> 24))

/*! \brief Reverses the bits of \a u16.
 *
 * \param u16 U16 of which to reverse the bits.
 *
 * \return Value resulting from \a u16 with reversed bits.
 */
#define bit_reverse16(u16)  ((U16)(bit_reverse32((U16)(u16)) >> 16))

/*! \brief Reverses the bits of \a u32.
 *
 * \param u32 U32 of which to reverse the bits.
 *
 * \return Value resulting from \a u32 with reversed bits.
 */

typedef unsigned int U32;
typedef unsigned char U8;

#if (defined __GNUC__)
  #define bit_reverse32(u32) \
  (\
    {\
      unsigned int __value = (U32)(u32);\
      __asm__ ("brev\t%0" : "+r" (__value) :  : "cc");\
      (U32)__value;\
    }\
  )
#elif (defined __ICCAVR32__)
  #define bit_reverse32(u32)  ((U32)__bit_reverse((U32)(u32)))
#endif

/*! \brief Reverses the bits of \a u64.
 *
 * \param u64 U64 of which to reverse the bits.
 *
 * \return Value resulting from \a u64 with reversed bits.
 */
#define bit_reverse64(u64)  ((U64)(((U64)bit_reverse32((U64)(u64) >> 32)) |\
                                   ((U64)bit_reverse32((U64)(u64)) << 32)))

