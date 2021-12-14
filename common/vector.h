#ifndef VECTOR_INCLUDED_H
#define VECTOR_INCLUDED_H

#include <stdlib.h>

#define TEMPLATE_VECTOR(VECTOR_data_type) typedef struct                      \
{                                                                             \
    VECTOR_data_type * begin;                                                 \
    VECTOR_data_type * end;                                                   \
    size_t capacity;                                                          \
}

#define NEW_VECTOR(CNEW_VECTOR_v) do {                                        \
    (CNEW_VECTOR_v).capacity = 10;                                            \
    (CNEW_VECTOR_v).begin                                                     \
        = malloc((CNEW_VECTOR_v).capacity * sizeof(*(CNEW_VECTOR_v).begin));  \
    (CNEW_VECTOR_v).end = (CNEW_VECTOR_v).begin;                              \
} while(0)

#define CONCATENATE(CCONCATENATE_reciever, CCONCATENATE_giver, CCONCATENATE_type) do { \
    const size_t CCONCATENATE_new_cap                                         \
        = SIZE((CCONCATENATE_reciever)) + SIZE((CCONCATENATE_giver));         \
    RESERVE((CCONCATENATE_reciever), CCONCATENATE_new_cap);                   \
    for(CCONCATENATE_type const * CCONCATENATE_it = (CCONCATENATE_giver).begin; \
        CCONCATENATE_it!=(CCONCATENATE_giver).end;                            \
        ++CCONCATENATE_it)                                                    \
    {                                                                         \
        *(CCONCATENATE_reciever).end = *CCONCATENATE_it;                      \
        ++(CCONCATENATE_reciever).end;                                        \
    }                                                                         \
} while(0)

#define PUSH(CPUSH_v, CPUSH_data) do {                                        \
    const size_t PUSH_size = (CPUSH_v).end - (CPUSH_v).begin;                 \
    if(PUSH_size >= (CPUSH_v).capacity) {                                     \
        (CPUSH_v).capacity += 3;   /* Minimum size */                         \
        (CPUSH_v).capacity *= 1.6; /* Growth factor */                        \
        (CPUSH_v).begin = realloc(                                            \
            (CPUSH_v).begin,                                                  \
            (CPUSH_v).capacity * sizeof(*(CPUSH_v).begin));                   \
        (CPUSH_v).end = (CPUSH_v).begin + PUSH_size;                          \
    }                                                                         \
    *(CPUSH_v).end = CPUSH_data;                                              \
    ++(CPUSH_v).end;                                                          \
} while(0)

#define POP(CPOP_v) do { --(CPOP_v).end; } while(0)

#define CLEAR(CCLEAR_v) do {                                                  \
    free((CCLEAR_v).begin);                                                   \
    (CCLEAR_v).begin = NULL;                                                  \
    (CCLEAR_v).end = NULL;                                                    \
    (CCLEAR_v).capacity = 0;                                                  \
} while(0)

#define SIZE(CSIZE_v) ((CSIZE_v).end - (CSIZE_v).begin)

#define PRINT(CPRINT_v, CPRINT_type, CPRINT_format) do {                      \
    CPRINT_type* CPRINT_it=(CPRINT_v).begin;                                  \
    printf("[ ");                                                             \
    while(CPRINT_it != (CPRINT_v).end) {                                      \
        printf(CPRINT_format, *CPRINT_it);                                    \
        ++CPRINT_it;                                                          \
        if(CPRINT_it == (CPRINT_v).end) break;                                \
        printf(", ");                                                         \
    }                                                                         \
    printf(" ]");                                                             \
} while(0)

#define RESERVE(CRESERVE_v, CRESERVE_size) do {                               \
    if((CRESERVE_v).capacity >= (CRESERVE_size)) break;                       \
    size_t len = SIZE(CRESERVE_v);                                            \
    (CRESERVE_v).begin = realloc(                                             \
        (CRESERVE_v).begin,                                                   \
        (CRESERVE_size) * sizeof(*(CRESERVE_v).begin));                       \
    (CRESERVE_v).end   = (CRESERVE_v).begin + len;                            \
    (CRESERVE_v).capacity = (CRESERVE_size);                                  \
} while(0)

#define SHRINK(CSHRINK_v) do {                                                \
    if((CSHRINK_v).begin == NULL) break;                                      \
    const size_t new_size = SIZE(CSHRINK_v);                                  \
    (CSHRINK_v).begin = realloc(                                              \
        (CSHRINK_v).begin,                                                    \
        new_size * sizeof(*(CSHRINK_v).begin));                               \
    (CSHRINK_v).end = (CSHRINK_v).begin + new_size;                           \
    (CSHRINK_v).capacity = new_size;                                          \
} while(0)

