
/* This file is part of ELF support for BFD, and contains the portions
   that are common to both the internal and external representations.
   For example, ELFMAG0 is the byte 0x7F in both the internal (in-memory)
   and external (in-file) representations.  */

//#ifndef _ELF_COMMON_H
//#define _ELF_COMMON_H

/* Fields in e_ident[].  */

#define EI_MAG0		0	/* File identification byte 0 index */
#define ELFMAG0		   0x7F	/* Magic number byte 0 */

#define EI_MAG1		1	/* File identification byte 1 index */
#define ELFMAG1		    'E'	/* Magic number byte 1 */

#define EI_MAG2		2	/* File identification byte 2 index */
#define ELFMAG2		    'L'	/* Magic number byte 2 */

#define EI_MAG3		3	/* File identification byte 3 index */
#define ELFMAG3		    'F'	/* Magic number byte 3 */

#define EI_CLASS	4	/* File class */
#define ELFCLASSNONE	      0	/* Invalid class */
#define ELFCLASS32	      1	/* 32-bit objects */
#define ELFCLASS64	      2	/* 64-bit objects */

#define EI_DATA		5	/* Data encoding */
#define ELFDATANONE	      0	/* Invalid data encoding */
#define ELFDATA2LSB	      1	/* 2's complement, little endian */
#define ELFDATA2MSB	      2	/* 2's complement, big endian */

#define EI_VERSION	6	/* File version */

#define EI_OSABI	7	/* Operating System/ABI indication */
#define ELFOSABI_NONE	      0	/* UNIX System V ABI */
#define ELFOSABI_HPUX	      1	/* HP-UX operating system */
#define ELFOSABI_NETBSD	      2	/* NetBSD */
#define ELFOSABI_GNU	      3	/* GNU */
#define ELFOSABI_LINUX	      3	/* Alias for ELFOSABI_GNU */
#define ELFOSABI_SOLARIS      6	/* Solaris */
#define ELFOSABI_AIX	      7	/* AIX */
#define ELFOSABI_IRIX	      8	/* IRIX */
#define ELFOSABI_FREEBSD      9	/* FreeBSD */
#define ELFOSABI_TRU64	     10	/* TRU64 UNIX */
#define ELFOSABI_MODESTO     11	/* Novell Modesto */
#define ELFOSABI_OPENBSD     12	/* OpenBSD */
#define ELFOSABI_OPENVMS     13	/* OpenVMS */
#define ELFOSABI_NSK	     14	/* Hewlett-Packard Non-Stop Kernel */
#define ELFOSABI_AROS	     15	/* AROS */
#define ELFOSABI_FENIXOS     16 /* FenixOS */
#define ELFOSABI_C6000_ELFABI 64 /* Bare-metal TMS320C6000 */
#define ELFOSABI_C6000_LINUX 65 /* Linux TMS320C6000 */
#define ELFOSABI_ARM	     97	/* ARM */
#define ELFOSABI_STANDALONE 255	/* Standalone (embedded) application */

#define EI_ABIVERSION	8	/* ABI version */

#define EI_PAD		9	/* Start of padding bytes */

/* Values for e_version.  */

#define EV_NONE		0		/* Invalid ELF version */
#define EV_CURRENT	1		/* Current version */


/* Values for e_type, which identifies the object file type.  */

#define ET_NONE		0	/* No file type */
#define ET_REL		1	/* Relocatable file */
#define ET_EXEC		2	/* Executable file */
#define ET_DYN		3	/* Shared object file */
#define ET_CORE		4	/* Core file */
#define ET_LOOS		0xFE00	/* Operating system-specific */
#define ET_HIOS		0xFEFF	/* Operating system-specific */
#define ET_LOPROC	0xFF00	/* Processor-specific */
#define ET_HIPROC	0xFFFF	/* Processor-specific */
