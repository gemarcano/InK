/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

#include "../../include/DSPLib.h"

#if defined(MSP_USE_LEA)

msp_status msp_interleave_q15(const msp_interleave_q15_params *params, const _q15 *src, _q15 *dst)
{
    uint16_t cmdId;
    uint16_t length;
    uint16_t channel;
    uint16_t numChannels;
    msp_status status;
    MSP_LEA_INTERLEAVE_PARAMS *leaParams;

    /* Initialize local variables from parameters. */
    length = params->length;
    channel = params->channel;
    numChannels = params->numChannels;

#ifndef MSP_DISABLE_DIAGNOSTICS
    /* Check that the channel is less than the total number of channels. */
    if (channel > numChannels) {
        return MSP_SIZE_ERROR;
    }
    
    /* Check that the data arrays are aligned and in a valid memory segment. */
    if (!(MSP_LEA_VALID_ADDRESS(src, 4) &
          MSP_LEA_VALID_ADDRESS(dst, 4))) {
        return MSP_LEA_INVALID_ADDRESS;
    }

    /* Check that the correct revision is defined. */
    if (MSP_LEA_REVISION != msp_lea_getRevision()) {
        return MSP_LEA_INCORRECT_REVISION;
    }

    /* Acquire lock for LEA module. */
    if (!msp_lea_acquireLock()) {
        return MSP_LEA_BUSY;
    }
#endif //MSP_DISABLE_DIAGNOSTICS

    /* Initialize LEA if it is not enabled. */
    if (!(LEAPMCTL & LEACMDEN)) {
        msp_lea_init();
    }
        
    /* Allocate MSP_LEA_INTERLEAVE_PARAMS structure. */
    leaParams = (MSP_LEA_INTERLEAVE_PARAMS *)msp_lea_allocMemory(sizeof(MSP_LEA_INTERLEAVE_PARAMS)/sizeof(uint32_t));

    /* Set MSP_LEA_INTERLEAVE_PARAMS structure. */
    leaParams->vectorSize = length;
    leaParams->channel = channel;
    leaParams->numChannels = numChannels;
    leaParams->output = MSP_LEA_CONVERT_ADDRESS(dst);

    /* Load source arguments to LEA. */
    LEAPMS0 = MSP_LEA_CONVERT_ADDRESS(src);
    LEAPMS1 = MSP_LEA_CONVERT_ADDRESS(leaParams);

    /* Determine which LEA deinterleave command to invoke. */
    if (channel & 1) {
        if (numChannels & 1) {
            /* Load the LEACMD__INTERLEAVEODDODD command into code memory. */
            cmdId = msp_lea_loadCommand(LEACMD__INTERLEAVEODDODD,MSP_LEA_INTERLEAVEODDODD,
                    sizeof(MSP_LEA_INTERLEAVEODDODD)/sizeof(MSP_LEA_INTERLEAVEODDODD[0]));
        }
        else {
            /* Load the LEACMD__INTERLEAVEODDEVEN command into code memory. */
            cmdId = msp_lea_loadCommand(LEACMD__INTERLEAVEODDEVEN,MSP_LEA_INTERLEAVEODDEVEN,
                    sizeof(MSP_LEA_INTERLEAVEODDEVEN)/sizeof(MSP_LEA_INTERLEAVEODDEVEN[0]));
        }
    }
    else {
        if (numChannels & 1) {
            /* Load the LEACMD__INTERLEAVEEVENODD command into code memory. */
            cmdId = msp_lea_loadCommand(LEACMD__INTERLEAVEEVENODD,MSP_LEA_INTERLEAVEEVENODD,
                    sizeof(MSP_LEA_INTERLEAVEEVENODD)/sizeof(MSP_LEA_INTERLEAVEEVENODD[0]));
        }
        else {
            /* Load the LEACMD__INTERLEAVEEVENEVEN command into code memory. */
            cmdId = msp_lea_loadCommand(LEACMD__INTERLEAVEEVENEVEN,MSP_LEA_INTERLEAVEEVENEVEN,
                    sizeof(MSP_LEA_INTERLEAVEEVENEVEN)/sizeof(MSP_LEA_INTERLEAVEEVENEVEN[0]));
        }
    }

    /* Invoke the loaded command. */
    msp_lea_invokeCommand(cmdId);

    /* Free MSP_LEA_INTERLEAVE_PARAMS structure. */
    msp_lea_freeMemory(sizeof(MSP_LEA_INTERLEAVE_PARAMS)/sizeof(uint32_t));

    /* Set status flag. */
    status = MSP_SUCCESS;

#ifndef MSP_DISABLE_DIAGNOSTICS
    /* Check LEA interrupt flags for any errors. */
    if (msp_lea_ifg & LEACOVLIFG) {
        status = MSP_LEA_COMMAND_OVERFLOW;
    }
    else if (msp_lea_ifg & LEAOORIFG) {
        status = MSP_LEA_OUT_OF_RANGE;
    }
    else if (msp_lea_ifg & LEASDIIFG) {
        status = MSP_LEA_SCALAR_INCONSISTENCY;
    }
#endif //MSP_DISABLE_DIAGNOSTICS

    /* Free lock for LEA module and return status. */
    msp_lea_freeLock();
    return status;
}

#else //MSP_USE_LEA

msp_status msp_interleave_q15(const msp_interleave_q15_params *params, const _q15 *src, _q15 *dst)
{
    uint16_t length;
    uint16_t channel;
    uint16_t numChannels;

    /* Initialize local variables from parameters. */
    length = params->length;
    channel = params->channel;
    numChannels = params->numChannels;

#ifndef MSP_DISABLE_DIAGNOSTICS
    /* Check that the channel is less than the total number of channels. */
    if (channel > numChannels) {
        return MSP_SIZE_ERROR;
    }
#endif //MSP_DISABLE_DIAGNOSTICS

    /* Insert the requested channel into the destination data. */
    dst += channel;
    while (length--) {
        *dst = *src++;
        dst += numChannels;
    }

    return MSP_SUCCESS;
}

#endif //MSP_USE_LEA
