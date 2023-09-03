/* unzip.h -- IO for uncompress .zip files using zlib
   Version 0.15 beta, Mar 19th, 1998,
 
   Copyright (C) 1998 Gilles Vollant
 
   This unzip package allow extract file from .ZIP file, compatible with PKZip 2.04g
     WinZip, InfoZip tools and compatible.
   Encryption and multi volume ZipFile (span) are not supported.
   Old compressions used by old PKZip 1.x are not supported
 
   THIS IS AN ALPHA VERSION. AT THIS STAGE OF DEVELOPPEMENT, SOMES API OR STRUCTURE
   CAN CHANGE IN FUTURE VERSION !!
   I WAIT FEEDBACK at mail info@winimage.com
   Visit also http://www.winimage.com/zLibDll/unzip.htm for evolution
 
   Condition of use and distribution are the same than zlib :
 
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
 
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
 
  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
 
 
*/
/* for more info about .ZIP format, see
      ftp://ftp.cdrom.com/pub/infozip/doc/appnote-970311-iz.zip
   PkWare has also a specification at :
      ftp://ftp.pkware.com/probdesc.zip */

#ifndef _unz_H
#define _unz_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _ZLIB_H
#include "zlib.h"
#endif

#if defined(STRICTUNZIP) || defined(STRICTZIPUNZIP)

   /* like the STRICT of WIN32, we define a pointer that cannot be converted
       from (void*) without cast */
   typedef struct TagunzFile__
   {
      int unused;
   }
   unzFile__;
   typedef unzFile__ *unzFile;
#else
typedef voidp unzFile;
#endif


#define UNZ_OK                                  (0)
#define UNZ_END_OF_LIST_OF_FILE (-100)
#define UNZ_ERRNO               (Z_ERRNO)
#define UNZ_EOF                 (0)
#define UNZ_PARAMERROR                  (-102)
#define UNZ_BADZIPFILE                  (-103)
#define UNZ_INTERNALERROR               (-104)
#define UNZ_CRCERROR                    (-105)


   /* tm_unz contain date/time info */
   typedef struct tm_unz_s
   {
      uInt tm_sec;              /* seconds after the minute - [0,59] */
      uInt tm_min;              /* minutes after the hour - [0,59] */
      uInt tm_hour;             /* hours since midnight - [0,23] */
      uInt tm_mday;             /* day of the month - [1,31] */
      uInt tm_mon;              /* months since January - [0,11] */
      uInt tm_year;             /* years - [1980..2044] */
   }
   tm_unz;

   /* unz_global_info structure contain global data about the ZIPfile
      These data comes from the end of central dir */
   typedef struct unz_global_info_s
   {
      uLong number_entry;           /* total number of entries in the central dir on this disk */
      uLong size_comment;           /* size of the global comment of the zipfile */
   }
   unz_global_info;


   /* unz_file_info contain information about a file in the zipfile */
   typedef struct unz_file_info_s
   {
      uLong version;                /* version made by                 2 bytes */
      uLong version_needed;         /* version needed to extract       2 bytes */
      uLong flag;                   /* general purpose bit flag        2 bytes */
      uLong compression_method;     /* compression method              2 bytes */
      uLong crc;                    /* crc-32                          4 bytes */
      union {
         uLong dosDate;
         struct
         {
            WORD wFatTime;
            WORD wFatDate;
         };
      };
      uLong compressed_size;        /* compressed size                 4 bytes */
      uLong uncompressed_size;      /* uncompressed size               4 bytes */
      uLong size_filename;          /* filename length                 2 bytes */
      uLong size_file_extra;        /* extra field length              2 bytes */
      uLong size_file_comment;      /* file comment length             2 bytes */

      uLong disk_num_start;         /* disk number start               2 bytes */
      uLong internal_fa;            /* internal file attributes        2 bytes */
      uLong external_fa;            /* external file attributes        4 bytes */

      tm_unz tmu_date;
   }
   unz_file_info;


   extern unzFile ZEXPORT unzOpen ( const char * path, int iOffset, int iZipSize, LPTSTR szOutputDir );
   extern int ZEXPORT unzClose OF( ( unzFile file ) );
   extern int ZEXPORT unzGetGlobalInfo OF( ( unzFile file, unz_global_info * pglobal_info ) );
   extern int ZEXPORT unzGoToFirstFile OF( ( unzFile file ) );
   extern int ZEXPORT unzGoToNextFile OF( ( unzFile file ) );
   extern int ZEXPORT unzLocateFile OF( ( unzFile file, const char * szFileName, int iCaseSensitivity ) );
   extern int ZEXPORT unzOpenCurrentFile OF( ( unzFile file ) );
   extern int ZEXPORT unzCloseCurrentFile OF( ( unzFile file ) );
   extern int ZEXPORT unzReadCurrentFile OF( ( unzFile file, voidp buf, unsigned len ) );
   extern z_off_t ZEXPORT unztell OF( ( unzFile file ) );
   

   extern int ZEXPORT unzGetCurrentFileInfo OF( ( unzFile file,
            unz_file_info * pfile_info,
            char * szFileName,
            uLong fileNameBufferSize,
            void * extraField,
            uLong extraFieldBufferSize,
            char * szComment,
            uLong commentBufferSize ) );



#ifdef __cplusplus
}
#endif

#endif /* _unz_H */
