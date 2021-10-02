import numpy as np
from matplotlib import pyplot as plt
x,y,z = np.loadtxt('graph.txt', delimiter=',',unpack=True)
a=[]
b=[]
c=[]
d=[]
e=[]
f=[]
g=[]
h=[]
j=[]
k=[]
for i in range(len(x)):
    if x[i]==4:
        a.append(y[i])
        b.append(z[i])
for i in range(len(x)):
    if x[i]==5:
        c.append(y[i])
        d.append(z[i])
for i in range(len(x)):
    if x[i]==6:
        e.append(y[i])
        f.append(z[i])
for i in range(len(x)):
    if x[i]==7:
        g.append(y[i])
        h.append(z[i])
for i in range(len(x)):
    if x[i]==8:
        j.append(y[i])
        k.append(z[i])
plt.plot(a,b,marker='o',label='PID4')
plt.plot(c,d,marker='o',label='PID5')
plt.plot(e,f,marker='o',label='PID6')
plt.plot(g,h,marker='o',label='PID7')
plt.plot(j,k,marker='o',label='PID8')

plt.xlabel('Number of ticks from beginning')
plt.ylabel('QUEUE NUMBER')
plt.title('MLFQ SCHEDULING')
plt.legend()


plt.show()
