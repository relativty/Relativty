/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_power_q7.c   
*   
* Description:	Sum of the squares of the elements of a Q7 vector.   
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
 * @brief Sum of the squares of the elements of a Q7 vector.   
 * @param[in]       *pSrc points to the input vector   
 * @param[in]       blockSize length of the input vector   
 * @param[out]      *pResult sum of the squares value returned here   
 * @return none.   
 *   
 * @details   
 * <b>Scaling and Overflow Behavior:</b>   
 *   
 * \par   
 * The function is implemented using a 32-bit internal accumulator.    
 * The input is represented in 1.7 format.  
 * Intermediate multiplication yields a 2.14 format, and this   
 * result is added without saturation to an accumulator in 18.14 format.   
 * With 17 guard bits in the accumulator, there is no risk of overflow, and the   
 * full precision of the intermediate multiplication is preserved.   
 * Finally, the return result is in 18.14 format.    
 *   
 */

void arm_power_q7(
  q7_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult)
{
  q31_t sum = 0;                                 /* Temporary result storage */
  q7_t in;                                       /* Temporary variable to store input */
  uint32_t blkCnt;                               /* loop counter */

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  q31_t input1;                                  /* Temporary variable to store packed input */
  q15_t in1, in2;                                /* Temporary variables to store input */

  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* Reading two inputs of pSrc vector and packing */
    in1 = (q15_t) * pSrc++;
    in2 = (q15_t) * pSrc++;
    input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16);

    /* C = A[0] * A[0] + A[1] * A[1] + A[2] * A[2] + ... + A[blockSize-1] * A[blockSize-1] */
    /* Compute Power and then store the result in a temporary variable, sum. */
    sum = __SMLAD(input1, input1, sum);

    /* Reading two inputs of pSrc vector and packing */
    in1 = (q15_t) * pSrc++;
    in2 = (q15_t) * pSrc++;
    input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16);

    /* C = A[0] * A[0] + A[1] * A[1] + A[2] * A[2] + ... + A[blockSize-1] * A[blockSize-1] */
    /* Compute Power and then store the result in a temporary variable, sum. */
    sum = __SMLAD(input1, input1, sum);

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
    sum += ((q15_t) in * in);

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Store the result in 18.14 format  */
  *pResult = sum;
}

/**   
 * @} end of power group   
 */
