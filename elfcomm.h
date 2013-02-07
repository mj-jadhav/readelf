#include<stdio.h>
#include <assert.h>

typedef unsigned long elf_vma;

/*
extern void (*byte_put) (unsigned char *, elf_vma, int);
extern void byte_put_little_endian (unsigned char *, elf_vma, int);
extern void byte_put_big_endian (unsigned char *, elf_vma, int);

extern elf_vma (*byte_get) (unsigned char *, int);
extern elf_vma byte_get_signed (unsigned char *, int);
extern elf_vma byte_get_little_endian (unsigned char *, int);
extern elf_vma byte_get_big_endian (unsigned char *, int);
extern void byte_get_64 (unsigned char *, elf_vma *, elf_vma *);
*/
#define BYTE_PUT(field, val)	byte_put (field, val, sizeof (field))
#define BYTE_GET(field)		byte_get (field, sizeof (field))
//#define BYTE_GET_SIGNED(field)	byte_get_signed (field, sizeof (field))

elf_vma (*byte_get) (unsigned char *, int);

elf_vma
byte_get_little_endian (unsigned char *field, int size)
{
  switch (size)
    {
    case 1:
      return *field;

    case 2:
      return  ((unsigned int) (field[0]))
	|    (((unsigned int) (field[1])) << 8);

    case 3:
      return  ((unsigned long) (field[0]))
	|    (((unsigned long) (field[1])) << 8)
	|    (((unsigned long) (field[2])) << 16);

    case 4:
      return  ((unsigned long) (field[0]))
	|    (((unsigned long) (field[1])) << 8)
	|    (((unsigned long) (field[2])) << 16)
	|    (((unsigned long) (field[3])) << 24);

    case 8:
      if (sizeof (elf_vma) == 8)
	return  ((elf_vma) (field[0]))
	  |    (((elf_vma) (field[1])) << 8)
	  |    (((elf_vma) (field[2])) << 16)
	  |    (((elf_vma) (field[3])) << 24)
	  |    (((elf_vma) (field[4])) << 32)
	  |    (((elf_vma) (field[5])) << 40)
	  |    (((elf_vma) (field[6])) << 48)
	  |    (((elf_vma) (field[7])) << 56);
      else if (sizeof (elf_vma) == 4)
	/* We want to extract data from an 8 byte wide field and
	   place it into a 4 byte wide field.  Since this is a little
	   endian source we can just use the 4 byte extraction code.  */
	return  ((unsigned long) (field[0]))
	  |    (((unsigned long) (field[1])) << 8)
	  |    (((unsigned long) (field[2])) << 16)
	  |    (((unsigned long) (field[3])) << 24);

    default:
      printf("Unhandled data length: %d\n", size);
      //abort ();
    }
}

elf_vma
byte_get_big_endian (unsigned char *field, int size)
{
  switch (size)
    {
    case 1:
      return *field;

    case 2:
      return ((unsigned int) (field[1])) | (((int) (field[0])) << 8);

    case 3:
      return ((unsigned long) (field[2]))
	|   (((unsigned long) (field[1])) << 8)
	|   (((unsigned long) (field[0])) << 16);

    case 4:
      return ((unsigned long) (field[3]))
	|   (((unsigned long) (field[2])) << 8)
	|   (((unsigned long) (field[1])) << 16)
	|   (((unsigned long) (field[0])) << 24);

    case 8:
      if (sizeof (elf_vma) == 8)
	return ((elf_vma) (field[7]))
	  |   (((elf_vma) (field[6])) << 8)
	  |   (((elf_vma) (field[5])) << 16)
	  |   (((elf_vma) (field[4])) << 24)
	  |   (((elf_vma) (field[3])) << 32)
	  |   (((elf_vma) (field[2])) << 40)
	  |   (((elf_vma) (field[1])) << 48)
	  |   (((elf_vma) (field[0])) << 56);
      else if (sizeof (elf_vma) == 4)
	{
	  /* Although we are extracing data from an 8 byte wide field,
	     we are returning only 4 bytes of data.  */
	  field += 4;
	  return ((unsigned long) (field[3]))
	    |   (((unsigned long) (field[2])) << 8)
	    |   (((unsigned long) (field[1])) << 16)
	    |   (((unsigned long) (field[0])) << 24);
	}

    default:
      printf("Unhandled data length: %d\n", size);
      //      abort ();
    }
}


void (*byte_put) (unsigned char *, elf_vma, int);

void
byte_put_little_endian (unsigned char * field, elf_vma value, int size)
{
  switch (size)
    {
    case 8:
      field[7] = (((value >> 24) >> 24) >> 8) & 0xff;
      field[6] = ((value >> 24) >> 24) & 0xff;
      field[5] = ((value >> 24) >> 16) & 0xff;
      field[4] = ((value >> 24) >> 8) & 0xff;
      /* Fall through.  */
    case 4:
      field[3] = (value >> 24) & 0xff;
      /* Fall through.  */
    case 3:
      field[2] = (value >> 16) & 0xff;
      /* Fall through.  */
    case 2:
      field[1] = (value >> 8) & 0xff;
      /* Fall through.  */
    case 1:
      field[0] = value & 0xff;
      break;

    default:
      printf("Unhandled data length: %d\n", size);
      //      abort ();
    }
}

void
byte_put_big_endian (unsigned char * field, elf_vma value, int size)
{
  switch (size)
    {
    case 8:
      field[7] = value & 0xff;
      field[6] = (value >> 8) & 0xff;
      field[5] = (value >> 16) & 0xff;
      field[4] = (value >> 24) & 0xff;
      value >>= 16;
      value >>= 16;
      /* Fall through.  */
    case 4:
      field[3] = value & 0xff;
      value >>= 8;
      /* Fall through.  */
    case 3:
      field[2] = value & 0xff;
      value >>= 8;
      /* Fall through.  */
    case 2:
      field[1] = value & 0xff;
      value >>= 8;
      /* Fall through.  */
    case 1:
      field[0] = value & 0xff;
      break;

    default:
      printf("Unhandled data length: %d\n", size);
      //      abort ();
    }
}
