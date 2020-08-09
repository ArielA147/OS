#include <stdio.h>
#include <stdlib.h>
#include "osqueue.h"
#include "threadPool.h"

void func (void* a)
{
  char arg = *((char*)a);
  printf("I've got: %c\n",arg);
}

void test_thread_pool_sanity()
{
   int i;
   ThreadPool* tp = tpCreate(4);
   
   for(i=0; i<10; ++i)
   {
		char arg1 = 'a';
		tpInsertTask(tp,func,(void *)&arg1);
   }
   
   tpDestroy(tp,1);
}

int main()
{
   test_thread_pool_sanity();

   return 0;
}