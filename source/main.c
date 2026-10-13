#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <console/console.h>
#include <input/input.h>
#include <usb/usbmain.h>
#include <xenon_soc/xenon_power.h>
#include <xenos/xenos.h>

int main(void)
{
    struct controller_data_s pad;

    xenos_init(VIDEO_MODE_AUTO);
    console_init();
    xenon_make_it_faster(XENON_SPEED_FULL);
    usb_init();
    usb_do_poll();
    console_clrscr();

    puts("StonePSP360 v8.4b");
    puts("Xbox 360 / libXenon boot test");
    puts("");
    puts("This is a platform smoke test only.");
    puts("The PSP emulation core is not integrated yet.");
    puts("Press Guide or Y to exit.");

    while (true) {
        usb_do_poll();

        if (get_controller_data(&pad, 0) && (pad.logo || pad.y)) {
            exit(0);
        }
    }

    return 0;
}
