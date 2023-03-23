/*
   Copyright (c) 2018 by Plexim GmbH
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

#ifndef SCI_H_
#define SCI_H_

#include "sci_impl.h"  // implementation specific

extern SCI_Handle_t SCI_init(void *aMemory, const size_t aNumBytes);
extern void SCI_configure(SCI_Handle_t aHandle, uint16_t port, uint32_t clk);
extern bool SCI_setupPort(SCI_Handle_t aHandle, uint32_t baudrate);

extern bool SCI_rxReady(SCI_Handle_t aHandle);
extern bool SCI_txIsBusy(SCI_Handle_t aHandle);

extern uint16_t SCI_getChar(SCI_Handle_t aHandle);
extern void SCI_putChar(SCI_Handle_t aHandle, uint16_t c);

extern bool SCI_breakOccurred(SCI_Handle_t aHandle);
extern void SCI_reset(SCI_Handle_t aHandle);

#endif /* SCI_H_ */
