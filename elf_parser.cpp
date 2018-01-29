/*
* file_name: elf_resolve.cpp
* author: asanzjx
* date: 2017-12-22~2018-1-17
* description: elf/so file resolve
*/


#include "elf_parser.h"


int main(int argc, char* argv[]) {
	ifstream elf_file;
	char ei_ident[EI_NIDENT] = {0};
	int ei_class;

	if (argc != 2) {
		cout << "[>>>]	argv error	[<<<]" << endl;
		cout << "[>>>]	Usage:elf_resolve <elf_file>	[<<<]" << endl;
		return -1;
	}

	cout << "[>>>]	file:" << argv[1] << "	[<<<]" << endl;

	elf_file.open(argv[1], ios::binary);
	if (elf_file.is_open()) {
		// get file size
		elf_file.seekg(0, ios::end);
		int file_size = (int)elf_file.tellg();	//
		elf_file.seekg(0, ios::beg);

		//1.check file
		elf_file.read(ei_ident, EI_NIDENT);
		if (ei_ident[0] != 0x7f || ei_ident[1] != 0x45 || ei_ident[2] != 0x4c || ei_ident[3] != 0x46) {
			cout << "[>>>]	 Not a elf/so file  [<<<]" << endl;
			elf_file.close();
			return -1;
		}
		//2.get class
		ei_class = ei_ident[4];

		// reset file point
		elf_file.seekg(0, ios::beg);

		if (ei_class == ELFCLASS32) {
			ELF32_DESC elf_desc;
			/* ELF32 header */
			elf32_header_parser(&elf_file, &elf_desc);
			/* ELF32 program header*/
			elf32_program_header_parser(&elf_file, &elf_desc);
			/* ELF32 section header */
			elf32_section_header_parser(&elf_file, &elf_desc);

			//find_section_header_name(&elf_file,&elf_desc, 9);

		}
		else if (ei_class == ELFCLASS64) {
			ELF64_DESC elf_desc;
			/* ELF64 header */
			elf64_header_parser(&elf_file, &elf_desc);
			/* ELF64 program header*/
			elf64_program_header_parser(&elf_file, &elf_desc);
			/* ELF64 section header */
			elf64_section_header_parser(&elf_file, &elf_desc);
		}
		else {
			cout << "not know file class" << endl;
			elf_file.close();
			return -1;
		}
	}else cout << "file can't open!" << endl;
	elf_file.close();

	printf("hehe");
	return 0;
}


/*
count section header name:
strnametable_addr = e_shstrndx*e_shentsize+e_shoff
strnametable_offset+sh_name
*/
void find_section_header_name(ifstream* elf_file,ELF32_DESC* elf_desc,unsigned int sh_name) {
	char section_name[32] = {0};
	unsigned int strnametable_addr = 0;
	unsigned long long file_position = elf_file->tellg();
	
	unsigned int strnametable_offset = elf_desc->elf_header.e_shentsize*elf_desc->elf_header.e_shstrndx + elf_desc->elf_header.e_shoff+offset_of(ELF32_SECTION_HEADER,sh_offset);

	elf_file->seekg(strnametable_offset, ios::beg);
	elf_file->read((char*)(&strnametable_addr), sizeof(unsigned int));
	
	// printf("\nstrnametable addr:%x\n", strnametable_addr);

	elf_file->seekg(strnametable_addr+sh_name, ios::beg);

	elf_file->read(section_name, 30);
	elf_file->seekg(file_position, ios::beg);

	printf("\tsection name:%s\n", section_name);
}


