/* unzip.c -- IO on .zip files using zlib
   Version 0.15 beta, Mar 19th, 1998,
 
   Read unzip.h for more info
*/

#include <windows.h>
#include "zlib.h"
#include "unzip.h"

#ifndef UNZ_BUFSIZE
   #define UNZ_BUFSIZE (16384)
#endif

#ifndef UNZ_MAXFILENAMEINZIP
   #define UNZ_MAXFILENAMEINZIP (256)
#endif

#ifndef ALLOC
   #define ALLOC(size) VirtualAlloc( NULL, size, MEM_COMMIT, PAGE_READWRITE);
#endif

#ifndef TRYFREE
   #define TRYFREE(p) {if (p) VirtualFree(p, 0, MEM_RELEASE); }
#endif

#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)

#ifndef INVALID_SET_FILE_POINTER
   #define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif


#ifdef  CASESENSITIVITYDEFAULT_NO
   #define CASESENSITIVITYDEFAULTVALUE 2
#else
   #define CASESENSITIVITYDEFAULTVALUE 1
#endif

#define BUFREADCOMMENT (0x400)


typedef struct unz_file_info_internal_s
{
   uLong offset_curfile;
}
unz_file_info_internal;



/* file_in_zip_read_info_s contain internal information about a file in zipfile,
    when reading and decompress it */
typedef struct
{
   HANDLE file;
   char *read_buffer;             /* internal buffer for compressed data */
   z_stream stream;                /* zLib stream structure for inflate */

   uLong pos_in_zipfile;           /* position in byte on the zipfile, for fseek*/
   uLong stream_initialised;       /* flag set if stream structure is initialised*/

   uLong offset_local_extrafield;    /* offset of the local extra field */
   uInt size_local_extrafield;    /* size of the local extra field */
   uLong pos_local_extrafield;       /* position in the local extra field in read*/

   uLong crc32;                    /* crc32 of all data uncompressed */
   uLong crc32_wait;               /* crc32 we must obtain after decompress all */
   uLong rest_read_compressed;     /* number of byte to be decompressed */
   uLong rest_read_uncompressed;    /*number of byte to be obtained after decomp*/

   uLong compression_method;       /* compression method (0==store) */
   uLong byte_before_the_zipfile;    /* byte before the zipfile, (>0 for sfx)*/
}
file_in_zip_read_info_s;



/* unz_s contain internal information about the zipfile
*/
typedef struct
{
   HANDLE file;                     /* io structore of the zipfile */
   unz_global_info gi;           /* public global information */
   uLong byte_before_the_zipfile;    /* byte before the zipfile, (>0 for sfx)*/
   uLong num_file;                 /* number of the current file in the zipfile*/
   uLong pos_in_central_dir;       /* pos of the current file in the central dir*/
   uLong current_file_ok;          /* flag about the usability of the current file*/
   uLong central_pos;              /* position of the beginning of the central dir*/

   uLong size_central_dir;         /* size of the central directory  */
   uLong offset_central_dir;       /* offset of start of central directory with
                                       respect to the starting disk number */

   unz_file_info cur_file_info;     /* public info about the current file in zip*/
   unz_file_info_internal cur_file_info_internal;     /* private info about it*/
   file_in_zip_read_info_s* pfile_in_zip_read;     /* structure about the current
                                                  file if we are decompressing it */
}
unz_s;



//static 
int unzlocal_getByte( HANDLE *hIn, int *pi )
{
   DWORD dwBytesRead;
   unsigned char c;

   BOOL bRead = ReadFile( hIn, &c, 1, &dwBytesRead, NULL );

   if ( dwBytesRead )
   {
      *pi = ( int ) c;
      return UNZ_OK;
   }
   else
   {
      if ( !bRead ) return UNZ_ERRNO;
      return UNZ_EOF;
   }
}

//static 
int unzlocal_getShort( HANDLE *hIn, uLong *pX )
{
   uLong x ;
   int i;
   int err;

   err = unzlocal_getByte( hIn, &i );
   x = ( uLong ) i;

   if ( err == UNZ_OK ) err = unzlocal_getByte( hIn, &i );
   x += ( ( uLong ) i ) << 8;

   if ( err == UNZ_OK )
      * pX = x;
   else
      *pX = 0;
   return err;
}

