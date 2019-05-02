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
 * @file     puthread.c
 * @brief    Implementation of the pthread utilities
 */

/**** Includes ***************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <errno.h>
#include <syscall.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <sched.h>
#include <limits.h>
#if !defined(__USE_GNU)
    #define  __USE_GNU
#endif /* !defined(__USE_GNU) */
#include "posutils.h"
#include "logging.h"
#include "pudefs.h"
#include "sll.h"

/**** Definitions ************************************************************/

/* Per thread context structure */
typedef struct pu_thread_context_tag
{
    pu_thread_fct_t fctMain;                 /* Thread main function (entry point) */
    void*           pMainArg;                /* Main argument                      */
    pthread_t       pid;                     /* Posix thread ID                    */
    pid_t           tid;                     /* Linux thread ID                    */
    char*           szName;                  /* Thread name                        */
    SLL_ENTRY(pu_thread_context_tag);
}   pu_thread_context_t;

#define PU_THREAD_STUPID_STACKSIZE (1024*1024)

/**** Macros ****************************************************************/

/**** Static declarations ***************************************************/
static pthread_rwlock_t     rwlock;
static pu_thread_context_t* pThreadList  = NULL;
static size_t               uiNumThreads = 0;
static size_t               uiPageSize   = 0;

/* 16 + extra NULLs */
static char szProcName[20] = "";

/**** Local function prototypes (NB Use static modifier) ********************/
static size_t               pu_thread_stacksize_fix( size_t uiStackSize );
static void*                pu_thread_entry_handler( void* pArg );
static void                 pu_thread_exit_handler( void* pArg );

/****************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                               */
/****************************************************************************/

static inline size_t pu_thread_stacksize_fix( size_t uiStackSize )
{
    size_t uiNewSize = uiStackSize;

    /* Less than the PTHREAD minimum, set size to the minimum + a guard page */
    if (uiNewSize < PTHREAD_STACK_MIN)
    {
        uiNewSize = (PTHREAD_STACK_MIN + uiPageSize);
    }

    /* Greater than or equal to the PTHREAD minimum
     * We round the stack size up to the next page boundary.
     * Also, we want a guard page. This comes out of the stack size, so we have to add
     * an extra page to account for it.
     * So we need to round up to the nearest TWO pages...
     */
    else
    {
        uiNewSize += ((2 * uiPageSize) - 1);
        uiNewSize /= uiPageSize;
        uiNewSize *= uiPageSize;
    }

    /* Done */
    return (uiNewSize);
}
/* pu_thread_stacksize_fix */

static void* pu_thread_entry_handler( void* pArg )
{
    pu_thread_context_t* pNode = (pu_thread_context_t*)pArg;
    void*                pReturn;

    /* Get the system thread ID */
    pNode->tid = (pid_t)syscall( SYS_gettid );

    /* Trace thread creation */
    LOG_TRACE(
        "PU_THREAD(create):proc=%s, thrd=%s, tid=%d\n",
        szProcName,
        pNode->szName,
        (int)pNode->tid );

    /* Insert the node at the head of the thread linked list */
    pthread_rwlock_wrlock( &rwlock );
    SLL_ELEM_ADD( pThreadList, pNode );
    uiNumThreads++;
    pthread_rwlock_unlock( &rwlock );
    
    /* register the exist handler */
    pthread_cleanup_push( pu_thread_exit_handler, pNode );

    /* Thread real main entry */  
    pReturn = pNode->fctMain( pNode->pMainArg );
    pthread_cleanup_pop( 1 );
    
    /* pass back the actual thread return value */
    return (pReturn);
}
/* pu_thread_entry_handler */

static void pu_thread_exit_handler( void* pArg )
{
    pu_thread_context_t* pNode = (pu_thread_context_t*)pArg;
    ASSERT( pNode );
    
    /* Remove the corresponding node from thread list
     * We needn't check the case: (pThreadList == NULL), it will never happen.
     * But.. in debug we do anyway..
     */
    pthread_rwlock_wrlock( &rwlock );
    ASSERT( pThreadList );
    ASSERT( uiNumThreads > 0 );

    /* Delete it. free the memory */
    SLL_ELEM_DEL( pu_thread_context_t, pThreadList, pNode );
    free( pNode );
    pNode = NULL;
    uiNumThreads--;
    pthread_rwlock_unlock( &rwlock );  
}
/* pu_thread_exit_handler */

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
 * Wrapped in the init, no init/exit test needed
 */
int pu_thread_init_private( void )
{
    int      iResult = -1;
    intptr_t iPageSize;
    
    /* Safe page size query - make sure it is at least 1K - this is merely for sanity */
    iPageSize = sysconf( _SC_PAGESIZE );
    ASSERT( iPageSize >= 1024 );
    if (iPageSize >= 1024)
    {
        /* Store for posterity.. */
        uiPageSize = (size_t)iPageSize;

        /* Initialise the readers-writers lock */
        iResult = pthread_rwlock_init( &rwlock, NULL );
        ASSERT( 0 == iResult );

        /* flood with NULL up to 1 byte PAST the 16 byte count. The name will be up to 16 bytes. It will
         * only be null terminated if it is less than 16 bytes. We enforce NULL termination
         * regardless of length
         */
        memset( szProcName, 0, 17 );
        if (0 != prctl( PR_GET_NAME, (unsigned long)szProcName, 0, 0, 0 ))
        {
            sprintf( szProcName, "????????");
        }
    }
    return (iResult);
}
/* pu_thread_init_private */

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
 * Wrapped in the sysutils exit, no init/exit test needed
 */