template<typename ELF_DESC>void find_section_name(ifstream* elf_file, ELF_DESC* elf_desc, unsigned int sh_name) {
	char section_name[32] = { 0 };
	unsigned int strnametable_addr = 0;
	unsigned long long file_position = elf_file->tellg();
	unsigned long long sh_offset = 0;

	if (typeid(elf_desc) == typeid(ELF32_DESC*)) {
		sh_offset = offset_of(ELF32_SECTION_HEADER, sh_offset);
	}
	else if(typeid(elf_desc) == typeid(ELF64_DESC*)) {
		sh_offset = offset_of(ELF64_SECTION_HEADER, sh_offset);
	}
	else {
		cout << "sh_offset error" << endl;
		return;
	}

	unsigned long long strnametable_offset = elf_desc->elf_header.e_shentsize*elf_desc->elf_header.e_shstrndx + elf_desc->elf_header.e_shoff + sh_offset;

	elf_file->seekg(strnametable_offset, ios::beg);
	elf_file->read((char*)(&strnametable_addr), sizeof(unsigned int));

	// printf("\nstrnametable addr:%x\n", strnametable_addr);

	elf_file->seekg(strnametable_addr + sh_name, ios::beg);

	elf_file->read(section_name, 30);
	elf_file->seekg(file_position, ios::beg);

	printf("\tsection name:%s\n", section_name);
}


void elf32_header_parser(ifstream* elf_file, ELF32_DESC *elf_desc) {
	int i = 0;
	//char elf_header_buffer[ELF32_HEADER_SIZE] = { 0 };
	ELF32_HEADER* elf_header_buffer = &elf_desc->elf_header;


	cout << "[>>>]	/* ELF Header */	[<<<]" << endl;
	cout << "Reading header size:" << ELF32_HEADER_SIZE << " byte" << endl;
	cout << "\n****************\n" << endl;

	// read data
	elf_file->read((char *)elf_header_buffer, ELF32_HEADER_SIZE);
	elf_desc->elf_header = *elf_header_buffer;

	//count offset address
	printf("0x00000000:\n");

	cout << "e_ident:";
	for (i; i < EI_NIDENT; i++) {
		if (i < 4 || i>6) printf("0x%x ", elf_desc->elf_header.e_ident[i]);

		switch (i) {
		case 4:
			printf("\n\tCLASS:0x%x\t", elf_desc->elf_header.e_ident[i]);
			switch (elf_desc->elf_header.e_ident[i]) {
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
			printf("\tDATA:0x%x\t", elf_desc->elf_header.e_ident[i]);
			switch (elf_desc->elf_header.e_ident[i]) {
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
			printf("\tVERSION:0x%x\n\t", elf_desc->elf_header.e_ident[i]);
			break;
		default:
			break;
		}

	}
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offset_of(ELF32_HEADER,e_type));

	
	cout << "e_type:0x";
	printf("%x\t", elf_desc->elf_header.e_type);
	switch (elf_desc->elf_header.e_type) {
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
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offset_of(ELF32_HEADER, e_machine));

	cout << "e_machine:0x";
	printf("%x\t", elf_desc->elf_header.e_machine);
	switch (elf_desc->elf_header.e_machine)
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

	
	printf("0x%0.8x:\n", offsetof(ELF32_HEADER, e_version));
	cout << "e_version:0x";
	printf("%x\t", elf_desc->elf_header.e_version);
	if (elf_desc->elf_header.e_version == 0) {
		cout << "illegal version";
	}
	else if (elf_desc->elf_header.e_version == 1) {
		cout << "current";
	}
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offsetof(ELF32_HEADER,e_entry));
	cout << "e_entry:0x";
	printf("%x\t", elf_desc->elf_header.e_entry);
	if (elf_desc->elf_header.e_entry == 0) {
		cout << "not exec elf";
	}
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offsetof(ELF32_HEADER, e_phoff));
	cout << "e_phoff:0x";
	printf("%x\t", elf_desc->elf_header.e_phoff);
	if (elf_desc->elf_header.e_phoff == 0) {
		cout << "no program header table";
	}
	else {
		cout << "program header table file offset";
	}
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offsetof(ELF32_HEADER, e_shoff));
	cout << "e_shoff:0x";
	printf("%x\t", elf_desc->elf_header.e_shoff);
	if (elf_desc->elf_header.e_shoff == 0) {
		cout << "no section header table";
	}
	else {
		cout << "section header table file offset";
	}
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offset_of(ELF32_HEADER, e_flags));
	cout << "e_flags:0x";
	printf("%x\t", elf_desc->elf_header.e_flags);
	if (elf_desc->elf_header.e_flags == 0) {
		cout << "Intel CPU flag";
	}
	else {
		cout << "not Intel CPU";
	}
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offset_of(ELF32_HEADER, e_ehsize));
	cout << "e_ehsize:0x";
	printf("%x\t", elf_desc->elf_header.e_ehsize);
	cout << "elf file header size";
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offset_of(ELF32_HEADER, e_phentsize));
	cout << "e_phentsize:0x";
	printf("%0.8x\t", elf_desc->elf_header.e_phentsize);
	cout << "program header table item size";
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offsetof(ELF32_HEADER, e_phnum));
	cout << "e_phnum:0x";
	printf("%0.8x\t", elf_desc->elf_header.e_phnum);
	cout << "program header item count\t";
	if (elf_desc->elf_header.e_phnum == 0)	cout << "no program table";
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offsetof(ELF32_HEADER, e_shentsize));
	cout << "e_shentsize:0x";
	printf("%0.8x\t", elf_desc->elf_header.e_shentsize);
	cout << "section header table item size";
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offsetof(ELF32_HEADER, e_shnum));
	cout << "e_shnum:0x";
	printf("%0.8x\t", elf_desc->elf_header.e_shnum);
	cout << "section header item count\t";
	if (elf_desc->elf_header.e_shnum == 0)	cout << "no section table";
	cout << "\n" << endl;

	
	printf("0x%0.8x:\n", offsetof(ELF32_HEADER, e_shstrndx));
	cout << "e_shstrndx:0x";
	printf("%0.8x\t", elf_desc->elf_header.e_shstrndx);
	cout << "section name table index\t";
	if (elf_desc->elf_header.e_shstrndx == 0)	cout << "no section name table";
	cout << "\n" << endl;
}


