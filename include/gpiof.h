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
#ifndef __GPIOF_H_
#define __GPIOF_H_

/**
 * @file     gpiof.h
 * @date     2019-04-30
 * @author   Martin
 * @brief    GPIO "factory"
 * Uses the factory pattern to open, manage and close handles on GPIO resources
 */

//=== System includes, namespaces, then local includes ========================
#include <stdlib.h>
#include <cstdint>
#include <vector>
using namespace std;

//=== Definitions =============================================================

// One user per pin
#define GPIO_POLICY_SINGLE_USER

#define GPIO_DIR_INP (0)          // Direction is input
#define GPIO_DIR_OUT (1)          // Direction is output
#define GPIO_VAL_SET (1)          // Value is 1 (i.e. set, high)
#define GPIO_VAL_CLR (0)          // Value is 0 (i.e. clear, low)

// Error values
#define GPIO_ERR_GENERAL    (-1)  // Some system error, i.e. couldn't export, open, etc
#define GPIO_ERR_HANDLE     (-2)  // Invalid pin handle
#define GPIO_ERR_WAITING    (-3)  // Valid pin, currently waiting for edge
#define GPIO_ERR_DIRECTION  (-4)  // Wrong pin direction for the action
#define GPIO_ERR_USED       (-5)  // Pin is already in use

// Edge settings
enum gpio_edge_t {
    GPIO_EDGE_NONE    = 0,
    GPIO_EDGE_RISING  = 1,
    GPIO_EDGE_FALLING = 2,
    GPIO_EDGE_BOTH    = 3,
    GPIO_EDGE_UNDEF   = 4
};

/**
 * Singleton GPIO factory
 * Usage: CGpioF->getInstance();
 */
class CGpioF
{
public:
    static CGpioF* getInstance();
    typedef void* pin_handle_t;

    // Handle open and close
    // Two possible policies
    // - one user per pin (default), 2nd open results in an error - GPIO_ERR_USED
    // - multiple users per pin, BUT they all get the same handle. Close() counts down users
    int open(
        uint32_t      uiPin,    // GPIO pin number
        int           iDir,     // input or output
        gpio_edge_t   enEdge,   // only used if direction is input, else ignored
        int           iVal,     // only used if direction is out, else ignored
        pin_handle_t* pHnd);    // populated with the handle on success
    int close(pin_handle_t hnd);

    // GPIO value
    // 0 = clear, low
    // non-0 - set, high
    int setVal( pin_handle_t hnd, int  iVal ); // set a value on an output pin only
    int getVal( pin_handle_t hnd, int* pVal ); // read on input (actual value) and output (stored value) pin

    // GPIO direction control
    // 0     = input direction
    // non-0 = output direction
    // Notes:
    // - if the pin is current being waited on for an edge, then the function will return a waiting error
    int setDir( pin_handle_t hnd, int  iDir);
    int getDir( pin_handle_t hnd, int* pDir);

    // GPIO edge (only valid if pin is input)
    int setEdge( pin_handle_t hnd, gpio_edge_t enEdge );
    int getEdge( pin_handle_t hnd, gpio_edge_t* pEdge );

    // Blocking wait for edge, CALL THIS FROM A THREAD
    // Only on an input pin
    // Will only set the edge value if it does not match
    int waitForEdge( pin_handle_t hnd, gpio_edge_t enEdge, int* pVal );

private:
    static CGpioF* m_pInstance;
    CGpioF() {}
    virtual ~CGpioF() {}

    // Internal details
    struct pin_t {
        uint32_t     uiMagic;
        uint32_t     uiPin;
        int          iDir;
        int          iVal;
        gpio_edge_t  enEdge;
        bool         bWaiting;
        int          iUsers;
    };
    std::vector<pin_t*> m_PinVec;
    pin_t* getPinByNumber( uint32_t uiPin );
    bool   isHandleOk( pin_handle_t hnd );


};


#endif /* __GPIOF_H_ */
