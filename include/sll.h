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
 * @file     sll.h
 * @author   Martin Gibson
 * @date     2019-05-01
 */
#ifndef __SLL_H_
#define __SLL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Single Linked List macros
 * @defgroup SLL Single Linked List macros
 * @ingroup SYSUTILS
 * A set of simple Single Linked List macros.
 * Yes, yes, I know I said that everyone should use the SLIST_XXX macros from <sys/queue.h>.
 * However, in my defence, setup, creation and initialisation of these is completely
 * incomprehensible. The only documentation is poorly authored MAN pages. The example below shows
 * an extremely simple case where we init a structure, add some elements, remove some elements.
 * @code
 * // Structure with forward (tag) definition
 * typedef struct my_struct_tag
 * {
 *     int iData;                 // Some data
 *     SLL_ENTRY(my_struct_tag);  // The linked list element
 * }   my_struct_t;
 *
 * // Static definition of pMyList, initialised to empty
 * static my_struct_t* MyList = NULL;
 *
 * int main( void )
 * {
 *     my_struct_t* pElem1;
 *     my_struct_t* pElem2;
 *     my_struct_t* pTmp;
 *
 *     // Create the elements
 *     pElem1 = (my_struct_t*)malloc( sizeof(my_struct_t) );
 *     pElem1->iData = 1;
 *     pElem2 = (my_struct_t*)malloc( sizeof(my_struct_t) );
 *     pElem2->iData = 2;
 *
 *     // Add the elements to the list, the 1st simply to the head, the 2nd to the tail
 *     SLL_ELEM_ADD( pMyList, pElem1 );
 *     SLL_ELEM_ADD_TAIL( pMyList, pElem2 );
 *
 *     // Show the data of every element in the list
 *     SLL_FOR_EACH( pMyList, pTmp )
 *     {
 *         printf( "iData = %d\n", pTmp->iData );
 *     }
 *
 *     // Delete the elements (one extra parameter for the type)
 *     SLL_ELEM_DEL( my_struct_t, pMyList, pElem1 );
 *     free( pElem1 );
 *     SLL_ELEM_DEL( my_struct_t, pMyList, pElem2  );
 *     free( pElem2 );
 * }
 * @endcode
 *
 * @{
 */

/**** Includes ***************************************************************/

/**** Definitions ************************************************************/

/**
 * This is the "next" element that is used to refer to the next struct in the list.
 * @param[in] tag_type : The forward referenced "tag" type of the structure
 *
 * @par Description
 * This is added to a structure to make it link-list-able.
 * In order for a structure to be used in a linked list you need:
 * - the structure to be created in tag notation
 * - the structure to contain this macro
 *
 * @note
 * We don't use the name "pNext" as it is common enough to cause conflicts
 */
#define SLL_ENTRY( tag_type ) struct tag_type* pSllNextElem

/**
 * Adds an element to the list
 * @param[in] list : The list to add the element to
 * @param[in] elem : The element to add (type*)
 *
 * @par Description
 * Adds the element to the list. The type of the element must be "type*", i.e. a pointer to a structure
 * of the correct type. Always adds to the head.
 */
#define SLL_ELEM_ADD( list, elem ) {\
if (NULL == (list)) {               \
    (elem)->pSllNextElem = NULL;    \
    (list) = (elem);                \
} else {                            \
    (elem)->pSllNextElem = (list);  \
    (list) = (elem);                \
}}

/**
 * Adds an element to the tail of the list
 * @param[in] type : The structure type used in the SLL
 * @param[in] list : The list to add the element to
 * @param[in] elem : The element to add (type*)
 *
 * @par Description
 * Adds the element to the list. The type of the element must be "type*", i.e. a pointer to a structure
 * of the correct type. Always adds to the tail. The \ref SLL_ELEM_ADD is much more efficient, this call is
 * for cases where you want to treat the SLL like a queue, i.e. FIFO
 */
#define SLL_ELEM_ADD_TAIL( type, list, elem ) { \
if (NULL == (list)) {                           \
    (elem)->pSllNextElem = NULL;                \
    (list) = (elem);                            \
} else {                                        \
    type* pCurr = (list);                       \
    while (pCurr->pSllNextElem) {               \
        pCurr = pCurr->pSllNextElem;            \
     }                                          \
    pCurr->pSllNextElem = (elem);               \
    (elem)->pSllNextElem = NULL;                \
}}

/**
 * Removes an element from the list
 * @param[in] type : The structure type used in the SLL
 * @param[in] list : The list to add the element to
 * @param[in] elem : The element to add (type*)
 *
 * @par Description
 * Removes an element from the list. This will do nothing if the element to
 * remove is not found.
 */
#define SLL_ELEM_DEL( type, list, elem ) {              \
    if ((elem) == (list)) { /* head.. */                \
        (list) = (elem)->pSllNextElem;                  \
    } else {  /* aargh, not the head.. */               \
        type* pCurr = (list);                           \
        type* pPrev = NULL;                             \
        while (pCurr && (pCurr != (elem))) {            \
            pPrev = pCurr;                              \
            pCurr = pCurr->pSllNextElem;                \
        }                                               \
        if (pPrev && pCurr && (pCurr == (elem))) {      \
            pPrev->pSllNextElem = pCurr->pSllNextElem;  \
        }                                               \
    }                                                   \
}

/**
 * Used to iterate through all the elements in the list
 * @param[in] list : The list
 * @param[in] temp : The temporary pointer for the current element
 */
#define SLL_FOR_EACH( list, temp ) for ((temp) = (list); (temp) != NULL; (temp) = (temp)->pSllNextElem)

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SLL_H_ */
