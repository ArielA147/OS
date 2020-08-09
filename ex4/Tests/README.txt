the tests check memory leakes ( still reachable bytes , definitely lost bytes)

Test 1:
prints ‘hello’ 5 times 
5 tasks, 5 threads
Same as the supplied sanity test.
Memory leaks were tested using this test too.

Test 2:
prints ‘answer: 42’ 
21 tasks, 1 thread

Test 3:
prints ‘Hello, world!’ 5 times, then prints ‘Bye Bye!’ (after tpDestroy)
5 tasks, 1 thread

Test 4:
prints ‘Hello, world!’ 18 times
18 tasks, 2 threads

Test 5:
prints ‘Hello, world!’ 4 times
4 tasks, 4 threads

Test 6:
prints ‘chicken comes first’ and ‘egg comes first’ (reversed order was also accepted)
2 tasks, 1 thread
different function for each task

Test 7:
prints ‘I’ve got: a’ 10 times
10 tasks, 4 threads
a char argument was passed (‘a’)

Test 8:
prints ‘So tired...’ only once
2 tasks, 1 thread
tpDestroy is called with 0 as the second argument, so we should not wait for new tasks (within the queue).
