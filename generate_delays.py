#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division

from math import pi, asin
from textwrap import wrap

to_zero_cross_us = 310
period_us = 10000  # 100Hz
clock = 16000000  # 16MHz
divider = 8
tick = (clock/1000000)/divider
steps = 255
period_tick = period_us * tick

def delay(step):
    delay = 1 - (asin(step/(steps/2)-1)+pi/2)/pi
    delay_tick = period_tick * delay
    return delay_tick + to_zero_cross_us * tick


def main():
    if (period_tick - delay(1)) < 0:
        print('WARNING: last power level starts %dus after '
              'zero-crossing interrupt!' % ((delay(1) - period_tick)/tick))
    print('unsigned int delays[] = {')
    values = ', '.join("%d" % round(delay(p)) for p in range(steps+1))
    for line in wrap(values, 74):
        print('\t%s' % line)
    print('};')

if __name__ == "__main__":
    main()
