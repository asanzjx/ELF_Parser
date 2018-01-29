#pragma once

#include <fstream>
#include <iostream>
#include "elf_format_v2.h"

#define offset_of(s,m)   (size_t)&(((s *)0)->m) 



template<typename ELF_DESC,typename ELF_HEADER>ELF_HEADER* elf_header_parser(ifstream* elf_file, ELF_DESC *elf_desc,ELF_HEADER* elf_header);

template<typename ELF_DESC, typename SECTION_HEADER>SECTION_HEADER* section_header_parser(ifstream* elf_file, ELF_DESC *elf_desc, SECTION_HEADER* section_header);

template<typename ELF_DESC, typename PROGRAM_HEADER>PROGRAM_HEADER* program_header_parser(ifstream* elf_file, ELF_DESC *elf_desc, PROGRAM_HEADER* program_header);


/* use template to find section name */
template<typename ELF_DESC>void find_section_name(ifstream* elf_file,ELF_DESC* elf_desc, unsigned int sh_name);