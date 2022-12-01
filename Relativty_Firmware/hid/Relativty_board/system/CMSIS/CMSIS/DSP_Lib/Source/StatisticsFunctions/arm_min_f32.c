/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_min_f32.c   
*   
* Description:	Minimum value of a floating-point vector.   
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
* ---------------------------------------------------------------------------- */

#include "arm_math.h"

/**   
 * @ingroup groupStats   
 */

/**   
 * @defgroup Min Minimum   
 *   
 * Computes the minimum value of an array of data.    
 * The function returns both the minimum value and its position within the array.    
 * There are separate functions for floating-point, Q31, Q15, and Q7 data types.   
 */

/**   
 * @addtogroup Min   
 * @{   
 */


/**   
 * @brief Minimum value of a floating-point vector.   
 * @param[in]       *pSrc points to the input vector   
 * @param[in]       blockSize length of the input vector   
 * @param[out]      *pResult minimum value returned here   
 * @param[out]      *pIndex index of minimum value returned here   
  * @return none.   
 *   
 */

void arm_min_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult,
  uint32_t * pIndex)
{
  float32_t minVal, out;                         /* Temporary variables to store the output value. */
  uint32_t blkCnt, outIndex;                     /* loop counter */

  /* Initialise the index value to zero. */
  outIndex = 0u;
  /* Load first input value that act as reference value for comparision */
  out = *pSrc++;

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */


  /* Loop over blockSize number of values */
  blkCnt = (blockSize - 1u);

  do
  {
    /* Initialize minVal to the next consecutive values one by one */
    minVal = *pSrc++;

    /* compare for the minimum value */
    if(out > minVal)
    {
      /* Update the minimum value and it's index */
      out = minVal;
      outIndex = blockSize - blkCnt;
    }

    blkCnt--;

  } while(blkCnt > 0u);

#else

  /* Run the below code for Cortex-M0 */

  /* Loop over blockSize - 1 number of values */
  blkCnt = (blockSize - 1u);

  while(blkCnt > 0u)
  {
    /* Initialize minVal to the next consecutive values one by one */
    minVal = *pSrc++;

    /* compare for the minimum value */
    if(out > minVal)
    {
      /* Update the minimum value and it's index */
      out = minVal;
      outIndex = blockSize - blkCnt;
    }
    /* Decrement the loop counter */
    blkCnt--;

  }

#endif /* #ifndef ARM_MATH_CM0 */


  /* Store the minimum value and it's index into destination pointers */
  *pResult = out;
  *pIndex = outIndex;
}

/**   
 * @} end of Min group   
 */
