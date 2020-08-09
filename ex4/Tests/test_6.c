#include <stdio.h>
#include <stdlib.h>
#include "osqueue.h"
#include "threadPool.h"

void chicken (void* a)
{
       printf("chicken comes first\n");
}

void egg (void* a)
{
       printf("egg comes first\n");
}

void test_thread_pool_sanity()
{
   int i;
   
   ThreadPool* tp = tpCreate(1);
   
   for(i=0; i<2; ++i)
   {
		if(i%2 == 0){
		      tpInsertTask(tp,chicken,NULL);
		}
        else{
		      tpInsertTask(tp,egg,NULL);
		}
   }
   
   tpDestroy(tp,1);
}

int main()
{
   test_thread_pool_sanity();

   return 0;
}