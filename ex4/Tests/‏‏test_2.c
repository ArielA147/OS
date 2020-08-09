#include <stdio.h>
#include <stdlib.h>
#include "osqueue.h"
#include "threadPool.h"

int answer = 0;

void hello (void* a)
{
    answer += 2;
}

void test_thread_pool_sanity()
{
   int i;
   
   ThreadPool* tp = tpCreate(1);
   
   for(i=0; i<21; ++i)
   {
      tpInsertTask(tp,hello,NULL);
   }
   
   tpDestroy(tp,1);
   printf("answer: %d\n", answer);
}

int main()
{
   test_thread_pool_sanity();

   return 0;
}
