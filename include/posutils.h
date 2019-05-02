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
#ifndef _POSUTILS_H_
#define _POSUTILS_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file     posutils.h
 * @date     2019-04-30
 * @author   Martin
 * @brief    Some posix utilities
 * Interface for:
 * - Thread creation
 * - Mutex creation
 */

/**** Includes ***************************************************************/
#if !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
#endif /* !defined(_GNU_SOURCE) */
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include "logging.h"

/**** Definitions ************************************************************/

/*===========================================================================*/
/* CENTRAL INIT AND EXIT                                                     */
/*===========================================================================*/

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
 */
int posutils_init( void );

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
int posutils_exit( void );

/*===========================================================================*/
/* POSIX MUTEX FUNCTIONS                                                     */
/*===========================================================================*/
/**
 * @defgroup PMTX Posix mutex utility
 * @ingroup  SYSUTILS
 *
 * @brief
 * This is a factory that creates standard pthread mutexes. It hides the complexity
 * of the creation from the user.
 *
 * @section pmtx_sect_1 Fast mutexes
 * The mutex may be created as a simple mutex for. This is the 99% case. These mutexes a used for
 * small fast critical section implementation. Semaphores should \b NOT be used for this!.
 *
 * @section pmtx_sect_2 Error mutexes
 * These are used in scenarios where it is important to know where and when a deadlock occurs. A macro is provided
 * trap deadlocks (\ref PU_MUTEX_LOCK_ERROR). This macro throws a fatal log if a deadlock is detected.
 *
 * @section pmtx_sect_3 Recursive mutexes
 * Last and definitely least, a mutex may be created as a recursive mutex for scenarios where nesting may occur
 * and you want to avoid deadlock. A recursive mutex has the following characteristics:
 * - only an owner may unlock the mutex
 * - the owner may lock the mutex multiple times without deadlocking
 * - the number of locks and unlocks must be symmetrical before the mutex is released
 * .
 * Recursive mutexes are to be \b AVOIDED. Recursive mutexes mean:
 * - poor performance (they are much slower than fast and error mutexes)
 * - poorly implemented logic
 * .
 *
 * @section pmtx_sect_4 Mutex usage
 * The factory simply creates a standard Posix pthread mutex with some constraints. All the
 * mutex calls may be used as normal, i.e.:
 * - \c pthread_mutex_lock( pthread_mutex_t* )
 * - \c pthread_mutex_unlock( pthread_mutex_t* )
 * - \c pthread_mutex_trylock( pthread_mutex_t* )
 * .
 *
 * @{
 */

/**
 * @brief Mutex types supported
 * The recommendation is to ALWAYS use the fast (default)
 *
 * @note
 * The error mutex is typically used to trap deadlocks. Use it sparingly and to trap specific debug
 * scenarios.
 *
 * @note
 * Avoid the use of recursive mutexes. They are slow, and the usage typically
 * indicates poor implementation logic.
 */
typedef enum
{
    PU_MUTEX_TYPE_FAST,     /*!< Default (timed) fast mutex       */
    PU_MUTEX_TYPE_ERROR,    /*!< Error mutex, to trap deadlocks   */
    PU_MUTEX_TYPE_RECURSIVE,/*!< Recursive, avoid like the plague */
    PU_MUTEX_TYPE_ENDDEF    /* Enum terminator                    */
}   pu_mutex_type;

/**
 * @brief   Creates (initialises) a mutex of the specified type with the constraints applied
 *
 * @param[in] pMtx   : Pointer to a valid mutex
 * @param[in] enType : Mutex type
 * @retval  0 for success
 * @retval  Non-zero for failure
 *
 * @pre     The mutex pointer is non-null
 * @post    The mutex is initialised
 *
 * @par Side Effects
 * None
 *
 * @par Description
 * Creates a standard Posix pthread mutex. An example is shown below:
 * @code
 * int              iResult;
 * pthread_mutex_t* pMtx = malloc( sizeof(pthread_mutex_t) );
 * O_ASSERT( pMtx );
 * if (pMtx)
 * {
 *     // create a fast mutex
 *     iResult = pu_mutex_create_type( pMtx, PU_MUTEX_TYPE_FAST );
 *     O_ASSERT( 0 == iResult );
 *     if (0 == iResult)
 *     {
 *         // do lots of useful things..
 *     }
 * }
 * @endcode
 */
int pu_mutex_create_type(
    pthread_mutex_t* pMtx,
    pu_mutex_type    enType );

/**
 * @param pMtx: pointer to a mutex
 *
 * @par Description
 * Locks an error checking mutex, and checks for the deadlock condition.
 *
 * @note
 * \b ONLY for use with the \ref PU_MUTEX_TYPE_ERROR mutex type
 */
#define PU_MUTEX_LOCK_ERROR(pMtx) {if (EDEADLK == pthread_mutex_lock( pMtx )) {LOG_FATAL( "MUTEX DEADLOCK (0x%zu)\n", (size_t)(pMtx) );}}