#define MIN_ENTRY(CMIN_ENTRY_v, CMIN_ENTRY_type, CMIN_ENTRY_result) do {      \
    (CMIN_ENTRY_result) = (CMIN_ENTRY_v).begin;                               \
    for(CMIN_ENTRY_type * CMIN_ENTRY_it = (CMIN_ENTRY_v).begin;               \
        CMIN_ENTRY_it != (CMIN_ENTRY_v).end;                                  \
        ++CMIN_ENTRY_it)                                                      \
    {                                                                         \
        if(*CMIN_ENTRY_it < *(CMIN_ENTRY_result))                             \
            (CMIN_ENTRY_result) = CMIN_ENTRY_it;                              \
    }                                                                         \
} while(0)

#define MAX_ENTRY(CMAX_ENTRY_v, CMAX_ENTRY_type, CMAX_ENTRY_result) do {      \
    (CMAX_ENTRY_result) = (CMAX_ENTRY_v).begin;                               \
    for(CMAX_ENTRY_type * CMAX_ENTRY_it = (CMAX_ENTRY_v).begin;               \
        CMAX_ENTRY_it != (CMAX_ENTRY_v).end;                                  \
        ++CMAX_ENTRY_it)                                                      \
    {                                                                         \
        if(*CMAX_ENTRY_it > *(CMAX_ENTRY_result))                             \
            (CMAX_ENTRY_result) = CMAX_ENTRY_it;                              \
    }                                                                         \
} while(0)

#define COMP_ENTRY(CCOMP_ENTRY_v, CCOMP_ENTRY_type, CCOMP_ENTRY_result, CCOMP_ENTRY_compare) do {  \
    (CCOMP_ENTRY_result) = (CCOMP_ENTRY_v).begin;                             \
    for(CCOMP_ENTRY_type * COMP_ENTRY_it = (CCOMP_ENTRY_v).begin;             \
        COMP_ENTRY_it != (CCOMP_ENTRY_v).end;                                 \
        ++COMP_ENTRY_it)                                                      \
    {                                                                         \
        if(CCOMP_ENTRY_compare(COMP_ENTRY_it, (CCOMP_ENTRY_result)))          \
            (CCOMP_ENTRY_result) = COMP_ENTRY_it;                             \
    }                                                                         \
} while(0)


#define SWAP(CSWAP_type, CSWAP_it1, CSWAP_it2) do {                           \
    if((CSWAP_it1) == (CSWAP_it2)) break;                                     \
    const CSWAP_type CSWAP_tmp = *(CSWAP_it1);                                \
    *(CSWAP_it1) = *(CSWAP_it2);                                              \
    *(CSWAP_it2) = CSWAP_tmp;                                                 \
} while(0)


#define DECLARE_FIND(CFIND_fun_name, CFIND_type)                              \
CFIND_type * CFIND_fun_name(                                                  \
    CFIND_type * const begin,                                                 \
    CFIND_type * const end,                                                   \
    const CFIND_type * const search,                                          \
    const bool is_sorted)

/* 
 * Must be sorted in ascending order
 */
#define DEFINE_FIND(CFIND_fun_name, CFIND_type)                               \
CFIND_type * CFIND_fun_name(                                                  \
    CFIND_type * begin,                                                       \
    CFIND_type * end,                                                         \
    const CFIND_type * const search,                                          \
    const bool is_sorted){                                                    \
    if(is_sorted && (end-begin) < 5)                                          \
    {                                                                         \
        CFIND_type * guess = begin + (end - begin)/2;                         \
        int comparisson = guess > search;                                     \
        if(guess > search)                                                    \
            return CFIND_fun_name(begin, guess, search, true);                \
        if(guess ==  search) return guess;                                    \
        return CFIND_fun_name(begin, guess, search, true);                    \
    }                                                                         \
    for(CFIND_type * it = begin; it != end; ++it)                             \
    {                                                                         \
        if(it == search) return it;                                           \
    }                                                                         \
    return NULL;                                                              \
}

#define DECLARE_FIND_COMP(CFIND_fun_name, CFIND_type)                         \
CFIND_type * CFIND_fun_name(                                                  \
    CFIND_type * const begin,                                                 \
    CFIND_type * const end,                                                   \
    const CFIND_type * const search,                                          \
    const bool is_sorted)

/* 
 * Comparison must be the same function used to sort the vector
 */
#define DEFINE_FIND_COMP(CFIND_fun_name, CFIND_type, CFIND_comparisson)       \
CFIND_type * CFIND_fun_name(                                                  \
    CFIND_type * begin,                                                       \
    CFIND_type * end,                                                         \
    const CFIND_type * const search,                                          \
    const bool is_sorted){                                                    \
    if(is_sorted && (end-begin) < 5)                                         \
    {                                                                         \
        CFIND_type * guess = begin + (end - begin)/2;                         \
        int comparisson = CFIND_comparisson(guess, search);                   \
        if(comparisson == 1) /*guess > search*/                               \
            return CFIND_fun_name(begin, guess, search, true);                \
        if(comparisson ==  0) return guess;                                   \
        return CFIND_fun_name(begin, guess, search, true);                    \
    }                                                                         \
    for(CFIND_type * it = begin; it != end; ++it)                             \
    {                                                                         \
        if(CFIND_comparisson(it, search)==0) return it;                       \
    }                                                                         \
    return NULL;                                                              \
}

