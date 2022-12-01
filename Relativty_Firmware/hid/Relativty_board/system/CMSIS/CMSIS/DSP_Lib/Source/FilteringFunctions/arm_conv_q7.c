/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_conv_q7.c   
*   
* Description:	Convolution of Q7 sequences. 
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
*    Production release and review comments incorporated   
*   
* Version 0.0.7  2010/06/10    
*    Misra-C changes done   
*   
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
 * @brief Convolution of Q7 sequences.   
 * @param[in] *pSrcA points to the first input sequence.   
 * @param[in] srcALen length of the first input sequence.   
 * @param[in] *pSrcB points to the second input sequence.   
 * @param[in] srcBLen length of the second input sequence.   
 * @param[out] *pDst points to the location where the output result is written.  Length srcALen+srcBLen-1.   
 * @return none.   
 *   
 * @details   
 * <b>Scaling and Overflow Behavior:</b>   
 *   
 * \par   
 * The function is implemented using a 32-bit internal accumulator.   
 * Both the inputs are represented in 1.7 format and multiplications yield a 2.14 result.   
 * The 2.14 intermediate results are accumulated in a 32-bit accumulator in 18.14 format.   
 * This approach provides 17 guard bits and there is no risk of overflow as long as <code>max(srcALen, srcBLen)<131072</code>.   
 * The 18.14 result is then truncated to 18.7 format by discarding the low 7 bits and then saturated to 1.7 format.   
 */