/**
 * @param pMtx: pointer to a mutex
 *
 * @par Description
 * Unlocks an error checking mutex, and checks for the case where the caller is not the "owner",
 * i.e. the owner did not originally lock the mutex. This is an unlikely scenario as mutexes are
 * normally (in well written code) hidden inside interfaces. It is included for completeness simply
 * because the error checking mutex type can detect this scenario.
 *
 * @note
 * \b ONLY for use with the \ref PU_MUTEX_TYPE_ERROR mutex type
 */
#define PU_MUTEX_UNLOCK_ERROR(pMtx) {if (EPERM == pthread_mutex_unlock( pMtx )) {LOG_FATAL("CANT UNLOCK MTX, NOT OWNER (0x%08x)\n", (size_t)(pMtx) );}}

/**
 * @deprecated This macro is retained for backwards compatibility. Use \ref pu_mutex_create_type
 * @brief   Creates (initialises) a mutex with the constraints applied
 *
 * @param[in] pMtx      : Pointer to a valid mutex
 * @param[in] bRecursive: true or false
 * @retval  0 for success
 * @retval  Non-zero for failure
 *
 * @pre     The mutex pointer is non-null
 * @post    The mutex is initialised
 *
 * @par Description
 * Creates a standard Posix pthread mutex. An example is shown below:
 * @code
 * int              iResult;
 * pthread_mutex_t* pMtx = malloc( sizeof(pthread_mutex_t) );
 * O_ASSERT( pMtx );
 * if (pMtx)
 * {
 *     // create recursive mutex
 *     iResult = pu_mutex_create( pMtx, true );
 *     O_ASSERT( 0 == iResult );
 *     if (0 == iResult)
 *     {
 *         // do lots of useful things..
 *     }
 * }
 * @endcode
 *
 * @note
 * Avoid the use of recursive mutexes. They are slow, and the usage typically
 * indicates poor implementation logic.
 */
#define pu_mutex_create( pMtx, bRecursive ) (pu_mutex_create_type(pMtx,(true == bRecursive ? PU_MUTEX_TYPE_RECURSIVE : PU_MUTEX_TYPE_FAST)))

/**
 * @}
 */

/*===========================================================================*/
/* POSIX THREAD FUNCTIONS                                                    */
/*===========================================================================*/
/**
 * @brief Posix thread utilities
 * @defgroup OPTHREAD Posix thread utilities
 * @ingroup  SYSUTILS
 * This is a factory that produces Posix pthreads. It handles the multiple creation
 * steps internally, and constrains various options, like:
 * - scheduling scheme (SCHED_OTHER)
 * - fixed stack sizes
 * - guards for dumb systems (like uClibc)
 * .
 *
 * @par Pthread usage
 * The factory creates a standard Posix pthread with some constraints. All the
 * thread calls (\c pthread_xxx()) may be used as normal.
 *
 * @par Thread lists
 * The factory keeps (on a per process basis) a list of all the threads. As they are created
 * they are added to the list, as they exit they are moved from the list. This list can
 * iterated through. This allows for things like debug and graceful shutdown.
 *
 * @{
 */

/**
 * @brief   The standard function type for a pthread entry point (main)
 *
 * @param[in] pArg : Argument
 * @retval  Exit value
 *
 * @pre     None
 * @pre     None
 *
 * @par Side Effects
 * None
 *
 * @par Description
 * The function type for a pthread entry point (main)
 */
typedef void* (*pu_thread_fct_t)( void* pArg );

/**
 * @brief   Creates a non-RT pthread with the Comet constraints applied
 *
 * @param[in] fctMain     : Thread main function (entry point)
 * @param[in] pMainArg    : Argument for main
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
    const char*     szName );

/**
 * @brief   Creates an automatically named pthread with exit handler
 *
 * @param[in] mainfct_    : Thread main function (entry point)
 * @param[in] mainarg_    : Main argument
 * @param[in] exitfct_    : Thread exit handler function
 * @param[in] exitarg_    : Exit handler argument
 * @param[in] stack_size_ : Stack size
 * @retval  A non-zero pthread ID indicates success
 * @retval  A zero pthread ID means failure
 *
 * @pre     The entry function (main) is non-NULL
 * @post    The pthread is created.
 *
 * @par Description
 * This is a helper macro that creates an automatically named non-RT pthread. The name is simply
 * a string-ised form of the main entry function. All of the constraints of the underlying
 * thread creation function still apply.
 * @see pu_thread_create
 */
#define PU_THREAD_CREATE(mainfct_,mainarg_,stack_size_)    \
    pu_thread_create(                                      \
        (mainfct_),(mainarg_),                             \
        (stack_size_),                                     \
        #mainfct_)

/**
 * @brief Gets the number of threads running
 *
 * @return Number of threads in the list
 *
 * @pre       None
 * @post      Number of threads is returned
 * @invariant The list is unchanged
 *
 * @par Description
 * Simple query for number of active threads. This works in both debug and non-debug builds.
 * Note that this will only return the number of active threads created with the "pu_thread" interface.
 */
size_t pu_thread_get_number_of_threads( void );


/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _POSUTILS_H_ */
