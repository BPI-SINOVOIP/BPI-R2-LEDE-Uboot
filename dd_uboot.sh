UBOOT=u-boot.bin
DEVICE=/dev/sdb

sudo dd if=${UBOOT}  of=${DEVICE}  bs=1024 seek=320
sync

sudo eject ${DEVICE}
