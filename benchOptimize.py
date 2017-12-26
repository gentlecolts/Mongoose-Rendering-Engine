#!/usr/bin/python3
import csv
from pprint import pprint
import math
import numpy
import multiprocessing

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

def bigfn(n,a,b,t):
	a=numpy.float128(a)
	b=numpy.float128(b)
	t=numpy.float128(t)
	
	b=b*n
	a2=a*a
	t3=t*t*t
	
	#print(a2,b,t3)
	#print(type(b),type(t3))
	
	exp1=pow(3.,9./2)*a2*b-4*t3
	x=cbrt(sqrt(b*exp1)/(2*pow(3.,9./4)*a2)-(exp1+2*t3)/(2*pow(3.,9./2)*a2*a))

	d=x+t*t/(27*a2*x)+t/(pow(3.,3./2)*a)
	return -d

def simplefn(n,a,b,t):
	a=numpy.float128(a)
	b=numpy.float128(b)
	t=numpy.float128(t)
	
	return a*cbrt(b*n+t)
	

#fn=bigfn
fn=simplefn

#observed data
y=[x for n,x in data]
mean=math.fsum(y)/len(y)

#sumSquare=0
#for yi in y:
#	sumSquare+=(yi-mean)**2
sumSquare=math.fsum((yi-mean)**2 for yi in y)

def r2(a,b,t):
	sumResidual=0
	#bleb=
	for n,x in data:
		#print(fn(n,a,b,t))
		sumResidual+=(x-fn(n,a,b,t))**2
	#sumResidual=math.fsum([(x-fn(n,a,b,t))**2 for x,n in data])
	
	#print(sumResidual,sumSquare)
	return 1-sumResidual/sumSquare

#print(r2(1,1,1))

def findBestInRange(amin,amax,bmin,bmax,tmin,tmax,n=10):
	amin=max(amin,0)
	amax=max(amax,0)
	bmin=max(bmin,0)
	bmax=max(bmax,0)
	tmin=max(tmin,0)
	tmax=max(tmax,0)
	
	best=[(math.inf,amin,bmin,tmin)]
	for a in numpy.linspace(amin,amax,n):
		for b in numpy.linspace(bmin,bmax,n):
			for t in numpy.linspace(tmin,tmax,n):
				r=1-r2(a,b,t)
				#print(r)
				if r<best[0][0]:#and a>0 and b>0 and t>0 and math.isfinite(r):
					best=[(r,a,b,t)]+best
					#print(best)
	#print(best[:3])
	return best[:3]

#print(findBestInRange(0,20,0,20,0,10,30))

def findBestGradient(a0,b0,t0,gmax,n=10):
	#print("in grad, got: ",a0,b0,t0,astep,bstep,tstep,gmax,n)
	
	def r(a,b,t):
		return 1-r2(a,b,t)
	
	h=numpy.float128(0.001)
	
	f=r(a0,b0,t0)
	da=(r(a0+h,b0,t0)-f)/h
	db=(r(a0,b0+h,t0)-f)/h
	dt=(r(a0,b0,t0+h)-f)/h
	
	#print(f,da,db,dt)
	
	def calcfn(y):
		a1=a0-y*da
		b1=b0-y*db
		t1=t0-y*dt
		r1=r(a1,b1,t1)
		if math.isfinite(r1):
			return (r1,a1,b1,t1)
		return None
	
	return [x for x in map(calcfn,numpy.linspace(0,gmax,n)) if x is not None]


a0=10
b0=10
t0=15
arange=a0
brange=b0
trange=t0
minAstep=4
minBstep=4
minTstep=4

scaleConst=1.5

nmax=60

best=[(math.inf,a0,b0,t0)]

n0=15
n=n0

gradN=100

def getnewbest(args):
	r,a0,b0,t0=args
	return findBestInRange(a0-arange,a0+arange,b0-brange,b0+brange,t0-trange,t0+trange,n)
	#return findBestGradient(a0,b0,t0,.1,gradN)

pool=multiprocessing.Pool()

while True:
	#newbest=[]
	#for r,a0,b0,t0 in best:
		#newbest+=findBestInRange(a0-arange,a0+arange,b0-brange,b0+brange,t0-trange,t0+trange,n)
		#newbest+=findBestGradient(a0,b0,t0,1,gradN)
	newbest=list(pool.map(getnewbest,best))[0]
	
	#add the original set too
	newbest+=best
	
	oldr,olda,oldb,oldt=best[0]
	
	#find the top elements
	best=sorted(newbest,key=lambda x:x[3])
	best=sorted(best,key=lambda x:x[0])[:5]
	
	r,a0,b0,t0=best[0]
	print("{}, {}, {}, r={}".format(a0,b0,t0,r))
	
	da=abs(a0-olda)*scaleConst
	db=abs(b0-oldb)*scaleConst
	dt=abs(t0-oldt)*scaleConst
	
	if False:
		arange=arange*scaleConst if da<minAstep else da
		brange=brange*scaleConst if db<minBstep else db
		trange=trange*scaleConst if dt<minTstep else dt
	else:
		arange=arange if da<minAstep else da
		brange=brange if db<minBstep else db
		trange=trange if dt<minTstep else dt
		
		if da<minAstep or db<minBstep or dt<minTstep:
			n*=scaleConst
			
			if n>=nmax:
				n=nmax
				arange/=scaleConst
				brange/=scaleConst
				trange/=scaleConst
		else:
			n=n0
