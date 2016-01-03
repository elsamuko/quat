/* Quat - A 3D fractal generation program */
/* Copyright (C) 1997,98 Dirk Meyer */
/* (email: dirk.meyer@studserv.uni-stuttgart.de) */
/* mail:  Dirk Meyer */
/*        Marbacher Weg 29 */
/*        D-71334 Waiblingen */
/*        Germany */
/* */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License */
/* as published by the Free Software Foundation; either version 2 */
/* of the License, or (at your option) any later version. */
/* */
/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */
/* */
/* You should have received a copy of the GNU General Public License */
/* along with this program; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>   /* malloc, free */
#include <string.h>
#include <math.h>
#include "png.h"

void make_crc_table( void );
unsigned long update_crc( unsigned long crc, const unsigned char* buf, long len );
unsigned long crc( unsigned char* buf, long len );
int do_deflate( struct png_internal_struct* i );
unsigned char PaethPredictor( unsigned char a, unsigned char b, unsigned char c );

/* NOTE: In this (new) version of PNG.C the buffer "readbuf" in */
/* png_internal_struct is allocated via malloc */
/* a call of EndPNG() is needed after use of these structure */
/* to deallocate the buffer */

unsigned char png_signature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

unsigned long crc_table[256];
int crc_table_computed = 0;

void make_crc_table( void ) {
    unsigned long c;
    int n, k;

    for( n = 0; n < 256; n++ ) {
        c = ( unsigned long ) n;

        for( k = 0; k < 8; k++ ) {
            if( c & 1 ) {
                c = 0xedb88320L ^ ( c >> 1 );
            } else {
                c = c >> 1;
            }
        }

        crc_table[n] = c;
    }

    crc_table_computed = 1;
}

unsigned long update_crc( unsigned long crc, const unsigned char* buf, long len ) {
    unsigned long c = crc;
    int n;

    if( !crc_table_computed ) {
        make_crc_table();
    }

    for( n = 0; n < len; n++ ) {
        c = crc_table[( int )( ( c ^ ( long )buf[n] ) & 0xffL )] ^ ( c >> 8 );
    }

    return( c );
}

unsigned long crc( unsigned char* buf, long len ) {
    return( update_crc( 0xffffffffL, buf, len ) ^ 0xffffffffL );
}

int CorrectLong1( unsigned long* l, unsigned char* Buf ) {
    Buf[0] = ( unsigned char )( *l >> 24 & 0xffL );
    Buf[1] = ( unsigned char )( *l >> 16 & 0xffL );
    Buf[2] = ( unsigned char )( *l >> 8  & 0xffL );
    Buf[3] = ( unsigned char )( *l & 0xffL );

    return( 0 );
}

int CorrectLong2( unsigned char* Buf, unsigned long* l ) {
    *l = Buf[0];
    *l <<= 8;
    *l |= Buf[1];
    *l <<= 8;
    *l |= Buf[2];
    *l <<= 8;
    *l |= Buf[3];

    return( 0 );
}

int InitPNG( FILE* png, struct png_info_struct* info, struct png_internal_struct* internal ) {
    unsigned char sig[8], LongBuf[4], infobuf[100];
    size_t max;

    internal->readbuf_initialized = 0;     /* for EndPNG, if no valid file */
    internal->png = png;
    fseek( internal->png, 0L, SEEK_SET );
    fread( sig, 1, 8, internal->png );
    {
        int i;

        for( i = 0; i < 8; ++i ) if( sig[i] != png_signature[i] ) {
                return -1;
            }
    }
    /*   if (strncmp(sig, png_signature, 8)!=0) return(-1); */
    fread( LongBuf, 1, 4, internal->png );
    CorrectLong2( LongBuf, &internal->length );
    fread( internal->chunk_type, 1, 4, internal->png );

    if( strncmp( internal->chunk_type, "IHDR", 4 ) != 0 ) {
        return( -1 );
    }

    max = ( size_t )internal->length;

    if( sizeof( struct png_info_struct ) < max ) {
        max = sizeof( struct png_info_struct );
    }

    if( max > 100 ) {
        max = 100;    /* should never happen in IHDR! */
    }

    fread( infobuf, 1, max, internal->png );
    {
        unsigned long l;
        CorrectLong2( &infobuf[0], &l );
        info->width = l;
        CorrectLong2( &infobuf[4], &l );
        info->height = l;
    }
    info->bit_depth = infobuf[8];
    info->color_type = infobuf[9];
    info->compression = infobuf[10];
    info->filter = infobuf[11];
    info->interlace = infobuf[12];
    fread( LongBuf, 1, 4, internal->png );
    CorrectLong2( LongBuf, &internal->crc );
    memcpy( internal, info, sizeof( struct png_info_struct ) );
    internal->PLTE = 0;
    internal->position = 0;
    internal->zlib_initialized = 0;
    internal->readbuf_initialized = 0;
    internal->buf_filled = 0;
    internal->chunk_pos = 0;
    internal->readbuf = NULL;
    return( 0 );
}

