/* ioapi.h
   Copyright (C) 2003 Tommi Mäkitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef _ZLIBIOAPI_H
#define _ZLIBIOAPI_H


#define ZLIB_FILEFUNC_SEEK_CUR (1)
#define ZLIB_FILEFUNC_SEEK_END (2)
#define ZLIB_FILEFUNC_SEEK_SET (0)

#define ZLIB_FILEFUNC_MODE_READ      (1)
#define ZLIB_FILEFUNC_MODE_WRITE     (2)
#define ZLIB_FILEFUNC_MODE_READWRITEFILTER (3)

#define ZLIB_FILEFUNC_MODE_EXISTING (4)
#define ZLIB_FILEFUNC_MODE_CREATE   (8)


#ifndef ZCALLBACK

#if (defined(WIN32) || defined (WINDOWS) || defined (_WINDOWS)) && defined(CALLBACK) && defined (USEWINDOWS_CALLBACK)
#define ZCALLBACK CALLBACK
#else
#define ZCALLBACK
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef voidpf (ZCALLBACK *open_file_func) OF((voidpf opaque, const char* filename, int mode));
typedef uLong  (ZCALLBACK *read_file_func) OF((voidpf opaque, voidpf stream, void* buf, uLong size));
typedef uLong  (ZCALLBACK *write_file_func) OF((voidpf opaque, voidpf stream, const void* buf, uLong size));
typedef long   (ZCALLBACK *tell_file_func) OF((voidpf opaque, voidpf stream));
typedef long   (ZCALLBACK *seek_file_func) OF((voidpf opaque, voidpf stream, uLong offset, int origin));
typedef int    (ZCALLBACK *close_file_func) OF((voidpf opaque, voidpf stream));
typedef int    (ZCALLBACK *testerror_file_func) OF((voidpf opaque, voidpf stream));

typedef struct zlib_filefunc_def_s
{
    open_file_func      zopen_file;
    read_file_func      zread_file;
    write_file_func     zwrite_file;
    tell_file_func      ztell_file;
    seek_file_func      zseek_file;
    close_file_func     zclose_file;
    testerror_file_func zerror_file;
    voidpf              opaque;
} zlib_filefunc_def;



void fill_fopen_filefunc OF((zlib_filefunc_def* pzlib_filefunc_def));

#define ZREAD(filefunc,filestream,buf,size) ((*((filefunc).zread_file))((filefunc).opaque,filestream,buf,size))
#define ZWRITE(filefunc,filestream,buf,size) ((*((filefunc).zwrite_file))((filefunc).opaque,filestream,buf,size))
#define ZTELL(filefunc,filestream) ((*((filefunc).ztell_file))((filefunc).opaque,filestream))
#define ZSEEK(filefunc,filestream,pos,mode) ((*((filefunc).zseek_file))((filefunc).opaque,filestream,pos,mode))
#define ZCLOSE(filefunc,filestream) ((*((filefunc).zclose_file))((filefunc).opaque,filestream))
#define ZERROR(filefunc,filestream) ((*((filefunc).zerror_file))((filefunc).opaque,filestream))


#ifdef __cplusplus
}
#endif

#endif