void arm_conv_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst)
{


#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  q7_t *pIn1;                                    /* inputA pointer */
  q7_t *pIn2;                                    /* inputB pointer */
  q7_t *pOut = pDst;                             /* output pointer */
  q7_t *px;                                      /* Intermediate inputA pointer */
  q7_t *py;                                      /* Intermediate inputB pointer */
  q7_t *pSrc1, *pSrc2;                           /* Intermediate pointers */
  q7_t x0, x1, x2, x3, c0, c1;                   /* Temporary variables to hold state and coefficient values */
  q31_t sum, acc0, acc1, acc2, acc3;             /* Accumulator */
  q31_t input1, input2;                          /* Temporary input variables */
  q15_t in1, in2;                                /* Temporary input variables */
  uint32_t j, k, count, blkCnt, blockSize1, blockSize2, blockSize3;     /* loop counter */


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
  blockSize2 = (srcALen - srcBLen) + 1u;
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

  /* The first stage starts here */
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
      /* x[0] , x[1] */
      in1 = (q15_t) * px++;
      in2 = (q15_t) * px++;
      input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

      /* y[srcBLen - 1] , y[srcBLen - 2] */
      in1 = (q15_t) * py--;
      in2 = (q15_t) * py--;
      input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

      /* x[0] * y[srcBLen - 1] */
      /* x[1] * y[srcBLen - 2] */
      sum = __SMLAD(input1, input2, sum);

      /* x[2] , x[3] */
      in1 = (q15_t) * px++;
      in2 = (q15_t) * px++;
      input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

      /* y[srcBLen - 3] , y[srcBLen - 4] */
      in1 = (q15_t) * py--;
      in2 = (q15_t) * py--;
      input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

      /* x[2] * y[srcBLen - 3] */
      /* x[3] * y[srcBLen - 4] */
      sum = __SMLAD(input1, input2, sum);

      /* Decrement the loop counter */
      k--;
    }

    /* If the count is not a multiple of 4, compute any remaining MACs here.   
     ** No loop unrolling is used. */
    k = count % 0x4u;

    while(k > 0u)
    {
      /* Perform the multiply-accumulates */
      sum += ((q15_t) * px++ * *py--);

      /* Decrement the loop counter */
      k--;
    }

    /* Store the result in the accumulator in the destination buffer. */
    *pOut++ = (q7_t) (__SSAT(sum >> 7u, 8));

    /* Update the inputA and inputB pointers for next MAC calculation */
    py = pIn2 + count;
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

  /* count is index by which the pointer pIn1 to be incremented */
  count = 1u;

  /* -------------------   
   * Stage2 process   
   * ------------------*/

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

      /* read x[0], x[1], x[2] samples */
      x0 = *(px++);
      x1 = *(px++);
      x2 = *(px++);

      /* Apply loop unrolling and compute 4 MACs simultaneously. */
      k = srcBLen >> 2u;

      /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.   
       ** a second loop below computes MACs for the remaining 1 to 3 samples. */
      do
      {
        /* Read y[srcBLen - 1] sample */
        c0 = *(py--);
        /* Read y[srcBLen - 2] sample */
        c1 = *(py--);

        /* Read x[3] sample */
        x3 = *(px++);

        /* x[0] and x[1] are packed */
        in1 = (q15_t) x0;
        in2 = (q15_t) x1;

        input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* y[srcBLen - 1]   and y[srcBLen - 2] are packed */
        in1 = (q15_t) c0;
        in2 = (q15_t) c1;

        input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* acc0 += x[0] * y[srcBLen - 1] + x[1] * y[srcBLen - 2]  */
        acc0 = __SMLAD(input1, input2, acc0);

        /* x[1] and x[2] are packed */
        in1 = (q15_t) x1;
        in2 = (q15_t) x2;

        input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* acc1 += x[1] * y[srcBLen - 1] + x[2] * y[srcBLen - 2]  */
        acc1 = __SMLAD(input1, input2, acc1);

        /* x[2] and x[3] are packed */
        in1 = (q15_t) x2;
        in2 = (q15_t) x3;

        input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* acc2 += x[2] * y[srcBLen - 1] + x[3] * y[srcBLen - 2]  */
        acc2 = __SMLAD(input1, input2, acc2);

        /* Read x[4] sample */
        x0 = *(px++);

        /* x[3] and x[4] are packed */
        in1 = (q15_t) x3;
        in2 = (q15_t) x0;

        input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* acc3 += x[3] * y[srcBLen - 1] + x[4] * y[srcBLen - 2]  */
        acc3 = __SMLAD(input1, input2, acc3);

        /* Read y[srcBLen - 3] sample */
        c0 = *(py--);
        /* Read y[srcBLen - 4] sample */
        c1 = *(py--);

        /* Read x[5] sample */
        x1 = *(px++);

        /* x[2] and x[3] are packed */
        in1 = (q15_t) x2;
        in2 = (q15_t) x3;

        input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* y[srcBLen - 3] and y[srcBLen - 4] are packed */
        in1 = (q15_t) c0;
        in2 = (q15_t) c1;

        input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* acc0 += x[2] * y[srcBLen - 3] + x[3] * y[srcBLen - 4]  */
        acc0 = __SMLAD(input1, input2, acc0);

        /* x[3] and x[4] are packed */
        in1 = (q15_t) x3;
        in2 = (q15_t) x0;

        input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* acc1 += x[3] * y[srcBLen - 3] + x[4] * y[srcBLen - 4]  */
        acc1 = __SMLAD(input1, input2, acc1);

        /* x[4] and x[5] are packed */
        in1 = (q15_t) x0;
        in2 = (q15_t) x1;

        input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* acc2 += x[4] * y[srcBLen - 3] + x[5] * y[srcBLen - 4]  */
        acc2 = __SMLAD(input1, input2, acc2);

        /* Read x[6] sample */
        x2 = *(px++);

        /* x[5] and x[6] are packed */
        in1 = (q15_t) x1;
        in2 = (q15_t) x2;

        input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* acc3 += x[5] * y[srcBLen - 3] + x[6] * y[srcBLen - 4]  */
        acc3 = __SMLAD(input1, input2, acc3);

      } while(--k);

      /* If the srcBLen is not a multiple of 4, compute any remaining MACs here.   
       ** No loop unrolling is used. */
      k = srcBLen % 0x4u;

      while(k > 0u)
      {
        /* Read y[srcBLen - 5] sample */
        c0 = *(py--);

        /* Read x[7] sample */
        x3 = *(px++);

        /* Perform the multiply-accumulates */
        /* acc0 +=  x[4] * y[srcBLen - 5] */
        acc0 += ((q15_t) x0 * c0);
        /* acc1 +=  x[5] * y[srcBLen - 5] */
        acc1 += ((q15_t) x1 * c0);
        /* acc2 +=  x[6] * y[srcBLen - 5] */
        acc2 += ((q15_t) x2 * c0);
        /* acc3 +=  x[7] * y[srcBLen - 5] */
        acc3 += ((q15_t) x3 * c0);

        /* Reuse the present samples for the next MAC */
        x0 = x1;
        x1 = x2;
        x2 = x3;

        /* Decrement the loop counter */
        k--;
      }


      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = (q7_t) (__SSAT(acc0 >> 7u, 8));
      *pOut++ = (q7_t) (__SSAT(acc1 >> 7u, 8));
      *pOut++ = (q7_t) (__SSAT(acc2 >> 7u, 8));
      *pOut++ = (q7_t) (__SSAT(acc3 >> 7u, 8));

      /* Update the inputA and inputB pointers for next MAC calculation */
      px = pIn1 + (count * 4u);
      py = pSrc2;

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

        /* Reading two inputs of SrcA buffer and packing */
        in1 = (q15_t) * px++;
        in2 = (q15_t) * px++;
        input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* Reading two inputs of SrcB buffer and packing */
        in1 = (q15_t) * py--;
        in2 = (q15_t) * py--;
        input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* Perform the multiply-accumulates */
        sum = __SMLAD(input1, input2, sum);

        /* Reading two inputs of SrcA buffer and packing */
        in1 = (q15_t) * px++;
        in2 = (q15_t) * px++;
        input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* Reading two inputs of SrcB buffer and packing */
        in1 = (q15_t) * py--;
        in2 = (q15_t) * py--;
        input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

        /* Perform the multiply-accumulates */
        sum = __SMLAD(input1, input2, sum);

        /* Decrement the loop counter */
        k--;
      }

      /* If the srcBLen is not a multiple of 4, compute any remaining MACs here.   
       ** No loop unrolling is used. */
      k = srcBLen % 0x4u;

      while(k > 0u)
      {
        /* Perform the multiply-accumulates */
        sum += ((q15_t) * px++ * *py--);

        /* Decrement the loop counter */
        k--;
      }

      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = (q7_t) (__SSAT(sum >> 7u, 8));

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
        sum += ((q15_t) * px++ * *py--);

        /* Decrement the loop counter */
        k--;
      }

      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = (q7_t) (__SSAT(sum >> 7u, 8));

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
  pSrc1 = pIn1 + (srcALen - (srcBLen - 1u));
  px = pSrc1;

  /* Working pointer of inputB */
  pSrc2 = pIn2 + (srcBLen - 1u);
  py = pSrc2;

  /* -------------------   
   * Stage3 process   
   * ------------------*/

  while(blockSize3 > 0u)
  {
    /* Accumulator is made zero for every iteration */
    sum = 0;

    /* Apply loop unrolling and compute 4 MACs simultaneously. */
    k = blockSize3 >> 2u;

    /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.   
     ** a second loop below computes MACs for the remaining 1 to 3 samples. */
    while(k > 0u)
    {
      /* Reading two inputs, x[srcALen - srcBLen + 1] and x[srcALen - srcBLen + 2] of SrcA buffer and packing */
      in1 = (q15_t) * px++;
      in2 = (q15_t) * px++;
      input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

      /* Reading two inputs, y[srcBLen - 1] and y[srcBLen - 2] of SrcB buffer and packing */
      in1 = (q15_t) * py--;
      in2 = (q15_t) * py--;
      input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

      /* sum += x[srcALen - srcBLen + 1] * y[srcBLen - 1] */
      /* sum += x[srcALen - srcBLen + 2] * y[srcBLen - 2] */
      sum = __SMLAD(input1, input2, sum);

      /* Reading two inputs, x[srcALen - srcBLen + 3] and x[srcALen - srcBLen + 4] of SrcA buffer and packing */
      in1 = (q15_t) * px++;
      in2 = (q15_t) * px++;
      input1 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

      /* Reading two inputs, y[srcBLen - 3] and y[srcBLen - 4] of SrcB buffer and packing */
      in1 = (q15_t) * py--;
      in2 = (q15_t) * py--;
      input2 = ((q31_t) in1 & 0x0000FFFF) | ((q31_t) in2 << 16u);

      /* sum += x[srcALen - srcBLen + 3] * y[srcBLen - 3] */
      /* sum += x[srcALen - srcBLen + 4] * y[srcBLen - 4] */
      sum = __SMLAD(input1, input2, sum);

      /* Decrement the loop counter */
      k--;
    }

    /* If the blockSize3 is not a multiple of 4, compute any remaining MACs here.   
     ** No loop unrolling is used. */
    k = blockSize3 % 0x4u;

    while(k > 0u)
    {
      /* Perform the multiply-accumulates */
      sum += ((q15_t) * px++ * *py--);

      /* Decrement the loop counter */
      k--;
    }

    /* Store the result in the accumulator in the destination buffer. */
    *pOut++ = (q7_t) (__SSAT(sum >> 7u, 8));

    /* Update the inputA and inputB pointers for next MAC calculation */
    px = ++pSrc1;
    py = pSrc2;

    /* Decrement the loop counter */
    blockSize3--;
  }

#else

  /* Run the below code for Cortex-M0 */

  q7_t *pIn1 = pSrcA;                            /* input pointer */
  q7_t *pIn2 = pSrcB;                            /* coefficient pointer */
  q31_t sum;                                     /* Accumulator */
  uint32_t i, j;                                 /* loop counter */

  /* Loop to calculate output of convolution for output length number of times */
  for (i = 0; i < (srcALen + srcBLen - 1); i++)
  {
    /* Initialize sum with zero to carry on MAC operations */
    sum = 0;

    /* Loop to perform MAC operations according to convolution equation */
    for (j = 0; j <= i; j++)
    {
      /* Check the array limitations */
      if(((i - j) < srcBLen) && (j < srcALen))
      {
        /* z[i] += x[i-j] * y[j] */
        sum += (q15_t) pIn1[j] * (pIn2[i - j]);
      }
    }

    /* Store the output in the destination buffer */
    pDst[i] = (q7_t) __SSAT((sum >> 7u), 8u);
  }

#endif /*   #ifndef ARM_MATH_CM0        */

}

/**   
 * @} end of Conv group   
 */
