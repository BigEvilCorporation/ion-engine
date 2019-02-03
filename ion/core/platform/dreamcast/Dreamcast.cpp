///////////////////////////////////////////////////
// File:		Platform.h
// Date:		26th January 2017
// Authors:		Matt Phillips
// Description:	Platform includes, types, macros
///////////////////////////////////////////////////

#include "Dreamcast.h"

#include <ion/core/debug/Debug.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <arch/timer.h>
#include <arch/arch.h>
#include <arch/irq.h>
#include <arch/rtc.h>
#include <kos/fs.h>
#include <kos/thread.h>
#include <kos/fs_pty.h>
#include <kos/fs_romdisk.h>
#include <kos/fs_ramdisk.h>
#include <kos/library.h>
#include <kos/net.h>
#include <kos/dbgio.h>
#include <dc/fs_iso9660.h>
#include <dc/fs_vmu.h>
#include <dc/fs_dcload.h>
#include <dc/fs_dclsocket.h>
#include <dc/spu.h>
#include <dc/pvr.h>
#include <dc/maple.h>
#include <dc/sound/sound.h>
#include <dc/scif.h>
#include <dc/ubc.h>
#include <dc/vmufs.h>
#include <arch/irq.h>
#include <arch/timer.h>
#include <dc/fb_console.h>

KOS_INIT_FLAGS(INIT_DEFAULT);
KOS_INIT_ROMDISK(KOS_INIT_ROMDISK_NONE);

#if 1 // defined DEBUG
#define KOS_DEBUG(msg) dbglog(DBG_INFO, msg);
#else
#define KOS_DEBUG(msg)
#endif

#define KOS_GDB_SERVER 0

namespace ion
{
	namespace platform
	{
		void Initialise()
		{
			debug::Log("ion::engine for SEGA Dreamcast");
		}

		void Shutdown()
		{

		}
	}
}

extern "C"
{
	void init(void);
	void fini(void);
	void __verify_newlib_patch();
	uint32 _fs_dclsocket_get_ip(void);

	int arch_auto_init()
	{
		union {
			uint32 ipl;
			uint8 ipb[4];
		} ip;

		//Initialize memory management
		mm_init();

		//Do this immediately so we can receive exceptions for init code
		//and use ints for dbgio receive.
		irq_init();		//IRQs
		irq_disable();	//Turn on exceptions

		//Init dc-load console, if applicable
		if (!(__kos_init_flags & INIT_NO_DCLOAD))
		{
			fs_dcload_init_console();
		}

		//Init SCIF for debug stuff
		scif_init();

		//Init console output
		dbgio_init();
		dbgio_enable();
		dbglog_set_level(DBG_KDEBUG);

		//Init timers
		KOS_DEBUG("KOS: timer_init()\n");
		timer_init();

		//DC low-level hardware init
		KOS_DEBUG("KOS: hardware_sys_init()\n");
		hardware_sys_init();

		//Initialize KOS timer
		KOS_DEBUG("KOS: timer_ms_enable()\n");
		timer_ms_enable();

		//Initialise RTC
		KOS_DEBUG("KOS: rtc_init()\n");
		rtc_init();

		// Initialise threads
		KOS_DEBUG("KOS: thd_init()\n");
		if (__kos_init_flags & INIT_THD_PREEMPT)
			thd_init(THD_MODE_PREEMPT);
		else
			thd_init(THD_MODE_COOP);

		KOS_DEBUG("KOS: nmmgr_init()\n");
		nmmgr_init();

		//Initialise filesystem
		KOS_DEBUG("KOS: fs_init()\n");
		fs_init();

		KOS_DEBUG("KOS: fs_pty_init()\n");
		fs_pty_init();

		//Ramdisk
		KOS_DEBUG("KOS: fs_ramdisk_init()\n");
		fs_ramdisk_init();

		//Romdisk
		KOS_DEBUG("KOS: fs_romdisk_init()\n");
		fs_romdisk_init();

		//DC peripherals
		KOS_DEBUG("KOS: hardware_periph_init()\n");
		hardware_periph_init();

		//Romdisk mount
		if (__kos_romdisk != NULL)
		{
			KOS_DEBUG("KOS: fs_romdisk_mount()\n");
			fs_romdisk_mount("/rd", (const uint8*)__kos_romdisk, 0);
		}

		//DCLoad console support
		if (!(__kos_init_flags & INIT_NO_DCLOAD) && *DCLOADMAGICADDR == DCLOADMAGICVALUE)
		{
			KOS_DEBUG("KOS: dc-load console support enabled\n");
			fs_dcload_init();
		}

		KOS_DEBUG("fs_iso9660_init()\n");
		fs_iso9660_init();

		KOS_DEBUG("KOS: vmufs_init()\n");
		vmufs_init();

		KOS_DEBUG("KOS: fs_vmu_init()\n");
		fs_vmu_init();

		// Initialize library handling
		KOS_DEBUG("KOS: library_init()\n");
		library_init();

		//Turn on IRQs
		if (__kos_init_flags & INIT_IRQ)
		{
			irq_enable();
			KOS_DEBUG("KOS: maple_wait_scan()\n");
			maple_wait_scan();  /* Wait for the maple scan to complete */
		}

		if (__kos_init_flags & INIT_NET)
		{
			ip.ipl = 0;

			//Check if the dcload-ip console is up, and if so, disable it,
			//otherwise we'll crash when we attempt to bring up the BBA
			if (!(__kos_init_flags & INIT_NO_DCLOAD) && dcload_type == DCLOAD_TYPE_IP)
			{
				//Grab the IP address from dcload before we disable dbgio
				ip.ipl = _fs_dclsocket_get_ip();
				dbglog(DBG_INFO, "KOS: dc-load IP address: %d.%d.%d.%d\n", ip.ipb[3], ip.ipb[2], ip.ipb[1], ip.ipb[0]);
				dbgio_disable();
			}

			//Enable networking (and drivers)
			KOS_DEBUG("KOS: net_init()\n");
			net_init(ip.ipl);

			if (!(__kos_init_flags & INIT_NO_DCLOAD) && dcload_type == DCLOAD_TYPE_IP)
			{
				KOS_DEBUG("KOS: fs_dclsocket_init_console()\n");
				fs_dclsocket_init_console();

				if (!fs_dclsocket_init())
				{
					dbgio_dev_select("fs_dclsocket");
					dbgio_enable();
					dbglog(DBG_INFO, "KOS: fs_dclsocket console support enabled\n");
				}
			}
		}

#if KOS_GDB_SERVER
		KOS_DEBUG("KOS: Starting GDB server\n");
		gdb_init();
#endif

		KOS_DEBUG("KOS: arch_auto_init() completed\n");

		return 0;
	}

	void arch_auto_shutdown()
	{
		fs_dclsocket_shutdown();
		net_shutdown();

		irq_disable();
		snd_shutdown();
		timer_shutdown();
		hardware_shutdown();
		pvr_shutdown();
		library_shutdown();
		fs_dcload_shutdown();
		fs_vmu_shutdown();
		vmufs_shutdown();
		fs_iso9660_shutdown();
		fs_ramdisk_shutdown();
		fs_romdisk_shutdown();
		fs_pty_shutdown();
		fs_shutdown();
		thd_shutdown();
		rtc_shutdown();
	}
}
