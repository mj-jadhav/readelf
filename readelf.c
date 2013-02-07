#include <stdio.h>
//#include <elf.h>
#include <string.h>
#include "elf_common.h"
#include "elfcomm.h"

static int do_archive_index;

/* The ar_date field of the armap (__.SYMDEF) member of an archive
   must be greater than the modified date of the entire file, or
   BSD-derived linkers complain.  We originally write the ar_date with
   this offset from the real file's mod-time.  After finishing the
   file, we rewrite ar_date if it's not still greater than the mod date.  */

struct ar_hdr
{
    char ar_name[16];		/* Name of this member.  */
    char ar_date[12];		/* File mtime.  */
    char ar_uid[6];		/* Owner uid; printed as decimal.  */
    char ar_gid[6];		/* Owner gid; printed as decimal.  */
    char ar_mode[8];		/* File mode, printed as octal.   */
    char ar_size[10];		/* File size, printed as decimal.  */
    char ar_fmag[2];		/* Should contain ARFMAG.  */
};

/* ELF Header (32-bit implementations) */

typedef struct {
  unsigned char	e_ident[16];		/* ELF "magic number" */
  unsigned char	e_type[2];		/* Identifies object file type */
  unsigned char	e_machine[2];		/* Specifies required architecture */
  unsigned char	e_version[4];		/* Identifies object file version */
  unsigned char	e_entry[4];		/* Entry point virtual address */
  unsigned char	e_phoff[4];		/* Program header table file offset */
  unsigned char	e_shoff[4];		/* Section header table file offset */
  unsigned char	e_flags[4];		/* Processor-specific flags */
  unsigned char	e_ehsize[2];		/* ELF header size in bytes */
  unsigned char	e_phentsize[2];		/* Program header table entry size */
  unsigned char	e_phnum[2];		/* Program header table entry count */
  unsigned char	e_shentsize[2];		/* Section header table entry size */
  unsigned char	e_shnum[2];		/* Section header table entry count */
  unsigned char	e_shstrndx[2];		/* Section header string table index */
} Elf32_External_Ehdr;

/* ELF Header */

#define EI_NIDENT	16		/* Size of e_ident[] */

typedef struct elf_internal_ehdr {
  unsigned char		e_ident[EI_NIDENT]; /* ELF "magic number" */
    /*bfd_vma*/ unsigned long		e_entry;	/* Entry point virtual address */
    /*bfd_size_type*/unsigned long		e_phoff;	/* Program header table file offset */
    /*bfd_size_type*/unsigned long		e_shoff;	/* Section header table file offset */
  unsigned long		e_version;	/* Identifies object file version */
  unsigned long		e_flags;	/* Processor-specific flags */
  unsigned short	e_type;		/* Identifies object file type */
  unsigned short	e_machine;	/* Specifies required architecture */
  unsigned int		e_ehsize;	/* ELF header size in bytes */
  unsigned int		e_phentsize;	/* Program header table entry size */
  unsigned int		e_phnum;	/* Program header table entry count */
  unsigned int		e_shentsize;	/* Section header table entry size */
  unsigned int		e_shnum;	/* Section header table entry count */
  unsigned int		e_shstrndx;	/* Section header string table index */
} Elf_Internal_Ehdr;

static Elf_Internal_Ehdr elf_header;


/* Structure to hold information about an archive file.  */

struct archive_info
{
  char * file_name;                     /* Archive file name.  */
  FILE * file;                          /* Open file descriptor.  */
    /* elf_vma*/ unsigned long long index_num;                    /* Number of symbols in table.  */
    /*elf_vma*/ unsigned long long * index_array;                /* The array of member offsets.  */
  char * sym_table;                     /* The symbol table.  */
  unsigned long sym_size;               /* Size of the symbol table.  */
  char * longnames;                     /* The long file names table.  */
  unsigned long longnames_size;         /* Size of the long file names table.  */
  unsigned long nested_member_origin;   /* Origin in the nested archive of the current member.  */
  unsigned long next_arhdr_offset;      /* Offset of the next archive header.  */
    /*bfd_boolean*/ int is_thin_archive;          /* TRUE if this is a thin archive.  */
    /*bfd_boolean*/ int uses_64bit_indicies;      /* TRUE if the index table uses 64bit entries.  */
  struct ar_hdr arhdr;                  /* Current archive header.  */
};

int is_32bit_elf;

