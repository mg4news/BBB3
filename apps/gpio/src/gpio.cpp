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
 * @file     gpio.cpp
 * @brief    Simple BeagleBone threaded GPIO
 */

/**** System includes, namespace, then local includes  ***********************/
#include <iostream>
#include <pthread.h>

// Local includes
#include "logging.h"
#include "posutils.h"
#include "gpiof.h"

// namespace
using namespace std;

//=== Local (anonymous) namespace =============================================

//=============================================================================
// Local function definitions
//=============================================================================
void* thread_fct(void* pArg) {
    cout << "Thread is running!"<< endl;

    // Write 1 to the GPIO

    return (NULL);
}


//=============================================================================
// Public function definitions
//=============================================================================

/**
 * Main
 * @param argc: unused
 * @param argv: unused
 * @return 0
 * Create all the processes, resources, barriers, etc.
 */
int main( int argc, char *argv[] )
{
    // Super complex program
    cout << "GPIO test" << endl;

    // Initialisation
    int iRet = posutils_init();
    ASSERT(0 == iRet);

    // Get the factory handle
    CGpioF* pGpio = CGpioF::getInstance();

    // Get a GPIO handle
    CGpioF::pin_handle_t hnd;
    iRet = pGpio->open( 62, GPIO_DIR_OUT, GPIO_EDGE_NONE, GPIO_VAL_CLR, &hnd);
    ASSERT(0 == iRet);
    iRet = pGpio->setVal(hnd, GPIO_VAL_SET);

    // done and dusted
    posutils_exit();
    return (0);
}
/* main */
