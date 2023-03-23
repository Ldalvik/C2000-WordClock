/*
   Copyright (c) 2019 by Plexim GmbH
   All rights reserved.

   A free license is granted to anyone to use this software for any legal
   non safety-critical purpose, including commercial applications, provided
   that:
   1) IT IS NOT USED TO DIRECTLY OR INDIRECTLY COMPETE WITH PLEXIM, and
   2) THIS COPYRIGHT NOTICE IS PRESERVED in its entirety.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 */

#include "includes.h"

#ifndef SCI_IMPL_H_
#define SCI_IMPL_H_

typedef struct SCI_OBJ {
    uint16_t port;
    uint32_t portHandle;
    uint32_t clk;
} SCI_Obj_t;

typedef SCI_Obj_t *SCI_Handle_t;

// bit-masks for "mode"
#define ONE_STOPBIT 0x00
#define TWO_STOPBITS 0x80
#define NO_PARITY 0x00
#define ODD_PARITY 0x20
#define EVEN_PARITY 0x60
#define SEVEN_BITS 0x06
#define EIGHT_BITS 0x07
#define SCI_LOOPBACK 0x10

#define SCI_REGS_PTR ((volatile struct SCI_REGS *)obj->portHandle)

inline void SCI_reset(SCI_Handle_t aHandle)
{
    SCI_Obj_t *obj = (SCI_Obj_t *)aHandle;

    EALLOW;
    SCI_REGS_PTR->SCICTL1.all = 0;       // reset
    SCI_REGS_PTR->SCICTL1.all = 0x0013;  // enable tx, rx
    SCI_REGS_PTR->SCICTL1.all = 0x0033;  // relinquish from reset
    EDIS;
}

inline uint16_t SCI_getChar(SCI_Handle_t aHandle)
{
    SCI_Obj_t *obj = (SCI_Obj_t *)aHandle;
    return SCI_REGS_PTR->SCIRXBUF.all;
}

inline void SCI_putChar(SCI_Handle_t aHandle, uint16_t c)
{
    SCI_Obj_t *obj = (SCI_Obj_t *)aHandle;
    SCI_REGS_PTR->SCITXBUF = c;
}

inline bool SCI_rxReady(SCI_Handle_t aHandle)
{
    SCI_Obj_t *obj = (SCI_Obj_t *)aHandle;
    return (SCI_REGS_PTR->SCIFFRX.bit.RXFFST != 0);
}

inline bool SCI_txIsBusy(SCI_Handle_t aHandle)
{
    SCI_Obj_t *obj = (SCI_Obj_t *)aHandle;
    return (SCI_REGS_PTR->SCICTL2.bit.TXRDY == 0);
}

inline bool SCI_breakOccurred(SCI_Handle_t aHandle)
{
    SCI_Obj_t *obj = (SCI_Obj_t *)aHandle;
    return ((SCI_REGS_PTR->SCIRXST.all & 0x0080) != 0);
}

#endif /* SCI_IMPL_H_ */
