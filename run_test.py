#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
sys.path.append('pytest')

import _dimmer
from dobj import dimmer

dimmer.set_power(0, 120)
dimmer.set_power(1, 210)
print(dimmer.status())
dimmer.zero_cross()
print(dimmer.status())
dimmer.timer_alarm()
print(dimmer.status())
dimmer.timer_alarm()
print(dimmer.status())
dimmer.timer_alarm()
print(dimmer.status())
