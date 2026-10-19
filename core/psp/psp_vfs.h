#ifndef STONEPSP360_PSP_VFS_H
#define STONEPSP360_PSP_VFS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PSP_VFS_MAX_FILES 32u
#define PSP_VFS_PATH_MAX 96u
#define PSP_VFS_FILE_MAX 4096u
#define PSP_VFS_TOTAL_MAX (PSP_VFS_MAX_FILES * PSP_VFS_FILE_MAX)
#define PSP_VFS_SERIALIZED_MAX (8u + PSP_VFS_MAX_FILES * (5u + PSP_VFS_PATH_MAX - 1u + PSP_VFS_FILE_MAX))

typedef struct PspVfsFile {
    char path[PSP_VFS_PATH_MAX];
    uint8_t data[PSP_VFS_FILE_MAX];
    size_t size;
    uint8_t used;
} PspVfsFile;

typedef struct PspVfs {
    PspVfsFile files[PSP_VFS_MAX_FILES];
    size_t file_count;
    size_t bytes_used;
} PspVfs;

void psp_vfs_init(PspVfs *vfs);
bool psp_vfs_write(PspVfs *vfs, const char *path, const void *data, size_t size);
bool psp_vfs_read(const PspVfs *vfs, const char *path, size_t offset,
                  void *buffer, size_t capacity, size_t *read_size);
bool psp_vfs_remove(PspVfs *vfs, const char *path);
bool psp_vfs_exists(const PspVfs *vfs, const char *path);
bool psp_vfs_size(const PspVfs *vfs, const char *path, size_t *size);
bool psp_vfs_path_valid(const char *path);
/* Stable little-endian image format for a caller-provided persistent medium. */
bool psp_vfs_export(const PspVfs *vfs, void *buffer, size_t capacity,
                    size_t *written);
bool psp_vfs_import(PspVfs *vfs, const void *buffer, size_t size);

#endif
