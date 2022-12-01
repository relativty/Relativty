/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_conv_fast_q15.c   
*   
* Description:	Fast Q15 Convolution.   
*   
* Target Processor: Cortex-M4/Cortex-M3
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
 * @ingroup groupFilters   
 */

/**   
 * @addtogroup Conv   
 * @{   
 */

/**   
 * @brief Convolution of Q15 sequences (fast version) for Cortex-M3 and Cortex-M4.   
 * @param[in] *pSrcA points to the first input sequence.   
 * @param[in] srcALen length of the first input sequence.   
 * @param[in] *pSrcB points to the second input sequence.   
 * @param[in] srcBLen length of the second input sequence.   
 * @param[out] *pDst points to the location where the output result is written.  Length srcALen+srcBLen-1.   
 * @return none.   
 *   
 * <b>Scaling and Overflow Behavior:</b>   
 *   
 * \par   
 * This fast version uses a 32-bit accumulator with 2.30 format.   
 * The accumulator maintains full precision of the intermediate multiplication results   
 * but provides only a single guard bit. There is no saturation on intermediate additions.   
 * Thus, if the accumulator overflows it wraps around and distorts the result.   
 * The input signals should be scaled down to avoid intermediate overflows.   
 * Scale down the inputs by log2(min(srcALen, srcBLen)) (log2 is read as log to the base 2) times to avoid overflows,   
 * as maximum of min(srcALen, srcBLen) number of additions are carried internally.   
 * The 2.30 accumulator is right shifted by 15 bits and then saturated to 1.15 format to yield the final result.   
 *   
 * \par   
 * See <code>arm_conv_q15()</code> for a slower implementation of this function which uses 64-bit accumulation to avoid wrap around distortion.   
 */

