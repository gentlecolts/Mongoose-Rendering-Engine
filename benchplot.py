#!/usr/bin/python3
import matplotlib.pyplot as plt
import math
import csv

x=[]
ys={}

with open('bench.csv') as f:
	dat=[]
	reader=csv.reader(f)
	for row in reader:
		x.append(float(row[0]))
		dat.append(float(row[1]))
	ys["raw"]=dat

rt3=math.sqrt(3.)

def logfn(x):
	return math.log(x,2)

def sqrtfn(x):
	return .9*math.sqrt(rt3*x/60)

def cbrt(x):
	if x<0:
		return -((-x)**(1/3))
	else:
		return x**(1/3)

def crazyfn(x):
	A=21
	B=10.5*x
	T=7
	#X=cbrt(math.sqrt(-B*(4*T**3-3**(9/2)*A**2*B))/(2*3**(9/4)*A**2)-(3**(9/2)*A**2*B-2*T**3)/(2*3**(9/2)*A**3))
	
	
	A2=A*A
	T3=T*T*T
	exp1=3**(9/2)*A2*B-4*T3
	X=cbrt(math.sqrt(B*exp1)/(2*3**(9/4)*A2)-(exp1+2*T3)/(2*3**(9/2)*A2*A))
	
	return -(X+T*T/(27*A2*X)+T/(3**1.5*A))

fns=[logfn,sqrtfn,crazyfn]

for fn in fns:
	ys[fn.__name__]=list(map(fn,x))

#print(x)
#print(ys)

for name,y in ys.items():
	plt.plot(x,y,label=name)

plt.legend()
plt.show()
