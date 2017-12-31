/*
* file_name: elf_resolve.cpp
* author: asanzjx
* date: 2017-12-22
* description: elf/so file resolve
*/


#include <fstream>
#include <iostream>
#include <bitset>
#include "elf_format.h"


using namespace std;

void header_resolve(ifstream* elf_file) {
	int i = 0;
	int header_size = sizeof(ELF_HEADER);
	ELF_HEADER* elf_header;
	char* header_buffer = new char[header_size];

	cout << "[>>>]	/* ELF Header */	[<<<]" << endl;
	cout << "Reading header size:" << header_size << " byte" << endl;
	cout << "\n****************\n" << endl;

	// read data as a block :
	elf_file->read(header_buffer, header_size);
	elf_header = (ELF_HEADER*)header_buffer;

	//count offset address
	int ident_addr = (int)(elf_header);
	printf("File offset address:0x00000000:\n");

	cout << "e_ident:";
	for (i; i < EI_NIDENT; i++) {
		if (i < 4 || i>6) {
			printf("0x%x ", elf_header->e_ident[i]);
		}
		
		switch (i){
		case 4:
			printf("\n\tCLASS:0x%x\t",elf_header->e_ident[i]);
			switch (elf_header->e_ident[i]) {
			case 0:
				cout << "illegal target file" << endl;
				break;
			case 1:
				cout << "32 bit target file" << endl;
				break;
			case 2:
				cout << "64 bit target file" << endl;
				break;
			default:
				break;
			}
			break;
		case 5:
			printf("\tDATA:0x%x\t", elf_header->e_ident[i]);
			switch (elf_header->e_ident[i]) {
			case 0:
				cout << "illegal code format" << endl;
				break;
			case 1:
				cout << "little endian" << endl;
				break;
			case 2:
				cout << "big endian" << endl;
				break;
			default:
				break;
			}
			break;
		case 6:
			printf("\tVERSION:0x%x\n\t", elf_header->e_ident[i]);
			break;
		default:
			break;
		}
 
	}
	cout <<"\n" << endl;

	int type_addr = (int)&(elf_header->e_type);
	printf("File offset address:0x%0.8x:\n",type_addr - ident_addr);

	cout << "e_type:0x";
	printf("%x\t",elf_header->e_type);
	switch (elf_header->e_type){
	case 0:
		cout << "none";
		break;
	case 1:
		cout << "rel";
		break;
	case 2:
		cout << "exec";
		break;
	case 3:
		cout << "dyn";
		break;
	case 4:
		cout << "core";
		break;
	case 0xff00:
		cout << "LOPROC";
		break;
	case 0xffff:
		cout << "HIPROC";
		break;
	default:
		break;
	}
	cout << "\n"<< endl;

	int machine_addr = (int)&(elf_header->e_machine);
	printf("File offset address:0x%0.8x:\n", machine_addr - ident_addr);

	cout << "e_machine:0x";
	printf("%x\t", elf_header->e_machine);
	switch (elf_header->e_machine)
	{
	case 0:
		cout << "none";
		break;
	case 3:
		cout << "Intel 386";
		break;
	case 7:
		cout << "Intel i860";
		break;
	case 40:
		cout << "ARM";
		break;
	case 62:
		cout << "x86-64";
		break;
	default:
		cout << "Other Machine";
		break;
	}
	cout << "\n" << endl;

	int version_addr = (int)&(elf_header->e_version);
	printf("File offset address:0x%0.8x:\n", version_addr - ident_addr);
	cout << "e_version:0x";
	printf("%x\t", elf_header->e_version);
	if (elf_header->e_version == 0) {
		cout << "illegal version";
	}
	else if(elf_header->e_version == 1){
		cout << "current";
	}
	cout << "\n" << endl;

	int entry_addr = (int)&(elf_header->e_entry);
	printf("File offset address:0x%0.8x:\n", entry_addr - ident_addr);
	cout << "e_entry:0x";
	printf("%x\t", elf_header->e_entry);
	if (elf_header->e_entry == 0) {
		cout << "not exec elf";
	}
	cout << "\n" << endl;

	int phoff_addr = (int)&(elf_header->e_phoff);
	printf("File offset address:0x%0.8x:\n", phoff_addr - ident_addr);
	cout << "e_phoff:0x";
	printf("%x\t", elf_header->e_phoff);
	if (elf_header->e_phoff == 0) {
		cout << "no program header table";
	}
	else {
		cout << "program header table file offset";
	}
	cout << "\n" << endl;

	int shoff_addr = (int)&(elf_header->e_shoff);
	printf("File offset address:0x%0.8x:\n", shoff_addr - ident_addr);
	cout << "e_shoff:0x";
	printf("%x\t", elf_header->e_shoff);
	if (elf_header->e_shoff == 0) {
		cout << "no section header table";
	}
	else {
		cout << "section header table file offset";
	}
	cout << "\n" << endl;

	int flags_addr = (int)&(elf_header->e_flags);
	printf("File offset address:0x%0.8x:\n", flags_addr - ident_addr);
	cout << "e_flags:0x";
	printf("%x\t", elf_header->e_flags);
	if (elf_header->e_flags == 0) {
		cout << "Intel CPU flag";
	}
	else {
		cout << "not Intel CPU";
	}
	cout << "\n" << endl;

	int ehsize_addr = (int)&(elf_header->e_ehsize);
	printf("File offset address:0x%0.8x:\n", ehsize_addr - ident_addr);
	cout << "e_ehsize:0x";
	printf("%x\t", elf_header->e_ehsize);
	cout << "elf file header size";
	cout << "\n" << endl;

	int phentsize_addr = (int)&(elf_header->e_phentsize);
	printf("File offset address:0x%0.8x:\n", phentsize_addr - ident_addr);
	cout << "e_phentsize:0x";
	printf("%x\t", elf_header->e_phentsize);
	cout << "program header table item size";
	cout << "\n" << endl;

	int phnum_addr = (int)&(elf_header->e_phnum);
	printf("File offset address:0x%0.8x:\n", phnum_addr - ident_addr);
	cout << "e_phnum:0x";
	printf("%x\t", elf_header->e_phnum);
	cout << "program header item count\t";
	if (elf_header->e_phnum == 0)	cout << "no program table";
	cout << "\n" << endl;

	int shentsize_addr = (int)&(elf_header->e_shentsize);
	printf("File offset address:0x%0.8x:\n", shentsize_addr - ident_addr);
	cout << "e_shentsize:0x";
	printf("%x\t", elf_header->e_shentsize);
	cout << "section header table item size";
	cout << "\n" << endl;

	int shnum_addr = (int)&(elf_header->e_shnum);
	printf("File offset address:0x%0.8x:\n", shnum_addr - ident_addr);
	cout << "e_shnum:0x";
	printf("%x\t", elf_header->e_shnum);
	cout << "section header item count\t";
	if (elf_header->e_shnum == 0)	cout << "no section table";
	cout << "\n" << endl;

	int shstrndx_addr = (int)&(elf_header->e_shstrndx);
	printf("File offset address:0x%0.8x:\n", shstrndx_addr - ident_addr);
	cout << "e_shstrndx:0x";
	printf("%x\t", elf_header->e_shstrndx);
	cout << "section name table index\t";
	if (elf_header->e_shstrndx == 0)	cout << "no section name table";
	cout << "\n" << endl;

	delete[] header_buffer;
}

void section_header_resolve(ifstream* elf_file) {

}

int main(int argc,char* argv[]) {
	ifstream elf_file;
	int i = 0;
	
	if (argc != 2) {
		cout << "[>>>]	argv error	[<<<]" << endl;
		cout << "[>>>]	Usage:elf_resolve <elf_file>	[<<<]" << endl;
		exit(1);
	}

	cout << "[>>>]	file:"<< argv[1] << "	[<<<]" << endl;
	elf_file.open(argv[1],ios::binary);
	if (elf_file.is_open()) {
		// get file size
		elf_file.seekg(0,ios::end);
		int file_size = (int)elf_file.tellg();	//
		elf_file.seekg(0, ios::beg);

		/* ELF Header */
		header_resolve(&elf_file);

		/* ELF  */

	}
	else {
		cout << "file can't open!" << endl;
	}
	elf_file.close();
}
