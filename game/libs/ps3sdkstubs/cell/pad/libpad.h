/*
 * PS3 SDK Stub - cell/pad/libpad.h
 * Stub implementation for PS3 port compilation
 */

#ifndef _CELL_PAD_LIBPAD_H
#define _CELL_PAD_LIBPAD_H

#ifdef __cplusplus
extern "C" {
#endif

#define CELL_PAD_MAX_PORT_NUM 7
#define CELL_PAD_MAX_CODES    64

// Pad info struct
typedef struct CellPadInfo2 {
    unsigned int max_connect;
    unsigned int now_connect;
    unsigned int system_info;
    unsigned int port_status[CELL_PAD_MAX_PORT_NUM];
    unsigned int port_setting[CELL_PAD_MAX_PORT_NUM];
    unsigned int device_capability[CELL_PAD_MAX_PORT_NUM];
    unsigned int device_type[CELL_PAD_MAX_PORT_NUM];
} CellPadInfo2;

// Pad data struct
typedef struct CellPadData {
    int len;
    unsigned short button[CELL_PAD_MAX_CODES];
} CellPadData;

// Pad capability info
typedef struct CellPadCapabilityInfo {
    unsigned int info[CELL_PAD_MAX_CODES];
} CellPadCapabilityInfo;

// Actuator (vibration) data
typedef struct CellPadActParam {
    unsigned char motor[2];
    unsigned char reserved[6];
} CellPadActParam;

// Button bit masks
#define CELL_PAD_CTRL_LEFT     (1 << 7)
#define CELL_PAD_CTRL_DOWN     (1 << 6)
#define CELL_PAD_CTRL_RIGHT    (1 << 5)
#define CELL_PAD_CTRL_UP       (1 << 4)
#define CELL_PAD_CTRL_START    (1 << 3)
#define CELL_PAD_CTRL_R3       (1 << 2)
#define CELL_PAD_CTRL_L3       (1 << 1)
#define CELL_PAD_CTRL_SELECT   (1 << 0)

#define CELL_PAD_CTRL_SQUARE   (1 << 15)
#define CELL_PAD_CTRL_CROSS    (1 << 14)
#define CELL_PAD_CTRL_CIRCLE   (1 << 13)
#define CELL_PAD_CTRL_TRIANGLE (1 << 12)
#define CELL_PAD_CTRL_R1       (1 << 11)
#define CELL_PAD_CTRL_L1       (1 << 10)
#define CELL_PAD_CTRL_R2       (1 << 9)
#define CELL_PAD_CTRL_L2       (1 << 8)

// Stub functions
inline int cellPadInit(int max_connect) { return 0; }
inline int cellPadEnd() { return 0; }
inline int cellPadGetInfo2(CellPadInfo2* info) { return 0; }
inline int cellPadGetData(int port_no, CellPadData* data) { return 0; }
inline int cellPadSetActDirect(int port_no, CellPadActParam* param) { return 0; }
inline int cellPadGetCapabilityInfo(int port_no, CellPadCapabilityInfo* info) { return 0; }
inline int cellPadClearBuf(int port_no) { return 0; }

#ifdef __cplusplus
}
#endif

#endif /* _CELL_PAD_LIBPAD_H */
