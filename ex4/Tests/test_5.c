#include <stdio.h>
#include <stdlib.h>
#include "osqueue.h"
#include "threadPool.h"

void hello (void* a)
{
       printf("Hello, world!\n");
}

void test_thread_pool_sanity()
{
   int i;
   
   ThreadPool* tp = tpCreate(4);
   
   for(i=0; i<4; ++i)
   {
      tpInsertTask(tp,hello,NULL);
   }
   
   tpDestroy(tp,1);
}

int main()
{
   test_thread_pool_sanity();

   return 0;
}
