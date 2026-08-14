/*===========================================================================
    bmp.cpp
    15-Feb-00 Created by Neall

    Copyright (c) 2000 Radical Entertainment, Inc.
    All rights reserved.
===========================================================================*/

#include <p3d/image.hpp>
#include <p3d/dxtn.hpp>
#include <p3d/file.hpp>
#include <p3d/error.hpp>
#include <p3d/utility.hpp>
#include <pddi/pdditype.hpp>
#include <string.h>

#ifdef RAD_PS3
#include <stdio.h>
#include <stdlib.h>
#endif



//////////////////////////////////////////////////////////////////////////////////////
///     all data struct defined were copied from ddraw.h to avoid of
///     including ddraw.h which is dependent on PC only

/*
 * dwLinearSize is valid
 */
#define DDSD_LINEARSIZE         0x00080000l


#ifndef DUMMYUNIONNAMEN
#if defined(__cplusplus) || !defined(NONAMELESSUNION)
#define DUMMYUNIONNAMEN(n)
#else
#define DUMMYUNIONNAMEN(n)      u##n
#endif
#endif


#ifndef MAKEFOURCC
     #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                     ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) |   \
                     ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)                     

/*
 * DDCOLORKEY
 */
typedef struct _DDCOLORKEY
{
     unsigned int       dwColorSpaceLowValue;   // low boundary of color space that is to
                                                     // be treated as Color Key, inclusive
     unsigned int       dwColorSpaceHighValue;  // high boundary of color space that is
                                                     // to be treated as Color Key, inclusive
} DDCOLORKEY;


/*
 * DDSCAPS2
 */
typedef struct _DDSCAPS2
{
     unsigned int       dwCaps;         // capabilities of surface wanted
     unsigned int       dwCaps2;
     unsigned int       dwCaps3;
     union
     {
          unsigned int       dwCaps4;
          unsigned int       dwVolumeDepth;
     } DUMMYUNIONNAMEN(1);
} DDSCAPS2;


/*
 * DDPIXELFORMAT
 */
typedef struct _DDPIXELFORMAT
{
     unsigned int       dwSize;                 // size of structure
     unsigned int       dwFlags;                // pixel format flags
     unsigned int       dwFourCC;               // (FOURCC code)
     union
     {
          unsigned int   dwRGBBitCount;          // how many bits per pixel
          unsigned int   dwYUVBitCount;          // how many bits per pixel
          unsigned int   dwZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
          unsigned int   dwAlphaBitDepth;        // how many bits for alpha channels
          unsigned int   dwLuminanceBitCount;    // how many bits per pixel
          unsigned int   dwBumpBitCount;         // how many bits per "buxel", total
          unsigned int   dwPrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture
                                                     // format list and if DDPF_D3DFORMAT is set
     } DUMMYUNIONNAMEN(1);
     union
     {
          unsigned int   dwRBitMask;             // mask for red bit
          unsigned int   dwYBitMask;             // mask for Y bits
          unsigned int   dwStencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
          unsigned int   dwLuminanceBitMask;     // mask for luminance bits
          unsigned int   dwBumpDuBitMask;        // mask for bump map U delta bits
          unsigned int   dwOperations;           // DDPF_D3DFORMAT Operations
     } DUMMYUNIONNAMEN(2);
     union
     {
          unsigned int   dwGBitMask;             // mask for green bits
          unsigned int   dwUBitMask;             // mask for U bits
          unsigned int   dwZBitMask;             // mask for Z bits
          unsigned int   dwBumpDvBitMask;        // mask for bump map V delta bits
          struct
          {
                unsigned short    wFlipMSTypes;       // Multisample methods supported via flip for this D3DFORMAT
                unsigned short    wBltMSTypes;        // Multisample methods supported via blt for this D3DFORMAT
          } MultiSampleCaps;

     } DUMMYUNIONNAMEN(3);
     union
     {
          unsigned int   dwBBitMask;             // mask for blue bits
          unsigned int   dwVBitMask;             // mask for V bits
          unsigned int   dwStencilBitMask;       // mask for stencil bits
          unsigned int   dwBumpLuminanceBitMask; // mask for luminance in bump map
     } DUMMYUNIONNAMEN(4);
     union
     {
          unsigned int   dwRGBAlphaBitMask;      // mask for alpha channel
          unsigned int   dwYUVAlphaBitMask;      // mask for alpha channel
          unsigned int   dwLuminanceAlphaBitMask;// mask for alpha channel
          unsigned int   dwRGBZBitMask;          // mask for Z channel
          unsigned int   dwYUVZBitMask;          // mask for Z channel
     } DUMMYUNIONNAMEN(5);
} DDPIXELFORMAT;

