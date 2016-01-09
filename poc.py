#!/usr/bin/env python
# -*- coding: utf-8 -*-


PIN_NUM = 13
  
# stop_idx -> power
stops = [0] * PIN_NUM
# pin_idx -> stop
pin2stop = [0] * PIN_NUM
# stop_idx -> pin[]
stop2pins = [[0] * PIN_NUM for x in range(PIN_NUM)]


def set_power(pin, power):
    pin_idx = pin - 1
    if pin2stop[pin_idx]:
        # remove pin from stop data
        stop = pin2stop[pin_idx]
        stop_pins = stop2pins[stop-1]
        zero_idx = 0;
        for idx in range(PIN_NUM):
            if stop_pins[idx] == pin:
                stop_pins[idx] = 0
                zero_idx = idx
            elif stop_pins[idx] == 0:
                prev_idx = idx - 1
                if prev_idx > zero_idx:
                    # move last value to empty slot
                    stop_pins[zero_idx] = stop_pins[prev_idx]
                    stop_pins[prev_idx] = 0
                break
        # remove stop if empty
        if zero_idx == 0:
            stops[stop-1] = 0
        pin2stop[pin_idx] = 0

    if power == 0:
        return

    # find empty stop or stop with the same power
    for idx in range(PIN_NUM):
        if stops[idx] == power:
            pass
        elif stops[idx] == 0:
            stops[idx] = power
        else:
            continue
        stop = idx + 1
        break
    assert stop

    pin2stop[pin_idx] = stop

    # add pin to stop data at first empty slot
    stop_pins = stop2pins[stop-1]
    for idx in range(PIN_NUM):
        if stop_pins[idx] == 0:
            stop_pins[idx] = pin
            break
    else:
        raise Exception('no empty pins in stop')


def pr_status():
    def norzero(li):
        nli = list(li)
        while nli and nli[-1] == 0:
            nli.pop()
        return nli

    print('stops: %s' % stops)
    print('pin stops: %s' % pin2stop)
    print('stop pins: %s' % dict(
        (idx, norzero(vals))
        for idx, vals in enumerate(stop2pins, 1)))

def test():
    #import pudb;pu.db
    pr_status()
    set_power(1, 128)
    set_power(2, 128)
    set_power(3, 128)
    set_power(4, 128)
    set_power(5, 128)
    pr_status()
    set_power(2, 129)
    set_power(4, 129)
    pr_status()
    set_power(1, 0)
    set_power(2, 0)
    set_power(3, 0)
    set_power(4, 0)
    set_power(5, 0)
    pr_status()


if __name__ == "__main__":
    test()