int GetNextChunk( struct png_internal_struct* i ) {
    unsigned char LongBuf[4];

    if( i->position == 1 ) {
        fseek( i->png, ( long )i->length + 4, SEEK_CUR );
    } else if( i->position == 2 ) {
        fseek( i->png, 4L, SEEK_CUR );
    }

    i->position = 1;
    fread( LongBuf, 1, 4, i->png );
    CorrectLong2( LongBuf, &i->length );
    fread( i->chunk_type, 1, 4, i->png );

    if( strncmp( i->chunk_type, "PLTE", 4 ) == 0 ) {
        i->PLTE++;
    }

    return( 0 );
}

int ReadChunkData( struct png_internal_struct* i, byte* mem ) {
    unsigned long checksum;
    unsigned char LongBuf[4];

    if( i->position != 1 ) {
        return( -1 );
    }

    i->mem_ptr = mem;
    fread( i->mem_ptr, 1, ( size_t )i->length, i->png );
    fread( LongBuf, 1, 4, i->png );
    CorrectLong2( LongBuf, &i->crc );
    checksum = update_crc( 0xffffffffL, ( unsigned char* )i->chunk_type, 4 );
    checksum = update_crc( checksum, i->mem_ptr, ( long )i->length );
    checksum ^= 0xffffffffL;

    if( checksum != i->crc ) {
        return( -2 );
    }

    i->position = 0;

    return( 0 );
}

int do_inflate( struct png_internal_struct* i ) {
    int err;

    if( i->zlib_initialized == 0 ) {
        i->zlib_initialized = 1;
        i->d_stream.zalloc = ( alloc_func )0;
        i->d_stream.zfree = ( free_func )0;

        err = inflateInit( &i->d_stream );
    }

    err = inflate( &i->d_stream, Z_NO_FLUSH );

    if( err == Z_STREAM_END ) {
        err = inflateEnd( &i->d_stream );
        return( 10 );
    }

    return( err );
}

