/* PS3 SDK string.h wrapper - fixes C++ inline overload conflict with GCC 4.1.1 */
/* The SDK's string.h has C++ inline overloads inside extern "C" scope which breaks */
#ifndef _PS3_STRING_FIX_H_
#define _PS3_STRING_FIX_H_

/* Skip the C++ inline overloads in SDK string.h - they conflict with extern "C" scope */
#ifndef _NO_CPP_INLINES
#define _NO_CPP_INLINES
#define _PS3_FIX_UNDEF_NO_CPP_INLINES
#endif

/* Include the real SDK string.h using #include_next */
#include_next <string.h>

/* Restore state and add our own overloads */
#ifdef _PS3_FIX_UNDEF_NO_CPP_INLINES
#undef _NO_CPP_INLINES
#undef _PS3_FIX_UNDEF_NO_CPP_INLINES
#endif

/* Bring standard string functions into global namespace for C++ code */
#if defined(__cplusplus)

/* Import string functions from std namespace */
using std::memcmp;
using std::memcpy;
using std::memset;
using std::memmove;
using std::strcat;
using std::strcmp;
using std::strcpy;
using std::strlen;
using std::strcoll;
using std::strcspn;
using std::strerror;
using std::strncat;
using std::strncmp;
using std::strncpy;
using std::strspn;
using std::strtok;
using std::strxfrm;
using std::strchr;
using std::strpbrk;
using std::strrchr;
using std::strstr;
using std::memchr;

/* Provide C++-safe non-const overloads outside of extern "C" */
inline char* strchr(char* _s, int _c) {
    return const_cast<char*>(std::strchr(const_cast<const char*>(_s), _c));
}

inline char* strpbrk(char* _s1, const char* _s2) {
    return const_cast<char*>(std::strpbrk(const_cast<const char*>(_s1), _s2));
}

inline char* strrchr(char* _s, int _c) {
    return const_cast<char*>(std::strrchr(const_cast<const char*>(_s), _c));
}

inline char* strstr(char* _s1, const char* _s2) {
    return const_cast<char*>(std::strstr(const_cast<const char*>(_s1), _s2));
}

inline void* memchr(void* _s, int _c, size_t _n) {
    return const_cast<void*>(std::memchr(const_cast<const void*>(_s), _c, _n));
}

/* strcmpi/stricmp compatibility - Windows-specific case-insensitive strcmp */
inline int p3d_strcasecmp(const char* s1, const char* s2)
{
    while (*s1 && *s2)
    {
        int c1 = *s1;
        int c2 = *s2;
        if (c1 >= 'A' && c1 <= 'Z') c1 += 'a' - 'A';
        if (c2 >= 'A' && c2 <= 'Z') c2 += 'a' - 'A';
        if (c1 != c2) return c1 - c2;
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

#ifndef strcmpi
#define strcmpi p3d_strcasecmp
#endif

#ifndef stricmp
#define stricmp p3d_strcasecmp
#endif

#ifndef _stricmp
#define _stricmp p3d_strcasecmp
#endif

#endif /* __cplusplus */

#endif /* _PS3_STRING_FIX_H_ */