/*
 * DDSURFACEDESC2
 */
typedef struct _DDSURFACEDESC2
{
     unsigned int               dwSize;                 // size of the DDSURFACEDESC structure
     unsigned int               dwFlags;                // determines what fields are valid
     unsigned int               dwHeight;               // height of surface to be created
     unsigned int               dwWidth;                // width of input surface
     union
     {
          int            lPitch;                 // distance to start of next line (return value only)
          unsigned int           dwLinearSize;           // Formless late-allocated optimized surface size
     } DUMMYUNIONNAMEN(1);
     union
     {
          unsigned int           dwBackBufferCount;      // number of back buffers requested
          unsigned int           dwDepth;                // the depth if this is a volume texture 
     } DUMMYUNIONNAMEN(5);
     union
     {
          unsigned int           dwMipMapCount;          // number of mip-map levels requestde
                                                                // dwZBufferBitDepth removed, use ddpfPixelFormat one instead
          unsigned int           dwRefreshRate;          // refresh rate (used when display mode is described)
          unsigned int           dwSrcVBHandle;          // The source used in VB::Optimize
     } DUMMYUNIONNAMEN(2);
     unsigned int               dwAlphaBitDepth;        // depth of alpha buffer requested
     unsigned int               dwReserved;             // reserved
     void *              lpSurface;              // pointer to the associated surface memory
     union
     {
          DDCOLORKEY      ddckCKDestOverlay;      // color key for destination overlay use
          unsigned int           dwEmptyFaceColor;       // Physical color for empty cubemap faces
     } DUMMYUNIONNAMEN(3);
     DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
     DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
     DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use
     union
     {
          DDPIXELFORMAT   ddpfPixelFormat;        // pixel format description of the surface
          unsigned int           dwFVF;                  // vertex format description of vertex buffers
     } DUMMYUNIONNAMEN(4);
     DDSCAPS2            ddsCaps;                // direct draw surface capabilities
     unsigned int               dwTextureStage;         // stage in multitexture cascade
} DDSURFACEDESC2;





static void LoadDXTNFile(tFile*,  tImageHandler::Builder* );
static pddiPixelFormat DecodePixelFormat(  DDPIXELFORMAT*  );


bool tDXTNHandler::CheckFormat(Format format)
{
    switch(format)
    {
        case IMG_DXT:
        case IMG_DXT1:
        case IMG_DXT2:            
        case IMG_DXT3:            
        case IMG_DXT4:            
        case IMG_DXT5:
            return true;
    }
    return false;
}

#ifdef RAD_PS3
// Software DXT1 decompression for PS3
static void DecodeDXT1Block(const unsigned char* block, unsigned int* output, int stride)
{
    // Read two RGB565 colors
    unsigned short c0 = block[0] | (block[1] << 8);
    unsigned short c1 = block[2] | (block[3] << 8);

    // Extract RGB components and expand to 8-bit
    unsigned char r0 = ((c0 >> 11) & 0x1F) * 255 / 31;
    unsigned char g0 = ((c0 >> 5) & 0x3F) * 255 / 63;
    unsigned char b0 = (c0 & 0x1F) * 255 / 31;

    unsigned char r1 = ((c1 >> 11) & 0x1F) * 255 / 31;
    unsigned char g1 = ((c1 >> 5) & 0x3F) * 255 / 63;
    unsigned char b1 = (c1 & 0x1F) * 255 / 31;

    // Build color table (ARGB format)
    unsigned int colors[4];
    colors[0] = 0xFF000000 | (r0 << 16) | (g0 << 8) | b0;
    colors[1] = 0xFF000000 | (r1 << 16) | (g1 << 8) | b1;

    if (c0 > c1)
    {
        // Opaque mode: interpolate 2 more colors
        colors[2] = 0xFF000000 |
                    (((2 * r0 + r1) / 3) << 16) |
                    (((2 * g0 + g1) / 3) << 8) |
                    ((2 * b0 + b1) / 3);
        colors[3] = 0xFF000000 |
                    (((r0 + 2 * r1) / 3) << 16) |
                    (((g0 + 2 * g1) / 3) << 8) |
                    ((b0 + 2 * b1) / 3);
    }
    else
    {
        // Transparent mode: 1 interpolated + 1 transparent
        colors[2] = 0xFF000000 |
                    (((r0 + r1) / 2) << 16) |
                    (((g0 + g1) / 2) << 8) |
                    ((b0 + b1) / 2);
        colors[3] = 0x00000000; // Transparent black
    }

    // Decode 4x4 pixel indices
    unsigned int indices = block[4] | (block[5] << 8) | (block[6] << 16) | (block[7] << 24);

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            int idx = indices & 3;
            indices >>= 2;
            output[y * stride + x] = colors[idx];
        }
    }
}

