/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_mat_mult_q15.c   
*   
* Description:	 Q15 matrix multiplication.   
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
*   
* Version 0.0.5  2010/04/26    
*    incorporated review comments and updated with latest CMSIS layer   
*   
* Version 0.0.3  2010/03/10    
*    Initial version   
* -------------------------------------------------------------------- */

#include "arm_math.h"

/**   
 * @ingroup groupMatrix   
 */

/**   
 * @addtogroup MatrixMult   
 * @{   
 */


/**   
 * @brief Q15 matrix multiplication   
 * @param[in]       *pSrcA points to the first input matrix structure   
 * @param[in]       *pSrcB points to the second input matrix structure   
 * @param[out]      *pDst points to output matrix structure   
 * @param[in]		*pState points to the array for storing intermediate results  
 * @return     		The function returns either   
 * <code>ARM_MATH_SIZE_MISMATCH</code> or <code>ARM_MATH_SUCCESS</code> based on the outcome of size checking.   
 *   
 * @details   
 * <b>Scaling and Overflow Behavior:</b>   
 *   
 * \par   
 * The function is implemented using a 64-bit internal accumulator. The inputs to the   
 * multiplications are in 1.15 format and multiplications yield a 2.30 result.   
 * The 2.30 intermediate   
 * results are accumulated in a 64-bit accumulator in 34.30 format. This approach   
 * provides 33 guard bits and there is no risk of overflow. The 34.30 result is then   
 * truncated to 34.15 format by discarding the low 15 bits and then saturated to   
 * 1.15 format.   
 *   
 * \par   
 * Refer to <code>arm_mat_mult_fast_q15()</code> for a faster but less precise version of this function for Cortex-M3 and Cortex-M4.   
 *   
 */

