extern "C" {
extern void *mmap (void *__addr, int __len, int __prot,
		   int __flags, int __fd, int __offset);

/* Deallocate any mapping for the region starting at ADDR and extending LEN
   bytes.  Returns 0 if successful, -1 for errors (and sets errno).  */
extern int munmap (void *__addr, int __len);
}
#define PROT_READ	0x1		/* Page can be read.  */
#define PROT_WRITE	0x2		/* Page can be written.  */
#define PROT_EXEC	0x4		/* Page can be executed.  */

#define MAP_SHARED	0x01		/* Changes are share.  */
#define MAP_PRIVATE	0x02		/* Changes are private.  */

#define MAP_ANONYMOUS	0x20		/* Don't use a file.  */
