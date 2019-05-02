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

/**
 * @file     posutils.c
 * @brief    Private atomic init/exit
 */

/**** Includes ***************************************************************/
#include "posutils.h"
#include "pudefs.h"
#include "logging.h"

/**** Definitions ************************************************************/
static int iIsInit = 0;

/**** Macros ****************************************************************/

/**** Static declarations ***************************************************/

/****************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                               */
/****************************************************************************/

/****************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                              */
/****************************************************************************/

/**
 * @brief Initialises the module
 *
 * @retval 0     Success
 * @retval non-0 Error
 *
 * @pre     None
 * @post    Module is initialised
 *
 * @par Description
 * Initialises the module. Creates any resources needed.
 * TODO: Add atomics
 */
int posutils_init( void ) {
    int iRet = 0;

    // Pseudo-atomic exit
    if (!iIsInit) {
        iIsInit = 1;
        iRet = pu_thread_init_private();
        ASSERT(0 == iRet);
    }
    return (iRet);
}

/**
 * @brief Closes (exits) the module
 *
 * @retval 0     Success
 * @retval non-0 Error
 *
 * @pre     None
 * @post    Module shuts down
 *
 * @par Description
 * Exits the module, cleans up any resources.
 */
int posutils_exit( void ) {
    int iRet = 0;

    // Pseudo-atomic exit
    if (iIsInit) {
        iIsInit = 0;
        pu_thread_exit_private();
    }
    return (iRet);
}
