Name : keshav bansal
Roll : 2019101019
Q1 report

1. The normal merge sort comes out to be faster than both Threaded and concurrent merge sort.
2. The threaded merge sort is faster than the mergesort using processes.

Logic:
The reason for slow multi-threaded and multi-process merge sort:
1.  when the left part of array is accessed and after that right part of the array is accessed, it can lead to cache misses and thus the performance degrades with each fork or thread.
2.  Also, it takes a large amount of time in context switching for multiprocess-sorting and thus it is the slowest. This time spent in switching is relatively lesser in threading.

