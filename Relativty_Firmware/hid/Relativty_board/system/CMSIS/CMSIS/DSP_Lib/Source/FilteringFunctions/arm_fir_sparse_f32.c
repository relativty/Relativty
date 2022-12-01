/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_fir_sparse_f32.c   
*   
* Description:	Floating-point sparse FIR filter processing function.  
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
* ------------------------------------------------------------------- */
#include "arm_math.h"

/**   
 * @ingroup groupFilters   
 */

/**   
 * @defgroup FIR_Sparse Finite Impulse Response (FIR) Sparse Filters   
 *   
 * This group of functions implements sparse FIR filters.    
 * Sparse FIR filters are equivalent to standard FIR filters except that most of the coefficients are equal to zero.  
 * Sparse filters are used for simulating reflections in communications and audio applications.  
 *  
 * There are separate functions for Q7, Q15, Q31, and floating-point data types.   
 * The functions operate on blocks  of input and output data and each call to the function processes   
 * <code>blockSize</code> samples through the filter.  <code>pSrc</code> and   
 * <code>pDst</code> points to input and output arrays respectively containing <code>blockSize</code> values.   
 *   
 * \par Algorithm:   
 * The sparse filter instant structure contains an array of tap indices <code>pTapDelay</code> which specifies the locations of the non-zero coefficients.  
 * This is in addition to the coefficient array <code>b</code>.  
 * The implementation essentially skips the multiplications by zero and leads to an efficient realization.  
 * <pre>  
 *     y[n] = b[0] * x[n-pTapDelay[0]] + b[1] * x[n-pTapDelay[1]] + b[2] * x[n-pTapDelay[2]] + ...+ b[numTaps-1] * x[n-pTapDelay[numTaps-1]]   
 * </pre>   
 * \par   
 * \image html FIRSparse.gif "Sparse FIR filter.  b[n] represents the filter coefficients"  
 * \par   
 * <code>pCoeffs</code> points to a coefficient array of size <code>numTaps</code>;   
 * <code>pTapDelay</code> points to an array of nonzero indices and is also of size <code>numTaps</code>;  
 * <code>pState</code> points to a state array of size <code>maxDelay + blockSize</code>, where  
 * <code>maxDelay</code> is the largest offset value that is ever used in the <code>pTapDelay</code> array.  
 * Some of the processing functions also require temporary working buffers.  
 *  
 * \par Instance Structure   
 * The coefficients and state variables for a filter are stored together in an instance data structure.   
 * A separate instance structure must be defined for each filter.   
 * Coefficient and offset arrays may be shared among several instances while state variable arrays cannot be shared.   
 * There are separate instance structure declarations for each of the 4 supported data types.   
 *   
 * \par Initialization Functions   
 * There is also an associated initialization function for each data type.   
 * The initialization function performs the following operations:   
 * - Sets the values of the internal structure fields.   
 * - Zeros out the values in the state buffer.   
 *   
 * \par   
 * Use of the initialization function is optional.   
 * However, if the initialization function is used, then the instance structure cannot be placed into a const data section.   
 * To place an instance structure into a const data section, the instance structure must be manually initialized.   
 * Set the values in the state buffer to zeros before static initialization.   
 * The code below statically initializes each of the 4 different data type filter instance structures   
 * <pre>   
 *arm_fir_sparse_instance_f32 S = {numTaps, 0, pState, pCoeffs, maxDelay, pTapDelay};   
 *arm_fir_sparse_instance_q31 S = {numTaps, 0, pState, pCoeffs, maxDelay, pTapDelay};   
 *arm_fir_sparse_instance_q15 S = {numTaps, 0, pState, pCoeffs, maxDelay, pTapDelay};   
 *arm_fir_sparse_instance_q7 S =  {numTaps, 0, pState, pCoeffs, maxDelay, pTapDelay};   
 * </pre>   
 * \par   
 *   
 * \par Fixed-Point Behavior   
 * Care must be taken when using the fixed-point versions of the sparse FIR filter functions.   
 * In particular, the overflow and saturation behavior of the accumulator used in each function must be considered.   
 * Refer to the function specific documentation below for usage guidelines.   
 */

