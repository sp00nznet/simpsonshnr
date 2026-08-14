/*
 * PS3 SDK Stub - cell/sysmodule.h
 * Stub implementation for PS3 port compilation
 */

#ifndef _CELL_SYSMODULE_H
#define _CELL_SYSMODULE_H

#ifdef __cplusplus
extern "C" {
#endif

// Module IDs
#define CELL_SYSMODULE_FS          0x000C
#define CELL_SYSMODULE_IO          0x0001
#define CELL_SYSMODULE_GCM_SYS     0x0010
#define CELL_SYSMODULE_AUDIO       0x0011
#define CELL_SYSMODULE_MIXER       0x0012
#define CELL_SYSMODULE_NET         0x0013
#define CELL_SYSMODULE_NETCTL      0x0014
#define CELL_SYSMODULE_HTTP        0x0015
#define CELL_SYSMODULE_USBD        0x0016
#define CELL_SYSMODULE_PAD         0x0017
#define CELL_SYSMODULE_SAVEDATA    0x0018
#define CELL_SYSMODULE_RESC        0x0019
#define CELL_SYSMODULE_SYSUTIL     0x0015
#define CELL_SYSMODULE_PNGDEC      0x0031

// Stub functions
inline int cellSysmoduleLoadModule(int id) { return 0; }
inline int cellSysmoduleUnloadModule(int id) { return 0; }

#ifdef __cplusplus
}
#endif

#endif /* _CELL_SYSMODULE_H */
