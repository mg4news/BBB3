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
 * @file     gpiof.cpp
 * @date     2019-04-30
 * @author   Martin
 * @brief    Implementation of the GPIO factory
 */

//=== System includes, namespaces, then local includes ========================
#include <stdlib.h>
#include <iterator>
#include <algorithm>
#include "gpiof.h"
#include "gpioutils.h"
#include "logging.h"

//=== Local (anonymous) namespace =============================================
namespace {
    const uint32_t MAGIC = 0x47524951; // ascii for GPIO
}
//=============================================================================
// CReqFactory: Request factory singleton
//=============================================================================

// Aargh!! C++11 doesnt allow initialisation of class statics inside the
// declaration. So, need to move it here..
CGpioF* CGpioF::m_pInstance = NULL;

// Creates the Singleton if needed
CGpioF* CGpioF::getInstance() {
    if (NULL == m_pInstance) {
        m_pInstance = new CGpioF;
    }
    return (m_pInstance);
}
// CGpioF::getInstance()

// Finds the pin structure matching the pin number
CGpioF::pin_t* CGpioF::getPinByNumber( uint32_t uiPin ) {
    std::vector<pin_t*>::iterator iter;

    for (iter = m_PinVec.begin(); iter != m_PinVec.end(); iter++) {
        if (((pin_t*)(*iter))->uiPin == uiPin) {
            return ((pin_t*)(*iter));
        }
    }
    return (NULL);
}
// CGpioF::getPinByNumber

// Checks if a handle is valid
bool CGpioF::isHandleOk( pin_handle_t hnd ) {
    pin_t* pPin = (pin_t*)hnd;

    ASSERT( pPin );
    ASSERT( MAGIC == pPin->uiMagic );
    if ((NULL != pPin) &&
        (MAGIC == pPin->uiMagic) &&
        (std::find(m_PinVec.begin(), m_PinVec.end(), pPin) != m_PinVec.end())) {
        return (true);
    }
    return (false);
}
// isHandleOk

// Open a handle on a pin
int CGpioF::open(
    uint32_t      uiPin,    // GPIO pin number
    int           iDir,     // input or output
    gpio_edge_t   enEdge,   // only used if direction is input, else ignored
    int           iVal,     // only used if direction is out, else ignored
    pin_handle_t* pHnd)     // populated with the handle on success
{
    int iRet = -1;
    pin_t* pPin = this->getPinByNumber( uiPin );

    // Simple debug test
    ASSERT(enEdge >= 0 && enEdge < GPIO_EDGE_UNDEF);

    // Normal case, lets allocate
    if (NULL == pPin) {
        pPin = new pin_t;
        ASSERT( pPin );
        if (pPin) {
            pPin->uiPin    = uiPin;
            pPin->uiMagic  = MAGIC;
            pPin->bWaiting = false;
            pPin->iVal     = (iVal == GPIO_VAL_CLR) ? GPIO_VAL_CLR : GPIO_VAL_SET;
            pPin->iDir     = (iDir == GPIO_DIR_INP) ? GPIO_DIR_INP : GPIO_DIR_OUT;
            pPin->enEdge   = enEdge;
            pPin->iUsers   = 1;

            // Set it up
            iRet = gpioutils_export( pPin->uiPin );
            ASSERT(0 == iRet);
            if (0 == iRet) {
                iRet = gpioutils_set_dir( pPin->uiPin, pPin->iDir );
                ASSERT(0 == iRet);
            }
            if ((0 == iRet) && (GPIO_DIR_OUT == pPin->iDir)) {
                iRet = gpioutils_set_val( pPin->uiPin, pPin->iVal );
                ASSERT(0 == iRet);
            }
            if ((0 == iRet) && (GPIO_DIR_INP == pPin->iDir)) {
                iRet = gpioutils_set_edge( pPin->uiPin, pPin->enEdge );
                ASSERT(0 == iRet);
            }

            // OK? add to vector, else clean up
            if (0 == iRet) {
                m_PinVec.push_back( pPin );
                *pHnd = (pin_handle_t)pPin;

            } else {
                LOG_ERROR( "Failed to set up pin %u\n", pPin->uiPin );
                gpioutils_unexport( pPin->uiPin );
                delete pPin;
                *pHnd = (pin_handle_t)0;
            }
        }
    }

    // Pin exists
    else {
#if defined(GPIO_POLICY_SINGLE_USER)
        LOG_ERROR( "Pin %u already in use", uiPin );
        iRet = GPIO_ERR_USED;
#else
        pPin->iUsers++;
        *pHnd = (pin_handle_t)pPin;
        iRet = 0;
#endif
    }

    // Done
    return (iRet);
}
// CGpioF::open

// Close a handle on a pin
int CGpioF::close(pin_handle_t hnd) {

    int iRet = GPIO_ERR_HANDLE;
    pin_t* pPin = (pin_t*)hnd;

    if (this->isHandleOk(hnd)) {
#if !defined(GPIO_POLICY_SINGLE_USER)
        pPin->iUsers--;
#endif // !defined(GPIO_POLICY_SINGLE_USER)
        if (pPin->iUsers <= 1) {
            gpioutils_unexport( pPin->uiPin );
            m_PinVec.erase(std::remove(m_PinVec.begin(), m_PinVec.end(), pPin), m_PinVec.end());
            delete pPin;
        }
        iRet = 0;
    }
    return (iRet);
}
// CGpioF::close
