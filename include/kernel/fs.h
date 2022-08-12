#pragma once
#include <stddef.h>

//// fs/fs.c
typedef struct {
    char* address;
    short fstype;
    size_t size;
    size_t pos;
} file_handle;

//// fs/tar.c
/* Typeflag field definitions */
#define REGTYPE 	'0'	/* Regular file. */
#define LNKTYPE		'1'	/* Link. */
#define SYMTYPE		'2'	/* Symbolic link. */
#define CHRTYPE		'3'	/* Character special. */
#define BLKTYPE		'4'	/* Block special. */
#define DIRTYPE		'5'	/* Directory. */
#define FIFOTYPE	'6'	/* FIFO special. */
#define CONTTYPE	'7'	/* Reserved. */
/* Mode field bit definitions (octal) */
#define	TSUID		04000	/* Set UID on execution. */
#define	TSGID		02000	/* Set GID on execution. */
#define	TSVTX		01000	/* On directories, restricted deletion flag. */
#define	TUREAD		00400	/* Read by owner. */
#define	TUWRITE		00200	/* Write by owner. */
#define	TUEXEC		00100	/* Execute/search by owner. */
#define	TGREAD		00040	/* Read by group. */
#define	TGWRITE		00020	/* Write by group. */
#define	TGEXEC		00010	/* Execute/search by group. */
#define	TOREAD		00004	/* Read by other. */
#define	TOWRITE		00002	/* Write by other. */
#define	TOEXEC		00001	/* Execute/search by other. */

typedef struct
{
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
} tar_header;

typedef struct tar_header_list
{
    tar_header* header;
    struct tar_header_list* next;
} tar_header_list;

void init_tar();
file_handle* open_tar(char* path, char mode);
size_t read_tar(file_handle* fd, char* str, size_t len);
size_t write_tar(file_handle* fd, char* str, size_t len);
void close_tar(file_handle* fd);

//// fs/fifo.c

typedef struct fifo {
    size_t size;
    struct fifo* next;
} fifo;

typedef struct {
    char* name;
    fifo* data;
} fifo_header;

typedef struct fifo_header_list
{
    fifo_header data;
    struct fifo_header_list* next;
    struct fifo_header_list* prev;
} fifo_header_list;

void init_fifo();
file_handle* open_fifo(char* path, char mode);
size_t read_fifo(file_handle* fd, char* str, size_t len);
size_t write_fifo(file_handle* fd, char* str, size_t len);
void close_fifo(file_handle* fd);
void create_fifo(char* name);
void destroy_fifo(char* name);