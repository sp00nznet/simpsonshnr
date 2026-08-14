//=============================================================================
// ps3tty.h
//
// Heap-safe TTY tracing for the PS3 port.
//
// printf must never be used on PS3 -- it fights the game's custom dlmalloc
// allocator and corrupts the heap -- and rReleasePrintf is compiled out
// (raddebug.hpp defines rReleaseString to nothing). So diagnostics go
// straight to sys_tty_write, which touches no heap.
//
// No-op on every other platform, so calls can be left in place.
//=============================================================================
#ifndef PS3TTY_H
#define PS3TTY_H

#ifdef RAD_PS3

#include <sys/tty.h>

// Writes "<a><b> <n>\n". b may be NULL, n < 0 suppresses the number.
inline void radPs3Trace( const char* a, const char* b = 0, int n = -1 )
{
    char buf[ 512 ];
    int i = 0;
    while( a && a[ i ] && i < 200 ) { buf[ i ] = a[ i ]; ++i; }
    int j = 0;
    while( b && b[ j ] && i < 460 ) { buf[ i++ ] = b[ j++ ]; }
    if( n >= 0 )
    {
        char digits[ 12 ];
        int d = 0;
        do { digits[ d++ ] = (char)( '0' + n % 10 ); n /= 10; } while( n );
        buf[ i++ ] = ' ';
        while( d ) { buf[ i++ ] = digits[ --d ]; }
    }
    buf[ i++ ] = '\n';
    unsigned int written;
    sys_tty_write( 0, buf, i, &written );
}

// Writes "<a> 0x<v>\n" -- for pointers and anything worth seeing in hex.
inline void radPs3TraceHex( const char* a, unsigned int v )
{
    static const char* kHex = "0123456789abcdef";
    char buf[ 256 ];
    int i = 0;
    while( a && a[ i ] && i < 200 ) { buf[ i ] = a[ i ]; ++i; }
    buf[ i++ ] = ' ';
    buf[ i++ ] = '0';
    buf[ i++ ] = 'x';
    for( int shift = 28; shift >= 0; shift -= 4 )
    {
        buf[ i++ ] = kHex[ ( v >> shift ) & 0xF ];
    }
    buf[ i++ ] = '\n';
    unsigned int written;
    sys_tty_write( 0, buf, i, &written );
}

#else

inline void radPs3Trace( const char*, const char* = 0, int = -1 ) {}
inline void radPs3TraceHex( const char*, unsigned int ) {}

#endif // RAD_PS3

#endif // PS3TTY_H
