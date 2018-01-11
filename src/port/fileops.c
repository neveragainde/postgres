/*-------------------------------------------------------------------------
 *
 * fileops.c
 *	   Portable file operations: Abstraction to use both POSIX and
 *	   CloudABI file semantics.
 *
 * Copyright (c) 2017, PostgreSQL Global Development Group
 *
 * src/port/fileops.c
 *
 *-------------------------------------------------------------------------
 */
#include "c.h"

#include <dirent.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "port/fileops.h"

#ifdef CLOUDABI

fs_directory_t
fs_directory_lookup(const char *path)
{
	int pathlen = 0;

	fs_directory_t *fsd;
	fs_directory_t  ret;

	memset(&ret, 0, sizeof(fs_directory_t));

	for (fsd = fs_directory_index; fsd->type != FS_DIRECTORY_NULL; fsd++) {
		if (
		   strncmp(fsd->path, path, strlen(fsd->path)) == 0 ||
		   strncmp(fsd->typestr, path, strlen(fsd->path)) == 0) {
			ret.type = fsd->type;
			strlcpy(ret.path, path + strlen(fsd->path), MAXPGPATH);
			ret.dirfd = fsd->dirfd;
			return ret;
		}
	}

	return ret;
}

DIR *
fs_opendir(const char *filename)
{
	fs_directory_t fsd;
	fsd = fs_directory_lookup(filename);
	return opendirat(fsd.dirfd, fsd.path);
}

int
fs_open(const char *path, int flags, ...)
{
	/*
	 * In the words of FreeBSD's open(2):
	 *
	 * The flags argument may indicate the file is to be created if it
	 * does not exist (by specifying the O_CREAT flag).  In this case
	 * open() and openat() require an additional argument mode_t mode [...]
	 */

	int ret;
	va_list ap;
	fs_directory_t fsd;

	fsd = fs_directory_lookup(path);

	if ((flags & O_CREAT) == 0)
		/* no mode argument required */
		return openat(fsd.dirfd, fsd.path, flags);

	/* mode argument required */
	va_start(ap, flags);
	ret = openat(fsd.dirfd, fsd.path, flags, (mode_t) va_arg(ap, int));
	va_end(ap);
	return ret;
}

int
fs_stat(const char *path, struct stat *sb, int flags)
{
	fs_directory_t fsd;
	fsd = fs_directory_lookup(path);
	return fstatat(fsd.dirfd, fsd.path, sb, flags);
}

int
fs_unlink(const char *path, int flags)
{
	fs_directory_t fsd;
	fsd = fs_directory_lookup(path);
	return unlinkat(fsd.dirfd, fsd.path, flags);
}

int
fs_mkdir(const char *path, mode_t mode)
{
	fs_directory_t fsd;
	fsd = fs_directory_lookup(path);
	return mkdirat(fsd.dirfd, fsd.path, mode);
}

char *
fs_getcwd(char *buf, size_t size)
{
	char *ret = buf;

	if (size <= 0) {
		errno = EINVAL;
		return NULL;
	}

	/*
	 * If buf is NULL, space is allocated as necessary to store the pathname.
	 * This space may later be free(3)'d.
	 */
	if (ret == NULL) {
		ret = malloc(size);

		if (ret == NULL) {
			errno = ENOMEM;
			return NULL;
		}
	}

	/*
	 * CloudABI as no concept of a working directory, so we'll return
	 * an empty string, which should make the most sense in most
	 * circumstances.
	 */
	strlcpy(ret, "", size);

	return ret;
}

int
fs_access(const char *path, int mode)
{
	fs_directory_t fsd;
	fsd = fs_directory_lookup(path);
	return faccessat(fsd.dirfd, fsd.path, mode, 0);
}

int
fs_rename(const char *from, const char *to)
{
	fs_directory_t fsd_from, fsd_to;
	fsd_from = fs_directory_lookup(from);
	fsd_to =   fs_directory_lookup(to);
	return renameat(fsd_from.dirfd, from, fsd_to.dirfd, to);
}

#endif /* CLOUDABI */

