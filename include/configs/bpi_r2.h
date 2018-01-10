/*
 * (C) Copyright 2012 Stephen Warren
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>
#include <configs/autoconf.h>
#include <asm/arch/mt_reg_base.h>

//#define DEBUG

#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE


/**********************************************************************************************
 *                                      ARM Cortex A7
 **********************************************************************************************/
/*
 *  Iverson 20140326 :
 *      We can disable dcache for accessing non-cachable address instead of using mmu page table to provide noncachable address.
 */
/* Cache */
// Iverson debug
#define CONFIG_SYS_DCACHE_OFF

/* Machine ID */
#define CONFIG_MACH_TYPE                   7623

/**********************************************************************************************
 *                                          Memory
 **********************************************************************************************/
/* Memory layout */
/* DRAM definition */
/*
 * Iverson 20140521 : We detect ram size automatically.
 *      CONFIG_SYS_SDRAM_SIZE define max uboot size.
 *      The max size that auto detection support is 256MB.
 */
#define CONFIG_NR_DRAM_BANKS		    1
#define CONFIG_SYS_SDRAM_BASE		    0x80000000

/* Code Layout */
#define CONFIG_SYS_TEXT_BASE		    0x81E00000

/* Uboot definition */
#define CONFIG_SYS_UBOOT_BASE		    CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_UBOOT_MAX_SIZE           SZ_2M
#define CONFIG_SYS_INIT_SP_ADDR             (CONFIG_SYS_TEXT_BASE + \
                                             CONFIG_SYS_UBOOT_MAX_SIZE - \
                                             GENERATED_GBL_DATA_SIZE)

#define CONFIG_SYS_MALLOC_LEN               SZ_32M

/* RichOS memory partitions */
#define CONFIG_SYS_DECOMP_ADDR              0x80008000
#define CONFIG_SYS_LOAD_ADDR                0x84000000
#define CONFIG_SYS_IMAGE_HDR_ADDR           CONFIG_SYS_LOAD_ADDR

/* Linux DRAM definition */
#define CONFIG_LOADADDR			    CONFIG_SYS_LOAD_ADDR

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 64 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTM_LEN	            0x4000000

/**********************************************************************************************
 *                                           Board
 **********************************************************************************************/

/* Board */
#define CONFIG_ADD_MTK_HEADER               "y"
#define CONFIG_BOARD_LATE_INIT

/**********************************************************************************************
 *                                          Devices
 **********************************************************************************************/

/********************** Flash *************************/

/*EMMC Configuration*/
/* Parallel Nor Flash */
/* Build error elimination*/
#define CONFIG_SYS_NO_FLASH

#define CONFIG_GENERIC_MMC
#define CONFIG_SUPPORT_EMMC_BOOT
#define FEATURE_MMC_BOOT_MODE

#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_ENV_SIZE                     SZ_16K
#define CONFIG_ENV_OFFSET                   0x100000
#define CONFIG_SYS_MAX_FLASH_SECT           512

#define CONFIG_EMMC_UBOOT_BLOCK             0x200
#define CONFIG_MAX_UBOOT_SIZE               0x200           // 0x200 * 512 = 256KB
#define CONFIG_EMMC_LINUX_BLOCK             0xA00
#define CONFIG_EMMC_RECOVERY_BLOCK          0x10A00

#define ENV_BOOT_WRITE_IMAGE \
    "boot_wr_img=image_check; if test ${img_result} = good; then image_blks " __stringify(CONFIG_SYS_MAX_FLASH_SECT) \
    " ${filesize};mmc device 0;mmc write ${loadaddr} " __stringify(CONFIG_EMMC_LINUX_BLOCK) " ${img_blks}; fi\0"

#define ENV_BOOT_READ_IMAGE \
    "boot_rd_img=mmc device 0;mmc read ${loadaddr} " __stringify(CONFIG_EMMC_LINUX_BLOCK) " 1" \
    ";image_blks " __stringify(CONFIG_SYS_MAX_FLASH_SECT) \
    ";mmc read ${loadaddr} " __stringify(CONFIG_EMMC_LINUX_BLOCK) " ${img_blks}\0"

#define ENV_WRITE_UBOOT \
    "wr_uboot=uboot_check;if test ${uboot_result} = good; then mmc device 0;mmc write ${loadaddr} " __stringify(CONFIG_EMMC_UBOOT_BLOCK) \
    " " __stringify(CONFIG_MAX_UBOOT_SIZE) ";reset; fi\0"

