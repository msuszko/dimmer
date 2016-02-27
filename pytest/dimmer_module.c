#include <Python.h>
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <unistd.h>


#include "power.c"


static char module_docstring[] =
    "This module provides an interface to test AVR uC dimmer code";
static char pyt_power_init_docstring[] =
    "Initialize power module.";
static char pyt_set_power_docstring[] =
    "set_power(pin, level) Set power level (0-255) to a pin (0-PIN_NUM).";
static char pyt_zero_cross_docstring[] =
    "Simulate zero cross detetection interrupt.";
static char pyt_timer_alarm_docstring[] =
    "Simulate timer overflow interrupt.";
static char pyt_get_globals_docstring[] =
    "returns (OCR1A, first_stop, next_stop, off_midstop, PORTB, PORTC, PORTD)";
static char pyt_get_pin2stop_docstring[] =
    "returns (pin, ...) - pin_idx to stop_numer mapping";
static char pyt_get_stop2pins_docstring[] =
    "returns ((pin_status, ...), ...) - stop to pin status (True or False)";
static char pyt_get_stop2masks_docstring[] =
    "returns ((pin_mask, ...), ...) - stop to pin masks";
static char pyt_get_stops_docstring[] =
    "returns ((next, power), ...) - stop to next stop and power";
static char pyt_get_pin_masks_docstring[] =
    "returns (pin_mask, ...)";
static char pyt_get_ports_docstring[] =
    "returns ((port, pin_mask), ...) for each pin";

static PyObject *pyt_power_init(PyObject *self);

static PyObject *pyt_set_power(PyObject *self, PyObject *args);

static PyObject *pyt_zero_cross(PyObject *self);

static PyObject *pyt_timer_alarm(PyObject *self);

static PyObject *pyt_get_globals(PyObject *self);

static PyObject *pyt_get_pin2stop(PyObject *self);

static PyObject *pyt_get_stop2pins(PyObject *self);

static PyObject *pyt_get_stop2masks(PyObject *self);

static PyObject *pyt_get_stops(PyObject *self);

static PyObject *pyt_get_pin_masks(PyObject *self);

static PyObject *pyt_get_ports(PyObject *self);


