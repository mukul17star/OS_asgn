#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <user/syscall.h>
#include "devices/input.h"
#include "devices/shutdown.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"

#define MAX_ARGS 3
#define USER_VADDR_BOTTOM ((void *) 0x08048000)

static void syscall_handler (struct intr_frame *);

int write_stdout (int fd, void *buffer, unsigned size){
	if(fd == 1){
		putbuf(buffer, size);
		return size;
	}
	return 0;
}

void check_valid_ptr (void *vaddr){
	if(!is_user_vaddr(vaddr) || vaddr < USER_VADDR_BOTTOM)
	{
		exit(-1);
	}
}

void* user_to_kernel_ptr (void *vaddr){

	check_valid_ptr(vaddr);
	void *ptr=pagedir_get_page(thread_current()->pagedir, vaddr);
	if(!ptr)
	{
		exit(-1);
	}
	return ptr;
}

void get_arg (struct intr_frame *f, int *arg, int n)
{
  int i;
  int *ptr;
  for (i = 0; i < n; i++)
    {
      ptr = (int *) f->esp + i + 1;
      check_valid_ptr((void *) ptr);
      arg[i] = *ptr;
    }
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");
  thread_exit ();
}
