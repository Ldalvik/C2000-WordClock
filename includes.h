/*
   Copyright (c) 2016-2020 by Plexim GmbH
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

#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "F2806x_Device.h"
#include "F2806x_SysCtrl.h"
#include <F2806x_usDelay.h>

// Clock configurations
#define SYSCLK_HZ 90000000L
#define LSPCLK_HZ (SYSCLK_HZ / 4l)
#define PLL_DIV 18
#define PLL_SRC 0
#define PLX_CPU_RATE 11.111111111111L

// hardware settings
#define LED_ON_34

// assertion macro
#define PLX_ASSERT(x) do {\
   if(!(x)){\
      asm("        ESTOP0");\
      for(;;);\
   }\
} while(0)

// delay macro
extern void F28x_usDelay(long LoopCount);
#define PLX_DELAY_US(A)  F28x_usDelay( \
        ((((long double) A * \
           1000.0L) / \
          (long double)PLX_CPU_RATE) - 9.0L) / 5.0L)

#endif // _INCLUDES_H_