void print_elf_header()
{
    // switch (elf_header.e_indent[EI_MAG0])
    printf("\n\n********** ELF HEADER **********\n\n");
    printf("\n Magic: \t\t %2.2x %2.2x %2.2x %2.2x\n",ELFMAG0,ELFMAG1,ELFMAG2,ELFMAG3);
    printf("\n Class: \t\t ");
    //printf("\n Elf class: %d",elf_header.e_ident[EI_CLASS]);
    switch(elf_header.e_ident[EI_CLASS])
	{
	case ELFCLASSNONE :
	    printf("Invalid Class\n");
	    break;

	case 32 :
	    printf("ELF32 (32-bit Objects)\n");
	    break;

	case 64 :
	    printf("ELF64 (64-bit Objects)\n");
	    break;

	default :
	    printf("\n Invalid Class name\n");
	    break;
	}

    printf("\n Data: \t\t\t ");

    switch(elf_header.e_ident[EI_DATA])
	{
	case ELFDATANONE :
	    printf("Invalid Data\n");
	    break;

	case ELFDATA2LSB :
	    printf("2's complement, little endian\n'");
	    break;

	case ELFDATA2MSB :
	    printf("2's complement, big endian\n");
	    break;
	}

    printf("\n ABI Version: \t\t ");
    switch(elf_header.e_ident[EI_VERSION])
	{
	case EV_NONE :
	    printf("0\n");
	    break;

	case EV_CURRENT :
	    printf("1\n");
	    break;
	}
    printf("\n OS/ABI: \t\t ");
    switch(elf_header.e_ident[EI_OSABI])
	{
	case ELFOSABI_NONE:
	    printf("UNIX System V\n ");
	    break;

	case ELFOSABI_HPUX:
	    printf("HP-UX operating system\n ");
	    break;

	case ELFOSABI_NETBSD:
	    printf("NetBSD \n");
	    break;

	    //case ELFOSABI_GNU:
	case ELFOSABI_LINUX:
	    printf("GNU/Alias for ELFOSABI_GNU \n");
	    break;

	case ELFOSABI_SOLARIS:
	    printf("Solaris\n ");
	    break;

	case ELFOSABI_AIX:
	    printf("AIX \n");
	    break;

	case ELFOSABI_IRIX:
	    printf("IRIX\n ");
	    break;

	default:
	    printf("Other\n");
	    break;
	    /* remaining */
	    //#define ELFOSABI_FREEBSD      9	/* FreeBSD */
	    //#define ELFOSABI_TRU64	     10	/* TRU64 UNIX */
	    //#define ELFOSABI_MODESTO     11	/* Novell Modesto */
	    //#define ELFOSABI_OPENBSD     12	/* OpenBSD */
	    //#define ELFOSABI_OPENVMS     13	/* OpenVMS */
	    //#define ELFOSABI_NSK	     14	/* Hewlett-Packard Non-Stop Kernel */
	    //#define ELFOSABI_AROS	     15	/* AROS */
	    //#define ELFOSABI_FENIXOS     16 /* FenixOS */
	    //#define ELFOSABI_C6000_ELFABI 64 /* Bare-metal TMS320C6000 */
	    //#define ELFOSABI_C6000_LINUX 65 /* Linux TMS320C6000 */
	    //#define ELFOSABI_ARM	     97	/* ARM */
	    //#define ELFOSABI_STANDALONE 255	/* Standalone (embedded) application */

	}
    printf("\n Type: \t\t\t ");

    switch(elf_header.e_type)
	{
	case ET_NONE:
	    printf("No file type\n ");
	    break;

	case ET_REL:
	    printf("Relocatable file\n ");
	    break;

	case ET_EXEC:
	    printf("Executable file \n");
	    break;

	    //case ELFOSABI_GNU:
	case ET_DYN:
	    printf("Shared Object File \n");
	    break;

	case ET_CORE:
	    printf("Core file\n ");
	    break;

       case ET_LOOS:
	    printf("Operating System Specific\n ");
	    break;

	case ET_HIOS:
	    printf("Operating system Specific\n ");
	    break;

	case ET_LOPROC:
	    printf("Processor specific \n");
	    break;

		//case ELFOSABI_GNU:
	case ET_HIPROC:
	    printf("Processor specific\n");
	    break;

	}

}


