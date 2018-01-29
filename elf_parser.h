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

template<typename ELF_DESC,typename ELF_HEADER>ELF_HEADER* elf_header_parser(ifstream* elf_file, ELF_DESC *elf_desc,ELF_HEADER* elf_header);

template<typename ELF_DESC, typename SECTION_HEADER>SECTION_HEADER* section_header_parser(ifstream* elf_file, ELF_DESC *elf_desc, SECTION_HEADER* section_header);

template<typename ELF_DESC, typename PROGRAM_HEADER>PROGRAM_HEADER* program_header_parser(ifstream* elf_file, ELF_DESC *elf_desc, PROGRAM_HEADER* program_header);


void find_section_header_name(ifstream* elf_file,ELF32_DESC* elf_desc,unsigned int sh_name);


/* use template to find section name */
template<typename ELF_DESC>void find_section_name(ifstream* elf_file,ELF_DESC* elf_desc, unsigned int sh_name);