/**   
 * @addtogroup FIR_Sparse   
 * @{   
 */

/**  
 * @brief Processing function for the floating-point sparse FIR filter.  
 * @param[in]  *S          points to an instance of the floating-point sparse FIR structure.  
 * @param[in]  *pSrc       points to the block of input data.  
 * @param[out] *pDst       points to the block of output data  
 * @param[in]  *pScratchIn points to a temporary buffer of size blockSize.  
 * @param[in]  blockSize   number of input samples to process per call.  
 * @return none.  
 */

void arm_fir_sparse_f32(
  arm_fir_sparse_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  float32_t * pScratchIn,
  uint32_t blockSize)
{

  float32_t *pState = S->pState;                 /* State pointer */
  float32_t *pCoeffs = S->pCoeffs;               /* Coefficient pointer */
  float32_t *px;                                 /* Scratch buffer pointer */
  float32_t *py = pState;                        /* Temporary pointers for state buffer */
  float32_t *pb = pScratchIn;                    /* Temporary pointers for scratch buffer */
  float32_t *pOut;                               /* Destination pointer */
  int32_t *pTapDelay = S->pTapDelay;             /* Pointer to the array containing offset of the non-zero tap values. */
  uint32_t delaySize = S->maxDelay + blockSize;  /* state length */
  uint16_t numTaps = S->numTaps;                 /* Number of filter coefficients in the filter  */
  int32_t readIndex;                             /* Read index of the state buffer */
  uint32_t tapCnt, blkCnt;                       /* loop counters */
  float32_t coeff = *pCoeffs++;                  /* Read the first coefficient value */



  /* BlockSize of Input samples are copied into the state buffer */
  /* StateIndex points to the starting position to write in the state buffer */
  arm_circularWrite_f32((int32_t *) py, delaySize, &S->stateIndex, 1,
                        (int32_t *) pSrc, 1, blockSize);


  /* Read Index, from where the state buffer should be read, is calculated. */
  readIndex = ((int32_t) S->stateIndex - (int32_t) blockSize) - *pTapDelay++;

  /* Wraparound of readIndex */
  if(readIndex < 0)
  {
    readIndex += (int32_t) delaySize;
  }

  /* Working pointer for state buffer is updated */
  py = pState;

  /* blockSize samples are read from the state buffer */
  arm_circularRead_f32((int32_t *) py, delaySize, &readIndex, 1,
                       (int32_t *) pb, (int32_t *) pb, blockSize, 1,
                       blockSize);

  /* Working pointer for the scratch buffer */
  px = pb;

  /* Working pointer for destination buffer */
  pOut = pDst;


#ifndef ARM_MATH_CM0

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  /* Loop over the blockSize. Unroll by a factor of 4.   
   * Compute 4 Multiplications at a time. */
  blkCnt = blockSize >> 2u;

  while(blkCnt > 0u)
  {
    /* Perform Multiplications and store in destination buffer */
    *pOut++ = *px++ * coeff;
    *pOut++ = *px++ * coeff;
    *pOut++ = *px++ * coeff;
    *pOut++ = *px++ * coeff;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4,   
   * compute the remaining samples */
  blkCnt = blockSize % 0x4u;

  while(blkCnt > 0u)
  {
    /* Perform Multiplications and store in destination buffer */
    *pOut++ = *px++ * coeff;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Load the coefficient value and   
   * increment the coefficient buffer for the next set of state values */
  coeff = *pCoeffs++;

  /* Read Index, from where the state buffer should be read, is calculated. */
  readIndex = ((int32_t) S->stateIndex - (int32_t) blockSize) - *pTapDelay++;

  /* Wraparound of readIndex */
  if(readIndex < 0)
  {
    readIndex += (int32_t) delaySize;
  }

  /* Loop over the number of taps. */
  tapCnt = (uint32_t) numTaps - 1u;

  while(tapCnt > 0u)
  {

    /* Working pointer for state buffer is updated */
    py = pState;

    /* blockSize samples are read from the state buffer */
    arm_circularRead_f32((int32_t *) py, delaySize, &readIndex, 1,
                         (int32_t *) pb, (int32_t *) pb, blockSize, 1,
                         blockSize);

    /* Working pointer for the scratch buffer */
    px = pb;

    /* Working pointer for destination buffer */
    pOut = pDst;

    /* Loop over the blockSize. Unroll by a factor of 4.   
     * Compute 4 MACS at a time. */
    blkCnt = blockSize >> 2u;

    while(blkCnt > 0u)
    {
      /* Perform Multiply-Accumulate */
      *pOut++ += *px++ * coeff;
      *pOut++ += *px++ * coeff;
      *pOut++ += *px++ * coeff;
      *pOut++ += *px++ * coeff;

      /* Decrement the loop counter */
      blkCnt--;
    }

    /* If the blockSize is not a multiple of 4,   
     * compute the remaining samples */
    blkCnt = blockSize % 0x4u;

    while(blkCnt > 0u)
    {
      /* Perform Multiply-Accumulate */
      *pOut++ += *px++ * coeff;

      /* Decrement the loop counter */
      blkCnt--;
    }

    /* Load the coefficient value and   
     * increment the coefficient buffer for the next set of state values */
    coeff = *pCoeffs++;

    /* Read Index, from where the state buffer should be read, is calculated. */
    readIndex = ((int32_t) S->stateIndex -
                 (int32_t) blockSize) - *pTapDelay++;

    /* Wraparound of readIndex */
    if(readIndex < 0)
    {
      readIndex += (int32_t) delaySize;
    }

    /* Decrement the tap loop counter */
    tapCnt--;
  }

#else

/* Run the below code for Cortex-M0 */

  blkCnt = blockSize;

  while(blkCnt > 0u)
  {
    /* Perform Multiplications and store in destination buffer */
    *pOut++ = *px++ * coeff;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Load the coefficient value and          
   * increment the coefficient buffer for the next set of state values */
  coeff = *pCoeffs++;

  /* Read Index, from where the state buffer should be read, is calculated. */
  readIndex = ((int32_t) S->stateIndex - (int32_t) blockSize) - *pTapDelay++;

  /* Wraparound of readIndex */
  if(readIndex < 0)
  {
    readIndex += (int32_t) delaySize;
  }

  /* Loop over the number of taps. */
  tapCnt = (uint32_t) numTaps - 1u;

  while(tapCnt > 0u)
  {

    /* Working pointer for state buffer is updated */
    py = pState;

    /* blockSize samples are read from the state buffer */
    arm_circularRead_f32((int32_t *) py, delaySize, &readIndex, 1,
                         (int32_t *) pb, (int32_t *) pb, blockSize, 1,
                         blockSize);

    /* Working pointer for the scratch buffer */
    px = pb;

    /* Working pointer for destination buffer */
    pOut = pDst;

    blkCnt = blockSize;

    while(blkCnt > 0u)
    {
      /* Perform Multiply-Accumulate */
      *pOut++ += *px++ * coeff;

      /* Decrement the loop counter */
      blkCnt--;
    }

    /* Load the coefficient value and          
     * increment the coefficient buffer for the next set of state values */
    coeff = *pCoeffs++;

    /* Read Index, from where the state buffer should be read, is calculated. */
    readIndex =
      ((int32_t) S->stateIndex - (int32_t) blockSize) - *pTapDelay++;

    /* Wraparound of readIndex */
    if(readIndex < 0)
    {
      readIndex += (int32_t) delaySize;
    }

    /* Decrement the tap loop counter */
    tapCnt--;
  }

#endif /*   #ifndef ARM_MATH_CM0        */

}

/**   
 * @} end of FIR_Sparse group   
 */
