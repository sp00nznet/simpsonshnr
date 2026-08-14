/*
 * PS3 SDK Stub - sys/ppu_thread.h
 * Stub implementation for PS3 port compilation
 */

#ifndef _SYS_PPU_THREAD_H
#define _SYS_PPU_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long sys_ppu_thread_t;

// Thread priority levels
#define SYS_PPU_THREAD_PRIORITY_MAX 0
#define SYS_PPU_THREAD_PRIORITY_MIN 3071

// Thread creation flags
#define SYS_PPU_THREAD_CREATE_JOINABLE  0x0001
#define SYS_PPU_THREAD_CREATE_INTERRUPT 0x0002

// Timer functions (used for sleep)
inline void sys_timer_usleep(unsigned long long usec) {}
inline void sys_timer_sleep(unsigned int sec) {}

// Thread functions
inline int sys_ppu_thread_create(
    sys_ppu_thread_t* thread_id,
    void (*entry)(unsigned long long),
    unsigned long long arg,
    int prio,
    unsigned int stacksize,
    unsigned long long flags,
    const char* threadname)
{
    return 0;
}

inline int sys_ppu_thread_join(sys_ppu_thread_t thread_id, unsigned long long* vptr) { return 0; }
inline int sys_ppu_thread_yield() { return 0; }
inline void sys_ppu_thread_exit(unsigned long long val) {}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_PPU_THREAD_H */