void elf64_header_parser(ifstream* elf_file, ELF64_DESC *elf_desc) {
	cout << "[>>>]	/* ELF Header */	[<<<]" << endl;
	cout << "\n****************\n" << endl;

	int i = 0;
	ELF64_HEADER* elf_header_buffer = &elf_desc->elf_header;

	// read data
	elf_file->read((char *)elf_header_buffer, ELF64_HEADER_SIZE);
	elf_desc->elf_header = *elf_header_buffer;

	//count offset address
	printf("0x00000000:\n");

	cout << "e_ident:";
	for (i; i < EI_NIDENT; i++) {
		if (i < 4 || i>6) printf("0x%x ", elf_desc->elf_header.e_ident[i]);

		switch (i) {
		case 4:
			printf("\n\tCLASS:0x%x\t", elf_desc->elf_header.e_ident[i]);
			switch (elf_desc->elf_header.e_ident[i]) {
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
			printf("\tDATA:0x%x\t", elf_desc->elf_header.e_ident[i]);
			switch (elf_desc->elf_header.e_ident[i]) {
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
			printf("\tVERSION:0x%x\n\t", elf_desc->elf_header.e_ident[i]);
			break;
		default:
			break;
		}

	}
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offset_of(ELF64_HEADER, e_type));


	cout << "e_type:0x";
	printf("%x\t", elf_desc->elf_header.e_type);
	switch (elf_desc->elf_header.e_type) {
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
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offset_of(ELF64_HEADER, e_machine));

	cout << "e_machine:0x";
	printf("%x\t", elf_desc->elf_header.e_machine);
	switch (elf_desc->elf_header.e_machine)
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


	printf("0x%0.8x:\n", offsetof(ELF64_HEADER, e_version));
	cout << "e_version:0x";
	printf("%x\t", elf_desc->elf_header.e_version);
	if (elf_desc->elf_header.e_version == 0) {
		cout << "illegal version";
	}
	else if (elf_desc->elf_header.e_version == 1) {
		cout << "current";
	}
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offsetof(ELF64_HEADER, e_entry));
	cout << "e_entry:0x";
	printf("%llx\t", elf_desc->elf_header.e_entry);
	if (elf_desc->elf_header.e_entry == 0) {
		cout << "not exec elf";
	}
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offsetof(ELF64_HEADER, e_phoff));
	cout << "e_phoff:0x";
	printf("%llx\t", elf_desc->elf_header.e_phoff);
	if (elf_desc->elf_header.e_phoff == 0) {
		cout << "no program header table";
	}
	else {
		cout << "program header table file offset";
	}
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offsetof(ELF64_HEADER, e_shoff));
	cout << "e_shoff:0x";
	printf("%llx\t", elf_desc->elf_header.e_shoff);
	if (elf_desc->elf_header.e_shoff == 0) {
		cout << "no section header table";
	}
	else {
		cout << "section header table file offset";
	}
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offset_of(ELF64_HEADER, e_flags));
	cout << "e_flags:0x";
	printf("%x\t", elf_desc->elf_header.e_flags);
	if (elf_desc->elf_header.e_flags == 0) {
		cout << "Intel CPU flag";
	}
	else {
		cout << "not Intel CPU";
	}
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offset_of(ELF64_HEADER, e_ehsize));
	cout << "e_ehsize:0x";
	printf("%x\t", elf_desc->elf_header.e_ehsize);
	cout << "elf file header size";
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offset_of(ELF64_HEADER, e_phentsize));
	cout << "e_phentsize:0x";
	printf("%0.8x\t", elf_desc->elf_header.e_phentsize);
	cout << "program header table item size";
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offsetof(ELF64_HEADER, e_phnum));
	cout << "e_phnum:0x";
	printf("%0.8x\t", elf_desc->elf_header.e_phnum);
	cout << "program header item count\t";
	if (elf_desc->elf_header.e_phnum == 0)	cout << "no program table";
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offsetof(ELF64_HEADER, e_shentsize));
	cout << "e_shentsize:0x";
	printf("%0.8x\t", elf_desc->elf_header.e_shentsize);
	cout << "section header table item size";
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offsetof(ELF64_HEADER, e_shnum));
	cout << "e_shnum:0x";
	printf("%0.8x\t", elf_desc->elf_header.e_shnum);
	cout << "section header item count\t";
	if (elf_desc->elf_header.e_shnum == 0)	cout << "no section table";
	cout << "\n" << endl;


	printf("0x%0.8x:\n", offsetof(ELF64_HEADER, e_shstrndx));
	cout << "e_shstrndx:0x";
	printf("%0.8x\t", elf_desc->elf_header.e_shstrndx);
	cout << "section name table index\t";
	if (elf_desc->elf_header.e_shstrndx == 0)	cout << "no section name table";
	cout << "\n" << endl;
}

