#    SPDX-FileCopyrightText: 2024 FelipeCecato
#   
#    SPDX-License-Identifier: CC-BY-ND-4.0

# AUTHORS
# ------------------------------

# FelipeCecato <fececato31@gmail.com>

# Attribution
# ------------------------------
# This project is a programming exercise based on SYSeg learning material.
# SYSeg is accessible at: https://gitlab.com/monaco/syseg-template.

#ifndef BIOS1_H
#define BIOS1_H

void __attribute__((fastcall)) clear (void);
void __attribute__((fastcall)) kwrite(const char*);
void __attribute__((fastcall)) kwriteln(const char*);
/* void __attribute__((fastcall)) kread(char *); */
void __attribute__((fastcall)) fatal(const char*);
void __attribute__((fastcall)) load_kernel(void);
void __attribute__((fastcall)) set_cursor(char, char);

#endif
