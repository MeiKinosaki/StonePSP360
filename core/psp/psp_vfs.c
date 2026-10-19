#include "psp_vfs.h"

#include <string.h>

#define VFS_IMAGE_MAGIC 0x31534650u /* "PFS1" in little-endian bytes */
#define VFS_IMAGE_VERSION 1u
#define VFS_IMAGE_HEADER_SIZE 8u

bool psp_vfs_path_valid(const char *path)
{
    size_t i, segment_start = 0u, len;
    if (path == NULL) return false;
    len = strlen(path);
    if (len == 0u || len >= PSP_VFS_PATH_MAX || path[0] == '/' ||
        path[0] == '\\' || (len >= 2u && path[1] == ':')) return false;
    for (i = 0; i <= len; ++i) {
        unsigned char c = (unsigned char)path[i];
        if (c == '\\' || (c < 0x20u && c != '\0')) return false;
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

static void put_u32le(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
    p[2] = (uint8_t)(value >> 16);
    p[3] = (uint8_t)(value >> 24);
}

static uint32_t get_u32le(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

bool psp_vfs_export(const PspVfs *vfs, void *buffer, size_t capacity,
                    size_t *written)
{
    uint8_t *out = (uint8_t *)buffer;
    size_t i, pos = VFS_IMAGE_HEADER_SIZE, count = 0u;
    if (written != NULL) *written = 0u;
    if (vfs == NULL || buffer == NULL || capacity < VFS_IMAGE_HEADER_SIZE ||
        vfs->file_count > PSP_VFS_MAX_FILES) return false;
    for (i = 0; i < PSP_VFS_MAX_FILES; ++i) {
        const PspVfsFile *f = &vfs->files[i];
        size_t path_len;
        if (!f->used) continue;
        if (!psp_vfs_path_valid(f->path) || f->size > PSP_VFS_FILE_MAX)
            return false;
        path_len = strlen(f->path);
        if (pos > capacity || capacity - pos < 5u + path_len ||
            capacity - pos - 5u - path_len < f->size) return false;
        out[pos++] = (uint8_t)path_len;
        put_u32le(out + pos, (uint32_t)f->size);
        pos += 4u;
        memcpy(out + pos, f->path, path_len);
        pos += path_len;
        if (f->size != 0u) memcpy(out + pos, f->data, f->size);
        pos += f->size;
        ++count;
    }
    if (count != vfs->file_count) return false;
    put_u32le(out, VFS_IMAGE_MAGIC);
    out[4] = (uint8_t)VFS_IMAGE_VERSION;
    out[5] = (uint8_t)count;
    out[6] = out[7] = 0u;
    if (written != NULL) *written = pos;
    return true;
}

bool psp_vfs_import(PspVfs *vfs, const void *buffer, size_t size)
{
    const uint8_t *in = (const uint8_t *)buffer;
    char paths[PSP_VFS_MAX_FILES][PSP_VFS_PATH_MAX];
    size_t sizes[PSP_VFS_MAX_FILES];
    size_t path_lengths[PSP_VFS_MAX_FILES];
    size_t i, pos = VFS_IMAGE_HEADER_SIZE, count;
    if (vfs == NULL || buffer == NULL || size < VFS_IMAGE_HEADER_SIZE ||
        get_u32le(in) != VFS_IMAGE_MAGIC || in[4] != VFS_IMAGE_VERSION ||
        in[6] != 0u || in[7] != 0u || in[5] > PSP_VFS_MAX_FILES)
        return false;
    count = in[5];
    for (i = 0; i < count; ++i) {
        size_t j, path_len, file_size;
        if (pos > size || size - pos < 5u) return false;
        path_len = in[pos++];
        file_size = get_u32le(in + pos);
        pos += 4u;
        if (path_len == 0u || path_len >= PSP_VFS_PATH_MAX ||
            file_size > PSP_VFS_FILE_MAX || pos > size ||
            size - pos < path_len || size - pos - path_len < file_size)
            return false;
        memcpy(paths[i], in + pos, path_len);
        paths[i][path_len] = '\0';
        pos += path_len;
        if (!psp_vfs_path_valid(paths[i])) return false;
        for (j = 0; j < i; ++j)
            if (strcmp(paths[j], paths[i]) == 0) return false;
        path_lengths[i] = path_len;
        sizes[i] = file_size;
        pos += file_size;
    }
    if (pos != size) return false;
    psp_vfs_init(vfs);
    pos = VFS_IMAGE_HEADER_SIZE;
    for (i = 0; i < count; ++i) {
        size_t path_len = path_lengths[i], file_size = sizes[i];
        pos += 5u + path_len;
        if (!psp_vfs_write(vfs, paths[i], in + pos, file_size)) {
            psp_vfs_init(vfs);
            return false;
        }
        pos += file_size;
    }
    return true;
}
