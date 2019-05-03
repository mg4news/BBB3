//=============================================================================
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// This is a simplified version of UNLICENSE. For more information,
// please refer to <http://unlicense.org/>
//=============================================================================
#ifndef __GPIOUTILS_H_
#define __GPIOUTILS_H_

/**
 * @file     gpioutils.h
 * @date     2019-04-30
 * @author   Martin
 * @brief    Internal GPIO utilities and definitions
 */

//=== System includes, namespaces, then local includes ========================
#include <stdlib.h>
#include <cstdint>

//=== Definitions =============================================================
int gpioutils_export(uint32_t uiPin);
int gpioutils_unexport(uint32_t uiPin);
int gpioutils_set_dir(uint32_t uiPin, int iDir);
int gpioutils_get_dir(uint32_t uiPin, int* pDir);
int gpioutils_set_val(uint32_t uiPin, int iVal);
int gpioutils_get_val(uint32_t uiPin, int* pVal);
int gpioutils_set_edge(uint32_t uiPin, gpio_edge_t enEdge);
int gpioutils_get_edge(uint32_t uiPin, gpio_edge_t* pEdge);
int gpio_utils_wait_for_edge(uint32_t uiPin, int* pVal);

#endif /* __GPIOUTILS_H_ */
