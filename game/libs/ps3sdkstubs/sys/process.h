/*
 * PS3 SDK Stub - sys/process.h
 * Stub implementation for PS3 port compilation
 */

#ifndef _SYS_PROCESS_H
#define _SYS_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

// SYS_PROCESS_PARAM macro - defines process parameters
#define SYS_PROCESS_PARAM(prio, stacksz) \
    static const char _sys_process_param[] __attribute__((unused)) = ""

// Process exit
inline void sys_process_exit(int status) {}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_PROCESS_H */