int ReadPNGLine( struct png_internal_struct* i, unsigned char* Buf ) {
    long toread;
    int err;
    int ByPP;
    unsigned char LongBuf[4];

    /* Test, if chunk is IDAT */
    if( strncmp( i->chunk_type, "IDAT", 4 ) != 0 ) {
        return( -1 );
    }

    /* Initialize on first call */
    if( i->chunk_pos == 0 ) {
        i->checksum = update_crc( 0xffffffffL, ( unsigned char* )i->chunk_type, 4 );
    }

    if( i->readbuf_initialized == 0 ) {
        i->readbuf = malloc( BSIZE );

        if( !i->readbuf ) {
            return( -2 );    /* Memory error */
        }

        i->readbuf_initialized = 1;
    }

    if( i->buf_filled == 0 ) {
        toread = ( i->length - i->chunk_pos );

        if( toread > BSIZE ) {
            toread = BSIZE;
        }

        i->buf_filled = fread( i->readbuf, ( size_t )1, ( size_t )toread, i->png );

        if( i->buf_filled == 0L ) {
            return( -1 );
        }

        i->buf_pos = 0;
        i->chunk_pos += i->buf_filled;
        i->checksum = update_crc( i->checksum, i->readbuf, toread );
    }

    ByPP = ( int )ceil( ( double )i->bit_depth / 8 );

    if( i->color_type == 2 ) {
        ByPP *= 3;
    }

    /* Only decompress a single line & filter byte */
    i->d_stream.avail_out = ( int )( i->width * ByPP + 1 );
    i->d_stream.next_out = Buf;

    if( i->color_type == 3 ) {
        switch( i->bit_depth ) {
            case 1 :
                i->d_stream.avail_out = ( int )( i->width / 8 + 1 );
                break;

            case 2 :
                i->d_stream.avail_out = ( int )( i->width / 4 + 1 );
                break;

            case 4 :
                i->d_stream.avail_out = ( int )( i->width / 2 + 1 );
                break;
        }
    }

    do {
        i->d_stream.next_in = i->readbuf + i->buf_pos;
        i->d_stream.avail_in = i->buf_filled - i->buf_pos;
        err = do_inflate( i );
        i->buf_pos = ( unsigned int )( i->d_stream.next_in - i->readbuf );

        /* ready ? */
        if( i->d_stream.avail_out == 0 ) {
            break;
        }

        /* no process made */
        if( i->d_stream.avail_out != 0 || i->buf_pos == i->buf_filled ) {
            /* because chunk at end */
            if( i->chunk_pos == i->length ) {
                /* verify crc */
                i->checksum ^= 0xffffffffL;
                fread( LongBuf, 1, 4, i->png );
                CorrectLong2( LongBuf, &i->crc );
                i->position = 0;

                if( i->checksum != i->crc ) {
                    return( -2 );
                }

                /* read next chunk header */
                fread( LongBuf, 1, 4, i->png );
                CorrectLong2( LongBuf, &i->length );
                fread( i->chunk_type, 1, 4, i->png );
                i->position = 1;

                /* is it an IDAT chunk? */
                if( strncmp( i->chunk_type, "IDAT", 4 ) != 0 ) {
                    return( -3 );
                }

                /* initialize checksum */
                i->checksum = update_crc( 0xffffffffL, ( unsigned char* )i->chunk_type, 4 );
                /* prepare reading chunk data */
                i->chunk_pos = 0;
                toread = i->length;

                if( toread > BSIZE ) {
                    toread = BSIZE;
                }

                i->buf_filled = fread( i->readbuf, 1, ( size_t )toread, i->png );

                if( i->buf_filled == 0L ) {
                    return( -1 );
                }

                i->buf_pos = 0;
                i->chunk_pos += i->buf_filled;
                i->checksum = update_crc( i->checksum, i->readbuf, toread );
            }
            /* because 2K buffer at end */
            else {
                toread = ( i->length - i->chunk_pos );

                if( toread > BSIZE ) {
                    toread = BSIZE;
                }

                i->buf_filled = fread( i->readbuf, 1, ( size_t )toread, i->png );

                if( i->buf_filled == 0L ) {
                    return( -1 );
                }

                i->buf_pos = 0;
                i->chunk_pos += i->buf_filled;
                i->checksum = update_crc( i->checksum, i->readbuf, toread );
            }
        }
    } while( i->d_stream.avail_out != 0 );

    return( 0 );
}

unsigned char PaethPredictor( unsigned char a, unsigned char b, unsigned char c ) {
    int p;
    unsigned int pa, pb, pc;

    /* a = left, b = above, c = upper left  */
    p = a + b - c;
    pa = abs( p - a );
    pb = abs( p - b );
    pc = abs( p - c );

    if( pa <= pb && pa <= pc ) {
        return a;
    } else if( pb <= pc ) {
        return b;
    } else {
        return c;
    }
}

int DoUnFiltering( struct png_internal_struct* i, unsigned char* Buf, unsigned char* Buf_up ) {
    unsigned long ByPP, uj;
    long up, prior, upperleft;

    ByPP = ( unsigned long )ceil( ( double )i->bit_depth / 8.0 );

    if( i->color_type == 2 ) {
        ByPP *= 3;
    }

    switch( Buf[0] ) {
        case 0:
            break;       /* No filter */

        case 1:              /* Sub */
            for( uj = ByPP + 1; uj <= i->width * ByPP; uj++ ) {
                Buf[uj] = ( ( unsigned int )Buf[uj] + ( unsigned int )Buf[uj - ByPP] ) % 256;
            }

            Buf[0] = 0;
            break;

        case 2:              /* Up */
            if( Buf_up == NULL ) {
                Buf[0] = 0;
                return( 0 ); /* according to png spec. it is assumed that */
                /* Buf_up is zero everywhere */
            }

            for( uj = 1; uj <= i->width * ByPP; uj++ ) {
                Buf[uj] = ( ( unsigned int )Buf[uj] + ( unsigned int )Buf_up[uj] ) % 256;
            }

            Buf[0] = 0;
            break;

        case 3:             /* Average */
            for( uj = 1; uj <= i->width * ByPP; uj++ ) {
                if( uj > ByPP ) {
                    prior = Buf[uj - ByPP];
                } else {
                    prior = 0;
                }

                if( Buf_up != NULL ) {
                    up = Buf_up[uj];
                } else {
                    up = 0;
                }

                Buf[uj] = ( ( long )Buf[uj] + ( ( prior + up ) >> 1 ) ) % 256;
            }

            Buf[0] = 0;
            break;

        case 4:             /* Paeth */
            for( uj = 1; uj <= i->width * ByPP; uj++ ) {
                if( Buf_up != NULL ) {
                    up = Buf_up[uj];
                } else {
                    up = 0;
                }

                if( uj > ByPP ) {
                    prior = Buf[uj - ByPP];
                } else {
                    prior = 0;
                }

                if( uj > ByPP && Buf_up != NULL ) {
                    upperleft = Buf_up[uj - ByPP];
                } else {
                    upperleft = 0;
                }

                Buf[uj] = ( ( unsigned int )Buf[uj] + ( unsigned int )PaethPredictor( prior,
                            up, upperleft ) );
            }

            Buf[0] = 0;
            break;
    }

    return( 0 );
}