arm_status arm_mat_mult_q15(
  const arm_matrix_instance_q15 * pSrcA,
  const arm_matrix_instance_q15 * pSrcB,
  arm_matrix_instance_q15 * pDst,
  q15_t * pState)
{
  q63_t sum;                                     /* accumulator */

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  q31_t in;                                      /* Temporary variable to hold the input value */
  q15_t *pSrcBT = pState;                        /* input data matrix pointer for transpose */
  q15_t *pInA = pSrcA->pData;                    /* input data matrix pointer A of Q15 type */
  q15_t *pInB = pSrcB->pData;                    /* input data matrix pointer B of Q15 type */
  q15_t *px;                                     /* Temporary output data matrix pointer */
  uint16_t numRowsA = pSrcA->numRows;            /* number of rows of input matrix A    */
  uint16_t numColsB = pSrcB->numCols;            /* number of columns of input matrix B */
  uint16_t numColsA = pSrcA->numCols;            /* number of columns of input matrix A */
  uint16_t numRowsB = pSrcB->numRows;            /* number of rows of input matrix A    */
  uint16_t col, i = 0u, row = numRowsB, colCnt;  /* loop counters */
  arm_status status;                             /* status of matrix multiplication */

#ifdef ARM_MATH_MATRIX_CHECK


  /* Check for matrix mismatch condition */

  if((pSrcA->numCols != pSrcB->numRows) ||
     (pSrcA->numRows != pDst->numRows) || (pSrcB->numCols != pDst->numCols))
  {
    /* Set status as ARM_MATH_SIZE_MISMATCH */
    status = ARM_MATH_SIZE_MISMATCH;
  }
  else
#endif /*    #ifdef ARM_MATH_MATRIX_CHECK    */

  {
    /* Matrix transpose */
    do
    {
      /* Apply loop unrolling and exchange the columns with row elements */
      col = numColsB >> 2;

      /* The pointer px is set to starting address of the column being processed */
      px = pSrcBT + i;

      /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.   
       ** a second loop below computes the remaining 1 to 3 samples. */
      while(col > 0u)
      {
        /* Read two elements from the row */
        in = *__SIMD32(pInB)++;

        /* Unpack and store one element in the destination */
#ifndef ARM_MATH_BIG_ENDIAN

        *px = (q15_t) in;

#else

        *px = (q15_t) ((in & (q31_t) 0xffff0000) >> 16);

#endif /*    #ifndef ARM_MATH_BIG_ENDIAN    */

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB;

        /* Unpack and store the second element in the destination */
#ifndef ARM_MATH_BIG_ENDIAN

        *px = (q15_t) ((in & (q31_t) 0xffff0000) >> 16);

#else

        *px = (q15_t) in;

#endif /*    #ifndef ARM_MATH_BIG_ENDIAN    */


        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB;

        /* Read two elements from the row */
        in = *__SIMD32(pInB)++;

        /* Unpack and store one element in the destination */
#ifndef ARM_MATH_BIG_ENDIAN

        *px = (q15_t) in;

#else

        *px = (q15_t) ((in & (q31_t) 0xffff0000) >> 16);

#endif /*    #ifndef ARM_MATH_BIG_ENDIAN    */

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB;

        /* Unpack and store the second element in the destination */

#ifndef ARM_MATH_BIG_ENDIAN

        *px = (q15_t) ((in & (q31_t) 0xffff0000) >> 16);

#else

        *px = (q15_t) in;

#endif /*    #ifndef ARM_MATH_BIG_ENDIAN    */

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB;

        /* Decrement the column loop counter */
        col--;
      }

      /* If the columns of pSrcB is not a multiple of 4, compute any remaining output samples here.   
       ** No loop unrolling is used. */
      col = numColsB % 0x4u;

      while(col > 0u)
      {
        /* Read and store the input element in the destination */
        *px = *pInB++;

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB;

        /* Decrement the column loop counter */
        col--;
      }

      i++;

      /* Decrement the row loop counter */
      row--;

    } while(row > 0u);

    /* Reset the variables for the usage in the following multiplication process */
    row = numRowsA;
    i = 0u;
    px = pDst->pData;

    /* The following loop performs the dot-product of each row in pSrcA with each column in pSrcB */
    /* row loop */
    do
    {
      /* For every row wise process, the column loop counter is to be initiated */
      col = numColsB;

      /* For every row wise process, the pIn2 pointer is set   
       ** to the starting address of the transposed pSrcB data */
      pInB = pSrcBT;

      /* column loop */
      do
      {
        /* Set the variable sum, that acts as accumulator, to zero */
        sum = 0;

        /* Apply loop unrolling and compute 2 MACs simultaneously. */
        colCnt = numColsA >> 1;

        /* Initiate the pointer pIn1 to point to the starting address of the column being processed */
        pInA = pSrcA->pData + i;

        /* matrix multiplication */
        while(colCnt > 0u)
        {
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
          sum = __SMLALD(*__SIMD32(pInA)++, *__SIMD32(pInB)++, sum);

          /* Decrement the loop counter */
          colCnt--;
        }

        /* process odd column samples */
        if((numColsA & 0x1u) > 0u)
        {
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
          sum += ((q31_t) * pInA * (*pInB++));
        }

        /* Saturate and store the result in the destination buffer */
        *px = (q15_t) (__SSAT((sum >> 15), 16));
        px++;

        /* Decrement the column loop counter */
        col--;

      } while(col > 0u);

      i = i + numColsA;

      /* Decrement the row loop counter */
      row--;

    } while(row > 0u);

#else

  /* Run the below code for Cortex-M0 */

  q15_t *pIn1 = pSrcA->pData;                    /* input data matrix pointer A */
  q15_t *pIn2 = pSrcB->pData;                    /* input data matrix pointer B */
  q15_t *pInA = pSrcA->pData;                    /* input data matrix pointer A of Q15 type */
  q15_t *pInB = pSrcB->pData;                    /* input data matrix pointer B of Q15 type */
  q15_t *pOut = pDst->pData;                     /* output data matrix pointer */
  q15_t *px;                                     /* Temporary output data matrix pointer */
  uint16_t numColsB = pSrcB->numCols;            /* number of columns of input matrix B */
  uint16_t numColsA = pSrcA->numCols;            /* number of columns of input matrix A */
  uint16_t numRowsA = pSrcA->numRows;            /* number of rows of input matrix A    */
  uint16_t col, i = 0u, row = numRowsA, colCnt;  /* loop counters */
  arm_status status;                             /* status of matrix multiplication */

#ifdef ARM_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if((pSrcA->numCols != pSrcB->numRows) ||
     (pSrcA->numRows != pDst->numRows) || (pSrcB->numCols != pDst->numCols))
  {
    /* Set status as ARM_MATH_SIZE_MISMATCH */
    status = ARM_MATH_SIZE_MISMATCH;
  }
  else
#endif /*    #ifdef ARM_MATH_MATRIX_CHECK    */

  {
    /* The following loop performs the dot-product of each row in pSrcA with each column in pSrcB */
    /* row loop */
    do
    {
      /* Output pointer is set to starting address of the row being processed */
      px = pOut + i;

      /* For every row wise process, the column loop counter is to be initiated */
      col = numColsB;

      /* For every row wise process, the pIn2 pointer is set         
       ** to the starting address of the pSrcB data */
      pIn2 = pSrcB->pData;

      /* column loop */
      do
      {
        /* Set the variable sum, that acts as accumulator, to zero */
        sum = 0;

        /* Initiate the pointer pIn1 to point to the starting address of pSrcA */
        pIn1 = pInA;

        /* Matrix A columns number of MAC operations are to be performed */
        colCnt = numColsA;

        /* matrix multiplication */
        while(colCnt > 0u)
        {
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
          /* Perform the multiply-accumulates */
          sum += (q31_t) * pIn1++ * *pIn2;
          pIn2 += numColsB;

          /* Decrement the loop counter */
          colCnt--;
        }

        /* Convert the result from 34.30 to 1.15 format and store the saturated value in destination buffer */
        /* Saturate and store the result in the destination buffer */
        *px++ = (q15_t) __SSAT((sum >> 15), 16);

        /* Decrement the column loop counter */
        col--;

        /* Update the pointer pIn2 to point to the  starting address of the next column */
        pIn2 = pInB + (numColsB - col);

      } while(col > 0u);

      /* Update the pointer pSrcA to point to the  starting address of the next row */
      i = i + numColsB;
      pInA = pInA + numColsA;

      /* Decrement the row loop counter */
      row--;

    } while(row > 0u);

#endif /* #ifndef ARM_MATH_CM0 */

    /* set status as ARM_MATH_SUCCESS */
    status = ARM_MATH_SUCCESS;
  }

  /* Return to application */
  return (status);
}

/**   
 * @} end of MatrixMult group   
 */
