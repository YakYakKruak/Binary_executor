#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


void failed(const char *message) {
    puts(message);
    exit(EXIT_FAILURE);
}

int get_section_index_by_name(const char* name, void* code) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)code;
    Elf64_Shdr *shdr = (Elf64_Shdr *)(code + ehdr->e_shoff);
    int shnum = ehdr->e_shnum;
    Elf64_Shdr *sh_strtab = &shdr[ehdr->e_shstrndx];
    const char *const sh_strtab_p = code + sh_strtab->sh_offset;
    for (int i = 0; i < shnum; ++i) {
        if(!strcmp(sh_strtab_p + shdr[i].sh_name,name))
            return i;
    }
}

int main(int argc, char** argv)
{
    char *line;
    void *code;
    FILE *data;
    int exec_file;
    struct stat buf;
    int (*func)(char*,size_t);
    int index;
    size_t length;
    Elf64_Ehdr header;

    if(argc!=3)
        failed("Usage: executable file and text file");

    data = fopen(argv[2],"r");
    exec_file = open(argv[1],O_RDONLY);

    if(!data||!exec_file)
        failed("Cannot open file!");

    if (fstat(exec_file, &buf) < 0)
        failed("Cannot store file!");

    code = mmap(0, buf.st_size, PROT_EXEC | PROT_READ, MAP_PRIVATE, exec_file, 0);

    if(code < 0)
        failed("Cannot map file!");

    close(exec_file);
    index = get_section_index_by_name(".text",code);
    header = *((Elf64_Ehdr *)code);
    Elf64_Shdr* section_header = code + header.e_shoff;
    section_header+=index;
    func = (int (*)(char*,size_t))(code+section_header->sh_offset);

    while (getline(&line, &length, data) > 0) {
        printf("%d\n" , func(line,length));
    }

    fclose(data);
    return 0;
}