void elf32_program_header_parser(ifstream* elf_file, ELF32_DESC *elf_desc) {
	cout << "[>>>]	/* ELF Program Header */	[<<<]" << endl;
	cout << "\n****************\n" << endl;

	// if program table exist
	if (elf_desc->elf_header.e_phoff == 0) {
		cout << "There have no program table" << endl;
		return ;
	}
	printf("Program addr:0x%0.8x,Program num:%d\n-------------------\n", elf_desc->elf_header.e_phoff,elf_desc->elf_header.e_phnum);

	int i = 0;
	ELF32_PROGRAM_HEADER* elf_program_header_buffer = (ELF32_PROGRAM_HEADER*)malloc(ELF32_PROGRAM_HEADER_SIZE);
	elf_file->seekg(elf_desc->elf_header.e_phoff, ios::beg);

	for (i; i < elf_desc->elf_header.e_phnum;i++) {
		elf_file->read((char *)elf_program_header_buffer, ELF32_PROGRAM_HEADER_SIZE);
		elf_desc->program_header_v.push_back(*elf_program_header_buffer);

		cout << "p_type:";
		printf("0x%0.8x\t", elf_desc->program_header_v[i].p_type);
		switch (elf_desc->program_header_v[i].p_type)
		{
		case PT_NULL:
			cout << "Unsed entry";
			break;
		case PT_LOAD:
			cout << "Loadable segment...//This is necessary segment";
			break;
		case PT_DYNAMIC:
			cout << "Dynamic linking information segment";
			break;
		case PT_INTERP:
			cout << "Pathname of interpreter";
			break;
		case PT_NOTE:
			cout << "Auxiliary information";
			break;
		case PT_SHILB:
			cout << "Reserved(not used)";
			break;
		case PT_PHDR:
			cout << "Location of program header itself";
			break;
		case PT_TLS:
			cout << "Thread local storage segment";
			break;
		case PT_SUNW_UNWIND:
			cout << "AMD64 program header";
			break;
		case PT_GNU_EH_FRAME:
			cout << "AMD64 program header";
			break;
		default:
			cout << "Unknow program type";
			break;
		}
		cout << endl;

		cout << "p_offset";
		printf("0x%0.8x\t", elf_desc->program_header_v[i].p_offset);
		cout << "file offset" << endl;

		cout << "p_vaddr:";
		printf("0x%0.8x\t", elf_desc->program_header_v[i].p_vaddr);
		cout << "virtual address in memory image" << endl;
		
		cout << "p_paddr:";
		printf("0x%0.8x\t", elf_desc->program_header_v[i].p_paddr);
		cout << "Physical address" << endl;

		cout << "p_filesz";
		printf("0x%0.8x\t", elf_desc->program_header_v[i].p_filesz);
		cout << "size of contents in file" << endl;

		cout << "p_memsz:";
		printf("0x%0.8x\t", elf_desc->program_header_v[i].p_memsz);
		cout << "size of contens in memory" << endl;
		
		cout << "p_flags:";
		printf("0x%0.8x\t", elf_desc->program_header_v[i].p_flags);
		cout << "access:";
		switch (elf_desc->program_header_v[i].p_flags)
		{
		case 0:
			cout << "no access" << endl;
			break;
		case PF_X:
			cout << "executable" << endl;
			break;
		case PF_W:
			cout << "writable" << endl;
			break;
		case PF_R:
			cout << "readable" << endl;
			break;
		case PF_R+PF_W:
			cout << "readable writable" << endl;
			break;
		case PF_W + PF_X:
			cout << "writable executable" << endl;
			break;
		case PF_R+PF_X:
			cout << "readable executable" << endl;
			break;
		case PF_R+PF_W+PF_X:
			cout << "readable writable executable " << endl;
			break;
		default:
			cout << "Unknow access" << endl;
			break;
		}

		cout << "p_align:";
		printf("0x%0.8x\t", elf_desc->program_header_v[i].p_align);
		cout << "Alignment in memory and file" << endl;

		cout << "\n-----------------------------------------------\n" << endl;
	}

	//free(elf32_program_header_parser);	/* can't free beacause elf_desc are using */
	elf_program_header_buffer = NULL;
}

