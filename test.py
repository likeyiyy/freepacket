#!/usr/bin/env python
# coding=utf-8

import commands
pktlen = [64,128,256,512,1024,1280,1518]

generator   = range(1,9,1)
parser      = range(1,9,1)
manager     = range(1,9,1)

gpool_size    = range(1000000,11000000,1000000)
pqueue_length = range(1000000,11000000,1000000)

counter = 0

exename = './simulation'

def averages(listn):
    sums = []
    for item in listn:
        temp = str(item).split(":")[1]
        if temp != 0:
            sums.append(int(temp))
    return sum(sums) / len(sums)

list64 = []
args64 = []

p = 64
fresult = open('result','w')
fargs   = open('fargs','w')
if p == 64:
    for g in generator:
        for pa in parser:
            for m in manager:
                for gp in gpool_size:
                    for pq in pqueue_length:
                        counter += 1
                        print [counter]
                        exe = '%s --pktlen %d -g %d -p %d -m %d --gpool_size %d --pqueue_length %d' % (exename,(p),(g),(pa),(m),(gp),(pq))
                        status,result = commands.getstatusoutput(exe)
                        listn =  result.split("\n")

                        max = averages(listn)
                        maxargs = [p,g,pa,m,gp,pq]
                        
                        list64.append(max)
                        args64.append(maxargs)

                        fresult.write(str(max)+'\n')
			fresult.flush()
                        fargs.write(str(maxargs)+'\n')
			fargs.flush()


			#print list64
			#print args64
    print list64