/********* Read the symbol table and long-name table from an archive ******/
int
setup_archive (struct archive_info *arch, const char *file_name,
	       FILE *file,/* bfd_boolean*/int is_thin_archive,
	       /*bfd_boolean*/int read_symbols)
{
    unsigned int got;
    /*size_t*/
    int i;

    arch->file_name = strdup (file_name);
    arch->file = file;
    arch->index_num = 0;
    arch->index_array = NULL;
    arch->sym_table = NULL;
    arch->sym_size = 0;
    arch->longnames = NULL;
    arch->longnames_size = 0;
    arch->nested_member_origin = 0;
    arch->is_thin_archive = is_thin_archive;
    arch->uses_64bit_indicies = 0/*FALSE*/;
    arch->next_arhdr_offset = 8/*SARMAG*/;

    /* Read the first archive member header.  */
    if (fseek (file, 8/*SARMAG*/, SEEK_SET) != 0)
	{
	    printf("%s: failed to seek to first archive header\n", file_name);
	    return 1;
	}
    got = fread (&arch->arhdr, 1, sizeof arch->arhdr, file);

    for(i=0;i<5;i++)
	printf("\n arch->arhdr->ar_name - %x",arch->arhdr.ar_name[i]);
    printf("\n arch->arhdr->ar_date - %s",arch->arhdr.ar_date);
    printf("\n arch->arhdr->ar_uid  - %s",arch->arhdr.ar_uid);
    printf("\n arch->arhdr->ar_gid  - %s",arch->arhdr.ar_gid);
    printf("\n arch->arhdr->ar_mode - %s",arch->arhdr.ar_mode);
    printf("\n arch->arhdr->ar_size - %s",arch->arhdr.ar_size);
    printf("\n arch->arhdr->ar_fmag - %s",arch->arhdr.ar_fmag);



    /* Read in the identity array.  */
    if (fread (elf_header.e_ident, EI_NIDENT, 1, file) != 1)
	return 0;

    /* Determine how to read the rest of the header.  */
    switch (elf_header.e_ident[EI_DATA])
	{
	default: /* fall through */
	case ELFDATANONE: /* fall through */
	case ELFDATA2LSB:
	    byte_get = byte_get_little_endian;
	    byte_put = byte_put_little_endian;
	    break;
	case ELFDATA2MSB:
	    byte_get = byte_get_big_endian;
	    byte_put = byte_put_big_endian;
	    break;
	}


 /* For now we only support 32 bit and 64 bit ELF files.  */
  is_32bit_elf = (elf_header.e_ident[EI_CLASS] != ELFCLASS64);

  /* Read in the rest of the header.  */
  if (is_32bit_elf)
    {
      Elf32_External_Ehdr ehdr32;

      if (fread (ehdr32.e_type, sizeof (ehdr32) - EI_NIDENT, 1, file) != 1)
	return 0;

      elf_header.e_type      = BYTE_GET (ehdr32.e_type);
      elf_header.e_machine   = BYTE_GET (ehdr32.e_machine);
      elf_header.e_version   = BYTE_GET (ehdr32.e_version);
      elf_header.e_entry     = BYTE_GET (ehdr32.e_entry);
      elf_header.e_phoff     = BYTE_GET (ehdr32.e_phoff);
      elf_header.e_shoff     = BYTE_GET (ehdr32.e_shoff);
      elf_header.e_flags     = BYTE_GET (ehdr32.e_flags);
      elf_header.e_ehsize    = BYTE_GET (ehdr32.e_ehsize);
      elf_header.e_phentsize = BYTE_GET (ehdr32.e_phentsize);
      elf_header.e_phnum     = BYTE_GET (ehdr32.e_phnum);
      elf_header.e_shentsize = BYTE_GET (ehdr32.e_shentsize);
      elf_header.e_shnum     = BYTE_GET (ehdr32.e_shnum);
      elf_header.e_shstrndx  = BYTE_GET (ehdr32.e_shstrndx);
    }
    print_elf_header();
    return 0;
}

/* Process an ELF archive.
   On entry the file is positioned just after the ARMAG string.
*/
static int process_archive(char * file_name, FILE * file,/* bfd_boolean*/ int is_thin_archive)
{
    struct archive_info arch;
    struct archive_info nested_arch;
    //size_t got;
    int ret;

    // show_name = 1;

    /* The ARCH structure is used to hold information about this archive.  */
    arch.file_name = NULL;
    arch.file = NULL;
    arch.index_array = NULL;
    arch.sym_table = NULL;
    arch.longnames = NULL;

    /* The NESTED_ARCH structure is used as a single-item cache of information
       about a nested archive (when members of a thin archive reside within
       another regular archive file).  */
    nested_arch.file_name = NULL;
    nested_arch.file = NULL;
    nested_arch.index_array = NULL;
    nested_arch.sym_table = NULL;
    nested_arch.longnames = NULL;

    if (setup_archive (&arch, file_name, file, is_thin_archive, do_archive_index) != 0)
	{
	    printf("\n Error in setup_archive method.");
	    return 0;
	    // ret = 1;
	    // goto out;
	}

    return 1;
}

static int process_file(char * file_name)
{
    FILE * file;
    int ret;

    file = fopen(file_name, "rb");
    if (file == NULL)
    {
	printf("Input file '%s' is not readable.\n", file_name);
	return 1;
    }

    ret = process_archive(file_name,file,0 /*false*/);

}


int main(int argc, char ** argv)
{
    int err;

    err = process_file(argv[1]);

    return 0;
}
