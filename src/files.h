/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#define MAX_LISTED_FILES    2048
#define MAX_LISTED_DEPTH    8

typedef struct file_info_s {
    size_t  size;
    time_t  ctime;
    time_t  mtime;
    char    name[1];
} file_info_t;

/* bits 0 - 1, enum */
#define FS_MODE_APPEND          0x00000000
#define FS_MODE_READ            0x00000001
#define FS_MODE_WRITE           0x00000002
#define FS_MODE_RDWR            0x00000003
#define FS_MODE_MASK            0x00000003

/* bit 2, enum */
#define FS_FLUSH_NONE           0x00000000
#define FS_FLUSH_SYNC           0x00000004
#define FS_FLUSH_MASK           0x00000004

/* bits 3 - 4, enum */
#define FS_TYPE_ANY             0x00000000
#define FS_TYPE_REAL            0x00000008
#define FS_TYPE_PAK             0x00000010
#define FS_TYPE_RESERVED        0x00000018
#define FS_TYPE_MASK            0x00000018

/* bits 5 - 6, flag */
#define FS_PATH_ANY             0x00000000
#define FS_PATH_BASE            0x00000020
#define FS_PATH_GAME            0x00000040
#define FS_PATH_MASK            0x00000060

/* bits 7 - 11, flag */
#define FS_SEARCH_BYFILTER      0x00000080
#define FS_SEARCH_SAVEPATH      0x00000100
#define FS_SEARCH_EXTRAINFO     0x00000200
#define FS_SEARCH_STRIPEXT      0x00000400
#define FS_SEARCH_DIRSONLY      0x00000800

/* bits 7 - 8, flag */
#define FS_FLAG_GZIP            0x00000080
#define FS_FLAG_EXCL            0x00000100

//
// Limit the maximum file size FS_LoadFile can handle, as a protection from
// malicious paks causing memory exhaustion.
//
// Maximum size of legitimate BSP file on disk is ~12.7 MiB, let's round this
// up to 16 MiB. Assume that no loadable Q2 resource should ever exceed this
// limit.
//
#define MAX_LOADFILE            0x1000000

// macros for dealing portably with files at OS level
#ifdef _WIN32
#define FS_strcmp   Q_strcasecmp
#define FS_strncmp  Q_strncasecmp
#else
#define FS_strcmp   strcmp
#define FS_strncmp  strncmp
#endif

#define FS_Malloc( size )       Z_TagMalloc( size, TAG_FILESYSTEM )
#define FS_Mallocz( size )      Z_TagMallocz( size, TAG_FILESYSTEM )
#define FS_CopyString( string ) Z_TagCopyString( string, TAG_FILESYSTEM )
#define FS_LoadFile( path, buf ) FS_LoadFileEx( path, buf, 0, TAG_FILESYSTEM )
#define FS_FreeFile( buf )      Z_Free( buf )

// just regular malloc for now
#define FS_AllocTempMem( size ) FS_Malloc( size )
#define FS_FreeTempMem( buf )   Z_Free( buf )

void    FS_Init( void );
void    FS_Shutdown( void );
void    FS_Restart( qboolean total );

#if USE_CLIENT
qerror_t FS_RenameFile( const char *from, const char *to );
#endif

qerror_t FS_CreatePath( char *path );

char    *FS_CopyExtraInfo( const char *name, const file_info_t *info );

ssize_t FS_FOpenFile( const char *filename, qhandle_t *f, unsigned mode );
void    FS_FCloseFile( qhandle_t f );
qhandle_t FS_EasyOpenFile( char *buf, size_t size, unsigned mode,
    const char *dir, const char *name, const char *ext );

qerror_t FS_FilterFile( qhandle_t f );

#define FS_FileExistsEx( path, flags ) \
    ( FS_LoadFileEx( path, NULL, flags, TAG_FREE ) != Q_ERR_NOENT )
#define FS_FileExists( path ) \
    FS_FileExistsEx( path, 0 )

ssize_t FS_LoadFileEx( const char *path, void **buffer, unsigned flags, memtag_t tag );
// a NULL buffer will just return the file length without loading
// length < 0 indicates error

qerror_t FS_WriteFile( const char *path, const void *data, size_t len );

ssize_t FS_Read( void *buffer, size_t len, qhandle_t f );
ssize_t FS_Write( const void *buffer, size_t len, qhandle_t f );
// properly handles partial reads

ssize_t FS_FPrintf( qhandle_t f, const char *format, ... ) q_printf( 2, 3 );
ssize_t FS_ReadLine( qhandle_t f, char *buffer, size_t size );

void    FS_Flush( qhandle_t f );

ssize_t FS_Tell( qhandle_t f );
qerror_t FS_Seek( qhandle_t f, size_t offset );

ssize_t  FS_GetFileLength( qhandle_t f );

qboolean FS_WildCmp( const char *filter, const char *string );
qboolean FS_ExtCmp( const char *extension, const char *string );

void    **FS_ListFiles( const char *path, const char *filter, unsigned flags, int *count_p );
void    **FS_CopyList( void **list, int count );
file_info_t *FS_CopyInfo( const char *name, size_t size, time_t ctime, time_t mtime );
void    FS_FreeList( void **list );

unsigned FS_HashPath( const char *s, unsigned size );
#if USE_REF
unsigned FS_HashPathLen( const char *s, size_t len, unsigned size );
#endif

#ifdef _WIN32
char *FS_ReplaceSeparators( char *s, int separator );
#endif

int FS_pathcmp( const char *s1, const char *s2 );
int FS_pathcmpn( const char *s1, const char *s2, size_t n );

void FS_File_g( const char *path, const char *ext, unsigned flags, genctx_t *ctx );

extern cvar_t   *fs_game;

extern char     fs_gamedir[];
