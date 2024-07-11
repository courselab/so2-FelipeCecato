/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.
 */

/* This source file implements the kernel entry function 'kmain' called
   by the bootloader, and the command-line interpreter. Other kernel functions
   were implemented separately in another source file for legibility. */

#include "bios1.h"		/* For kwrite() etc.            */
#include "bios2.h"		/* For kread() etc.             */
#include "kernel.h"		/* Essential kernel functions.  */
#include "kaux.h"		/* Auxiliary kernel functions.  */
#include "load.h"

#define FS_SIGNATURE "\xeb\xety"
#define FS_SIGLEN 4  

/* Kernel's entry function. */

void kmain(void)
{
  int i, j;
  
  register_syscall_handler();	/* Register syscall handler at int 0x21.*/

  splash();			/* Uncessary spash screen.              */

  shell();			/* Invoke the command-line interpreter. */
  
  halt();			/* On exit, halt.                       */
  
}

/* Tiny Shell (command-line interpreter). */

char buffer[BUFF_SIZE];
int go_on = 1;

void shell()
{
  int i;
  clear();
  kwrite ("EPIEF DOS 1.0\n");

  while (go_on)
    {

      /* Read the user input. 
	 Commands are single-word ASCII tokens with no blanks. */
      do
	{
	  kwrite(PROMPT);
	  kread (buffer);
	}
      while (!buffer[0]);

      /* Check for matching built-in commands */
      
      i=0;
      while (cmds[i].funct)
	{
	  if (!strcmp(buffer, cmds[i].name))
	    {
	      cmds[i].funct();
	      break;
	    }
	  i++;
	}

      /* If the user input does not match any built-in command name, just
	 ignore and read the next command. If we were to execute external
	 programs, on the other hand, this is where we would search for a 
	 corresponding file with a matching name in the storage device, 
	 load it and transfer it the execution. Left as exercise. */
      
      if (!cmds[i].funct)
	kwrite ("Command not found\n");
    }
}


/* Array with built-in command names and respective function pointers. 
   Function prototypes are in kernel.h. */

struct cmd_t cmds[] =
  {
    {"help",    f_help},     /* Print a help message.       */
    {"quit",    f_quit},     /* Exit TyDOS.                 */
    {"exec",    f_exec},     /* Execute an example program. */
    {"list",    f_list},     /*List */
    {0, 0} 
  };


/* Build-in shell command: help. */

void f_help()
{
  kwrite ("   You can try also some commands:\n");
  kwrite ("      exec    (to execute an user program example\n");
  kwrite ("      quit    (to exit TyDOS)\n");
  kwrite ("      list    (list local files");
}

void f_quit()
{
  kwrite ("The end is near. Bye.");
  go_on = 0;
}

/* Built-in shell command: example.

   Execute an example user program which invokes a syscall.

   The example program (built from the source 'prog.c') is statically linked
   to the kernel by the linker script (tydos.ld). In order to extend the
   example, and load and external C program, edit 'f_exec' and 'prog.c' choosing
   a different name for the entry function, such that it does not conflict with
   the 'main' function of the external program.  Even better: remove 'f_exec'
   entirely, and suppress the 'example_program' section from the tydos.ld, and
   edit the Makefile not to include 'prog.o' and 'libtydos.o' from 'tydos.bin'.

  */
void f_list() {
    int sectors_num, coordinate_init_sector;
    struct fs_header_t *fs_header = getFsHeader();

    //Entrys on load_disk function
    sectors_num = (fs_header->number_of_file_entries * DIR_ENTRY_LEN ) / SECTOR_SIZE; //Number of sectors to read
    coordinate_init_sector = 1 + fs_header->number_of_boot_sectors;
    
    extern char _MEM_POOL;
    void *directory_pos = (void *)&_MEM_POOL;

    // Load disk into memory to list all the files
    load_disk(coordinate_init_sector, sectors_num, directory_pos);

    // List files
    for (int i = 0; i < fs_header->number_of_file_entries; i++) {
        char *file = directory_pos + i * FILE_LEN;
        if (file[0]) {
            kwrite(" >> ");
            kwrite(file);
            kwrite("\n");

        }
    }
}

//Function out of order
void f_exec()
{
    char buffer[BUFF_SIZE];
    int coordinate_h, sectors_num, offset, coordinate_bin, i;
    struct fs_header_t *fs_header;
    kwrite("Select the command: ");
    kread(buffer);
    
    fs_header = getFsHeader();
    coordinate_h = 1 + fs_header->number_of_boot_sectors;
    sectors_num = fs_header->number_of_file_entries * FILE_LEN / SECTOR_SIZE + 1;
    offset = fs_header->number_of_file_entries* FILE_LEN - ( sectors_num - 1) * 512;
  
    extern char _MEM_POOL;
    void *dir = (void *)&_MEM_POOL;
    
    load_disk(coordinate_h, sectors_num , dir);
    
    for (i = 0; i < fs_header->number_of_file_entries; i++) {
      char *file = dir + i * FILE_LEN;
        if (!strcmp(file, buffer)) {
          coordinate_bin =   coordinate_h +   sectors_num + fs_header->max_file_size * i - 1;
          break;
        }
     } 
     if (i == fs_header->number_of_file_entries){
        kwrite("File not found\n");
        return;
     }
        

}