void arm_conv_fast_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst)
{
  q15_t *pIn1;                                   /* inputA pointer */
  q15_t *pIn2;                                   /* inputB pointer */
  q15_t *pOut = pDst;                            /* output pointer */
  q31_t sum, acc0, acc1, acc2, acc3;             /* Accumulator */
  q15_t *px;                                     /* Intermediate inputA pointer  */
  q15_t *py;                                     /* Intermediate inputB pointer  */
  q15_t *pSrc1, *pSrc2;                          /* Intermediate pointers */
  q31_t x0, x1, x2, x3, c0;                      /* Temporary variables to hold state and coefficient values */
  uint32_t blockSize1, blockSize2, blockSize3, j, k, count, blkCnt;     /* loop counter */
  q31_t *pb;                                     /* 32 bit pointer for inputB buffer */


  /* The algorithm implementation is based on the lengths of the inputs. */
  /* srcB is always made to slide across srcA. */
  /* So srcBLen is always considered as shorter or equal to srcALen */
  if(srcALen >= srcBLen)
  {
    /* Initialization of inputA pointer */
    pIn1 = pSrcA;

    /* Initialization of inputB pointer */
    pIn2 = pSrcB;
  }
  else
  {
    /* Initialization of inputA pointer */
    pIn1 = pSrcB;

    /* Initialization of inputB pointer */
    pIn2 = pSrcA;

    /* srcBLen is always considered as shorter or equal to srcALen */
    j = srcBLen;
    srcBLen = srcALen;
    srcALen = j;
  }

  /* conv(x,y) at n = x[n] * y[0] + x[n-1] * y[1] + x[n-2] * y[2] + ...+ x[n-N+1] * y[N -1] */
  /* The function is internally   
   * divided into three stages according to the number of multiplications that has to be   
   * taken place between inputA samples and inputB samples. In the first stage of the   
   * algorithm, the multiplications increase by one for every iteration.   
   * In the second stage of the algorithm, srcBLen number of multiplications are done.   
   * In the third stage of the algorithm, the multiplications decrease by one   
   * for every iteration. */

  /* The algorithm is implemented in three stages.   
     The loop counters of each stage is initiated here. */
  blockSize1 = srcBLen - 1u;
  blockSize2 = srcALen - (srcBLen - 1u);
  blockSize3 = blockSize1;

  /* --------------------------   
   * Initializations of stage1   
   * -------------------------*/

  /* sum = x[0] * y[0]   
   * sum = x[0] * y[1] + x[1] * y[0]   
   * ....   
   * sum = x[0] * y[srcBlen - 1] + x[1] * y[srcBlen - 2] +...+ x[srcBLen - 1] * y[0]   
   */

  /* In this stage the MAC operations are increased by 1 for every iteration.   
     The count variable holds the number of MAC operations performed */
  count = 1u;

  /* Working pointer of inputA */
  px = pIn1;

  /* Working pointer of inputB */
  py = pIn2;


  /* ------------------------   
   * Stage1 process   
   * ----------------------*/

  /* For loop unrolling by 4, this stage is divided into two. */
  /* First part of this stage computes the MAC operations less than 4 */
  /* Second part of this stage computes the MAC operations greater than or equal to 4 */

  /* The first part of the stage starts here */
  while((count < 4u) && (blockSize1 > 0u))
  {
    /* Accumulator is made zero for every iteration */
    sum = 0;

    /* Loop over number of MAC operations between   
     * inputA samples and inputB samples */
    k = count;

    while(k > 0u)
    {
      /* Perform the multiply-accumulates */
      sum = __SMLAD(*px++, *py--, sum);

      /* Decrement the loop counter */
      k--;
    }

    /* Store the result in the accumulator in the destination buffer. */
    *pOut++ = (q15_t) (sum >> 15);

    /* Update the inputA and inputB pointers for next MAC calculation */
    py = pIn2 + count;
    px = pIn1;

    /* Increment the MAC count */
    count++;

    /* Decrement the loop counter */
    blockSize1--;
  }

  /* The second part of the stage starts here */
  /* The internal loop, over count, is unrolled by 4 */
  /* To, read the last two inputB samples using SIMD:   
   * y[srcBLen] and y[srcBLen-1] coefficients, py is decremented by 1 */
  py = py - 1;

  while(blockSize1 > 0u)
  {
    /* Accumulator is made zero for every iteration */
    sum = 0;

    /* Apply loop unrolling and compute 4 MACs simultaneously. */
    k = count >> 2u;

    /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.   
     ** a second loop below computes MACs for the remaining 1 to 3 samples. */
    while(k > 0u)
    {
      /* Perform the multiply-accumulates */
      /* x[0], x[1] are multiplied with y[srcBLen - 1], y[srcBLen - 2] respectively */
      sum = __SMLADX(*__SIMD32(px)++, *__SIMD32(py)--, sum);
      /* x[2], x[3] are multiplied with y[srcBLen - 3], y[srcBLen - 4] respectively */
      sum = __SMLADX(*__SIMD32(px)++, *__SIMD32(py)--, sum);

      /* Decrement the loop counter */
      k--;
    }

    /* For the next MAC operations, the pointer py is used without SIMD   
     * So, py is incremented by 1 */
    py = py + 1u;

    /* If the count is not a multiple of 4, compute any remaining MACs here.   
     ** No loop unrolling is used. */
    k = count % 0x4u;

    while(k > 0u)
    {
      /* Perform the multiply-accumulates */
      sum = __SMLAD(*px++, *py--, sum);

      /* Decrement the loop counter */
      k--;
    }

    /* Store the result in the accumulator in the destination buffer. */
    *pOut++ = (q15_t) (sum >> 15);

    /* Update the inputA and inputB pointers for next MAC calculation */
    py = pIn2 + (count - 1u);
    px = pIn1;

    /* Increment the MAC count */
    count++;

    /* Decrement the loop counter */
    blockSize1--;
  }

  /* --------------------------   
   * Initializations of stage2   
   * ------------------------*/

  /* sum = x[0] * y[srcBLen-1] + x[1] * y[srcBLen-2] +...+ x[srcBLen-1] * y[0]   
   * sum = x[1] * y[srcBLen-1] + x[2] * y[srcBLen-2] +...+ x[srcBLen] * y[0]   
   * ....   
   * sum = x[srcALen-srcBLen-2] * y[srcBLen-1] + x[srcALen] * y[srcBLen-2] +...+ x[srcALen-1] * y[0]   
   */

  /* Working pointer of inputA */
  px = pIn1;

  /* Working pointer of inputB */
  pSrc2 = pIn2 + (srcBLen - 1u);
  py = pSrc2;

  /* Initialize inputB pointer of type q31 */
  pb = (q31_t *) (py - 1u);

  /* count is the index by which the pointer pIn1 to be incremented */
  count = 1u;


  /* --------------------   
   * Stage2 process   
   * -------------------*/

  /* Stage2 depends on srcBLen as in this stage srcBLen number of MACS are performed.   
   * So, to loop unroll over blockSize2,   
   * srcBLen should be greater than or equal to 4 */
  if(srcBLen >= 4u)
  {
    /* Loop unroll over blockSize2, by 4 */
    blkCnt = blockSize2 >> 2u;

    while(blkCnt > 0u)
    {
      /* Set all accumulators to zero */
      acc0 = 0;
      acc1 = 0;
      acc2 = 0;
      acc3 = 0;


      /* read x[0], x[1] samples */
      x0 = *(q31_t *) (px++);
      /* read x[1], x[2] samples */
      x1 = *(q31_t *) (px++);


      /* Apply loop unrolling and compute 4 MACs simultaneously. */
      k = srcBLen >> 2u;

      /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.   
       ** a second loop below computes MACs for the remaining 1 to 3 samples. */
      do
      {
        /* Read the last two inputB samples using SIMD:   
         * y[srcBLen - 1] and y[srcBLen - 2] */
        c0 = *(pb--);

        /* acc0 +=  x[0] * y[srcBLen - 1] + x[1] * y[srcBLen - 2] */
        acc0 = __SMLADX(x0, c0, acc0);

        /* acc1 +=  x[1] * y[srcBLen - 1] + x[2] * y[srcBLen - 2] */
        acc1 = __SMLADX(x1, c0, acc1);

        /* Read x[2], x[3] */
        x2 = *(q31_t *) (px++);

        /* Read x[3], x[4] */
        x3 = *(q31_t *) (px++);

        /* acc2 +=  x[2] * y[srcBLen - 1] + x[3] * y[srcBLen - 2] */
        acc2 = __SMLADX(x2, c0, acc2);

        /* acc3 +=  x[3] * y[srcBLen - 1] + x[4] * y[srcBLen - 2] */
        acc3 = __SMLADX(x3, c0, acc3);

        /* Read y[srcBLen - 3] and y[srcBLen - 4] */
        c0 = *(pb--);

        /* acc0 +=  x[2] * y[srcBLen - 3] + x[3] * y[srcBLen - 4] */
        acc0 = __SMLADX(x2, c0, acc0);

        /* acc1 +=  x[3] * y[srcBLen - 3] + x[4] * y[srcBLen - 4] */
        acc1 = __SMLADX(x3, c0, acc1);

        /* Read x[4], x[5] */
        x0 = *(q31_t *) (px++);

        /* Read x[5], x[6] */
        x1 = *(q31_t *) (px++);

        /* acc2 +=  x[4] * y[srcBLen - 3] + x[5] * y[srcBLen - 4] */
        acc2 = __SMLADX(x0, c0, acc2);

        /* acc3 +=  x[5] * y[srcBLen - 3] + x[6] * y[srcBLen - 4] */
        acc3 = __SMLADX(x1, c0, acc3);

      } while(--k);

      /* For the next MAC operations, SIMD is not used   
       * So, the 16 bit pointer if inputB, py is updated */
      py = (q15_t *) pb;
      py = py + 1;

      /* If the srcBLen is not a multiple of 4, compute any remaining MACs here.   
       ** No loop unrolling is used. */
      k = srcBLen % 0x4u;

      if(k == 1u)
      {
        /* Read y[srcBLen - 5] */
        c0 = *(py);
#ifdef  ARM_MATH_BIG_ENDIAN

//          c0 = unallign_rev(p, c0); 
        c0 = c0 << 16;
#endif /*      #ifdef  ARM_MATH_BIG_ENDIAN     */

        /* Read x[7] */
        x3 = *(q31_t *) px++;

        /* Perform the multiply-accumulates */
        acc0 = __SMLAD(x0, c0, acc0);
        acc1 = __SMLAD(x1, c0, acc1);
        acc2 = __SMLADX(x1, c0, acc2);
        acc3 = __SMLADX(x3, c0, acc3);
      }

      if(k == 2u)
      {
        /* Read y[srcBLen - 5], y[srcBLen - 6] */
        c0 = *(pb);

        /* Read x[7], x[8] */
        x3 = *(q31_t *) px++;

        /* Read x[9] */
        x2 = *(q31_t *) px++;

        /* Perform the multiply-accumulates */
        acc0 = __SMLADX(x0, c0, acc0);
        acc1 = __SMLADX(x1, c0, acc1);
        acc2 = __SMLADX(x3, c0, acc2);
        acc3 = __SMLADX(x2, c0, acc3);
      }

      if(k == 3u)
      {
        /* Read y[srcBLen - 5], y[srcBLen - 6] */
        c0 = *pb--;

        /* Read x[7], x[8] */
        x3 = *(q31_t *) px++;

        /* Read x[9] */
        x2 = *(q31_t *) px++;

        /* Perform the multiply-accumulates */
        acc0 = __SMLADX(x0, c0, acc0);
        acc1 = __SMLADX(x1, c0, acc1);
        acc2 = __SMLADX(x3, c0, acc2);
        acc3 = __SMLADX(x2, c0, acc3);

        /* Read y[srcBLen - 7] */
#ifdef  ARM_MATH_BIG_ENDIAN

        c0 = (*pb);
//              c0 = (c0 & 0x0000FFFF)<<16;     
        c0 = (c0) << 16;

#else

        c0 = (q15_t) (*pb >> 16);

#endif /*      #ifdef  ARM_MATH_BIG_ENDIAN     */

        /* Read x[10] */
        x3 = *(q31_t *) px++;

        /* Perform the multiply-accumulates */
        acc0 = __SMLADX(x1, c0, acc0);
        acc1 = __SMLAD(x2, c0, acc1);
        acc2 = __SMLADX(x2, c0, acc2);
        acc3 = __SMLADX(x3, c0, acc3);
      }

      /* Store the results in the accumulators in the destination buffer. */
#ifndef ARM_MATH_BIG_ENDIAN

      *__SIMD32(pOut)++ = __PKHBT((acc0 >> 15), (acc1 >> 15), 16);
      *__SIMD32(pOut)++ = __PKHBT((acc2 >> 15), (acc3 >> 15), 16);

#else

      *__SIMD32(pOut)++ = __PKHBT((acc1 >> 15), (acc0 >> 15), 16);
      *__SIMD32(pOut)++ = __PKHBT((acc3 >> 15), (acc2 >> 15), 16);

#endif /*      #ifndef  ARM_MATH_BIG_ENDIAN    */
      /* Update the inputA and inputB pointers for next MAC calculation */
      px = pIn1 + (count * 4u);
      py = pSrc2;
      pb = (q31_t *) (py - 1);

      /* Increment the pointer pIn1 index, count by 1 */
      count++;

      /* Decrement the loop counter */
      blkCnt--;
    }

    /* If the blockSize2 is not a multiple of 4, compute any remaining output samples here.   
     ** No loop unrolling is used. */
    blkCnt = blockSize2 % 0x4u;

    while(blkCnt > 0u)
    {
      /* Accumulator is made zero for every iteration */
      sum = 0;

      /* Apply loop unrolling and compute 4 MACs simultaneously. */
      k = srcBLen >> 2u;

      /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.   
       ** a second loop below computes MACs for the remaining 1 to 3 samples. */
      while(k > 0u)
      {
        /* Perform the multiply-accumulates */
        sum += ((q31_t) * px++ * *py--);
        sum += ((q31_t) * px++ * *py--);
        sum += ((q31_t) * px++ * *py--);
        sum += ((q31_t) * px++ * *py--);

        /* Decrement the loop counter */
        k--;
      }

      /* If the srcBLen is not a multiple of 4, compute any remaining MACs here.   
       ** No loop unrolling is used. */
      k = srcBLen % 0x4u;

      while(k > 0u)
      {
        /* Perform the multiply-accumulates */
        sum += ((q31_t) * px++ * *py--);

        /* Decrement the loop counter */
        k--;
      }

      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = (q15_t) (sum >> 15);

      /* Update the inputA and inputB pointers for next MAC calculation */
      px = pIn1 + count;
      py = pSrc2;

      /* Increment the pointer pIn1 index, count by 1 */
      count++;

      /* Decrement the loop counter */
      blkCnt--;
    }
  }
  else
  {
    /* If the srcBLen is not a multiple of 4,   
     * the blockSize2 loop cannot be unrolled by 4 */
    blkCnt = blockSize2;

    while(blkCnt > 0u)
    {
      /* Accumulator is made zero for every iteration */
      sum = 0;

      /* srcBLen number of MACS should be performed */
      k = srcBLen;

      while(k > 0u)
      {
        /* Perform the multiply-accumulate */
        sum += ((q31_t) * px++ * *py--);

        /* Decrement the loop counter */
        k--;
      }

      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = (q15_t) (sum >> 15);

      /* Update the inputA and inputB pointers for next MAC calculation */
      px = pIn1 + count;
      py = pSrc2;

      /* Increment the MAC count */
      count++;

      /* Decrement the loop counter */
      blkCnt--;
    }
  }


  /* --------------------------   
   * Initializations of stage3   
   * -------------------------*/

  /* sum += x[srcALen-srcBLen+1] * y[srcBLen-1] + x[srcALen-srcBLen+2] * y[srcBLen-2] +...+ x[srcALen-1] * y[1]   
   * sum += x[srcALen-srcBLen+2] * y[srcBLen-1] + x[srcALen-srcBLen+3] * y[srcBLen-2] +...+ x[srcALen-1] * y[2]   
   * ....   
   * sum +=  x[srcALen-2] * y[srcBLen-1] + x[srcALen-1] * y[srcBLen-2]   
   * sum +=  x[srcALen-1] * y[srcBLen-1]   
   */

  /* In this stage the MAC operations are decreased by 1 for every iteration.   
     The blockSize3 variable holds the number of MAC operations performed */

  /* Working pointer of inputA */
  pSrc1 = (pIn1 + srcALen) - (srcBLen - 1u);
  px = pSrc1;

  /* Working pointer of inputB */
  pSrc2 = pIn2 + (srcBLen - 1u);
  pIn2 = pSrc2 - 1u;
  py = pIn2;

  /* -------------------   
   * Stage3 process   
   * ------------------*/

  /* For loop unrolling by 4, this stage is divided into two. */
  /* First part of this stage computes the MAC operations greater than 4 */
  /* Second part of this stage computes the MAC operations less than or equal to 4 */

  /* The first part of the stage starts here */
  j = blockSize3 >> 2u;

  while((j > 0u) && (blockSize3 > 0u))
  {
    /* Accumulator is made zero for every iteration */
    sum = 0;

    /* Apply loop unrolling and compute 4 MACs simultaneously. */
    k = blockSize3 >> 2u;

    /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.   
     ** a second loop below computes MACs for the remaining 1 to 3 samples. */
    while(k > 0u)
    {
      /* x[srcALen - srcBLen + 1], x[srcALen - srcBLen + 2] are multiplied   
       * with y[srcBLen - 1], y[srcBLen - 2] respectively */
      sum = __SMLADX(*__SIMD32(px)++, *__SIMD32(py)--, sum);
      /* x[srcALen - srcBLen + 3], x[srcALen - srcBLen + 4] are multiplied   
       * with y[srcBLen - 3], y[srcBLen - 4] respectively */
      sum = __SMLADX(*__SIMD32(px)++, *__SIMD32(py)--, sum);

      /* Decrement the loop counter */
      k--;
    }

    /* For the next MAC operations, the pointer py is used without SIMD   
     * So, py is incremented by 1 */
    py = py + 1u;

    /* If the blockSize3 is not a multiple of 4, compute any remaining MACs here.   
     ** No loop unrolling is used. */
    k = blockSize3 % 0x4u;

    while(k > 0u)
    {
      /* sum += x[srcALen - srcBLen + 5] * y[srcBLen - 5] */
      sum = __SMLAD(*px++, *py--, sum);

      /* Decrement the loop counter */
      k--;
    }

    /* Store the result in the accumulator in the destination buffer. */
    *pOut++ = (q15_t) (sum >> 15);

    /* Update the inputA and inputB pointers for next MAC calculation */
    px = ++pSrc1;
    py = pIn2;

    /* Decrement the loop counter */
    blockSize3--;

    j--;
  }

  /* The second part of the stage starts here */
  /* SIMD is not used for the next MAC operations,   
   * so pointer py is updated to read only one sample at a time */
  py = py + 1u;

  while(blockSize3 > 0u)
  {
    /* Accumulator is made zero for every iteration */
    sum = 0;

    /* Apply loop unrolling and compute 4 MACs simultaneously. */
    k = blockSize3;

    while(k > 0u)
    {
      /* Perform the multiply-accumulates */
      /* sum +=  x[srcALen-1] * y[srcBLen-1] */
      sum = __SMLAD(*px++, *py--, sum);

      /* Decrement the loop counter */
      k--;
    }

    /* Store the result in the accumulator in the destination buffer. */
    *pOut++ = (q15_t) (sum >> 15);

    /* Update the inputA and inputB pointers for next MAC calculation */
    px = ++pSrc1;
    py = pSrc2;

    /* Decrement the loop counter */
    blockSize3--;
  }

}

/**   
 * @} end of Conv group   
 */