int pu_thread_exit_private( void )
{
    int iResult;

    /* Destroy the readers-writers lock */
    uiPageSize = 0;
    iResult    = pthread_rwlock_destroy( &rwlock );
    ASSERT( 0 == iResult );
    return (iResult);
}
/* pu_thread_exit_private */

/**
 * @brief   Creates a non-RT pthread with the Comet constraints applied
 *
 * @param[in] fctMain     : Thread main function (entry point)
 * @param[in] pMainArg    : Argument for main
 * @param[in] fctExit     : Thread exit handler function
 * @param[in] pExitArg    : Exit handler argument
 * @param[in] uiStackSize : Stack size
 * @param[in] szName      : Thread name
 * @retval  A non-zero pthread ID indicates success
 * @retval  A zero pthread ID means failure
 *
 * @pre     The entry function (main) is non-NULL
 * @pre     The name is non-NULL
 * @post    The pthread is created.
 *
 * @par Description
 * Creates a standard pthread (SCHED_OTHER) with the passed in parameters. The stack size will be
 * rounded up to the nearest multiple of the page size, i.e. (n * 4k).
 *
 * @par Exit handling
 * The are two main ways a thread can exit. It can terminate on its own either by simply coming to
 * the end of the main loop or invoking pthread_exit(). In this case the thread is aware it is going to exit,
 * and will naturally clean up after itself. The second scenario is when the thread is terminated
 * by another thread, i.e. as a result of a call to pthread_cancel(pid). Is this case the exit handler
 * will be invoked. This allows us to do clean up during an on-demand process shutdown.
 *
 * @par Note
 * If there is no need for an exit handler, then simply set the \c fctExit parameter to NULL.
 *
 * @par Note
 * The thread name (szName) \b MUST be persistent and null terminated.. In the code the \b pointer to the
 * name will simply be copied, the string itself will not be copied (i.e. there will be no strcpy call).
 * If you use the provided macros this will not be an issue, as the entry function is string-ised into a
 * constant persistent string.
 */
pthread_t pu_thread_create(
    pu_thread_fct_t fctMain,
    void*           pMainArg,
    size_t          uiStackSize,
    const char*     szName )
{
    pthread_attr_t       attr;
    int                  iResult = -1;
    pu_thread_context_t* pNode = NULL;
    pthread_t            iPid = (pthread_t)0;

    /* pre-condition */
    ASSERT( uiPageSize > 0 );
    ASSERT( fctMain );
    ASSERT( szName );
    ASSERT( uiStackSize <= PU_THREAD_STUPID_STACKSIZE);
    if ((uiPageSize > 0) && fctMain && szName && (uiStackSize <= PU_THREAD_STUPID_STACKSIZE))
    {
        iResult = pthread_attr_init( &attr );
        ASSERT( 0 == iResult );
        if (0 == iResult)
        {
            /* set stack size and guard size */
            uiStackSize = pu_thread_stacksize_fix( uiStackSize );
            iResult = pthread_attr_setstacksize( &attr, uiStackSize );
            ASSERT( 0 == iResult );

            /* GLIBC will by default set the guard size to one page whenever we set the stack size.
             * uClibC does NOT do this, so we always force the guard size
             */
            if (0 == iResult)
            {
                iResult = pthread_attr_setguardsize( &attr, uiPageSize );
                ASSERT( 0 == iResult );
            }
            if (0 == iResult)
            {
                pNode = (pu_thread_context_t*)malloc( sizeof( pu_thread_context_t ) );
                ASSERT( NULL != pNode );
                if (NULL != pNode)
                {
                    memset( pNode, 0, sizeof(pu_thread_context_t) );
                    pNode->fctMain  = fctMain;
                    pNode->pMainArg = pMainArg;

                    /* simply copy the name pointer. This is constant and persistent,
                     * it does not need a separate allocation
                     */
                    pNode->szName = (char*)szName;
                    iResult = pthread_create(
                        &(pNode->pid),
                        &attr,
                        pu_thread_entry_handler,
                        (void*)pNode );
                    ASSERT( 0 == iResult );
                    if (0 != iResult)
                    {
                        free( pNode );
                    }

                    /* Store the PID, set the system thread name. This name is 15+null long,
                     * so will often cause the input name to be truncated. This means the debug name and
                     * the name in the system may be different.
                     */
                    else
                    {
                        iPid = pNode->pid;
                        char szSysName[16];
                        strncpy( szSysName, szName, 16 );
                        szSysName[15] = 0;
                        pthread_setname_np( iPid, szSysName );

                        /* TODO: Find a way around this!!
                         * At this point we would like to use:
                         * - pthread_setname_np()
                         * to set the thread name in the system. but it does not seem
                         * to work
                         *
                         * char szSysName[16];
                         * strncpy( szSysName, szName, 16 );
                         * szSysName[15] = 0;
                         * pthread_setname_np( iPid, szSysName );
                         */
                    }
                }
            }
            pthread_attr_destroy( &attr );
        }
    }
    if (0 == iPid)
    {
        LOG_ERROR( "PU_THREAD(create):proc=%s, cannot create %s\n", szProcName, szName );
        ASSERT(0 != iPid);
    }

    /* Done */
    return (iPid);
}
/* pu_thread_create */

/**
 * @brief Gets the number of threads in the list
 *
 * @return Number of threads in the list
 *
 * @pre       None
 * @post      Number of threads is returned
 * @invariant The list is unchanged
 *
 * @par Description
 * Simple query for number of active threads. This works in both debug and non-debug builds
 */
size_t pu_thread_get_number_of_threads( void )
{
   return (uiNumThreads);
}
/* pu_thread_get_number_of_threads */

