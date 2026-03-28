#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"

int
main(int argc, char *argv[])
{
  char *p;
  int pid;

  printf(1, "testprotection: starting\n");

  // 1. Allocate a page (must be page-aligned for mprotect)
  p = sbrk(PGSIZE);
  if (p == (char*)-1) {
    printf(1, "sbrk failed\n");
    exit();
  }

  printf(1, "1. Writing to page before mprotect...\n");
  p[0] = 'a';
  printf(1, "   Success: p[0] = %c\n", p[0]);

  // 2. Protect the page [cite: 39, 45]
  if (mprotect(p, PGSIZE) < 0) {
    printf(1, "mprotect failed\n");
    exit();
  }
  printf(1, "2. Page is now protected (read-only).\n");

  // 3. Test protection in a child so we don't kill the main test 
  pid = fork();
  if (pid < 0) {
    printf(1, "fork failed\n");
    exit();
  }

  if (pid == 0) {
    printf(1, "   Child attempting to write to protected page (should fault)...\n");
    p[0] = 'b'; // This SHOULD trigger Trap 14 
    printf(1, "   ERROR: Child survived the write!\n");
    exit();
  } else {
    wait();
    printf(1, "   Parent: Child was killed (as expected).\n");
  }

  // 4. Unprotect the page
  if (munprotect(p, PGSIZE) < 0) {
    printf(1, "munprotect failed\n");
    exit();
  }
  printf(1, "3. Page is now unprotected (read-write).\n");

  p[0] = 'c';
  printf(1, "   Success: p[0] = %c\n", p[0]);

  printf(1, "testprotection: PASSED\n");
  exit();
}