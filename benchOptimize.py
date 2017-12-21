#!/usr/bin/python3
import csv
from pprint import pprint
import math
import numpy

data=[]
with open('bench.csv') as f:
	reader=csv.reader(f)
	for row in reader:
		data.append((int(row[0]),int(row[1])))

#pprint(data)

def cbrt(x):
	if x<0:
		return -((-x)**(1./3))
	else:
		return x**(1./3)
def sqrt(x):
	return x**(0.5)

def fn(n,a,b,t):
	b=b*n
	a2=a*a
	t3=t*t*t
	
	#print(a2,b,t3)
	#print(type(b),type(t3))
	
	exp1=pow(3.,9./2)*a2*b-4*t3
	x=cbrt(sqrt(b*exp1)/(2*pow(3.,9./4)*a2)-(exp1+2*t3)/(2*pow(3.,9./2)*a2*a))

	d=x+t*t/(27*a2*x)+t/(pow(3.,3./2)*a)
	return -d

#note: not actual coefficient of determination formula
def r2(a,b,t):
	r=0
	for n,x in data:
		r+=(fn(n,a,b,t)-x)**2
	return r

#print(r2(1,1,1))

def findBestInRange(amin,amax,bmin,bmax,tmin,tmax,n=10):
	best=[(math.inf,amin,bmin,tmin)]
	for a in numpy.linspace(amin,amax,n):
		for b in numpy.linspace(bmin,bmax,n):
			for t in numpy.linspace(tmin,tmax,n):
				r=r2(a,b,t)
				if r<best[0][0]:
					best=[(r,a,b,t)]+best
					#print(best)
	#print(best[:3])
	return best[:3]

#print(findBestInRange(0,20,0,20,0,10,30))


a0=15
b0=15
t0=10
arange=a0
brange=b0
trange=t0

best=[(math.inf,a0,b0,t0)]

for i in range(40):
	newbest=[]
	for r,a0,b0,t0 in best:
		newbest+=findBestInRange(
			a0-arange,a0+arange,
			b0-brange,b0+brange,
			t0-trange,t0+trange,
			15
		)
	#add the original set too
	newbest+=best
	
	#find the top elements
	best=sorted(newbest,key=lambda x:x[0])[:5]
	
	arange/=2
	brange/=2
	trange/=2
	
	r,a0,b0,t0=best[0]
	print("{}, {}, {}, r={}".format(a0,b0,t0,r))