int DoFiltering( struct png_internal_struct* i, unsigned char* Buf ) {
    int ByPP, j, t;

    if( Buf[0] == 0 ) {
        return( 0 );    /* No filter */
    }

    if( Buf[0] == 1 ) {       /* Sub       */
        ByPP = ( int )ceil( ( double )i->bit_depth / 8 );

        if( i->color_type == 2 ) {
            ByPP *= 3;
        }

        for( j = ( int )i->width * ByPP; j >= ByPP + 1; j-- ) {
            t = ( ( int )Buf[j] - ( int )Buf[j - ByPP] );

            if( t < 0 ) {
                t += 256;
            }

            Buf[j] = t % 256;
        }
    }

    return( 0 );
}

int InitWritePNG( FILE* png, struct png_info_struct* info, struct png_internal_struct* i ) {
    unsigned long length, crc_v;
    unsigned char Buf[20];

    i->png = png;

    if( fwrite( png_signature, 1, 8, png ) != 8 ) {
        return( -1 );
    }

    i->length = length = 13;
    CorrectLong1( &length, Buf );

    if( fwrite( Buf, 1, 4, png ) != 4 ) {
        return( -1 );
    }

    if( fwrite( "IHDR", 1, 4, png ) != 4 ) {
        return( -1 );
    }

    CorrectLong1( &info->width, Buf );
    CorrectLong1( &info->height, Buf + 4 );
    Buf[8] = info->bit_depth;
    Buf[9] = info->color_type;
    Buf[10] = info->compression;
    Buf[11] = info->filter;
    Buf[12] = info->interlace;

    if( fwrite( Buf, 1, 13, png ) != 13 ) {
        return( -1 );
    }

    crc_v = update_crc( 0xffffffffL, ( unsigned char* )"IHDR", 4 );
    crc_v = update_crc( crc_v, Buf, ( long )i->length );
    crc_v ^= 0xffffffffL;
    CorrectLong1( &crc_v, Buf );

    if( fwrite( Buf, 1, 4, png ) != 4 ) {
        return( -1 );
    }

    memcpy( i, info, sizeof( struct png_info_struct ) );
    strncpy( i->chunk_type, "IHDR", 4 );
    i->PLTE = 0;
    i->position = 0;
    i->zlib_initialized = 0;
    i->readbuf_initialized = 0;
    i->buf_filled = 0;
    i->chunk_pos = 0;
    i->readbuf = NULL;

    return( 0 );
}

int WriteChunk( struct png_internal_struct* i, unsigned char* Buf ) {
    unsigned long length, crc_v;
    unsigned char LongBuf[4];

    length = i->length;
    CorrectLong1( &length, LongBuf );

    if( fwrite( LongBuf, 1, 4, i->png ) != 4 ) {
        return( -1 );
    }

    if( fwrite( i->chunk_type, 1, 4, i->png ) != 4 ) {
        return( -1 );
    }

    length = fwrite( Buf, 1, ( size_t )i->length, i->png );

    if( length != i->length ) {
        return( -1 );
    }

    crc_v = update_crc( 0xffffffffL, ( unsigned char* )i->chunk_type, 4 );
    crc_v = update_crc( crc_v, Buf, ( long )i->length );
    crc_v ^= 0xffffffffL;
    CorrectLong1( &crc_v, LongBuf );

    if( fwrite( LongBuf, 1, 4, i->png ) != 4 ) {
        return( -1 );
    }

    i->position = 0;

    return( 0 );
}

int do_deflate( struct png_internal_struct* i ) {
    int err;

    if( i->zlib_initialized == 0 ) {
        i->zlib_initialized = 1;
        i->d_stream.zalloc = ( alloc_func )Z_NULL;
        i->d_stream.zfree = ( free_func )Z_NULL;
        err = deflateInit( &i->d_stream, Z_DEFAULT_COMPRESSION );
    }

    err = deflate( &i->d_stream, Z_NO_FLUSH );

    return( err );
}

