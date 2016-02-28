#!/usr/bin/env python
# -*- coding: utf-8 -*-

from collections import namedtuple

dglobals = namedtuple('globals', 'OCR1A first_stop next_stop off_midstop '
                                 'PORTB PORTC PORTD')

import _dimmer


class _Dimmer(object):

    def __init__(self):
        _dimmer.power_init()

    def set_power(self, pin, power):
        _dimmer.set_power(pin, power)

    def zero_cross(self):
        _dimmer.zero_cross()

    def timer_alarm(self):
        _dimmer.timer_alarm()

    def status(self):
        return dglobals(*_dimmer.get_globals())

    @property
    def power_stops(self):
        stops = _dimmer.get_stops()
        idx = self.status().first_stop
        path = []
        while idx != -1:
            idx, power = stops[idx]
            path.append(power)
        return path

dimmer = _Dimmer()
