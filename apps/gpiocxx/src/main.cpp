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
#include "gpiod.h"
#include "gpiod.hpp"
#include "logging.h"
#include "posutils.h"

// namespace
using namespace std;

//=== Local (anonymous) namespace =============================================

//=============================================================================
// Local function definitions
//=============================================================================
void* thread_fct(void* pArg) {
    struct gpiod_chip* pChip = (struct gpiod_chip*)pArg;

    // Dump the basic information
    cout << "Thread for " << gpiod_chip_name(pChip) << endl;
    cout << "  label = " << gpiod_chip_label(pChip) << endl;
    cout << "  lines = " << gpiod_chip_num_lines(pChip) << endl;

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
    pthread_t          pThreadList[4];
    struct gpiod_chip* pChipList[4];
    int                iChips = 0;

    // // Dump the library version
    cout << "LIBGPIOD version: " <<  gpiod_version_string() << endl;

    // Initialisation
    int iRet = posutils_init();
    ASSERT(0 == iRet);

    // For each chip, create a separate thread to manage it
    for (unsigned int uiNum = 0; uiNum < 4; uiNum++) {
        struct gpiod_chip* pChip = gpiod_chip_open_by_number( uiNum );
        if (!pChip) {
            cout << "Unable to open GPIO chip:" << uiNum << endl;
        } else {
            pChipList[iChips]   = pChip;
            pThreadList[iChips] = PU_THREAD_CREATE(thread_fct, (void*)pChip, 16*1024);
            ASSERT(0 != pThreadList[iChips]);
            iChips++;
        }
    }

    // Wait for the threads to terminate, clean up the chip handles
    for (int i = 0; i < iChips; i++) {
        if (pThreadList[i] != 0) {
            cout << "Waiting for thread: " << i << endl;
            pthread_join(pThreadList[i], NULL);
            cout << "Dead thread: " << i << endl;

            // close the chip
            gpiod_chip_close(pChipList[i]);

        }
    }

    // done and dusted
    posutils_exit();
    return (0);
}
/* main */
