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
 * @file     threads.cpp
 * @brief    Simple BeagleBone threading example
 */

/**** System includes, namespace, then local includes  ***********************/
#include <iostream>
#include <pthread.h>
#include "posutils.h"

// namespace
using namespace std;

/**** Local (anonymous) namespace *******************************************/

/**** Definitions ************************************************************/
#define NUM_THREADS (5)

/**** Macros ****************************************************************/

/**** Local function prototypes (NB Use static modifier) ********************/
static void* thread_fct(void* pArg) {
    int iId = (int)pArg;
    cout << "Thread:" << iId << " is running!"<< endl;
    return (NULL);
}

/****************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                               */
/****************************************************************************/

/****************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                              */
/****************************************************************************/

/**
 * Main
 * @param argc: unused
 * @param argv: unused
 * @return 0
 * Create all the processes, resources, barriers, etc.
 */
int main( int argc, char *argv[] )
{
    pthread_t pThreadList[NUM_THREADS];

    // Super complex program
    cout << "Simple thread test app" << endl;

    // Initialisation
    int iRet = posutils_init();
    ASSERT(0 == iRet);

    // Body
    if (0 == iRet) {
        int i;

        // Create the threads
        for (i = 0; i < NUM_THREADS; i++) {
            cout << "Creating thread:" << i << endl;
            pThreadList[i] = PU_THREAD_CREATE(thread_fct, (void*)i, 16*1024);
            ASSERT(0 != pThreadList[i]);
        }

        // Wait for the threads to finish
        for (i = 0; i < NUM_THREADS; i++) {
            if (pThreadList[i] != 0) {
                cout << "Waiting for thread: " << i << endl;
                pthread_join(pThreadList[i], NULL);
                cout << "Dead thread: " << i << endl;
            }
        }
    }

    // Clean up
    posutils_exit();
    return (0);
}
/* main */
