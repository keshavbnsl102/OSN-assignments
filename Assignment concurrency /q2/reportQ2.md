Name: Keshav bansal
roll: 2019101019

Q2:
Threads:
I have made one thread each for every student,company or vaccination zone

Mutexes:
I have made two array of mutexes, one for company and other for vaccination zones

Logic:

1. Student iterates over all the zones under the check of mutexes so that no 2 students get into the same vaccination zone at the same time to check availability and if it is not found, then, it goes back again into the while loop, thus,busy waiting for a zone to become avaialble.
2. Zone iterates over all the Companies under the check of mutexes so that no 2 zones take a batch from the same company at the same time to avoid conflicts and if it is found it busy waits till its capacity gets emptied and if it is not found it again busy waits to check availability in a company.
3. Company produces batches and busy waits and keeps a track of all the zones that it supplied to using an array in every company struct and produces again if the array gets emptied.

