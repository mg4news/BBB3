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
#ifndef __PUDEFS_H_
#define __PUDEFS_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file     pudefs.h
 * @date     2019-04-30
 * @author   Martin
 * @brief    Private definitions for the posutils library
 */

/**** Includes ***************************************************************/
#include <stdbool.h>
#include <stdint.h>

/**** Definitions ************************************************************/
int pu_thread_init_private( void );
int pu_thread_exit_private( void );

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __PUDEFS_H_ */