static void LoadDXTNFilePS3(tFile* file, tImageHandler::Builder* builder)
{
    // Check for DDS magic number to determine if this is a DDS file or raw DXT data
    unsigned int magic;
    file->GetData(&magic, 4, tFile::BYTE);  // Read as bytes to avoid endian issues

    int width = 0, height = 0;
    int compressedSize = 0;
    bool isDDSFile = false;

    // Check for "DDS " magic (0x20534444 little-endian, 0x44445320 big-endian)
    if (magic == 0x20534444 || magic == 0x44445320)
    {
        isDDSFile = true;

        // Read header as raw bytes to avoid structure alignment issues
        unsigned char headerBytes[124];
        file->GetData(headerBytes, 124, tFile::BYTE);

        // Extract fields manually (DDS is little-endian)
        unsigned int dwHeight = headerBytes[8] | (headerBytes[9] << 8) |
                                (headerBytes[10] << 16) | (headerBytes[11] << 24);
        unsigned int dwWidth = headerBytes[12] | (headerBytes[13] << 8) |
                               (headerBytes[14] << 16) | (headerBytes[15] << 24);

        // dwFourCC is at offset 72 (DDPIXELFORMAT start) + 8 = 80 within the header
        unsigned int dwFourCC = headerBytes[80] | (headerBytes[81] << 8) |
                                (headerBytes[82] << 16) | (headerBytes[83] << 24);

        // Check for DXT1 format (MAKEFOURCC('D','X','T','1') = 0x31545844)
        unsigned int DXT1_FOURCC = 0x31545844;  // "DXT1" in little-endian
        if (dwFourCC != DXT1_FOURCC)
        {
            return;
        }

        width = dwWidth;
        height = dwHeight;
    }
    else
    {
        // Raw DXT data embedded in P3D file - no DDS header
        // Use dimensions from the P3D IMAGE chunk header if available
        int expectedWidth = builder->GetExpectedWidth();
        int expectedHeight = builder->GetExpectedHeight();
        int expectedSize = builder->GetExpectedFileSize();

        if (expectedWidth > 0 && expectedHeight > 0)
        {
            width = expectedWidth;
            height = expectedHeight;
            compressedSize = expectedSize > 0 ? expectedSize : ((width + 3) / 4) * ((height + 3) / 4) * 8;
        }
        else if (expectedSize > 0)
        {
            compressedSize = expectedSize;
            // Fallback: guess square dimensions from compressed size
            int totalPixels = compressedSize * 2;
            int side = 1;
            while (side * side < totalPixels) side *= 2;
            width = height = side;
        }
        else
        {
            return;
        }
    }

    if (width <= 0 || height <= 0 || width > 4096 || height > 4096)
    {
        return;
    }

    int blocksX = (width + 3) / 4;
    int blocksY = (height + 3) / 4;

    if (isDDSFile)
    {
        compressedSize = blocksX * blocksY * 8; // 8 bytes per DXT1 block
    }

    if (compressedSize <= 0 || compressedSize > 16 * 1024 * 1024)
    {
        return;
    }

    // Read compressed data
    unsigned char* compressedData = (unsigned char*)malloc(compressedSize);
    if (!compressedData)
    {
        return;
    }

    if (!isDDSFile)
    {
        // For raw data, we already read 4 bytes when checking for magic
        memcpy(compressedData, &magic, 4);
        file->GetData(compressedData + 4, compressedSize - 4, tFile::BYTE);
    }
    else
    {
        file->GetData(compressedData, compressedSize, tFile::BYTE);
    }

    // Begin image as 32-bit RGBA
    bool ok = builder->BeginImage(width, height, 32, tImageHandler::Builder::TOP, NULL);
    if (!ok)
    {
        free(compressedData);
        return;
    }

    // Allocate temp buffer for one row of blocks (4 scanlines)
    int blockRowSize = width * 4 * sizeof(unsigned int);
    unsigned int* blockRow = (unsigned int*)malloc(blockRowSize);
    if (!blockRow)
    {
        free(compressedData);
        return;
    }

    const unsigned char* srcBlock = compressedData;

    for (int by = 0; by < blocksY; by++)
    {
        // Clear block row
        memset(blockRow, 0, width * 4 * sizeof(unsigned int));

        // Decode all blocks in this row
        for (int bx = 0; bx < blocksX; bx++)
        {
            DecodeDXT1Block(srcBlock, blockRow + bx * 4, width);
            srcBlock += 8;
        }

        // Output the 4 scanlines (or fewer for the last row)
        int rowsToOutput = (by < blocksY - 1) ? 4 : (height - by * 4);
        for (int row = 0; row < rowsToOutput; row++)
        {
            builder->ProcessScanline32(blockRow + row * width);
        }
    }

    free(blockRow);
    free(compressedData);

    builder->EndImage();
}
#endif // RAD_PS3

