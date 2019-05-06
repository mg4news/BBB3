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
 * @file     gpioutils.cpp
 * @date     2019-04-30
 * @author   Martin
 * @brief    Internal utilities
 */

//=== System includes, namespaces, then local includes ========================
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

using namespace std;

#include "gpiof.h"
#include "gpioutils.h"
#include "logging.h"
#include "sll.h"

//=== Local (anonymous) namespace =============================================
namespace {
    const char* SZ_GPIO_EXPORT      = "/sys/class/gpio/export";
    const char* SZ_GPIO_UNEXPORT    = "/sys/class/gpio/export";
    const char* SZ_GPIO_DIRECTION   = "/sys/class/gpio/gpio%u/direction";
    const char* SZ_GPIO_VALUE       = "/sys/class/gpio/gpio%u/value";
    const char* SZ_GPIO_EDGE        = "/sys/class/gpio/gpio%u/edge";

    const char* SZ_EDGE_TYPES[GPIO_EDGE_UNDEF] = {"none", "rising", "falling", "both"};

    // Linked list of pin contexts, stores the epoll temporarily so a wait can be interrupted
    typedef struct pin_ctxt_tag {
        uint32_t uiPin;
        int      fdEpoll;
        SLL_ENTRY(pin_ctxt_tag);
    }   pin_ctxt_t;
    pin_ctxt_t* pCtxtList  = NULL;
}

//=============================================================================
// Local function definitions
//=============================================================================

//=============================================================================
// Public function definitions
//=============================================================================

int gpioutils_export(uint32_t uiPin) {

    std::ofstream path( SZ_GPIO_EXPORT );

    ASSERT(path);
    if (!path) {
        LOG_ERROR("Cant export pin %u\n", uiPin);
        return (-1);
    }

    // All good
    path << uiPin;
    path.close();

    // add an entry to the SLL
    pin_ctxt_t* pCtxt = (pin_ctxt_t*)malloc(sizeof(pin_ctxt_t));
    pCtxt->uiPin   = uiPin;
    pCtxt->fdEpoll = -1;
    SLL_ELEM_ADD(pCtxtList, pCtxt);

    return (0);
}
// gpioutils_export

int gpioutils_unexport(uint32_t uiPin) {

    std::ofstream path( SZ_GPIO_UNEXPORT );

    ASSERT(path);
    if (!path) {
        LOG_ERROR("Cant unexport pin %u\n", uiPin);
        return (-1);
    }

    // All good
    path << uiPin;
    path.close();

    // Kill the pin context
    pin_ctxt_t* pCtxt;
    SLL_FOR_EACH(pCtxtList, pCtxt) {
        if (pCtxt->uiPin == uiPin) {
            SLL_ELEM_DEL(pin_ctxt_t, pCtxtList, pCtxt);
            break;
        }
    }
    return (0);
}
// gpioutils_unexport

int gpioutils_set_dir(uint32_t uiPin, int iDir) {

    char szPath[64];

    sprintf( szPath, SZ_GPIO_DIRECTION, uiPin );
    std::ofstream path( szPath );
    ASSERT(path);
    if (!path) {
        LOG_ERROR("Cant set pin %u direction\n", uiPin);
        return (-1);
    }

    // All good
    if (GPIO_DIR_INP == iDir) {
        path << "in";
    } else {
        path << "out";
    }
    path.close();
    return (0);
}
// gpioutils_set_dir

int gpioutils_get_dir(uint32_t uiPin, int* pDir) {

    char szPath[64];
    char cDir;

    sprintf( szPath, SZ_GPIO_DIRECTION, uiPin );
    std::ifstream path( szPath );
    ASSERT(path);
    if (!path) {
        LOG_ERROR("Cant get pin %u direction\n", uiPin);
        return (-1);
    }

    // All good
    path >> cDir;
    if ('i' == cDir) {
        *pDir = GPIO_DIR_INP;
    } else {
        *pDir = GPIO_DIR_OUT;
    }
    path.close();
    return (0);
}
// gpioutils_get_dir

int gpioutils_set_val(uint32_t uiPin, int iVal) {

    char szPath[64];

    sprintf( szPath, SZ_GPIO_VALUE, uiPin );
    std::ofstream path( szPath );
    ASSERT(path);
    if (!path) {
        LOG_ERROR("Cant set pin %u value\n", uiPin);
        return (-1);
    }

    // All good
    path << iVal;
    path.close();
    return (0);
}
// gpioutils_set_val

int gpioutils_get_val(uint32_t uiPin, int* pVal) {

    char szPath[64];

    sprintf( szPath, SZ_GPIO_VALUE, uiPin );
    std::ifstream path( szPath );
    ASSERT(path);
    if (!path) {
        LOG_ERROR("Cant get pin %u value\n", uiPin);
        return (-1);
    }

    // All good
    path >> *pVal;
    path.close();
    return (0);
}
// gpioutils_set_val

