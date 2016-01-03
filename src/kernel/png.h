#ifndef _PNG_H
#define _PNG_H

#include <zlib.h>

#ifndef BSIZE
#define BSIZE 20480
#endif

#define byte unsigned char

struct png_internal_struct {
    unsigned long width;
    unsigned long height;
    byte bit_depth, color_type, comression, filter, interlace;
    byte PLTE;
    FILE* png;
    byte position;
    /* position in file:
       0: between chunks (between crc of old and length of new)
       1: before chunk data (length + header already read)
       2: after chunk data (crc not yet read)
    */
    unsigned long length, crc;
    char chunk_type[4];
    /* values of chunk read last */
    unsigned char* mem_ptr;
    z_stream d_stream;
    /* for zlib library status */
    int zlib_initialized;
    int readbuf_initialized;
    byte* readbuf;
    int buf_pos, buf_filled;
    unsigned long chunk_pos;
    unsigned long checksum;
} ;

struct png_info_struct {
    unsigned long width;
    unsigned long height;
    byte bit_depth, color_type, compression, filter, interlace;
};

#ifdef __cplusplus
extern "C" {
#endif
int CorrectLong1( unsigned long* l, unsigned char* Buf );
int CorrectLong2( unsigned char* Buf, unsigned long* l );
int InitPNG( FILE* png, struct png_info_struct* info, struct png_internal_struct* internal );
/* Has to be called after opening and before reading the PNG file      */
/* returns -1 if png signature check failed or first chunk is not IHDR */
int GetNextChunk( struct png_internal_struct* i );
/* Reads length and chunk type of next chunk in file */
/* and sets fields length and chunk_type */
int ReadChunkData( struct png_internal_struct* i, byte* mem );
/* Reads data of a chunk. It is required that the length and header */
/* of the chunk has already been read. If not, function returns */
/* -1. -2 indicates an invalid CRC */
int do_inflate( struct png_internal_struct* i );
/* is used to call the function for decompressing of zlib */
/* returns return code of zlib and 10 if Z_STREAM_END */
int ReadPNGLine( struct png_internal_struct* i, unsigned char* Buf );
/* reads and decompresses a single line (+ filter byte) from file into Buf */
/* -1 if file position isn't at the beginning of an IDAT chunk */
/* -2 CRC error */
/* -3 if the next chunk needed to reconstruct line isn't IDAT */
int DoUnFiltering( struct png_internal_struct* i, unsigned char* Buf, unsigned char* Buf_up );
/* This funtion is feeded with a line aquired by ReadPNGLine (*buf) */
/* It calculates filtered data -> raw data */
/* Buf up is the raw date of the line above. May be NULL */
int DoFiltering( struct png_internal_struct* i, unsigned char* Buf );
int InitWritePNG( FILE* png, struct png_info_struct* info, struct png_internal_struct* i );
int WriteChunk( struct png_internal_struct* i, unsigned char* Buf );
int WritePNGLine( struct png_internal_struct* i, unsigned char* Buf );
int EndIDAT( struct png_internal_struct* i );
int PosOverIEND( struct png_internal_struct* i );
int PosOverIHDR( struct png_internal_struct* i );
int EndPNG( struct png_internal_struct* i );

#ifdef __cplusplus
}
#endif
#endif
