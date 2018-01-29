#pragma once


/*
* file_name: elf_format.cpp
* author: asanzjx
* date: 2017-12-22~2018-1-17
* description: elf/so file resolve header file
32bit/64bit file
*/



/*
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long int uint64_t;


typedef uint32_t Elf32_Addr/Elf32_Off/Elf32_Word
typedef uint16_t Elf32_Half
typedef int32_t Elf32_Sword
typedef uint64_t Elf32_Lword

typedef uint64_t	Elf64_Addr;
typedef uint16_t	Elf64_Half;
typedef uint64_t	Elf64_Off;

typedef int32_t		Elf64_Sword;
typedef int64_t		Elf64_Sxword;
typedef uint32_t	Elf64_Word;
typedef uint64_t	Elf64_Lword;
typedef uint64_t	Elf64_Xword;
*/

#include <vector>


#define EI_NIDENT	16
#define ELFCLASSNONE	0
#define ELFCLASS32	1
#define ELFCLASS64	2

/* Values for p_type */
#define PT_NULL	0
#define PT_LOAD	1
#define PT_DYNAMIC	2
#define PT_INTERP	3
#define PT_NOTE	4
#define	PT_SHILB	5
#define	PT_PHDR	6
#define	PT_TLS	7
#define	PT_LOOS	0x60000000
#define	PT_SUNW_UNWIND	0x6464e550
#define	PT_GNU_EH_FRAME	0x6474e550

/* Values for p_flags */
#define PF_X	0x1	//exec
#define PF_W	0x2	//writable
#define PF_R	0x4	//readable

/* sh_type */
#define SHT_NULL		0	/* inactive */
#define SHT_PROGBITS		1	/* program defined information */
#define SHT_SYMTAB		2	/* symbol table section */
#define SHT_STRTAB		3	/* string table section */
#define SHT_RELA		4	/* relocation section with addends */
#define SHT_HASH		5	/* symbol hash table section */
#define SHT_DYNAMIC		6	/* dynamic section */ 
#define SHT_NOTE		7	/* note section */
#define SHT_NOBITS		8	/* no space section */
#define SHT_REL			9	/* relocation section - no addends */
#define SHT_SHLIB		10	/* reserved - purpose unknown */
#define SHT_DYNSYM		11	/* dynamic symbol table section */ 
#define SHT_INIT_ARRAY		14	/* Initialization function pointers. */
#define SHT_FINI_ARRAY		15	/* Termination function pointers. */
#define SHT_PREINIT_ARRAY	16	/* Pre-initialization function ptrs. */
#define SHT_GROUP		17	/* Section group. */
#define SHT_SYMTAB_SHNDX	18	/* Section indexes (see SHN_XINDEX). */
#define SHT_LOOS		0x60000000	/* First of OS specific semantics */
#define SHT_GNU_versym		0x6fffffff	/* Symbol version table */

/* values of section flags */
#define SHF_WRITE	0x1
#define SHF_ALLOC	0x2
#define SHF_EXECINSTR	0x4
#define SHF_MASHPROC	0xf0000000

#define ELF32_HEADER_SIZE	sizeof(ELF32_HEADER)
#define ELF64_HEADER_SIZE	sizeof(ELF64_HEADER)
#define ELF32_PROGRAM_HEADER_SIZE	sizeof(ELF32_PROGRAM_HEADER)
#define ELF64_PROGRAM_HEADER_SIZE	sizeof(ELF64_PROGRAM_HEADER)
#define ELF32_SECTION_HEADER_SIZE	sizeof(ELF32_SECTION_HEADER)
#define ELF64_SECTION_HEADER_SIZE	sizeof(ELF64_SECTION_HEADER)




using namespace std;

/*	ELF header	*/
typedef struct {
	unsigned char e_ident[EI_NIDENT];
	unsigned short int e_type;
	unsigned short int e_machine;
	unsigned int e_version;
	unsigned int e_entry;
	unsigned int e_phoff;
	unsigned int e_shoff;
	unsigned int e_flags;
	unsigned short int e_ehsize;
	unsigned short int e_phentsize;
	unsigned short int e_phnum;
	unsigned short int e_shentsize;
	unsigned short int e_shnum;
	unsigned short int e_shstrndx;
}ELF32_HEADER;

typedef struct {
	unsigned char e_ident[EI_NIDENT];
	unsigned short int e_type;
	unsigned short int e_machine;
	unsigned int e_version;
	//unsigned long int e_entry;
	//unsigned long int e_phoff;
	//unsigned long int e_shoff;
	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;
	unsigned int e_flags;
	unsigned short int e_ehsize;
	unsigned short int e_phentsize;
	unsigned short int e_phnum;
	unsigned short int e_shentsize;
	unsigned short int e_shnum;
	unsigned short int e_shstrndx;
}ELF64_HEADER;


/*	section header	*/
typedef struct {
	unsigned int sh_name;
	unsigned int sh_type;
	unsigned int sh_flags;
	unsigned int sh_addr;
	unsigned int sh_offset;
	unsigned int sh_size;
	unsigned int sh_link;
	unsigned int sh_info;
	unsigned int sh_addralign;
	unsigned int sh_entsize;
} ELF32_SECTION_HEADER;

typedef struct {
	unsigned int sh_name;
	unsigned int sh_type;
	//unsigned long int sh_flags;
	//unsigned long int sh_addr;
	//unsigned long int sh_offset;
	//unsigned long int sh_size;
	uint64_t sh_flags;
	uint64_t sh_addr;
	uint64_t sh_offset;
	uint64_t sh_size;
	unsigned int sh_link;
	unsigned int sh_info;
	//unsigned long int sh_addralign;
	//unsigned long int sh_entsize;
	uint64_t sh_addralign;
	uint64_t sh_entsize;
} ELF64_SECTION_HEADER;


/*	program header	*/
typedef struct {
	unsigned int p_type;
	unsigned int p_offset;
	unsigned int p_vaddr;
	unsigned int p_paddr;
	unsigned int p_filesz;
	unsigned int p_memsz;
	unsigned int p_flags;
	unsigned int p_align;
} ELF32_PROGRAM_HEADER;

typedef struct {
	unsigned int p_type;
	unsigned int p_offset;
	//unsigned long int p_vaddr;
	//unsigned long int p_paddr;
	//unsigned long int p_filesz;
	//unsigned long int p_memsz;
	//unsigned long int p_flags;
	//unsigned long int p_align;
	uint64_t p_vaddr;
	uint64_t p_paddr;
	uint64_t p_filesz;
	uint64_t p_memsz;
	uint64_t p_flags;
	uint64_t p_align;
} ELF64_PROGRAM_HEADER;

typedef struct {
	ELF32_HEADER elf_header;
	vector<ELF32_PROGRAM_HEADER> program_header_v;
	//ELF32_PROGRAM_HEADER *program_header;
	vector<ELF32_SECTION_HEADER> section_header_v;
}ELF32_DESC;

typedef struct {
	ELF64_HEADER elf_header;
	vector<ELF64_PROGRAM_HEADER> program_header_v;
	vector<ELF64_SECTION_HEADER> section_header_v;
}ELF64_DESC;


