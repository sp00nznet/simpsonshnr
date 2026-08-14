/*
 * PS3 SDK Stub - cell/codec/pngdec.h
 * PNG Decoder library stub for PS3 port
 */

#ifndef _CELL_CODEC_PNGDEC_H
#define _CELL_CODEC_PNGDEC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Return codes
#define CELL_OK 0
#define CELL_PNGDEC_ERROR_HEADER -1
#define CELL_PNGDEC_ERROR_STREAM_FORMAT -2
#define CELL_PNGDEC_ERROR_ARG -3
#define CELL_PNGDEC_ERROR_OPEN_FILE -4
#define CELL_PNGDEC_ERROR_FATAL -5

// Thread enable flags
#define CELL_PNGDEC_SPU_THREAD_DISABLE 0
#define CELL_PNGDEC_SPU_THREAD_ENABLE  1

// Source select
#define CELL_PNGDEC_FILE   0
#define CELL_PNGDEC_BUFFER 1

// Output mode
#define CELL_PNGDEC_TOP_TO_BOTTOM 0
#define CELL_PNGDEC_BOTTOM_TO_TOP 1

// Color space
#define CELL_PNGDEC_GRAYSCALE       1
#define CELL_PNGDEC_RGB             2
#define CELL_PNGDEC_PALETTE         3
#define CELL_PNGDEC_GRAYSCALE_ALPHA 4
#define CELL_PNGDEC_RGBA            6
#define CELL_PNGDEC_ARGB            10

// Pack flag
#define CELL_PNGDEC_1BYTE_PER_NPIXEL 0
#define CELL_PNGDEC_1BYTE_PER_1PIXEL 1

// Alpha select
#define CELL_PNGDEC_STREAM_ALPHA 0
#define CELL_PNGDEC_FIX_ALPHA    1

// Decode status
#define CELL_PNGDEC_DEC_STATUS_FINISH 0
#define CELL_PNGDEC_DEC_STATUS_STOP   1

// Handles
typedef void* CellPngDecMainHandle;
typedef void* CellPngDecSubHandle;

// Memory callback types
typedef void* (*CellPngDecCbCtrlMallocFunc)(uint32_t size, void* userData);
typedef int32_t (*CellPngDecCbCtrlFreeFunc)(void* ptr, void* userData);

// Thread input parameters
typedef struct CellPngDecThreadInParam {
    int spuThreadEnable;
    uint32_t ppuThreadPriority;
    uint32_t spuThreadPriority;
    CellPngDecCbCtrlMallocFunc cbCtrlMallocFunc;
    void* cbCtrlMallocArg;
    CellPngDecCbCtrlFreeFunc cbCtrlFreeFunc;
    void* cbCtrlFreeArg;
} CellPngDecThreadInParam;

// Thread output parameters
typedef struct CellPngDecThreadOutParam {
    uint32_t pngCodecVersion;
} CellPngDecThreadOutParam;

// Source parameters
typedef struct CellPngDecSrc {
    int srcSelect;
    const char* fileName;
    int64_t fileOffset;
    uint32_t fileSize;
    const void* streamPtr;
    uint32_t streamSize;
    int spuThreadEnable;
} CellPngDecSrc;

// Open info
typedef struct CellPngDecOpnInfo {
    uint32_t initSpaceAllocated;
} CellPngDecOpnInfo;

// PNG info
typedef struct CellPngDecInfo {
    uint32_t imageWidth;
    uint32_t imageHeight;
    uint32_t numComponents;
    uint32_t colorSpace;
    uint32_t bitDepth;
    int interlaceType;
    uint32_t chunkInformation;
} CellPngDecInfo;

// Input parameters
typedef struct CellPngDecInParam {
    void* commandPtr;
    int outputMode;
    int outputColorSpace;
    uint32_t outputBitDepth;
    int outputPackFlag;
    int outputAlphaSelect;
    uint32_t outputColorAlpha;
} CellPngDecInParam;

// Output parameters - field order must match PS3 SDK exactly!
typedef struct CellPngDecOutParam {
    uint32_t outputWidth;
    uint32_t outputHeight;
    uint32_t outputComponents;
    uint32_t outputBitDepth;
    int outputMode;
    int outputColorSpace;
    uint32_t useMemorySpace;    // This comes BEFORE outputWidthByte
    uint32_t outputWidthByte;   // This comes AFTER useMemorySpace
} CellPngDecOutParam;

// Data control parameters
typedef struct CellPngDecDataCtrlParam {
    uint64_t outputBytesPerLine;
} CellPngDecDataCtrlParam;

// Data output info
typedef struct CellPngDecDataOutInfo {
    uint32_t chunkInformation;
    uint32_t numText;
    uint32_t numUnknownChunk;
    int status;
} CellPngDecDataOutInfo;

// Function declarations - these are the actual PS3 SDK functions
// In a real build with the PS3 SDK, these would link to the actual library

int cellPngDecCreate(CellPngDecMainHandle* mainHandle,
                     const CellPngDecThreadInParam* threadInParam,
                     CellPngDecThreadOutParam* threadOutParam);

int cellPngDecDestroy(CellPngDecMainHandle mainHandle);

int cellPngDecOpen(CellPngDecMainHandle mainHandle,
                   CellPngDecSubHandle* subHandle,
                   const CellPngDecSrc* src,
                   CellPngDecOpnInfo* openInfo);

int cellPngDecClose(CellPngDecMainHandle mainHandle,
                    CellPngDecSubHandle subHandle);

int cellPngDecReadHeader(CellPngDecMainHandle mainHandle,
                         CellPngDecSubHandle subHandle,
                         CellPngDecInfo* info);

int cellPngDecSetParameter(CellPngDecMainHandle mainHandle,
                           CellPngDecSubHandle subHandle,
                           const CellPngDecInParam* inParam,
                           CellPngDecOutParam* outParam);

int cellPngDecDecodeData(CellPngDecMainHandle mainHandle,
                         CellPngDecSubHandle subHandle,
                         uint8_t* data,
                         const CellPngDecDataCtrlParam* dataCtrlParam,
                         CellPngDecDataOutInfo* dataOutInfo);

#ifdef __cplusplus
}
#endif

#endif /* _CELL_CODEC_PNGDEC_H */