//static 
int unzlocal_getLong ( HANDLE *hIn, uLong *pX )
{
   uLong x ;
   int i;
   int err;

   err = unzlocal_getByte( hIn, &i );
   x = ( uLong ) i;

   if ( err == UNZ_OK ) err = unzlocal_getByte( hIn, &i );
   x += ( ( uLong ) i ) << 8;

   if ( err == UNZ_OK ) err = unzlocal_getByte( hIn, &i );
   x += ( ( uLong ) i ) << 16;

   if ( err == UNZ_OK ) err = unzlocal_getByte( hIn, &i );
   x += ( ( uLong ) i ) << 24;

   if ( err == UNZ_OK )
      * pX = x;
   else
      *pX = 0;
   return err;
}

/*
  Locate the Central directory of a zipfile (at the end, just before the global comment)
*/
//static 
uLong unzlocal_SearchCentralDir( HANDLE hIn, int iOffset, int iZipSize )
{
   DWORD dwBytesRead;
   unsigned char* buf;
   uLong uSizeFile;
   uLong uBackRead;
   uLong uMaxBack = 0xffff;     /* maximum size of global comment */
   uLong uPosFound = 0;

   if ( SetFilePointer( hIn, iOffset + iZipSize, NULL, FILE_END ) == INVALID_SET_FILE_POINTER ) return 0;

   uSizeFile = iOffset + iZipSize;

   if ( uMaxBack > uSizeFile ) uMaxBack = uSizeFile;

   buf = ( unsigned char* ) ALLOC( BUFREADCOMMENT + 4 );

   if ( buf == NULL ) return 0;

   uBackRead = 4;

   while ( uBackRead < uMaxBack )
   {
      uLong uReadSize, uReadPos ;
      int i;
      if ( uBackRead + BUFREADCOMMENT > uMaxBack ) uBackRead = uMaxBack;
      else uBackRead += BUFREADCOMMENT;

      uReadPos = uSizeFile - uBackRead;

      uReadSize = ( ( BUFREADCOMMENT + 4 ) < ( uSizeFile - uReadPos ) ) ? ( BUFREADCOMMENT + 4 ) : ( uSizeFile - uReadPos );

      if ( SetFilePointer( hIn, uReadPos, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) break;
      if ( !ReadFile( hIn, buf, ( uInt ) uReadSize, &dwBytesRead, NULL ) ) break;

      for ( i = ( int ) uReadSize - 3; ( i-- ) > 0; )
      {
         if ( ( ( *( buf + i ) ) == 0x50 ) && ( ( *( buf + i + 1 ) ) == 0x4b ) && ( ( *( buf + i + 2 ) ) == 0x05 ) && ( ( *( buf + i + 3 ) ) == 0x06 ) )
         {
            uPosFound = uReadPos + i;
            break;
         }
      }
      if ( uPosFound != 0 ) break;
   }

   TRYFREE( buf );
   return uPosFound;
}


extern unzFile ZEXPORT unzOpen ( const char *path, int iOffset, int iZipSize, LPTSTR szOutputDir )
{
   unz_s us;
   unz_s *s;
   uLong central_pos, uL;
   HANDLE *hIn;

   uLong number_disk;              /* number of the current dist, used for spaning ZIP, unsupported, always 0*/
   uLong number_disk_with_CD;      /* number the the disk with central dir, used for spaning ZIP, unsupported, always 0*/
   uLong number_entry_CD;          /* total number of entries in the central dir  (same than number_entry on nospan) */


   int err = UNZ_OK;

   hIn = CreateFile( path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

   central_pos = unzlocal_SearchCentralDir( hIn, iOffset, iZipSize );

   if ( SetFilePointer( hIn, central_pos, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) err = UNZ_ERRNO;

   /* the signature, already checked */
   if ( unzlocal_getLong( hIn, &uL ) != UNZ_OK ) err = UNZ_ERRNO;

   /* number of this disk */
   if ( unzlocal_getShort( hIn, &number_disk ) != UNZ_OK ) err = UNZ_ERRNO;

   /* number of the disk with the start of the central directory */
   if ( unzlocal_getShort( hIn, &number_disk_with_CD ) != UNZ_OK ) err = UNZ_ERRNO;

   /* total number of entries in the central dir on this disk */
   if ( unzlocal_getShort( hIn, &us.gi.number_entry ) != UNZ_OK ) err = UNZ_ERRNO;

   /* total number of entries in the central dir */
   if ( unzlocal_getShort( hIn, &number_entry_CD ) != UNZ_OK ) err = UNZ_ERRNO;

   if ( ( number_entry_CD != us.gi.number_entry ) || ( number_disk_with_CD != 0 ) || ( number_disk != 0 ) ) err = UNZ_BADZIPFILE;

   /* size of the central directory */
   if ( unzlocal_getLong( hIn, &us.size_central_dir ) != UNZ_OK ) err = UNZ_ERRNO;

   /* offset of start of central directory with respect to the
         starting disk number */
   if ( unzlocal_getLong( hIn, &us.offset_central_dir ) != UNZ_OK ) err = UNZ_ERRNO;

   /* zipfile comment length */
   if ( unzlocal_getShort( hIn, &us.gi.size_comment ) != UNZ_OK ) err = UNZ_ERRNO;

   if ( ( central_pos < us.offset_central_dir + us.size_central_dir ) && ( err == UNZ_OK ) ) err = UNZ_BADZIPFILE;

   if ( err != UNZ_OK )
   {
      CloseHandle( hIn );
      return NULL;
   }

   us.file = hIn;
   us.byte_before_the_zipfile = central_pos - ( us.offset_central_dir + us.size_central_dir );
   us.central_pos = central_pos;
   us.pfile_in_zip_read = NULL;

   s = ( unz_s* ) ALLOC( sizeof( unz_s ) );
   *s = us;
   unzGoToFirstFile( ( unzFile ) s );

   return( unzFile ) s;
}


/*
   Close a ZipFile opened with unzipOpen.
   If there is files inside the .Zip opened with unzipOpenCurrentFile (see later),
   these files MUST be closed with unzipCloseCurrentFile before call unzipClose.
   return UNZ_OK if there is no problem.
*/
extern int ZEXPORT unzClose ( unzFile file )
{
   unz_s * s;
   if ( file == NULL ) return UNZ_PARAMERROR;

   s = ( unz_s* ) file;

   if ( s->pfile_in_zip_read != NULL ) unzCloseCurrentFile( file );

   CloseHandle( s->file );
   TRYFREE( s );
   return UNZ_OK;
}



/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem. */
extern int ZEXPORT unzGetGlobalInfo ( unzFile file, unz_global_info *pglobal_info )
{
   unz_s * s;
   if ( file == NULL ) return UNZ_PARAMERROR;

   s = ( unz_s* ) file;
   *pglobal_info = s->gi;
   return UNZ_OK;
}


/*
  Get Info about the current file in the zipfile, with internal only info
*/
static int unzlocal_GetCurrentFileInfoInternal OF( ( unzFile file,
                                                     unz_file_info *pfile_info,
                                                     unz_file_info_internal
                                                     *pfile_info_internal,
                                                     char *szFileName,
                                                     uLong fileNameBufferSize,
                                                     void *extraField,
                                                     uLong extraFieldBufferSize,
                                                     char *szComment,
                                                     uLong commentBufferSize ) );

static int unzlocal_GetCurrentFileInfoInternal ( file,
                                                 pfile_info,
                                                 pfile_info_internal,
                                                 szFileName, fileNameBufferSize,
                                                 extraField, extraFieldBufferSize,
                                                 szComment, commentBufferSize )
unzFile file;
unz_file_info *pfile_info;
unz_file_info_internal *pfile_info_internal;
char *szFileName;
uLong fileNameBufferSize;
void *extraField;
uLong extraFieldBufferSize;
char *szComment;
uLong commentBufferSize;
{
   unz_s* s;
   unz_file_info file_info;
   unz_file_info_internal file_info_internal;
   int err = UNZ_OK;
   uLong uMagic;
   long lSeek = 0;

   DWORD dwDummy;

   if ( file == NULL ) return UNZ_PARAMERROR;

   s = ( unz_s* ) file;
   if ( SetFilePointer( s->file, s->pos_in_central_dir + s->byte_before_the_zipfile, 0, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) err = UNZ_ERRNO;


   if ( err == UNZ_OK )
      if ( unzlocal_getLong( s->file, &uMagic ) != UNZ_OK ) err = UNZ_ERRNO;
      else if ( uMagic != 0x02014b50 ) err = UNZ_BADZIPFILE;


   if ( unzlocal_getShort( s->file, &file_info.version ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getShort( s->file, &file_info.version_needed ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getShort( s->file, &file_info.flag ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getShort( s->file, &file_info.compression_method ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getLong( s->file, &file_info.dosDate ) != UNZ_OK ) err = UNZ_ERRNO;

   if ( unzlocal_getLong( s->file, &file_info.crc ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getLong( s->file, &file_info.compressed_size ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getLong( s->file, &file_info.uncompressed_size ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getShort( s->file, &file_info.size_filename ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getShort( s->file, &file_info.size_file_extra ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getShort( s->file, &file_info.size_file_comment ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getShort( s->file, &file_info.disk_num_start ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getShort( s->file, &file_info.internal_fa ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getLong( s->file, &file_info.external_fa ) != UNZ_OK ) err = UNZ_ERRNO;
   if ( unzlocal_getLong( s->file, &file_info_internal.offset_curfile ) != UNZ_OK ) err = UNZ_ERRNO;

   lSeek += file_info.size_filename;

   if ( ( err == UNZ_OK ) && ( szFileName != NULL ) )
   {
      uLong uSizeRead ;
      if ( file_info.size_filename < fileNameBufferSize )
      {
         *( szFileName + file_info.size_filename ) = '\0';
         uSizeRead = file_info.size_filename;
      }
      else
         uSizeRead = fileNameBufferSize;

      if ( ( file_info.size_filename > 0 ) && ( fileNameBufferSize > 0 ) )
         if ( !ReadFile( s->file, szFileName, ( uInt ) uSizeRead, &dwDummy, NULL ) ) err = UNZ_ERRNO;

      lSeek -= uSizeRead;
   }


   if ( ( err == UNZ_OK ) && ( extraField != NULL ) )
   {
      uLong uSizeRead ;
      if ( file_info.size_file_extra < extraFieldBufferSize )
         uSizeRead = file_info.size_file_extra;
      else
         uSizeRead = extraFieldBufferSize;

      if ( lSeek != 0 )
         if ( SetFilePointer( s->file, lSeek, NULL, FILE_CURRENT ) != INVALID_SET_FILE_POINTER )
            lSeek = 0;
         else err = UNZ_ERRNO;

      if ( ( file_info.size_file_extra > 0 ) && ( extraFieldBufferSize > 0 ) )
         if ( !ReadFile( s->file, extraField, ( uInt ) uSizeRead, &dwDummy, NULL ) ) err = UNZ_ERRNO;

      lSeek += file_info.size_file_extra - uSizeRead;
   }
   else
      lSeek += file_info.size_file_extra;


   if ( ( err == UNZ_OK ) && ( szComment != NULL ) )
   {
      uLong uSizeRead ;
      if ( file_info.size_file_comment < commentBufferSize )
      {
         *( szComment + file_info.size_file_comment ) = '\0';
         uSizeRead = file_info.size_file_comment;
      }
      else
         uSizeRead = commentBufferSize;

      if ( lSeek != 0 )
         if ( SetFilePointer( s->file, lSeek, NULL, FILE_CURRENT ) != INVALID_SET_FILE_POINTER ) lSeek = 0;
         else err = UNZ_ERRNO;

      if ( ( file_info.size_file_comment > 0 ) && ( commentBufferSize > 0 ) )
         if ( !ReadFile( s->file, szComment, ( uInt ) uSizeRead, &dwDummy, NULL ) ) err = UNZ_ERRNO;

      lSeek += file_info.size_file_comment - uSizeRead;
   }
   else
      lSeek += file_info.size_file_comment;

   if ( ( err == UNZ_OK ) && ( pfile_info != NULL ) )
      * pfile_info = file_info;

   if ( ( err == UNZ_OK ) && ( pfile_info_internal != NULL ) )
      * pfile_info_internal = file_info_internal;

   return err;
}



/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem.
*/
extern int ZEXPORT unzGetCurrentFileInfo ( unzFile file, unz_file_info *pfile_info, char *szFileName, uLong fileNameBufferSize, void *extraField, uLong extraFieldBufferSize, char *szComment, uLong commentBufferSize )
{
   return unzlocal_GetCurrentFileInfoInternal( file, pfile_info, NULL,
                                               szFileName, fileNameBufferSize,
                                               extraField, extraFieldBufferSize,
                                               szComment, commentBufferSize );
}

/*
  Set the current file of the zipfile to the first file.
  return UNZ_OK if there is no problem
*/
extern int ZEXPORT unzGoToFirstFile ( unzFile file )
{
   int err = UNZ_OK;
   unz_s* s;
   if ( file == NULL ) return UNZ_PARAMERROR;

   s = ( unz_s* ) file;
   s->pos_in_central_dir = s->offset_central_dir;
   s->num_file = 0;
   err = unzlocal_GetCurrentFileInfoInternal( file, &s->cur_file_info,
                                              &s->cur_file_info_internal,
                                              NULL, 0, NULL, 0, NULL, 0 );
   s->current_file_ok = ( err == UNZ_OK );
   return err;
}


/*
  Set the current file of the zipfile to the next file.
  return UNZ_OK if there is no problem
  return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
*/
extern int ZEXPORT unzGoToNextFile( unzFile file )
{
   unz_s * s;
   int err;

   if ( file == NULL ) return UNZ_PARAMERROR;
   s = ( unz_s* ) file;
   if ( !s->current_file_ok ) return UNZ_END_OF_LIST_OF_FILE;

   if ( s->num_file + 1 == s->gi.number_entry ) return UNZ_END_OF_LIST_OF_FILE;

   s->pos_in_central_dir += SIZECENTRALDIRITEM + s->cur_file_info.size_filename + s->cur_file_info.size_file_extra + s->cur_file_info.size_file_comment ;
   s->num_file++;
   err = unzlocal_GetCurrentFileInfoInternal( file, &s->cur_file_info,
                                              &s->cur_file_info_internal,
                                              NULL, 0, NULL, 0, NULL, 0 );
   s->current_file_ok = ( err == UNZ_OK );
   return err;
}


/*
  Try locate the file szFileName in the zipfile.
  For the iCaseSensitivity signification, see unzipStringFileNameCompare
 
  return value :
  UNZ_OK if the file is found. It becomes the current file.
  UNZ_END_OF_LIST_OF_FILE if the file is not found
*/
extern int ZEXPORT unzLocateFile ( unzFile file, const char *szFileName, int iCaseSensitivity )
{
   unz_s * s;
   int err;
   uLong num_fileSaved;
   uLong pos_in_central_dirSaved;

   if ( file == NULL ) return UNZ_PARAMERROR;

   if ( lstrlen( szFileName ) >= UNZ_MAXFILENAMEINZIP ) return UNZ_PARAMERROR;

   s = ( unz_s* ) file;

   if ( !s->current_file_ok ) return UNZ_END_OF_LIST_OF_FILE;

   num_fileSaved = s->num_file;
   pos_in_central_dirSaved = s->pos_in_central_dir;

   err = unzGoToFirstFile( file );

   while ( err == UNZ_OK )
   {
      char szCurrentFileName[ UNZ_MAXFILENAMEINZIP + 1 ];
      unzGetCurrentFileInfo( file, NULL, szCurrentFileName, sizeof( szCurrentFileName ) - 1, NULL, 0, NULL, 0 );

      if ( !lstrcmpi( szCurrentFileName, szFileName ) ) return UNZ_OK;
      err = unzGoToNextFile( file );
   }

   s->num_file = num_fileSaved ;
   s->pos_in_central_dir = pos_in_central_dirSaved ;
   return err;
}



/*
  Read the local header of the current zipfile
  Check the coherency of the local header and info in the end of central
        directory about this file
  store in *piSizeVar the size of extra info in local header
        (filename and size of extra field data)
*/
static int unzlocal_CheckCurrentFileCoherencyHeader( unz_s* s, uInt* piSizeVar, uLong *poffset_local_extrafield, uInt *psize_local_extrafield )
{
   uLong uMagic, uData, uFlags;
   uLong size_filename;
   uLong size_extra_field;
   int err = UNZ_OK;

   *piSizeVar = 0;
   *poffset_local_extrafield = 0;
   *psize_local_extrafield = 0;

   if ( SetFilePointer( s->file, s->cur_file_info_internal.offset_curfile + s->byte_before_the_zipfile, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) return UNZ_ERRNO;

   if ( err == UNZ_OK )
      if ( unzlocal_getLong( s->file, &uMagic ) != UNZ_OK )
         err = UNZ_ERRNO;
      else if ( uMagic != 0x04034b50 ) err = UNZ_BADZIPFILE;

   if ( unzlocal_getShort( s->file, &uData ) != UNZ_OK ) err = UNZ_ERRNO;

   if ( unzlocal_getShort( s->file, &uFlags ) != UNZ_OK ) err = UNZ_ERRNO;

   if ( unzlocal_getShort( s->file, &uData ) != UNZ_OK ) err = UNZ_ERRNO;
   else if ( ( err == UNZ_OK ) && ( uData != s->cur_file_info.compression_method ) ) err = UNZ_BADZIPFILE;

   if ( ( err == UNZ_OK ) && ( s->cur_file_info.compression_method != 0 ) && ( s->cur_file_info.compression_method != Z_DEFLATED ) ) err = UNZ_BADZIPFILE;

   if ( unzlocal_getLong( s->file, &uData ) != UNZ_OK )     /* date/time */
      err = UNZ_ERRNO;

   if ( unzlocal_getLong( s->file, &uData ) != UNZ_OK )     /* crc */
      err = UNZ_ERRNO;
   else if ( ( err == UNZ_OK ) && ( uData != s->cur_file_info.crc ) &&
             ( ( uFlags & 8 ) == 0 ) )
      err = UNZ_BADZIPFILE;

   if ( unzlocal_getLong( s->file, &uData ) != UNZ_OK )     /* size compr */
      err = UNZ_ERRNO;
   else if ( ( err == UNZ_OK ) && ( uData != s->cur_file_info.compressed_size ) &&
             ( ( uFlags & 8 ) == 0 ) )
      err = UNZ_BADZIPFILE;

   if ( unzlocal_getLong( s->file, &uData ) != UNZ_OK )     /* size uncompr */
      err = UNZ_ERRNO;
   else if ( ( err == UNZ_OK ) && ( uData != s->cur_file_info.uncompressed_size ) &&
             ( ( uFlags & 8 ) == 0 ) )
      err = UNZ_BADZIPFILE;


   if ( unzlocal_getShort( s->file, &size_filename ) != UNZ_OK )
      err = UNZ_ERRNO;
   else if ( ( err == UNZ_OK ) && ( size_filename != s->cur_file_info.size_filename ) )
      err = UNZ_BADZIPFILE;

   *piSizeVar += ( uInt ) size_filename;

   if ( unzlocal_getShort( s->file, &size_extra_field ) != UNZ_OK )
      err = UNZ_ERRNO;
   *poffset_local_extrafield = s->cur_file_info_internal.offset_curfile +
                               SIZEZIPLOCALHEADER + size_filename;
   *psize_local_extrafield = ( uInt ) size_extra_field;

   *piSizeVar += ( uInt ) size_extra_field;

   return err;
}



/*
  Open for reading data the current file in the zipfile.
  If there is no error and the file is opened, the return value is UNZ_OK.
*/
extern int ZEXPORT unzOpenCurrentFile ( file )
unzFile file;
{
   int err = UNZ_OK;
   int Store;
   uInt iSizeVar;
   unz_s* s;
   file_in_zip_read_info_s* pfile_in_zip_read_info;
   uLong offset_local_extrafield;      /* offset of the local extra field */
   uInt size_local_extrafield;        /* size of the local extra field */

   if ( file == NULL ) return UNZ_PARAMERROR;
   s = ( unz_s* ) file;
   if ( !s->current_file_ok ) return UNZ_PARAMERROR;

   if ( s->pfile_in_zip_read != NULL ) unzCloseCurrentFile( file );

   if ( unzlocal_CheckCurrentFileCoherencyHeader( s, &iSizeVar, &offset_local_extrafield, &size_local_extrafield ) != UNZ_OK )
      return UNZ_BADZIPFILE;

   pfile_in_zip_read_info = ( file_in_zip_read_info_s* )
                            ALLOC( sizeof( file_in_zip_read_info_s ) );
   if ( pfile_in_zip_read_info == NULL )
      return UNZ_INTERNALERROR;

   pfile_in_zip_read_info->read_buffer = ( char* ) ALLOC( UNZ_BUFSIZE );
   pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
   pfile_in_zip_read_info->size_local_extrafield = size_local_extrafield;
   pfile_in_zip_read_info->pos_local_extrafield = 0;

   if ( pfile_in_zip_read_info->read_buffer == NULL )
   {
      TRYFREE( pfile_in_zip_read_info );
      return UNZ_INTERNALERROR;
   }

   pfile_in_zip_read_info->stream_initialised = 0;

   if ( ( s->cur_file_info.compression_method != 0 ) &&
        ( s->cur_file_info.compression_method != Z_DEFLATED ) )
      err = UNZ_BADZIPFILE;
   Store = s->cur_file_info.compression_method == 0;

   pfile_in_zip_read_info->crc32_wait = s->cur_file_info.crc;
   pfile_in_zip_read_info->crc32 = 0;
   pfile_in_zip_read_info->compression_method = s->cur_file_info.compression_method;
   pfile_in_zip_read_info->file = s->file;
   pfile_in_zip_read_info->byte_before_the_zipfile = s->byte_before_the_zipfile;

   pfile_in_zip_read_info->stream.total_out = 0;

   if ( !Store )
   {
      pfile_in_zip_read_info->stream.zalloc = ( alloc_func ) 0;
      pfile_in_zip_read_info->stream.zfree = ( free_func ) 0;
      pfile_in_zip_read_info->stream.opaque = ( voidpf ) 0;

      err = inflateInit2( &pfile_in_zip_read_info->stream, -MAX_WBITS );
      if ( err == Z_OK ) pfile_in_zip_read_info->stream_initialised = 1;
   }
   pfile_in_zip_read_info->rest_read_compressed =
   s->cur_file_info.compressed_size ;
   pfile_in_zip_read_info->rest_read_uncompressed =
   s->cur_file_info.uncompressed_size ;


   pfile_in_zip_read_info->pos_in_zipfile =
   s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER +
   iSizeVar;

   pfile_in_zip_read_info->stream.avail_in = ( uInt ) 0;


   s->pfile_in_zip_read = pfile_in_zip_read_info;
   return UNZ_OK;
}


/*
  Read bytes from the current file.
  buf contain buffer where data must be copied
  len the size of buf.
 
  return the number of byte copied if somes bytes are copied
  return 0 if the end of file was reached
  return <0 with error code if there is an error
    (UNZ_ERRNO for IO error, or zLib error for uncompress error)
*/
extern int ZEXPORT unzReadCurrentFile ( unzFile file, voidp buf, unsigned len )
{
   int err = UNZ_OK;
   uInt iRead = 0;
   unz_s* s;
   DWORD dwDummy;

   file_in_zip_read_info_s* pfile_in_zip_read_info;
   if ( file == NULL )
      return UNZ_PARAMERROR;
   s = ( unz_s* ) file;
   pfile_in_zip_read_info = s->pfile_in_zip_read;

   if ( pfile_in_zip_read_info == NULL )
      return UNZ_PARAMERROR;


   if ( ( pfile_in_zip_read_info->read_buffer == NULL ) )
      return UNZ_END_OF_LIST_OF_FILE;
   if ( len == 0 )
      return 0;

   pfile_in_zip_read_info->stream.next_out = ( Bytef* ) buf;

   pfile_in_zip_read_info->stream.avail_out = ( uInt ) len;

   if ( len > pfile_in_zip_read_info->rest_read_uncompressed )
      pfile_in_zip_read_info->stream.avail_out =
      ( uInt ) pfile_in_zip_read_info->rest_read_uncompressed;

   while ( pfile_in_zip_read_info->stream.avail_out > 0 )
   {
      if ( ( pfile_in_zip_read_info->stream.avail_in == 0 ) &&
           ( pfile_in_zip_read_info->rest_read_compressed > 0 ) )
      {
         uInt uReadThis = UNZ_BUFSIZE;
         if ( pfile_in_zip_read_info->rest_read_compressed < uReadThis )
            uReadThis = ( uInt ) pfile_in_zip_read_info->rest_read_compressed;
         if ( uReadThis == 0 )
            return UNZ_EOF;
         if ( SetFilePointer( pfile_in_zip_read_info->file, pfile_in_zip_read_info->pos_in_zipfile + pfile_in_zip_read_info->byte_before_the_zipfile, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) return UNZ_ERRNO;
         if ( !ReadFile( pfile_in_zip_read_info->file, pfile_in_zip_read_info->read_buffer, uReadThis, &dwDummy, NULL ) ) return UNZ_ERRNO;

         pfile_in_zip_read_info->pos_in_zipfile += uReadThis;

         pfile_in_zip_read_info->rest_read_compressed -= uReadThis;

         pfile_in_zip_read_info->stream.next_in =
         ( Bytef* ) pfile_in_zip_read_info->read_buffer;
         pfile_in_zip_read_info->stream.avail_in = ( uInt ) uReadThis;
      }

      if ( pfile_in_zip_read_info->compression_method == 0 )
      {
         uInt uDoCopy, i ;
         if ( pfile_in_zip_read_info->stream.avail_out <
              pfile_in_zip_read_info->stream.avail_in )
            uDoCopy = pfile_in_zip_read_info->stream.avail_out ;
         else
            uDoCopy = pfile_in_zip_read_info->stream.avail_in ;

         for ( i = 0; i < uDoCopy; i++ )
            *( pfile_in_zip_read_info->stream.next_out + i ) =
            *( pfile_in_zip_read_info->stream.next_in + i );

         pfile_in_zip_read_info->crc32 = crc32( pfile_in_zip_read_info->crc32,
                                                pfile_in_zip_read_info->stream.next_out,
                                                uDoCopy );
         pfile_in_zip_read_info->rest_read_uncompressed -= uDoCopy;
         pfile_in_zip_read_info->stream.avail_in -= uDoCopy;
         pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
         pfile_in_zip_read_info->stream.next_out += uDoCopy;
         pfile_in_zip_read_info->stream.next_in += uDoCopy;
         pfile_in_zip_read_info->stream.total_out += uDoCopy;
         iRead += uDoCopy;
      }
      else
      {
         uLong uTotalOutBefore, uTotalOutAfter;
         const Bytef *bufBefore;
         uLong uOutThis;
         int flush = Z_SYNC_FLUSH;

         uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
         bufBefore = pfile_in_zip_read_info->stream.next_out;
         err = inflate( &pfile_in_zip_read_info->stream, flush );

         uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
         uOutThis = uTotalOutAfter - uTotalOutBefore;

         pfile_in_zip_read_info->crc32 =
         crc32( pfile_in_zip_read_info->crc32, bufBefore,
                ( uInt ) ( uOutThis ) );

         pfile_in_zip_read_info->rest_read_uncompressed -=
         uOutThis;

         iRead += ( uInt ) ( uTotalOutAfter - uTotalOutBefore );

         if ( err == Z_STREAM_END )
            return( iRead == 0 ) ? UNZ_EOF : iRead;
         if ( err != Z_OK )
            break;
      }
   }

   if ( err == Z_OK )
      return iRead;
   return err;
}


/*
  Close the file in zip opened with unzipOpenCurrentFile
  Return UNZ_CRCERROR if all the file was read but the CRC is not good
*/
extern int ZEXPORT unzCloseCurrentFile ( unzFile file )
{
   int err = UNZ_OK;

   unz_s* s;
   file_in_zip_read_info_s* pfile_in_zip_read_info;
   if ( file == NULL )
      return UNZ_PARAMERROR;
   s = ( unz_s* ) file;
   pfile_in_zip_read_info = s->pfile_in_zip_read;

   if ( pfile_in_zip_read_info == NULL )
      return UNZ_PARAMERROR;


   if ( pfile_in_zip_read_info->rest_read_uncompressed == 0 )
   {
      if ( pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32_wait )
         err = UNZ_CRCERROR;
   }


   TRYFREE( pfile_in_zip_read_info->read_buffer );
   pfile_in_zip_read_info->read_buffer = NULL;
   if ( pfile_in_zip_read_info->stream_initialised )
      inflateEnd( &pfile_in_zip_read_info->stream );

   pfile_in_zip_read_info->stream_initialised = 0;
   TRYFREE( pfile_in_zip_read_info );

   s->pfile_in_zip_read = NULL;

   return err;
}
