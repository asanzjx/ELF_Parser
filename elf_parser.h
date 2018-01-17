#pragma once

#include <fstream>
#include <iostream>
#include "elf_format.h"

#define offset_of(s,m)   (size_t)&(((s *)0)->m) 

void elf32_header_parser(ifstream* elf_file, ELF32_DESC *elf_desc);
void elf32_program_header_parser(ifstream* elf_file, ELF32_DESC *elf_desc);
void elf32_section_header_parser(ifstream* elf_file, ELF32_DESC *elf_desc);

void elf64_header_parser(ifstream* elf_file, ELF64_DESC *elf_desc);
void elf64_program_header_parser(ifstream* elf_file, ELF64_DESC *elf_desc);
void elf64_section_header_parser(ifstream* elf_file, ELF64_DESC *elf_desc);