#define ENV_BOOT_WRITE_SEC_IMAGE \
    "boot_wr_sec_img=image_check; if test ${img_result} = good; then image_blks " __stringify(CONFIG_SYS_MAX_FLASH_SECT) \
    " ${filesize};mmc device 0;mmc write ${loadaddr} " __stringify(CONFIG_EMMC_RECOVERY_BLOCK) " ${img_blks}; fi\0"

#define ENV_DUIMAGE_RECOVERY \
    "duimage_recovery=serious_image_check; if test ${img_result} = bad; then run boot_rd_sec_img" \
    ";serious_image_check; if test ${img_result} = good; then reco_message; run boot_wr_img; fi; fi\0"


#define ENV_BOOT_READ_SEC_IMAGE \
    "boot_rd_sec_img=mmc device 0;mmc read ${loadaddr} " __stringify(CONFIG_EMMC_RECOVERY_BLOCK) " 1" \
    ";image_blks " __stringify(CONFIG_SYS_MAX_FLASH_SECT) \
    ";mmc read ${loadaddr} " __stringify(CONFIG_EMMC_RECOVERY_BLOCK) " ${img_blks}\0"

#define ENV_DUIMAGE_BACKUP \
   "duimage_backup=mmc device 0;mmc read ${loadaddr} " __stringify(CONFIG_EMMC_RECOVERY_BLOCK) " 1" \
   ";image_check; if test ${img_result} = bad" \
   ";then run boot_rd_img; serious_image_check; if test ${img_result} = good; then backup_message" \
   ";run boot_wr_sec_img; fi; fi\0"


//#define ENV_BOOT_FROM_SD "boot5=mmc init; fatload mmc 0:1 0x84000000 uimage ; bootm\0"


#define CONFIG_ENV_VARS_UBOOT_CONFIG

/********************** MMC ***********************************/
#define PART_DEBUG
#define CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_MEDIATEK_MMC

#define CONFIG_FS_FAT
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
/********************** GPIO *************************/
//#define CONFIG_MTGPIO

/********************** Watchdog *************************/
#define CONFIG_WATCHDOG_OFF

/********************** Console UART *************************/
/* Uart baudrate */
#define CONFIG_BAUDRATE                     115200

/* Console configuration */
#define CONFIG_SYS_CBSIZE		    1024
#define CONFIG_SYS_PBSIZE		    (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

#define CONFIG_SYS_CONSOLE_IS_IN_ENV

#define ENV_DEVICE_SETTINGS \
	"stdin=serial\0" \
	"stdout=serial\0" \
	"stderr=serial\0"

/********************** Ethernet *************************/
#define CONFIG_ETHADDR                      00:0C:E7:11:22:33
#define CONFIG_IPADDR                       10.10.10.254
#define CONFIG_SERVERIP                     10.10.10.1
#define CONFIG_BOOTFILE                     "uImage"
#define CONFIG_CMD_NET
#define CONFIG_RT2880_ETH

#define RALINK_REG(x)		(*((volatile u32 *)(x)))

/**********************************************************************************************
 *                                       Boot Menu
 **********************************************************************************************/
#define CONFIG_BOOTDELAY                    3
#define CONFIG_BOOTCOMMAND                  "run getbootdevice; mmc dev ${bootdevice}; mmc read ${kerneladdr} 1000 6000; run configBootargs; bootm ${kerneladdr};"

#define ENV_BOOT_CMD0 \
    "boot0=tftpboot; bootm\0"

#define ENV_BOOT_CMD1 \
    "boot1=tftpboot;run boot_wr_img;run boot_rd_img;bootm\0"


#define ENV_BOOT_CMD2 \
    "boot2=run boot_rd_img;run rescue;bootm\0"

#define ENV_BOOT_CMD3 \
    "boot3=tftpboot ${loadaddr} u-boot-mtk.bin;run wr_uboot\0"

#define ENV_BOOT_CMD4 \
    "boot4=loadb;run boot_wr_img;run boot_rd_img;bootm\0"

#define ENV_BOOT_CMD5 \
    "boot5=loadb;run wr_uboot\0"

#define ENV_BOOT_CMD6 \
    "boot6=usb start;run boot_wr_img;bootm\0"

#define ENV_BOOT_CMD7 \
    "boot7=http_upgrade " __stringify(CONFIG_SERVERIP) ";run boot_wr_img;run boot_rd_img;bootm\0"