int WritePNGLine( struct png_internal_struct* i, unsigned char* Buf ) {
    int err = err;
    int ByPP;

    /* Initialize on first call */
    if( i->chunk_pos == 0 ) {
        i->checksum = update_crc( 0xffffffffL, ( unsigned char* )"IDAT", 4 );

        if( fwrite( i, 1, 4, i->png ) != 4 ) {
            return( -1 );    /* length as dummy */
        }

        if( fwrite( "IDAT", 1, 4, i->png ) != 4 ) {
            return( -1 );
        }
    }

    if( i->readbuf_initialized == 0 ) {
        i->readbuf = malloc( BSIZE );

        if( !i->readbuf ) {
            return( -2 );    /* Memory error */
        }

        i->readbuf_initialized = 1;
    }

    ByPP = ( int )ceil( ( double )i->bit_depth / 8 );

    if( i->color_type == 2 ) {
        ByPP *= 3;
    }

    i->d_stream.avail_in = ( int )( ( i->width ) * ByPP + 1 ); /* Only compress a single line */
    i->d_stream.next_in = Buf;

    if( i->color_type == 3 ) {
        switch( i->bit_depth ) {
            case 1 :
                i->d_stream.avail_in = ( int )( ( i->width ) / 8 + 1 );
                break;

            case 2 :
                i->d_stream.avail_in = ( int )( ( i->width ) / 4 + 1 );
                break;

            case 4 :
                i->d_stream.avail_in = ( int )( ( i->width ) / 2 + 1 );
                break;
        }
    }

    do {
        i->d_stream.next_out = i->readbuf;
        i->d_stream.avail_out = BSIZE;
        err = do_deflate( i );

        i->buf_pos = ( int )( i->d_stream.next_out - i->readbuf );
        i->checksum = update_crc( i->checksum, i->readbuf, i->buf_pos );

        if( fwrite( i->readbuf, 1, ( size_t )i->buf_pos, i->png ) != ( size_t )i->buf_pos ) {
            return( -1 );
        }

        i->chunk_pos += i->buf_pos;
    } while( i->d_stream.avail_out == 0 );

    return( 0 );
}

int EndIDAT( struct png_internal_struct* i ) {
    int err = err;
    unsigned long length;
    unsigned char LongBuf[4];

    if( !i->readbuf ) {
        return( -1 );    /* EndIDAT called too early */
    }

    do {
        i->d_stream.next_out = i->readbuf;
        i->d_stream.avail_out = BSIZE;
        err = deflate( &i->d_stream, Z_FINISH );
        i->buf_pos = ( int )( i->d_stream.next_out - i->readbuf );

        if( fwrite( i->readbuf, 1, ( size_t )i->buf_pos, i->png ) != ( size_t )i->buf_pos ) {
            return( -1 );
        }

        i->chunk_pos += i->buf_pos;
        i->checksum = update_crc( i->checksum, i->readbuf, i->buf_pos );
    } while( i->d_stream.avail_out == 0 );

    err = deflateEnd( &i->d_stream );

    i->checksum ^= 0xffffffffL;
    CorrectLong1( &i->checksum, LongBuf );

    if( fwrite( LongBuf, 1, 4, i->png ) != 4 ) {
        return( -1 );
    }

    fseek( i->png, -( long )( i->chunk_pos + 12 ), SEEK_CUR );
    length = i->chunk_pos;
    CorrectLong1( &length, LongBuf );

    if( fwrite( LongBuf, 1, 4, i->png ) != 4 ) {
        return( -1 );
    }

    fseek( i->png, ( long )i->chunk_pos + 8, SEEK_CUR );
    i->position = 0;

    return( 0 );
}

int PosOverIEND( struct png_internal_struct* i ) {
    struct png_internal_struct last;

    while( strncmp( i->chunk_type, "IEND", 4 ) != 0 ) {
        last = *i;
        GetNextChunk( i );
    }

    fseek( i->png, -8, SEEK_CUR );
    *i = last;
    return( 0 );
}

int PosOverIHDR( struct png_internal_struct* i ) {
    unsigned char LongBuf[4];

    fseek( i->png, 8, SEEK_SET );
    fread( LongBuf, 1, 4, i->png );
    CorrectLong2( LongBuf, &i->length );
    fread( i->chunk_type, 1, 4, i->png );
    i->position = 1;
    return( 0 );
}

int EndPNG( struct png_internal_struct* i ) {
    if( i->readbuf_initialized != 0 ) {
        free( i->readbuf );
    }

    return( 0 );
}

