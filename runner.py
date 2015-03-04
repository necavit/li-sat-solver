#!/usr/bin/env python

import os

if __name__ == '__main__':
  params = [50, 100, 250, 500, 1000, 10000, 100000]
  for param in params:
    print 'executing with update rate: ' + str(param)
    os.system('time bin/sat ' + str(param) + ' < sample_problems/vars-250-5.cnf')