void elf64_program_header_parser(ifstream* elf_file, ELF64_DESC *elf_desc) {
	cout << "[>>>]	/* ELF Program Header */	[<<<]" << endl;
	cout << "\n****************\n" << endl;

	// if program table exist
	if (elf_desc->elf_header.e_phoff == 0) {
		cout << "There have no program table" << endl;
		return;
	}
	printf("Program addr:0x%llx,Program num:%x\n-------------------\n", elf_desc->elf_header.e_phoff, elf_desc->elf_header.e_phnum);

	
	ELF64_PROGRAM_HEADER* elf_program_header_buffer = (ELF64_PROGRAM_HEADER*)malloc(ELF64_PROGRAM_HEADER_SIZE);
	int i = 0;
	elf_file->seekg(elf_desc->elf_header.e_phoff, ios::beg);

	for (i; i < elf_desc->elf_header.e_phnum; i++) {
		elf_file->read((char *)elf_program_header_buffer, ELF64_PROGRAM_HEADER_SIZE);
		elf_desc->program_header_v.push_back(*elf_program_header_buffer);

		cout << "p_type:";
		printf("0x%0.8x\t", elf_desc->program_header_v[i].p_type);
		switch (elf_desc->program_header_v[i].p_type)
		{
		case PT_NULL:
			cout << "Unsed entry";
			break;
		case PT_LOAD:
			cout << "Loadable segment...//This is necessary segment";
			break;
		case PT_DYNAMIC:
			cout << "Dynamic linking information segment";
			break;
		case PT_INTERP:
			cout << "Pathname of interpreter";
			break;
		case PT_NOTE:
			cout << "Auxiliary information";
			break;
		case PT_SHILB:
			cout << "Reserved(not used)";
			break;
		case PT_PHDR:
			cout << "Location of program header itself";
			break;
		case PT_TLS:
			cout << "Thread local storage segment";
			break;
		case PT_SUNW_UNWIND:
			cout << "AMD64 program header";
			break;
		case PT_GNU_EH_FRAME:
			cout << "AMD64 program header";
			break;
		default:
			cout << "Unknow program type";
			break;
		}
		cout << endl;

		cout << "p_offset";
		printf("0x%0.8x\t", elf_desc->program_header_v[i].p_offset);
		cout << "file offset" << endl;

		cout << "p_vaddr:";
		printf("0x%llx\t", elf_desc->program_header_v[i].p_vaddr);
		cout << "virtual address in memory image" << endl;

		cout << "p_paddr:";
		printf("0x%llx\t", elf_desc->program_header_v[i].p_paddr);
		cout << "Physical address" << endl;

		cout << "p_filesz";
		printf("0x%llx\t", elf_desc->program_header_v[i].p_filesz);
		cout << "size of contents in file" << endl;

		cout << "p_memsz:";
		printf("0x%llx\t", elf_desc->program_header_v[i].p_memsz);
		cout << "size of contens in memory" << endl;

		cout << "p_flags:";
		printf("0x%llx\t", elf_desc->program_header_v[i].p_flags);
		cout << "access:";
		switch (elf_desc->program_header_v[i].p_flags)
		{
		case 0:
			cout << "no access" << endl;
			break;
		case PF_X:
			cout << "executable" << endl;
			break;
		case PF_W:
			cout << "writable" << endl;
			break;
		case PF_R:
			cout << "readable" << endl;
			break;
		case PF_R + PF_W:
			cout << "readable writable" << endl;
			break;
		case PF_W + PF_X:
			cout << "writable executable" << endl;
			break;
		case PF_R + PF_X:
			cout << "readable executable" << endl;
			break;
		case PF_R + PF_W + PF_X:
			cout << "readable writable executable " << endl;
			break;
		default:
			cout << "Unknow access" << endl;
			break;
		}

		cout << "p_align:";
		printf("0x%llx\t", elf_desc->program_header_v[i].p_align);
		cout << "Alignment in memory and file" << endl;

		cout << "\n-----------------------------------------------\n" << endl;
	}

	//free(elf32_program_header_parser);	/* can't free beacause elf_desc are using */
	elf_program_header_buffer = NULL;
}

