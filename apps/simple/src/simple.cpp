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
 * @file     simple.cpp
 * @brief    Simple BeagleBone "hello world" to test cross compiling
 */

/**** System includes, namespace, then local includes  ***********************/
#include <iostream>

// Local includes
#include "logging.h"

// namespace
using namespace std;

/**** Local (anonymous) namespace *******************************************/

/**** Definitions ************************************************************/

/**** Macros ****************************************************************/

/**** Local function prototypes (NB Use static modifier) ********************/
static void info(void);

/****************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                               */
/****************************************************************************/
void info(void) {
    cout << "Built on " __DATE__ " at " __TIME__ << endl;
    cout << "Uses C++ standard " << __cplusplus << endl;
#if defined(__clang__)
    cout << "Compiled with Clang " << __clang_version__ << endl;
#else
    cout << "Compiled with GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << endl;
#endif
}

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
    // Super complex program
    LOG_TRACE("Testing the tracing\n");
    LOG_ERROR("Testing the errors\n");
    info();
    cout << "Dagse wereld!!" << endl;

    // done and dusted
    return (0);
}
/* main */
