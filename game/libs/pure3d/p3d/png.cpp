//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================


// PNG image loader
// 15-June-98, NV
// requires libpng.lib, and zlib.dll

#include <p3d/png.hpp>
#include <p3d/image.hpp>
#include <p3d/file.hpp>
#include <p3d/utility.hpp>

#ifdef RAD_PS3
// PS3 PNG loading using software decoder with zlib
// cellPngDec has compatibility issues with some emulators

#include <pddi/pdditype.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Use Pure3D's bundled zlib
extern "C" {
#include <pure3d/lib/zlib/zlib.h>
}

// PNG signature
static const unsigned char PNG_SIG[8] = {137, 80, 78, 71, 13, 10, 26, 10};

// PNG chunk types
#define PNG_IHDR 0x49484452
#define PNG_IDAT 0x49444154
#define PNG_IEND 0x49454E44
#define PNG_PLTE 0x504C5445
#define PNG_tRNS 0x74524E53

// Read big-endian 32-bit value
static unsigned int readBE32(const unsigned char* p)
{
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

// Paeth predictor for PNG filtering
static unsigned char paeth(int a, int b, int c)
{
    int p = a + b - c;
    int pa = p > a ? p - a : a - p;
    int pb = p > b ? p - b : b - p;
    int pc = p > c ? p - c : c - p;
    if (pa <= pb && pa <= pc) return (unsigned char)a;
    if (pb <= pc) return (unsigned char)b;
    return (unsigned char)c;
}

// Apply PNG filter to a scanline
static void pngUnfilter(unsigned char* row, unsigned char* prev, int filter, int width, int bpp)
{
    int byteWidth = (bpp + 7) / 8;  // bytes per pixel

    switch (filter)
    {
    case 0: // None
        break;
    case 1: // Sub
        for (int i = byteWidth; i < width; i++)
            row[i] += row[i - byteWidth];
        break;
    case 2: // Up
        if (prev)
            for (int i = 0; i < width; i++)
                row[i] += prev[i];
        break;
    case 3: // Average
        for (int i = 0; i < width; i++)
        {
            int a = (i >= byteWidth) ? row[i - byteWidth] : 0;
            int b = prev ? prev[i] : 0;
            row[i] += (a + b) / 2;
        }
        break;
    case 4: // Paeth
        for (int i = 0; i < width; i++)
        {
            int a = (i >= byteWidth) ? row[i - byteWidth] : 0;
            int b = prev ? prev[i] : 0;
            int c = (prev && i >= byteWidth) ? prev[i - byteWidth] : 0;
            row[i] += paeth(a, b, c);
        }
        break;
    }
}

bool tPNGHandler::CheckFormat(Format format)
{
    return format == IMG_PNG;
}

void tPNGHandler::CreateImage(tFile* file, tImageHandler::Builder* builder)
{
    // Get the expected file size from the builder (set by ParseAsTexture from chunk data)
    // If not set (-1), fall back to remaining file size (for standalone PNG files)
    int expectedSize = builder->GetExpectedFileSize();
    unsigned int fileSize;
    if (expectedSize > 0)
    {
        fileSize = (unsigned int)expectedSize;
    }
    else
    {
        fileSize = file->GetSize() - file->GetPosition();
    }

    if (fileSize < 8)
    {
        // PNG too small
        return;
    }

    unsigned char* pngData = (unsigned char*)p3d::MallocTemp(fileSize);
    if (!pngData)
    {
        // Allocation failed
        return;
    }

    file->GetData(pngData, fileSize, tFile::BYTE);

    // Check PNG signature
    if (memcmp(pngData, PNG_SIG, 8) != 0)
    {
        // Invalid signature
        p3d::FreeTemp(pngData);
        return;
    }

    // Parse chunks
    unsigned int pos = 8;
    int width = 0, height = 0, bitDepth = 0, colorType = 0;
    unsigned char* idatData = NULL;
    unsigned int idatSize = 0;
    unsigned int idatCapacity = 0;
    pddiColour palette[256];
    int paletteSize = 0;

    memset(palette, 0, sizeof(palette));
    for (int i = 0; i < 256; i++) palette[i].SetAlpha(255);

    while (pos + 12 <= fileSize)
    {
        unsigned int chunkLen = readBE32(pngData + pos);
        unsigned int chunkType = readBE32(pngData + pos + 4);
        const unsigned char* chunkData = pngData + pos + 8;

        if (pos + 12 + chunkLen > fileSize)
        {
            // Chunk exceeds file
            break;
        }

        if (chunkType == PNG_IHDR)
        {
            width = readBE32(chunkData);
            height = readBE32(chunkData + 4);
            bitDepth = chunkData[8];
            colorType = chunkData[9];
        }
        else if (chunkType == PNG_PLTE)
        {
            paletteSize = chunkLen / 3;
            for (int i = 0; i < paletteSize && i < 256; i++)
            {
                palette[i] = pddiColour(chunkData[i*3], chunkData[i*3+1], chunkData[i*3+2], 255);
            }
        }
        else if (chunkType == PNG_tRNS)
        {
            int transSize = chunkLen;
            if (transSize > 256) transSize = 256;
            for (int i = 0; i < transSize && i < paletteSize; i++)
            {
                palette[i].SetAlpha(chunkData[i]);
            }
        }
        else if (chunkType == PNG_IDAT)
        {
            // Accumulate IDAT chunks
            if (idatSize + chunkLen > idatCapacity)
            {
                unsigned int newCap = idatCapacity == 0 ? 65536 : idatCapacity * 2;
                while (newCap < idatSize + chunkLen) newCap *= 2;
                unsigned char* newBuf = (unsigned char*)p3d::MallocTemp(newCap);
                if (!newBuf)
                {
                    // IDAT alloc failed
                    if (idatData) p3d::FreeTemp(idatData);
                    p3d::FreeTemp(pngData);
                    return;
                }
                if (idatData)
                {
                    memcpy(newBuf, idatData, idatSize);
                    p3d::FreeTemp(idatData);
                }
                idatData = newBuf;
                idatCapacity = newCap;
            }
            memcpy(idatData + idatSize, chunkData, chunkLen);
            idatSize += chunkLen;
        }
        else if (chunkType == PNG_IEND)
        {
            break;
        }

        pos += 12 + chunkLen;
    }

    p3d::FreeTemp(pngData);
    pngData = NULL;

    if (width == 0 || height == 0 || idatData == NULL)
    {
        // Invalid PNG structure
        if (idatData) p3d::FreeTemp(idatData);
        return;
    }

    // Calculate scanline size
    int bitsPerPixel;
    switch (colorType)
    {
    case 0: bitsPerPixel = bitDepth; break;           // Grayscale
    case 2: bitsPerPixel = bitDepth * 3; break;       // RGB
    case 3: bitsPerPixel = bitDepth; break;           // Indexed
    case 4: bitsPerPixel = bitDepth * 2; break;       // Grayscale + Alpha
    case 6: bitsPerPixel = bitDepth * 4; break;       // RGBA
    default:
        // Unsupported colorType
        p3d::FreeTemp(idatData);
        return;
    }

    int scanlineBytes = (width * bitsPerPixel + 7) / 8;
    int rawSize = (scanlineBytes + 1) * height;  // +1 for filter byte per row

    // Allocate buffer for decompressed data
    unsigned char* rawData = (unsigned char*)p3d::MallocTemp(rawSize);
    if (!rawData)
    {
        // Raw buffer alloc failed
        p3d::FreeTemp(idatData);
        return;
    }

    // Decompress using zlib
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    zs.next_in = idatData;
    zs.avail_in = idatSize;
    zs.next_out = rawData;
    zs.avail_out = rawSize;

    int ret = inflateInit(&zs);
    if (ret != Z_OK)
    {
        // inflateInit failed
        p3d::FreeTemp(rawData);
        p3d::FreeTemp(idatData);
        return;
    }

    ret = inflate(&zs, Z_FINISH);
    if (ret != Z_STREAM_END && ret != Z_OK)
    {
        // inflate failed
        inflateEnd(&zs);
        p3d::FreeTemp(rawData);
        p3d::FreeTemp(idatData);
        return;
    }

    unsigned int decompressedSize = zs.total_out;
    inflateEnd(&zs);
    p3d::FreeTemp(idatData);
    idatData = NULL;

    // Unfilter the image data
    unsigned char* prevRow = NULL;
    unsigned char* currRow = rawData;

    for (int y = 0; y < height; y++)
    {
        int filter = currRow[0];
        unsigned char* rowData = currRow + 1;

        pngUnfilter(rowData, prevRow, filter, scanlineBytes, bitsPerPixel);

        prevRow = rowData;
        currRow += scanlineBytes + 1;
    }

    // Feed to builder based on color type
    if (colorType == 3)  // Indexed (paletted)
    {
        int bpp = (bitDepth <= 4) ? 4 : 8;
        builder->BeginImage(width, height, bpp, tImageHandler::Builder::TOP, palette);

        unsigned char* scanline = (unsigned char*)p3d::MallocTemp(width);
        currRow = rawData;

        for (int y = 0; y < height; y++)
        {
            unsigned char* rowData = currRow + 1;  // Skip filter byte

            if (bitDepth == 8)
            {
                builder->ProcessScanline8(rowData);
            }
            else if (bitDepth == 4)
            {
                // Expand 4-bit to 8-bit indices
                for (int x = 0; x < width; x++)
                {
                    int byteIdx = x / 2;
                    int nibble = (x & 1) ? (rowData[byteIdx] & 0x0F) : (rowData[byteIdx] >> 4);
                    scanline[x] = nibble;
                }
                builder->ProcessScanline8(scanline);
            }
            else if (bitDepth == 2)
            {
                for (int x = 0; x < width; x++)
                {
                    int byteIdx = x / 4;
                    int shift = 6 - (x & 3) * 2;
                    scanline[x] = (rowData[byteIdx] >> shift) & 0x03;
                }
                builder->ProcessScanline8(scanline);
            }
            else if (bitDepth == 1)
            {
                for (int x = 0; x < width; x++)
                {
                    int byteIdx = x / 8;
                    int bit = 7 - (x & 7);
                    scanline[x] = (rowData[byteIdx] >> bit) & 0x01;
                }
                builder->ProcessScanline8(scanline);
            }

            currRow += scanlineBytes + 1;
        }

        p3d::FreeTemp(scanline);
    }
    else if (colorType == 2 || colorType == 6)  // RGB or RGBA
    {
        int bpp = (colorType == 6) ? 32 : 24;
        builder->BeginImage(width, height, bpp, tImageHandler::Builder::TOP, NULL);

        unsigned int* scanline = (unsigned int*)p3d::MallocTemp(width * 4);
        currRow = rawData;

        for (int y = 0; y < height; y++)
        {
            unsigned char* rowData = currRow + 1;

            if (colorType == 6)  // RGBA
            {
                for (int x = 0; x < width; x++)
                {
                    unsigned char r = rowData[x * 4 + 0];
                    unsigned char g = rowData[x * 4 + 1];
                    unsigned char b = rowData[x * 4 + 2];
                    unsigned char a = rowData[x * 4 + 3];
                    scanline[x] = (a << 24) | (r << 16) | (g << 8) | b;
                }
            }
            else  // RGB
            {
                for (int x = 0; x < width; x++)
                {
                    unsigned char r = rowData[x * 3 + 0];
                    unsigned char g = rowData[x * 3 + 1];
                    unsigned char b = rowData[x * 3 + 2];
                    scanline[x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
                }
            }

            builder->ProcessScanline32(scanline);
            currRow += scanlineBytes + 1;
        }

        p3d::FreeTemp(scanline);
    }
    else if (colorType == 0 || colorType == 4)  // Grayscale or Grayscale+Alpha
    {
        int bpp = (colorType == 4) ? 32 : 24;
        builder->BeginImage(width, height, bpp, tImageHandler::Builder::TOP, NULL);

        unsigned int* scanline = (unsigned int*)p3d::MallocTemp(width * 4);
        currRow = rawData;

        for (int y = 0; y < height; y++)
        {
            unsigned char* rowData = currRow + 1;

            if (colorType == 4)  // Grayscale + Alpha
            {
                for (int x = 0; x < width; x++)
                {
                    unsigned char g = rowData[x * 2 + 0];
                    unsigned char a = rowData[x * 2 + 1];
                    scanline[x] = (a << 24) | (g << 16) | (g << 8) | g;
                }
            }
            else  // Grayscale
            {
                for (int x = 0; x < width; x++)
                {
                    unsigned char g = rowData[x];
                    scanline[x] = (0xFF << 24) | (g << 16) | (g << 8) | g;
                }
            }

            builder->ProcessScanline32(scanline);
            currRow += scanlineBytes + 1;
        }

        p3d::FreeTemp(scanline);
    }

    builder->EndImage();
    p3d::FreeTemp(rawData);
}

bool tPNGHandler::SaveImage(tImage* image, char* filename)
{
    return false;
}

#else
// Non-PS3 platforms use libpng
#include <png.h>

static void LoadPNG4(png_structp, png_infop, tImageHandler::Builder*);
static void LoadPNG8(png_structp, png_infop, tImageHandler::Builder*);
static void LoadPNG32(png_structp, png_infop, tImageHandler::Builder*);

// User I/O routines for libpng.  By default it uses stdio.
static void pngRead(png_structp png_ptr, png_bytep data, png_uint_32 length)
{
    tFile* read = (tFile*)png_get_io_ptr(png_ptr);
    //if(readPtr->pos + length > readPtr->size)
    //   return;
    read->GetData(data, length, tFile::BYTE);
}

// user error routines
static void p3d_png_err(png_structp, png_const_charp message)
{
    P3DASSERTMSG(0,message,"");
}

static void p3d_png_warn(png_structp, png_const_charp message)
{
    p3d::print(message);
    p3d::print("\n");
}

// user memory routines
static void * p3d_png_malloc(png_structp, png_size_t size)
{
    return p3d::MallocTemp(size);
}

static void p3d_png_free(png_structp, png_structp data)
{
    p3d::FreeTemp(data);
}

//-------------------------------------------------------------------
bool tPNGHandler::CheckFormat(Format format)
{
    return format == IMG_PNG;
}


void tPNGHandler::CreateImage(tFile* file, tImageHandler::Builder* builder)
{
    png_structp pPNG = png_create_read_struct_2
         (PNG_LIBPNG_VER_STRING, 
         0, p3d_png_err, p3d_png_warn, 
         0, p3d_png_malloc, p3d_png_free);

    if(!pPNG)
    {
        return;
    }

    png_infop pngInfo = png_create_info_struct(pPNG);
    if(!pngInfo)
    {
        png_destroy_read_struct(&pPNG, 0, 0);
        return;
    }

    // install user i/o routines to parse the memory
    png_set_read_fn(pPNG, file, (png_rw_ptr)pngRead);

    // grab info
    png_read_info(pPNG, pngInfo);

    int channelDepth = png_get_bit_depth(pPNG, pngInfo);

    // check for supported colour type
    unsigned colourType = png_get_color_type(pPNG, pngInfo);
    if(colourType != PNG_COLOR_TYPE_PALETTE &&
        colourType != PNG_COLOR_TYPE_RGB &&
        colourType != PNG_COLOR_TYPE_RGB_ALPHA)
    {
        png_destroy_read_struct(&pPNG, 0, 0);
        return;
    }
    
    // we can't handle interlaced data
    unsigned interlaceType = png_get_interlace_type(pPNG, pngInfo);
    if(interlaceType != 0)
    {
        png_destroy_info_struct(pPNG, &pngInfo);
        png_destroy_read_struct(&pPNG, 0, 0);
        return;
    }
    
    // palette handling
    if(png_get_valid(pPNG, pngInfo, PNG_INFO_PLTE))
    {
        // pnglib likes to set the alpha of palette entry 0 to 0.
        // this will supress that
        png_set_strip_alpha(pPNG);
    }
    
    // convert 16 bit/channel images to 8 bit/channel   
    if(channelDepth == 16)
    {
        png_set_strip_16(pPNG);
    }
    
    // convert RGB pixels to BGR
    png_set_bgr(pPNG);
    
    // expand 24 bit pixels to 32
    if(pngInfo->pixel_depth != 32)
    {
        png_set_filler(pPNG, 0xff, PNG_FILLER_AFTER);
    }

    // depalettise image
    if((colourType == PNG_COLOR_TYPE_PALETTE) &&
        (channelDepth != 4) && (channelDepth != 8))
    {
        png_set_expand(pPNG);
    }

    // transform data to appropriate format
    png_read_update_info(pPNG, pngInfo);

    if(channelDepth == 4 && colourType == PNG_COLOR_TYPE_PALETTE)
    {
        LoadPNG4(pPNG, pngInfo, builder);
    }
    else if(channelDepth == 8 && colourType == PNG_COLOR_TYPE_PALETTE)
    {
        LoadPNG8(pPNG, pngInfo, builder);
    }
    else
    {
        LoadPNG32(pPNG, pngInfo, builder);
    }

    // we are done!
    png_destroy_info_struct(pPNG, &pngInfo);
    png_destroy_read_struct(&pPNG, 0, 0);
}

bool tPNGHandler::SaveImage(tImage* image, char* filename)
{
    // someday, maybe
    return false;
}

void LoadPNG4(png_structp pPNG, png_infop pngInfo, tImageHandler::Builder* builder)
{
    const int bpp = 4;
    int width = png_get_image_width(pPNG, pngInfo);
    int height = png_get_image_height(pPNG, pngInfo);
    
    int numPalette = 0;
    png_color* srcPalette;
    png_get_PLTE(pPNG, pngInfo, &srcPalette, &numPalette);      
    
    pddiColour dstPalette[256];
    memset(dstPalette, 0, sizeof(pddiColour)*256);

    for(int i=0; i < numPalette; i++)
    {
        dstPalette[i].c = pddiColour(srcPalette[i].red, srcPalette[i].green, srcPalette[i].blue, 255);
    }

    // load transparency channel for palette
    if (png_get_valid(pPNG, pngInfo, PNG_INFO_tRNS) > 0)
    {
        int numAlpha;
        png_byte *srcAlpha;
        png_get_tRNS(pPNG, pngInfo, &srcAlpha, &numAlpha, 0);
        
        for(int i = 0; i < numAlpha; ++i)
        {
            dstPalette[i].SetAlpha(srcAlpha[i]);
        }
    }

    builder->BeginImage(width, height, bpp, tImageHandler::Builder::TOP, (pddiColour*)dstPalette);
    
    // create an image, read in the bits
    unsigned char* srcRow = (unsigned char*)p3d::MallocTemp(png_get_rowbytes(pPNG, pngInfo));
    unsigned char* dstRow = (unsigned char*)p3d::MallocTemp(width);
    
    for(int y = 0; y < height; y++)
    {
        png_read_row(pPNG, (unsigned char*)srcRow, (unsigned char*)NULL);

        // promote 4-bit palette indexes to 8-bit palette indexes
        for (int i=0; i<width/2; i++)
        {
            dstRow[(i*2)+0] = srcRow[i] >> 4;
            dstRow[(i*2)+1] = srcRow[i] & 0x0f;
        }
        
        builder->ProcessScanline8((unsigned char*)dstRow);
    }
    
    p3d::FreeTemp(srcRow);
    p3d::FreeTemp(dstRow);

    builder->EndImage();
}


void LoadPNG8(png_structp pPNG, png_infop pngInfo, tImageHandler::Builder* builder)
{
    const int bpp = 8;

    int width = png_get_image_width(pPNG, pngInfo);
    int height = png_get_image_height(pPNG, pngInfo);

    int numPalette = 0;
    png_color* srcPalette;
    png_get_PLTE(pPNG, pngInfo, &srcPalette, &numPalette);
    
    pddiColour dstPalette[256];
    for (int i=0; i<numPalette; i++)
    {
        dstPalette[i] = pddiColour(srcPalette[i].red, srcPalette[i].green, srcPalette[i].blue, 255);
    }

    // load transparency channel for palette
    if (png_get_valid(pPNG, pngInfo, PNG_INFO_tRNS) > 0)
    {
        int numAlpha;
        png_byte *srcAlpha;
        png_get_tRNS(pPNG, pngInfo, &srcAlpha, &numAlpha, 0);

        for(int i=0; i < numAlpha; ++i)
        {
            dstPalette[i].SetAlpha(srcAlpha[i]);
        }
    }

    builder->BeginImage(width, height, bpp, tImageHandler::Builder::TOP, (pddiColour*)dstPalette);

    // create an image, read in the bits
    unsigned char* row = (unsigned char*)p3d::MallocTemp(png_get_rowbytes(pPNG, pngInfo));

    for(int y = 0; y < height; y++)
    {
        png_read_row(pPNG, (unsigned char*)row, (unsigned char*)NULL);
        builder->ProcessScanline8((unsigned char*)row);
    }

    p3d::FreeTemp(row);

    builder->EndImage();
}


void LoadPNG32(png_structp pPNG, png_infop pngInfo, tImageHandler::Builder* builder)
{
    int bpp = (png_get_color_type(pPNG, pngInfo) == PNG_COLOR_TYPE_RGB_ALPHA) ? 32 : 24;
    int width = png_get_image_width(pPNG, pngInfo);
    int height = png_get_image_height(pPNG, pngInfo);

    builder->BeginImage(width, height, bpp, tImageHandler::Builder::TOP, (pddiColour*)NULL);
    
    // create an image, read in the bits
    unsigned* row = (unsigned*)p3d::MallocTemp(png_get_rowbytes(pPNG, pngInfo));  
    
    for(int y = 0; y < height; y++)
    {
        png_read_row(pPNG, (unsigned char*)row, (unsigned char*)NULL);
#ifdef RAD_GAMECUBE
        int a;
        for (a = 0; a < width; a++) row[a] = __lwbrx(&row[a], 0);
#endif
        builder->ProcessScanline32(row);
    }
    
    p3d::FreeTemp(row);
    builder->EndImage();
}

#endif // !RAD_PS3