void elf32_section_header_parser(ifstream* elf_file, ELF32_DESC *elf_desc) {
	cout << "[>>>]	/* ELF Section Header */	[<<<]" << endl;
	cout << "\n****************\n" << endl;

	// if program table exist
	if (elf_desc->elf_header.e_shoff == 0) {
		cout << "There have no section table" << endl;
		return;
	}
	printf("Section addr:0x%0.8x,Section Num:%d\n-------------------\n", elf_desc->elf_header.e_shoff,elf_desc->elf_header.e_shnum);

	ELF32_SECTION_HEADER* elf_section_header_buffer = (ELF32_SECTION_HEADER*)malloc(ELF32_SECTION_HEADER_SIZE);

	int i = 0;
	//reset file pointer
	elf_file->seekg(elf_desc->elf_header.e_shoff, ios::beg);

	for (i; i < elf_desc->elf_header.e_shnum; i++) {
		elf_file->read((char *)elf_section_header_buffer, ELF32_SECTION_HEADER_SIZE);
		elf_desc->section_header_v.push_back(*elf_section_header_buffer);
		
		cout << "sh_name:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_name);
		cout << "section name index";
		//find_section_header_name(elf_file, elf_desc, elf_desc->section_header_v[i].sh_name);
		find_section_name<ELF32_DESC>(elf_file,elf_desc,elf_desc->section_header_v[i].sh_name);
		cout << endl;

		cout << "sh_type:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_type);
		cout << "section type\t";
		switch (elf_desc->section_header_v[i].sh_type)
		{
		case SHT_NULL:
			cout << "inactive section";
			break;
		case SHT_PROGBITS:
			cout << "program defined information";
			break;
		case SHT_SYMTAB:
			cout << "symbol table section";
			break;
		case SHT_STRTAB:
			cout << "string table section";
			/*
			cout << "\n";
			printf("string table file offset:0x%x\n", elf_desc->section_header_v[i].sh_offset);
			//unsigned int addr = &(*elf_file);
			//printf("string table string:%s\n", elf_desc->section_header_v[i].sh_offset+elf_file);
			//getchar();
			*/
			break;
		case SHT_RELA:
			cout << "relocation section with addends";
			break;
		case SHT_HASH:
			cout << "symbol hash table";
			break;
		case SHT_DYNAMIC:
			cout << "dynamic section";
			break;
		case SHT_NOTE:
			cout << "note section";
			break;
		case SHT_NOBITS:
			cout << "no space section";
			break;
		case SHT_REL:
			cout << "relocation section no addends";
			break;
		case SHT_SHLIB:
			cout << "reserved not used";
			break;
		case SHT_DYNSYM:
			cout << "dynamic symbol table";
			break;
		case SHT_GNU_versym:
			cout << "symbol version table";
			break;
		default:
			cout << "Unknow section type";
			break;
		}
		cout << endl;

		cout << "sh_flags:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_flags);
		cout << "section flags\t";
		switch(elf_desc->section_header_v[i].sh_flags){
		case SHF_WRITE:
			cout << "section writerable";
			break;
		case SHF_ALLOC:
			cout << "section need real memory";
			break;
		case SHF_EXECINSTR:
			cout << "this is code section";
			break;
		case SHF_MASHPROC:
			cout << "reserved,not used";
			break;
		default:
			cout << "not knowe flag";
			break;
		}
		cout << endl;

		cout << "sh_addr:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_addr);
		cout << "Address in memory image\t";
		if (elf_desc->section_header_v[i].sh_addr == 0) {
			cout << "This section don't need image";
		}
		cout << endl;

		cout << "sh_offset:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_offset);
		cout << "Offset in file" << endl;

		cout << "sh_size:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_size);
		cout << "section size" << endl;

		cout << "sh_link:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_link);
		cout << "Index of a related section " << endl;

		cout << "sh_info:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_info);
		cout << "section info Depends on section type" << endl;

		cout << "sh_addralign:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_addralign);
		cout << "Alignment" << endl;

		cout << "sh_entsize:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_entsize);
		cout << "every section item size" << endl;

		
		cout << "\n current tellg:" << hex << elf_file->tellg()<<"-----------------------------------------------\n" << endl;
	}


	//free(elf_section_header_buffer);	/* can't free beacause elf_desc are using */
	elf_section_header_buffer = NULL;
}


