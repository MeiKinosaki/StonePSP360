#include "../core/psp/psp_vfs.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    PspVfs vfs, restored;
    const char initial[] = "PSP save data";
    const char replacement[] = "OK";
    char output[32] = { 0 };
    uint8_t image[PSP_VFS_SERIALIZED_MAX];
    size_t n = 0u, size = 0u, image_size = 0u;

    psp_vfs_init(&vfs);
    assert(psp_vfs_path_valid("PSP/SAVEDATA/slot0.dat"));
    assert(!psp_vfs_path_valid("../escape"));
    assert(!psp_vfs_path_valid("PSP/../escape"));
    assert(!psp_vfs_path_valid("/absolute"));
    assert(!psp_vfs_path_valid("C:/escape"));
    assert(!psp_vfs_path_valid("PSP\\escape"));

    assert(psp_vfs_write(&vfs, "PSP/SAVEDATA/slot0.dat", initial,
                         sizeof(initial)));
    assert(vfs.file_count == 1u && vfs.bytes_used == sizeof(initial));
    assert(psp_vfs_exists(&vfs, "PSP/SAVEDATA/slot0.dat"));
    assert(psp_vfs_read(&vfs, "PSP/SAVEDATA/slot0.dat", 4u,
                        output, 4u, &n));
    assert(n == 4u && memcmp(output, "save", 4u) == 0);
    assert(psp_vfs_size(&vfs, "PSP/SAVEDATA/slot0.dat", &size));
    assert(size == sizeof(initial));

    assert(psp_vfs_export(&vfs, image, sizeof(image), &image_size));
    psp_vfs_init(&restored);
    assert(psp_vfs_import(&restored, image, image_size));
    assert(psp_vfs_exists(&restored, "PSP/SAVEDATA/slot0.dat"));
    memset(output, 0, sizeof(output));
    assert(psp_vfs_read(&restored, "PSP/SAVEDATA/slot0.dat", 0u,
                        output, sizeof(output), &n));
    assert(n == sizeof(initial) && memcmp(output, initial, n) == 0);

    assert(psp_vfs_write(&vfs, "PSP/SAVEDATA/slot0.dat", replacement,
                         sizeof(replacement)));
    assert(vfs.file_count == 1u && vfs.bytes_used == sizeof(replacement));
    memset(output, 0, sizeof(output));
    assert(psp_vfs_read(&vfs, "PSP/SAVEDATA/slot0.dat", 0u,
                        output, sizeof(output), &n));
    assert(n == sizeof(replacement) && memcmp(output, replacement, n) == 0);
    assert(!psp_vfs_read(&vfs, "missing.dat", 0u, output, sizeof(output), &n));
    assert(!psp_vfs_write(&vfs, "too-big", output, PSP_VFS_FILE_MAX + 1u));
    assert(!psp_vfs_export(&vfs, image, 2u, &image_size));
    image[0] = 0u;
    assert(!psp_vfs_import(&restored, image, image_size));
    assert(psp_vfs_remove(&vfs, "PSP/SAVEDATA/slot0.dat"));
    assert(vfs.file_count == 0u && vfs.bytes_used == 0u);
    assert(!psp_vfs_exists(&vfs, "PSP/SAVEDATA/slot0.dat"));

    puts("PSP virtual filesystem tests passed");
    return 0;
}