#define ENV_BOOT_CMD8 \
    "boot8=mmc init; fatload mmc 0:1 ${loadaddr} " __stringify(CONFIG_UPGFILE) ";run boot_wr_img; bootm\0"
#define ENV_BOOT_CMD9 \
    "boot9=mmc init; fatload mmc 0:1 ${loadaddr} ${bpi}/${board}/${service}/${kernel}; bootm\0"
#define ENV_BOOT_CMD10 \
    "boot10=mmc init; run boot_normal; bootm\0"

#define ENV_BOOT_CMD \
    ENV_BOOT_WRITE_IMAGE \
    ENV_BOOT_READ_IMAGE \
    ENV_WRITE_UBOOT \
    ENV_DUIMAGE_RECOVERY \
    ENV_BOOT_READ_SEC_IMAGE \
    ENV_DUIMAGE_BACKUP \
    ENV_BOOT_WRITE_SEC_IMAGE \
    ENV_BOOT_CMD0 \
    ENV_BOOT_CMD1 \
    ENV_BOOT_CMD2 \
    ENV_BOOT_CMD3 \
    ENV_BOOT_CMD4 \
    ENV_BOOT_CMD5 \
    ENV_BOOT_CMD6 \
    ENV_BOOT_CMD7 \
    ENV_BOOT_CMD8 \
    ENV_BOOT_CMD9 \
    ENV_BOOT_CMD10


#define ENV_BOOT_MENU \
    "bpiver=1\0" \
    "bpi=bananapi\0" \
    "board=bpi-r2\0" \
    "chip=MT7623N\0" \
    "service=linux\0" \
    "scriptaddr=0x83000000\0" \
    "kerneladdr=0x84000000\0" \
    "device=mmc\0" \
    "partition=1:1\0" \
    "kernel=uImage\0" \
    "root=/dev/mmcblk0p2\0" \
    "debug=7\0" \
    "bootenv=uEnv.txt\0" \
    "checksd=fatinfo ${device} 1:1\0" \
    "loadbootenv=fatload ${device} ${partition} ${scriptaddr} ${bpi}/${board}/${service}/${bootenv} || fatload ${device} ${partition} ${scriptaddr} ${bootenv}\0" \
    "boot_normal=if run checksd; then echo Boot from SD ; setenv partition 1:1; else echo Boot from eMMC ; mmc init 0 ; setenv partition 0:1 ; fi; if run loadbootenv; then echo Loaded environment from ${bootenv}; env import -t ${scriptaddr} ${filesize}; fi; run uenvcmd; fatload mmc 0:1 ${loadaddr} ${bpi}/${board}/${service}/${kernel}; bootm\0" \
    "bootmenu_delay=30\0" \
    ""

#define ENV_BOOT_GET_DEVICE \
    "getbootdevice=setenv bootdevice 1; if test ${bootemmc} = true; then setenv bootdevice 0; fi;\0"

#define ENV_BOOT_SET_BOOTARGS \
    "configBootargs=run getbootdevice; setenv bootargs earlyprintk block2mtd.block2mtd=/dev/mmcblk${bootdevice},65536,RootFs,5 mtdparts=RootFs:256k(mbr)ro,512k(uboot)ro,256k(config)ro,256k(factory)ro,32M(kernel),32M(recovery),1024M(rootfs),2048M(usrdata),-(bmtpool) rootfstype=squashfs,jffs2\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
	ENV_DEVICE_SETTINGS \
	ENV_BOOT_CMD        \
	ENV_BOOT_MENU       \
	ENV_BOOT_GET_DEVICE \
        ENV_BOOT_SET_BOOTARGS


/**********************************************************************************************
 *                                       UBoot Command
 **********************************************************************************************/
/* Shell */
#define CONFIG_SYS_MAXARGS		            32
#define CONFIG_SYS_PROMPT		            "BPI-R2> "
#define CONFIG_COMMAND_HISTORY

/* Commands */
#include <config_cmd_default.h>

/* Device tree support */
#define CONFIG_OF_LIBFDT	1
#define CONFIG_OF_BOARD_SETUP   1
/* ATAGs support for bootm/bootz */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG  1
#define CONFIG_INITRD_TAG

#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_NFS

#define CONFIG_SYS_LONGHELP
#define CONFIG_CMD_PING

/* Enable Hush Parser */
#define CONFIG_SYS_HUSH_PARSER

/* Compression */
/*
 * Iverson 20150408 :
 *      We use kernel to decompress kernel.
 */
//#define CONFIG_LZMA

#endif