int gpioutils_set_edge(uint32_t uiPin, gpio_edge_t enEdge) {

    char szPath[64];

    sprintf( szPath, SZ_GPIO_EDGE, uiPin );
    std::ofstream path( szPath );
    ASSERT(path);
    if (!path) {
        LOG_ERROR("Cant set pin %u edge\n", uiPin);
        return (-1);
    }

    // All good
    path << SZ_EDGE_TYPES[enEdge];
    path.close();
    return (0);
}
// gpioutils_set_edge

int gpioutils_get_edge(uint32_t uiPin, gpio_edge_t* pEdge) {

    char szPath[64];
    char szEdge[16];
    szEdge[0] = 0;
    sprintf( szPath, SZ_GPIO_EDGE, uiPin );
    std::ifstream path( szPath );
    ASSERT(path);
    if (!path) {
        LOG_ERROR("Cant set pin %u edge\n", uiPin);
        return (-1);
    }

    // All good
    path >> szEdge;
    path.close();
    for (int i = 0; i < GPIO_EDGE_UNDEF; i++) {
        if (strcmp(szEdge, SZ_EDGE_TYPES[i]) == 0) {
            *pEdge = (gpio_edge_t)i;
            return (0);
        }
    }

    // Don't understand the edge
    LOG_ERROR("Unknown edge %s on pin %u", szEdge, uiPin);
    return (-1);
}
// gpioutils_get_edge

int gpioutils_wait_for_edge(uint32_t uiPin, int* pVal) {
    #define EVT_RD_SIZE (15)
    char szPath[64];
    char pEvt[EVT_RD_SIZE+1];
    int  fd_gpio;
    int  fd_epoll;
    int  iRet = -1;
    struct epoll_event events, ev;

    // open the file descriptors for the pin value and epoll
    sprintf( szPath, SZ_GPIO_VALUE, uiPin );
    fd_gpio = open( szPath, O_RDONLY | O_NONBLOCK );
    ASSERT(-1 != fd_gpio);
    if (-1 != fd_gpio) {
        fd_epoll = epoll_create( 1 );
        ASSERT(-1 != fd_epoll);
    }

    // OK if both are open
    if ((-1 != fd_gpio) && (-1 != fd_epoll)) {

        // Context management, allows interrupting of the wait
        pin_ctxt_t* pCtxt = NULL;
        SLL_FOR_EACH(pCtxtList, pCtxt) {
            if (pCtxt->uiPin == uiPin) {
                pCtxt->fdEpoll = fd_epoll;
                break;
            }
        }

        // Populate and register event structure
        ev.data.fd = fd_gpio;
        ev.events  = (EPOLLIN | EPOLLET | EPOLLPRI);
        epoll_ctl( fd_epoll, EPOLL_CTL_ADD, fd_gpio, &ev );

        // Loop on events, discard first
        // - waiting on 1 event (parameter 3)
        // - Indefinite timeout (parameter 4)
        // Use "iRet" to trap errors so we ALWAYS clean up the fd's
        iRet = 0;
        for (int i = 0; i < 2; i++) {
            if (epoll_wait(fd_epoll, &events, 1, -1) == -1)  {
                iRet = -1;
                break;
            }
        }

        // Check the event buffer if there were no errors
        // There will be AT LEAST one event, and we only care about the first one
        if (0 == iRet) {
            lseek(events.data.fd, 0, SEEK_SET);
            if (read(events.data.fd, &pEvt, EVT_RD_SIZE) >= 1) {
                *pVal = pEvt[0] & 1;
            } else {
                iRet = -1;
            }
        }

        // Clean up context pointer
        if ((pCtxt) && (pCtxt->uiPin == uiPin)) {
            pCtxt->fdEpoll = -1;
        }
    }

    // always clean up
    close( fd_epoll );
    close( fd_gpio );
    return (iRet);
}
// gpioutils_wait_for_edge

// Interrupts a thread waiting on epoll, by sending an event to the poller.
int gpioutils_interrupt_wait(uint32_t uiPin) {
    pin_ctxt_t* pCtxt = NULL;
    uint8_t     pBuff[4];

    // Value doesn't matter
    pBuff[0] = 1;
    SLL_FOR_EACH(pCtxtList, pCtxt) {
        if (pCtxt->uiPin == uiPin) {
            int iRW = write( pCtxt->fdEpoll, pBuff, 1 );
            ASSERT(1 == iRW);
            break;
        }
    }
    return (0);
}
//  gpioutils_interrupt_wait

