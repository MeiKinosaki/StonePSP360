#include "psp_vfs.h"

#include <string.h>

bool psp_vfs_path_valid(const char *path)
{
    size_t i, segment_start = 0u, len;
    if (path == NULL) return false;
    len = strlen(path);
    if (len == 0u || len >= PSP_VFS_PATH_MAX || path[0] == '/' ||
        path[0] == '\\' || (len >= 2u && path[1] == ':')) return false;
    for (i = 0; i <= len; ++i) {
        unsigned char c = (unsigned char)path[i];
        if (c == '\\' || c < 0x20u) return false;
        if (c == '/' || c == '\0') {
            size_t segment_len = i - segment_start;
            if (segment_len == 0u ||
                (segment_len == 1u && path[segment_start] == '.') ||
                (segment_len == 2u && path[segment_start] == '.' &&
                 path[segment_start + 1u] == '.')) return false;
            segment_start = i + 1u;
        }
    }
    return true;
}

void psp_vfs_init(PspVfs *vfs)
{
    if (vfs != NULL) memset(vfs, 0, sizeof(*vfs));
}

static PspVfsFile *find_file(PspVfs *vfs, const char *path)
{
    size_t i;
    for (i = 0; i < PSP_VFS_MAX_FILES; ++i)
        if (vfs->files[i].used && strcmp(vfs->files[i].path, path) == 0)
            return &vfs->files[i];
    return NULL;
}

static const PspVfsFile *find_file_const(const PspVfs *vfs, const char *path)
{
    size_t i;
    for (i = 0; i < PSP_VFS_MAX_FILES; ++i)
        if (vfs->files[i].used && strcmp(vfs->files[i].path, path) == 0)
            return &vfs->files[i];
    return NULL;
}

bool psp_vfs_write(PspVfs *vfs, const char *path, const void *data, size_t size)
{
    PspVfsFile *file;
    size_t i;
    if (vfs == NULL || !psp_vfs_path_valid(path) || size > PSP_VFS_FILE_MAX ||
        (size != 0u && data == NULL)) return false;
    file = find_file(vfs, path);
    if (file == NULL) {
        if (vfs->file_count >= PSP_VFS_MAX_FILES) return false;
        for (i = 0; i < PSP_VFS_MAX_FILES && vfs->files[i].used; ++i) { }
        if (i == PSP_VFS_MAX_FILES) return false;
        file = &vfs->files[i];
        memset(file, 0, sizeof(*file));
        memcpy(file->path, path, strlen(path) + 1u);
        file->used = 1u;
        ++vfs->file_count;
    }
    vfs->bytes_used -= file->size;
    if (size != 0u) memcpy(file->data, data, size);
    if (size < file->size) memset(file->data + size, 0, file->size - size);
    file->size = size;
    vfs->bytes_used += size;
    return true;
}

bool psp_vfs_read(const PspVfs *vfs, const char *path, size_t offset,
                  void *buffer, size_t capacity, size_t *read_size)
{
    const PspVfsFile *file;
    size_t amount;
    if (read_size != NULL) *read_size = 0u;
    if (vfs == NULL || !psp_vfs_path_valid(path) ||
        (capacity != 0u && buffer == NULL)) return false;
    file = find_file_const(vfs, path);
    if (file == NULL || offset > file->size) return false;
    amount = file->size - offset;
    if (amount > capacity) amount = capacity;
    if (amount != 0u) memcpy(buffer, file->data + offset, amount);
    if (read_size != NULL) *read_size = amount;
    return true;
}

bool psp_vfs_remove(PspVfs *vfs, const char *path)
{
    PspVfsFile *file;
    if (vfs == NULL || !psp_vfs_path_valid(path)) return false;
    file = find_file(vfs, path);
    if (file == NULL) return false;
    vfs->bytes_used -= file->size;
    --vfs->file_count;
    memset(file, 0, sizeof(*file));
    return true;
}

bool psp_vfs_exists(const PspVfs *vfs, const char *path)
{
    if (vfs == NULL || !psp_vfs_path_valid(path)) return false;
    return find_file_const(vfs, path) != NULL;
}

bool psp_vfs_size(const PspVfs *vfs, const char *path, size_t *size)
{
    const PspVfsFile *file;
    if (vfs == NULL || size == NULL || !psp_vfs_path_valid(path)) return false;
    file = find_file_const(vfs, path);
    if (file == NULL) return false;
    *size = file->size;
    return true;
}
