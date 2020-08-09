#include <stdio.h>
#include <stdlib.h>
#include "osqueue.h"
#include "threadPool.h"
#include <unistd.h>

void func (void* a)
{
  printf("So tired...\n");
  sleep(4);
}

void test_thread_pool_sanity()
{
   int i;
   ThreadPool* tp = tpCreate(1);
   
   for(i=0; i<2; ++i)
   {
		tpInsertTask(tp,func,NULL);
		sleep(1);
   }
   
   tpDestroy(tp,0);
}

int main()
{
   test_thread_pool_sanity();

   return 0;
}