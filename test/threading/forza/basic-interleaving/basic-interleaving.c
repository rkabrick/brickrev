#include <stdlib.h>                                                                                                                                                                                                                                                                                                                                                  
#include <stdint.h>
#include "../../../../common/syscalls/syscalls.h"
#define assert(x) if (!(x)) { asm(".byte 0x00"); asm(".byte 0x00"); asm(".byte 0x00"); asm(".byte 0x00"); }

void *thread1(int* counter){
  *counter = 1;
  const char msg2[29] = "Thread1 Incremented Counter\n";
    rev_write(STDOUT_FILENO, msg2, sizeof(msg2));
  return 0;
}

void *thread2(int* counter){
  const char msg[29] = "Thread2 waiting for counter\n";
  while( *counter < 1 ){
    rev_write(STDOUT_FILENO, msg, sizeof(msg));
  }
  *counter = 1;
  return 0;
}


// volatile mailbox *mb = 0x37fa000;

int main(int argc, char **argv)
{
    int* ctr = 0;

    rev_pthread_t t1, t2;
    rev_pthread_create(&t1, thread1, &ctr);
    rev_pthread_create(&t2, thread2, &ctr);

    rev_pthread_join(t1);
    rev_pthread_join(t2);
    
    // assert(*ctr == 2);
    
    return 0;
}
