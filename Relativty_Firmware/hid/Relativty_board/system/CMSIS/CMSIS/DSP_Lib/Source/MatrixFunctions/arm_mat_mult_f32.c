/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_mat_mult_f32.c   
*   
* Description:  Floating-point matrix multiplication.   
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
 * @defgroup MatrixMult Matrix Multiplication   
 *   
 * Multiplies two matrices.   
 *   
 * \image html MatrixMultiplication.gif "Multiplication of two 3 x 3 matrices"   
   
 * Matrix multiplication is only defined if the number of columns of the   
 * first matrix equals the number of rows of the second matrix.   
 * Multiplying an <code>M x N</code> matrix with an <code>N x P</code> matrix results   
 * in an <code>M x P</code> matrix.   
 * When matrix size checking is enabled, the functions check: (1) that the inner dimensions of   
 * <code>pSrcA</code> and <code>pSrcB</code> are equal; and (2) that the size of the output   
 * matrix equals the outer dimensions of <code>pSrcA</code> and <code>pSrcB</code>.   
 */


/**   
 * @addtogroup MatrixMult   
 * @{   
 */

/**   
 * @brief Floating-point matrix multiplication.   
 * @param[in]       *pSrcA points to the first input matrix structure   
 * @param[in]       *pSrcB points to the second input matrix structure   
 * @param[out]      *pDst points to output matrix structure   
 * @return     		The function returns either   
 * <code>ARM_MATH_SIZE_MISMATCH</code> or <code>ARM_MATH_SUCCESS</code> based on the outcome of size checking.   
 */

arm_status arm_mat_mult_f32(
  const arm_matrix_instance_f32 * pSrcA,
  const arm_matrix_instance_f32 * pSrcB,
  arm_matrix_instance_f32 * pDst)
{
  float32_t *pIn1 = pSrcA->pData;                /* input data matrix pointer A */
  float32_t *pIn2 = pSrcB->pData;                /* input data matrix pointer B */
  float32_t *pInA = pSrcA->pData;                /* input data matrix pointer A  */
  float32_t *pOut = pDst->pData;                 /* output data matrix pointer */
  float32_t *px;                                 /* Temporary output data matrix pointer */
  float32_t sum;                                 /* Accumulator */
  uint16_t numRowsA = pSrcA->numRows;            /* number of rows of input matrix A */
  uint16_t numColsB = pSrcB->numCols;            /* number of columns of input matrix B */
  uint16_t numColsA = pSrcA->numCols;            /* number of columns of input matrix A */

#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  uint16_t col, i = 0u, j, row = numRowsA, colCnt;      /* loop counters */
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
#endif /*      #ifdef ARM_MATH_MATRIX_CHECK    */

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

      j = 0u;

      /* column loop */
      do
      {
        /* Set the variable sum, that acts as accumulator, to zero */
        sum = 0.0f;

        /* Initiate the pointer pIn1 to point to the starting address of the column being processed */
        pIn1 = pInA;

        /* Apply loop unrolling and compute 4 MACs simultaneously. */
        colCnt = numColsA >> 2;

        /* matrix multiplication        */
        while(colCnt > 0u)
        {
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
          sum += *pIn1++ * (*pIn2);
          pIn2 += numColsB;
          sum += *pIn1++ * (*pIn2);
          pIn2 += numColsB;
          sum += *pIn1++ * (*pIn2);
          pIn2 += numColsB;
          sum += *pIn1++ * (*pIn2);
          pIn2 += numColsB;

          /* Decrement the loop count */
          colCnt--;
        }

        /* If the columns of pSrcA is not a multiple of 4, compute any remaining MACs here.   
         ** No loop unrolling is used. */
        colCnt = numColsA % 0x4u;

        while(colCnt > 0u)
        {
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
          sum += *pIn1++ * (*pIn2);
          pIn2 += numColsB;

          /* Decrement the loop counter */
          colCnt--;
        }

        /* Store the result in the destination buffer */
        *px++ = sum;

        /* Update the pointer pIn2 to point to the  starting address of the next column */
        j++;
        pIn2 = pSrcB->pData + j;

        /* Decrement the column loop counter */
        col--;

      } while(col > 0u);

#else

  /* Run the below code for Cortex-M0 */

  float32_t *pInB = pSrcB->pData;                /* input data matrix pointer B */
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
#endif /*      #ifdef ARM_MATH_MATRIX_CHECK    */

  {
    /* The following loop performs the dot-product of each row in pInA with each column in pInB */
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
        sum = 0.0f;

        /* Initialize the pointer pIn1 to point to the starting address of the row being processed */
        pIn1 = pInA;

        /* Matrix A columns number of MAC operations are to be performed */
        colCnt = numColsA;

        while(colCnt > 0u)
        {
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
          sum += *pIn1++ * (*pIn2);
          pIn2 += numColsB;

          /* Decrement the loop counter */
          colCnt--;
        }

        /* Store the result in the destination buffer */
        *px++ = sum;

        /* Decrement the column loop counter */
        col--;

        /* Update the pointer pIn2 to point to the  starting address of the next column */
        pIn2 = pInB + (numColsB - col);

      } while(col > 0u);

#endif /* #ifndef ARM_MATH_CM0 */

      /* Update the pointer pInA to point to the  starting address of the next row */
      i = i + numColsB;
      pInA = pInA + numColsA;

      /* Decrement the row loop counter */
      row--;

    } while(row > 0u);
    /* Set status as ARM_MATH_SUCCESS */
    status = ARM_MATH_SUCCESS;
  }

  /* Return to application */
  return (status);
}

/**   
 * @} end of MatrixMult group   
 */