void elf64_section_header_parser(ifstream* elf_file, ELF64_DESC *elf_desc) {
	cout << "[>>>]	/* ELF Section Header */	[<<<]" << endl;
	cout << "\n****************\n" << endl;

	// if program table exist
	if (elf_desc->elf_header.e_shoff == 0) {
		cout << "There have no section table" << endl;
		return;
	}
	printf("Section addr:0x%llx,Section Num:%d\n-------------------\n", elf_desc->elf_header.e_shoff, elf_desc->elf_header.e_shnum);

	ELF64_SECTION_HEADER* elf_section_header_buffer = (ELF64_SECTION_HEADER*)malloc(ELF64_SECTION_HEADER_SIZE);

	int i = 0;
	//reset file pointer
	elf_file->seekg(elf_desc->elf_header.e_shoff, ios::beg);

	for (i; i < elf_desc->elf_header.e_shnum; i++) {
		elf_file->read((char *)elf_section_header_buffer, ELF64_SECTION_HEADER_SIZE);
		elf_desc->section_header_v.push_back(*elf_section_header_buffer);

		cout << "sh_name:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_name);
		cout << "section name index";
		find_section_name<ELF64_DESC>(elf_file, elf_desc, elf_desc->section_header_v[i].sh_name);
		cout << endl;

		cout << "sh_type:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_type);
		cout << "section type\t";
		switch (elf_desc->section_header_v[i].sh_type)
		{
		case SHT_NULL:
			cout << "inactive section";
			break;
		case SHT_PROGBITS:
			cout << "program defined information";
			break;
		case SHT_SYMTAB:
			cout << "symbol table section";
			break;
		case SHT_STRTAB:
			cout << "string table section";
			/*
			cout << "\n";
			printf("string table file offset:0x%llx\n", elf_desc->section_header_v[i].sh_offset);
			//unsigned int addr = &(*elf_file);
			//printf("string table string:%s\n", elf_desc->section_header_v[i].sh_offset+elf_file);
			//getchar();
			*/
			break;
		case SHT_RELA:
			cout << "relocation section with addends";
			break;
		case SHT_HASH:
			cout << "symbol hash table";
			break;
		case SHT_DYNAMIC:
			cout << "dynamic section";
			break;
		case SHT_NOTE:
			cout << "note section";
			break;
		case SHT_NOBITS:
			cout << "no space section";
			break;
		case SHT_REL:
			cout << "relocation section no addends";
			break;
		case SHT_SHLIB:
			cout << "reserved not used";
			break;
		case SHT_DYNSYM:
			cout << "dynamic symbol table";
			break;
		case SHT_GNU_versym:
			cout << "symbol version table";
			break;
		default:
			cout << "Unknow section type";
			break;
		}
		cout << endl;

		cout << "sh_flags:";
		printf("0x%llx\t", elf_desc->section_header_v[i].sh_flags);
		cout << "section flags\t";
		switch (elf_desc->section_header_v[i].sh_flags) {
		case SHF_WRITE:
			cout << "section writerable";
			break;
		case SHF_ALLOC:
			cout << "section need real memory";
			break;
		case SHF_EXECINSTR:
			cout << "this is code section";
			break;
		case SHF_MASHPROC:
			cout << "reserved,not used";
			break;
		default:
			cout << "not knowe flag";
			break;
		}
		cout << endl;

		cout << "sh_addr:";
		printf("0x%llx\t", elf_desc->section_header_v[i].sh_addr);
		cout << "Address in memory image\t";
		if (elf_desc->section_header_v[i].sh_addr == 0) {
			cout << "This section don't need image";
		}
		cout << endl;

		cout << "sh_offset:";
		printf("0x%llx\t", elf_desc->section_header_v[i].sh_offset);
		cout << "Offset in file" << endl;

		cout << "sh_size:";
		printf("0x%llx\t", elf_desc->section_header_v[i].sh_size);
		cout << "section size" << endl;

		cout << "sh_link:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_link);
		cout << "Index of a related section " << endl;

		cout << "sh_info:";
		printf("0x%0.8x\t", elf_desc->section_header_v[i].sh_info);
		cout << "section info Depends on section type" << endl;

		cout << "sh_addralign:";
		printf("0x%llx\t", elf_desc->section_header_v[i].sh_addralign);
		cout << "Alignment" << endl;

		cout << "sh_entsize:";
		printf("0x%llx\t", elf_desc->section_header_v[i].sh_entsize);
		cout << "every section item size" << endl;

		cout << "\n-----------------------------------------------\n" << endl;
	}


	//free(elf_section_header_buffer);	/* can't free beacause elf_desc are using */
	elf_section_header_buffer = NULL;
}