void tDXTNHandler::CreateImage(tFile* file, tImageHandler::Builder* builder)
{
#ifdef RAD_PS3
    LoadDXTNFilePS3(file, builder);
#elif defined(RAD_GAMECUBE)
    bool origswap = file->GetEndianSwap();
    file->SetEndianSwap(true);
    LoadDXTNFile( file, builder );
    file->SetEndianSwap(origswap);
#else
    LoadDXTNFile( file, builder );
#endif
}

bool tDXTNHandler::SaveImage(tImage*, char*)
{
    return false;
}

static void LoadDXTNFile(tFile* file,  tImageHandler::Builder* builder)
{
/////////////////////////////////////////
///     ProcessScanline8 need to be rewritten to for DXTN format, or May be
////    we should call another new func to add the read-in data to texture
///  
     // get palette
     long ids;

     //read magic number
     file->GetData(&ids, 1, tFile::DWORD);

     if (ids == MAKEFOURCC(' ','S','D','D'))
     {
          file->SetEndianSwap(!file->GetEndianSwap());
     }

     DDSURFACEDESC2 header;

     file->GetData( (void*) &header, sizeof( DDSURFACEDESC2 ) / 4, tFile::DWORD );

     //set the  right texture type for the DXTn format
     pddiPixelFormat format = DecodePixelFormat( &header.ddpfPixelFormat );
     pddiTextureType type;
     switch( format )
     {
     case PDDI_PIXEL_DXT1:
          type = PDDI_TEXTYPE_DXT1;
          break;

     case PDDI_PIXEL_DXT2:
          type = PDDI_TEXTYPE_DXT2;
          break;

     case PDDI_PIXEL_DXT3:
          type = PDDI_TEXTYPE_DXT3;
          break;
        
     case PDDI_PIXEL_DXT4:
          type = PDDI_TEXTYPE_DXT4;
          break;

     case PDDI_PIXEL_DXT5:
          type = PDDI_TEXTYPE_DXT5;
          break;

     default:    //error;
          return;
     }

     builder->SetTextureType( type );

     bool ok = builder->BeginImage(header.dwWidth, header.dwHeight, 2, tImageHandler::Builder::TOP, NULL );
     if (!ok) return; // Can't load on this platform


//    file->SetPosition( sizeof(long) + sizeof(DDSURFACEDESC2) );

     if( header.dwFlags & DDSD_LINEARSIZE )
     {
          unsigned char *scanline_in = (unsigned char*) builder->GetMemoryImagePtr();
          if (scanline_in == NULL) return;
          file->GetData(scanline_in, header.dwLinearSize, tFile::BYTE);
     }
     else
     {
          int width = header.dwWidth * header.ddpfPixelFormat.dwRGBBitCount / 8;

          //each time we read int data of "width" long, although the
          //real data space take "lPitch" long.
          unsigned char * scanline_in = (unsigned char*)p3d::MallocTemp(header.lPitch*header.dwHeight);

          if( scanline_in == NULL ) return;

          unsigned char *buf = scanline_in;

          for( unsigned int yp = 0; yp < header.dwHeight; yp++ )
          {
                file->GetData(buf, width, tFile::BYTE);
                buf += header.lPitch;            
          }

          builder->DirectCopy(scanline_in, header.lPitch*header.dwHeight );
          p3d::FreeTemp(scanline_in);

     }
     builder->EndImage();
}

static pddiPixelFormat DecodePixelFormat(  DDPIXELFORMAT* pddpf )
{
     switch( pddpf->dwFourCC )
     {

          case MAKEFOURCC('D','X','T','1'):
                return PDDI_PIXEL_DXT1;           

          case MAKEFOURCC('D','X','T','2'):
                return PDDI_PIXEL_DXT2;  

          case MAKEFOURCC('D','X','T','3'):
                return PDDI_PIXEL_DXT3;      

          case MAKEFOURCC('D','X','T','4'):
                return PDDI_PIXEL_DXT4;  

          case MAKEFOURCC('D','X','T','5'):
                return PDDI_PIXEL_DXT5;  
             
        default:
                return PDDI_PIXEL_UNKNOWN;
     }
}

