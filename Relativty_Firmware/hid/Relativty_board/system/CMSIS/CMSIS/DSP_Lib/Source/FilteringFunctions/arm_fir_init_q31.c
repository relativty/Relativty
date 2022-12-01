/* ----------------------------------------------------------------------   
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*   
* $Date:        15. July 2011  
* $Revision: 	V1.0.10  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_fir_init_q31.c   
*   
* Description:	Q31 FIR filter initialization function.   
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
* 	 incorporated review comments and updated with latest CMSIS layer   
*   
* Version 0.0.3  2010/03/10    
*    Initial version   
* -------------------------------------------------------------------- */

#include "arm_math.h"

/**   
 * @ingroup groupFilters   
 */

/**   
 * @addtogroup FIR   
 * @{   
 */

/**   
 * @details   
 *   
 * @param[in,out] *S points to an instance of the Q31 FIR filter structure.   
 * @param[in] 	  numTaps  Number of filter coefficients in the filter.   
 * @param[in] 	  *pCoeffs points to the filter coefficients buffer.   
 * @param[in] 	  *pState points to the state buffer.   
 * @param[in] 	  blockSize number of samples that are processed per call.   
 * @return        none.   
 *   
 * <b>Description:</b>   
 * \par   
 * <code>pCoeffs</code> points to the array of filter coefficients stored in time reversed order:   
 * <pre>   
 *    {b[numTaps-1], b[numTaps-2], b[N-2], ..., b[1], b[0]}   
 * </pre>   
 * \par   
 * <code>pState</code> points to the array of state variables.   
 * <code>pState</code> is of length <code>numTaps+blockSize-1</code> samples, where <code>blockSize</code> is the number of input samples processed by each call to <code>arm_fir_q31()</code>.   
 */

void arm_fir_init_q31(
  arm_fir_instance_q31 * S,
  uint16_t numTaps,
  q31_t * pCoeffs,
  q31_t * pState,
  uint32_t blockSize)
{
  /* Assign filter taps */
  S->numTaps = numTaps;

  /* Assign coefficient pointer */
  S->pCoeffs = pCoeffs;

  /* Clear state buffer and state array size is (blockSize + numTaps - 1) */
  memset(pState, 0, (blockSize + ((uint32_t) numTaps - 1u)) * sizeof(q31_t));

  /* Assign state pointer */
  S->pState = pState;

}

/**   
 * @} end of FIR group   
 */
