/*-------------------------------------------------------------------------
 *
 * fileops.h
 *	   Portable file operations: Abstraction to use both POSIX and
 *	   CloudABI file semantics.
 *
 * Copyright (c) 2017, PostgreSQL Global Development Group
 *
 * src/include/port/fileops.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef FILEOPS_H
#define FILEOPS_H

#ifdef CLOUDABI

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

/* cf. src/port/pg_config_paths.h */
typedef enum {
	FS_DIRECTORY_NULL = 0,
	FS_DIRECTORY_PGDATA,
	FS_DIRECTORY_PGBINDIR,
	FS_DIRECTORY_PGSHAREDIR,
	FS_DIRECTORY_SYSCONFDIR,
	FS_DIRECTORY_INCLUDEDIR,
	FS_DIRECTORY_PKGINCLUDEDIR,
	FS_DIRECTORY_INCLUDEDIRSERVER,
	FS_DIRECTORY_LIBDIR,
	FS_DIRECTORY_PKGLIBDIR,
	FS_DIRECTORY_LOCALEDIR,
	FS_DIRECTORY_DOCDIR,
	FS_DIRECTORY_HTMLDIR,
	FS_DIRECTORY_MANDIR,
} fs_directory_type_t;

typedef struct {
	fs_directory_type_t type;	/* directory type, e.g. FS_DIRECTORY_DATA */
	char typestr[20];	/* directory type as string, e.g. PGBINDIR or SYSCONFDIR */
	char path[MAXPGPATH]; /* path, either the absolut data path, or the remainder */
	int dirfd;	/* global directory fd */
} fs_directory_t;

fs_directory_t fs_directory_index[] = {
	{ FS_DIRECTORY_PGDATA, "/pgdata", "PGDATA", -1 },
	{ FS_DIRECTORY_PGBINDIR, "/opt/pg/bin", "PGBINDIR", -1 },
	{ FS_DIRECTORY_SYSCONFDIR, "/opt/pg/etc", "SYSCONFDIR", -1 },
	{ FS_DIRECTORY_INCLUDEDIR, "/opt/pg/include", "INCLUDEDIR", -1 },
	{ FS_DIRECTORY_NULL, "", "", -1 },
};

extern fs_directory_t fs_directory_lookup(const char *path);

extern int fs_open(const char *path, int flags, ...);
#define open(p, ...)    fs_open(p, __VA_ARGS__)

extern DIR * fs_opendir(const char *filename);
#define opendir(f) fs_opendir(f)

extern int fs_stat(const char *path, struct stat *sb, int flags);
#define stat(p, sb)  fs_stat(p, sb, AT_SYMLINK_FOLLOW)
#define lstat(p, sb) fs_stat(p, sb, AT_SYMLINK_NOFOLLOW)

extern int fs_unlink(const char *path, int flags);
#define rmdir(p)  fs_unlink(p, AT_REMOVEDIR)
#define unlink(p) fs_unlink(p, 0)

extern int fs_mkdir(const char *path, mode_t mode);
#define mkdir(p, m)  fs_mkdir(p, m)

extern char *fs_getcwd(char *buf, size_t size);
#define getcwd(b, s) fs_getcwd(b, s)

extern int fs_access(const char *path, int mode);
#define access(p, m) fs_access(p, m)

extern int fs_rename(const char *from, const char *to);
#define rename(f, t) fs_rename(f, t)

#endif /* !CLOUDABI */
#endif /* !FILEOPS_H */

