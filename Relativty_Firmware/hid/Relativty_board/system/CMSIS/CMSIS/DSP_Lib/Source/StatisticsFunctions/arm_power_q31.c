/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_power_q31.c   
*   
* Description:	Sum of the squares of the elements of a Q31 vector.  
*   
* Target Processor: Cortex-M4/Cortex-M3/Cortex-M0
*  
* Version 1.0.10 2011/7/15 
*    Big Endian support added and Merged M0 and M3/M4 Source code.  
*   
* Version 1.0.3 2010/11/29  
*    Re-organized the CMSIS folders and updated documentation.   
*    
* Version 1.0.2 2010/11/11   
*    Documentation updated.    
*   
* Version 1.0.1 2010/10/05    
*    Production release and review comments incorporated.   
*   
* Version 1.0.0 2010/09/20    
*    Production release and review comments incorporated.   
* -------------------------------------------------------------------- */

#include "arm_math.h"

/**   
 * @ingroup groupStats   
 */

/**   
 * @addtogroup power   
 * @{   
 */

/**   
 * @brief Sum of the squares of the elements of a Q31 vector.   
 * @param[in]       *pSrc points to the input vector   
 * @param[in]       blockSize length of the input vector   
 * @param[out]      *pResult sum of the squares value returned here   
 * @return none.   
 *   
 * @details   
 * <b>Scaling and Overflow Behavior:</b>   
 *   
 * \par   
 * The function is implemented using a 64-bit internal accumulator.   
 * The input is represented in 1.31 format.   
 * Intermediate multiplication yields a 2.62 format, and this   
 * result is truncated to 2.48 format by discarding the lower 14 bits.   
 * The 2.48 result is then added without saturation to a 64-bit accumulator in 16.48 format.   
 * With 15 guard bits in the accumulator, there is no risk of overflow, and the   
 * full precision of the intermediate multiplication is preserved.   
 * Finally, the return result is in 16.48 format.    
 *   
 */

void arm_power_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q63_t * pResult)
{
  q63_t sum = 0;                                 /* Temporary result storage */
  q31_t in;
  uint32_t blkCnt;                               /* loop counter */


#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + A[2] * A[2] + ... + A[blockSize-1] * A[blockSize-1] */
    /* Compute Power then shift intermediate results by 14 bits to maintain 16.48 format and then store the result in a temporary variable sum, providing 15 guard bits. */
    in = *pSrc++;
    sum += ((q63_t) in * in) >> 14u;

    in = *pSrc++;
    sum += ((q63_t) in * in) >> 14u;

    in = *pSrc++;
    sum += ((q63_t) in * in) >> 14u;

    in = *pSrc++;
    sum += ((q63_t) in * in) >> 14u;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.   
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

#else

  /* Run the below code for Cortex-M0 */

  /* Loop over blockSize number of values */
  blkCnt = blockSize;

#endif /* #ifndef ARM_MATH_CM0 */

  while(blkCnt > 0u)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + A[2] * A[2] + ... + A[blockSize-1] * A[blockSize-1] */
    /* Compute Power and then store the result in a temporary variable, sum. */
    in = *pSrc++;
    sum += ((q63_t) in * in) >> 14u;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Store the results in 16.48 format  */
  *pResult = sum;
}

/**   
 * @} end of power group   
 */