static PyMethodDef module_methods[] = {
    {"power_init", (PyCFunction)pyt_power_init, METH_NOARGS, pyt_power_init_docstring},
    {"set_power", (PyCFunction)pyt_set_power, METH_VARARGS, pyt_set_power_docstring},
    {"zero_cross", (PyCFunction)pyt_zero_cross, METH_NOARGS, pyt_zero_cross_docstring},
    {"timer_alarm", (PyCFunction)pyt_timer_alarm, METH_NOARGS, pyt_timer_alarm_docstring},
    {"get_globals", (PyCFunction)pyt_get_globals, METH_NOARGS, pyt_get_globals_docstring},
    {"get_pin2stop", (PyCFunction)pyt_get_pin2stop, METH_NOARGS, pyt_get_pin2stop_docstring},
    {"get_stop2pins", (PyCFunction)pyt_get_stop2pins, METH_NOARGS, pyt_get_stop2pins_docstring},
    {"get_stop2masks", (PyCFunction)pyt_get_stop2masks, METH_NOARGS, pyt_get_stop2masks_docstring},
    {"get_stops", (PyCFunction)pyt_get_stops, METH_NOARGS, pyt_get_stops_docstring},
    {"get_pin_masks", (PyCFunction)pyt_get_pin_masks, METH_NOARGS, pyt_get_pin_masks_docstring},
    {"get_ports", (PyCFunction)pyt_get_ports, METH_NOARGS, pyt_get_ports_docstring},
    {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC PyInit__dimmer(void)
{
    
    PyObject *module;
    static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_dimmer",
        module_docstring,
        -1,
        module_methods,
        NULL,
        NULL,
        NULL,
        NULL
    };
    module = PyModule_Create(&moduledef);
    if (!module) return NULL;

    return module;
}


static PyObject *
pyt_power_init(PyObject *self)
{
	power_init();
	Py_RETURN_NONE;
}


static PyObject *
pyt_set_power(PyObject *self, PyObject *args)
{
	int8_t result, pin;
	short int short_pin;
	uint8_t power;

	if (!PyArg_ParseTuple(args, "hb", &short_pin, &power))
		return NULL;
	if (short_pin < 0 || short_pin > PIN_NUM) {
		PyErr_SetString(PyExc_ValueError,
		        		"pin has to be between 0 and PIN_NUM");
		return NULL;
	}
	pin = (int8_t)short_pin;

	result = set_power(pin, power);
	PyObject *ret = Py_BuildValue("h", (short int)result);
    return ret;
}


static PyObject *
pyt_zero_cross(PyObject *self)
{
	zero_cross();
	Py_RETURN_NONE;
}


static PyObject *
pyt_timer_alarm(PyObject *self)
{
	timer_alarm();
	Py_RETURN_NONE;
}

static PyObject *
pyt_get_globals(PyObject *self)
{
	PyObject *ret = Py_BuildValue("hhhhBBB",
			(short int)OCR1A, (short int)first_stop,
		    (short int)next_stop, (short int)off_midstop,
		    PORTB, PORTC, PORTD);
    return ret;
}

static PyObject *
pyt_get_pin2stop(PyObject *self)
{
	PyObject* t;

	t = PyTuple_New(PIN_NUM);
	for (short pin=0; pin<PIN_NUM; pin++) {
		PyTuple_SetItem(t, pin, PyLong_FromLong((long)pin2stop[pin]));
	}
	return t;
}

static PyObject *
pyt_get_stop2masks(PyObject *self)
{
	PyObject* t;
	short stop;

	t = PyTuple_New(PIN_NUM);
	for (stop=0; stop<PIN_NUM; stop++) {
		PyTuple_SetItem(t, stop, PyByteArray_FromStringAndSize((char*)stop2pins[stop], 3));
	}
	return t;
}

static PyObject *
pyt_get_stop2pins(PyObject *self)
{
	PyObject* t;
	PyObject* pins_t;
	PyObject* state;
	short stop, pin;
	uint8_t mask;

	t = PyTuple_New(PIN_NUM);
	for (stop=0; stop<PIN_NUM; stop++) {
		pins_t = PyTuple_New(PIN_NUM);
		for (pin=0; pin<PIN_NUM; pin++) {
			mask = stop2pins[stop][ports[pin].port_no];
			if ((mask & (ports[pin].mask)) == 0) {
				state = Py_False;
			} else {
				state = Py_True;
			}
			PyTuple_SetItem(pins_t, pin, state);
		}
		PyTuple_SetItem(t, stop, pins_t);
	}
	return t;
}

static PyObject *
pyt_get_stops(PyObject *self)
{
	PyObject* t;
	PyObject* pair_t;
	spoint stop_pair;

	t = PyTuple_New(PIN_NUM);
	for (short stop=0; stop<PIN_NUM; stop++) {
		stop_pair = stops[stop];
		pair_t = PyTuple_New(2);
		PyTuple_SetItem(pair_t, 0, PyLong_FromLong((long)(stop_pair.next)));
		PyTuple_SetItem(pair_t, 1, PyLong_FromLong((long)(stop_pair.power)));
		PyTuple_SetItem(t, stop, pair_t);
	}
	return t;
}

static PyObject *
pyt_get_pin_masks(PyObject *self)
{
	PyObject* t;
	t = PyByteArray_FromStringAndSize((char*)pin_mask, 3);
	return t;

}

static PyObject *
pyt_get_ports(PyObject *self)
{
	PyObject* t;
	PyObject* pair_t;

	t = PyTuple_New(PIN_NUM);
	for (short pin=0; pin<PIN_NUM; pin++) {
		pair_t = PyTuple_New(2);
		PyTuple_SetItem(pair_t, 0, PyLong_FromLong((long)(ports[pin].port_no)));
		PyTuple_SetItem(pair_t, 1, PyLong_FromUnsignedLong((unsigned long)(ports[pin].mask)));
		PyTuple_SetItem(t, pin, pair_t);
	}
	return t;
}
