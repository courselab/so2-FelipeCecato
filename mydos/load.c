#include "load.h"
#include "kaux.h"

struct fs_header_t *getFsHeader() {
  return (struct fs_header_t *)0x7c00;
}

void load_disk(int coordinate_block , int sectors_num, void *target_address) {
  int coordinate_sector = (coordinate_block - 1) % SECTOR_COUNT + 1;
  int coordinate_head = ((coordinate_block - 1) / SECTOR_COUNT) % HEAD_COUNT;
  int coordinate_cylinder = (coordinate_block - 1) / (HEAD_COUNT * SECTOR_COUNT);

  __asm__ volatile(
      "pusha \n"
      "mov boot_drive, %%dl \n"    /* Select the boot drive (from rt0.o). */
      "mov $0x2, %%ah \n"          /* BIOS disk service: op. read sector. */
      "mov %[sectorsToRead], %%al \n" /* How many sectors to read          */
      "mov %[CoordinateCylinder], %%ch \n"          /* Cylinder coordinate (starts at 0).  */
      "mov %[CoordinateSector], %%cl \n"  /* Sector coordinate   (starts at 1).  */
      "mov %[CoordinateHead], %%dh \n"  /* Head coordinate     (starts at 0).      */
      "mov %[targetAddress], %%bx \n" /* Where to load the file system (rt0.o).   */
      "int $0x13 \n"               /* Call BIOS disk service 0x13.        */
      "popa \n" ::
      [CoordinateHead] "g"(coordinate_head),
      [CoordinateSector] "g"(coordinate_sector),
      [CoordinateCylinder] "g"(coordinate_cylinder),
      [sectorsToRead] "g"(sectors_num),
      [targetAddress] "g"(target_address));
}