#define DECLARE_QUICKSORT(CQS_FUN_NAME, CQS_type)                             \
void CQS_FUN_NAME(CQS_type * CQS_begin, CQS_type * CQS_end) {                 \

#define DEFINE_QUICKSORT(CQS_FUN_NAME, CQS_type)                              \
void CQS_FUN_NAME(CQS_type * CQS_begin, CQS_type * CQS_end) {                 \
    if(CQS_end - CQS_begin <= 1) return;                                      \
    CQS_type CQS_pivot = *CQS_begin;                                          \
    SWAP(CQS_type, CQS_begin, CQS_end-1);                                     \
    CQS_type * CQS_unsorted_begin = CQS_begin;                                \
    CQS_type * CQS_unsorted_end = CQS_end-1;                                  \
    for(CQS_type * CQS_it = CQS_unsorted_begin;                               \
        CQS_it != CQS_unsorted_end;                                           \
        ++CQS_it)                                                             \
    {                                                                         \
        if(*CQS_it <= CQS_pivot) {                                            \
            SWAP(CQS_type, CQS_unsorted_begin, CQS_it);                       \
            if(CQS_it != CQS_unsorted_begin) --CQS_it;                        \
            ++CQS_unsorted_begin;                                             \
        } else {                                                              \
            --CQS_unsorted_end;                                               \
            SWAP(CQS_type, CQS_unsorted_end, CQS_it);                         \
            --CQS_it;                                                         \
        }                                                                     \
    }                                                                         \
    CQS_FUN_NAME(CQS_begin, CQS_unsorted_begin);                              \
    CQS_FUN_NAME(CQS_unsorted_end, CQS_end);                                  \
}

#define DECLARE_QUICKSORT_COMP(CQSC_FUN_NAME, CQSC_type)                      \
void CQSC_FUN_NAME(CQSC_type * CQSC_begin, CQSC_type * CQSC_end);

#define DEFINE_QUICKSORT_COMP(CQSC_FUN_NAME, CQSC_type, CQSC_comparisson)     \
void CQSC_FUN_NAME(CQSC_type * CQSC_begin, CQSC_type * CQSC_end) {            \
    if(CQSC_end - CQSC_begin <= 1) return;                                    \
    SWAP(CQSC_type, CQSC_begin, CQSC_end-1);                                  \
    CQSC_type * CQSC_pivot = CQSC_end-1;                                      \
    CQSC_type * CQSC_unsorted_begin = CQSC_begin;                             \
    CQSC_type * CQSC_unsorted_end = CQSC_end-1;                               \
    for(CQSC_type * CQSC_it = CQSC_unsorted_begin;                            \
        CQSC_it != CQSC_unsorted_end;                                         \
        ++CQSC_it)                                                            \
    {                                                                         \
        if(CQSC_comparisson(CQSC_it, CQSC_pivot) != 1) {                      \
            SWAP(CQSC_type, CQSC_unsorted_begin, CQSC_it);                    \
            if(CQSC_it > CQSC_unsorted_begin) --CQSC_it;                      \
            ++CQSC_unsorted_begin;                                            \
        } else {                                                              \
            --CQSC_unsorted_end;                                              \
            SWAP(CQSC_type, CQSC_unsorted_end, CQSC_it);                      \
            --CQSC_it;                                                        \
        }                                                                     \
    }                                                                         \
    CQSC_FUN_NAME(CQSC_begin, CQSC_unsorted_begin);                           \
    CQSC_FUN_NAME(CQSC_unsorted_end, CQSC_end);                               \
}

#define STRING_TO_VECTOR(S2V_vector, S2V_line, S2V_separator, S2V_data_type) do {   \
    char * S2V_begin = (S2V_line);                                            \
    for(char * S2V_end = (S2V_line); *S2V_end != '\0'; ++S2V_end) {           \
        if(strchr((S2V_separator), *S2V_end)) {                               \
            *S2V_end = '\0';                                                  \
            const S2V_data_type val = (S2V_data_type) atol(S2V_begin);        \
            PUSH((S2V_vector), val);                                          \
            S2V_begin = S2V_end + 1;                                          \
        }                                                                     \
    }                                                                         \
    if(*S2V_begin != '\0')                                                    \
    {                                                                         \
        const S2V_data_type val = (S2V_data_type) atol(S2V_begin);            \
        PUSH((S2V_vector), val);                                              \
    }                                                                         \
} while(0)

#endif
