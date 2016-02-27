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

static PyObject *pyt_power_init(PyObject *self, PyObject *args);

static PyObject *pyt_set_power(PyObject *self, PyObject *args);

static PyObject *pyt_zero_cross(PyObject *self, PyObject *args);

static PyObject *pyt_timer_alarm(PyObject *self, PyObject *args);


/*
void power_init(void);

int8_t set_power(int8_t pin, uint8_t power);

void zero_cross(void);

void timer_alarm(void);
*/

/*
const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

int main(void) {
	power_init();
	printf("first_stop: %hhd, next_stop: %hhd\n", first_stop, next_stop);
	set_power(1, 128);
	set_power(2, 127);
	printf("set_power(1, 128)\n");
	printf("first_stop: %hhd, next_stop: %hhd\n", first_stop, next_stop);
	printf("stop2pins[next_stop][2]: %s\n", byte_to_binary(stop2pins[next_stop][2]));
	zero_cross();
	printf("zero_cross()\n");
	printf("first_stop: %hhd, next_stop: %hhd\n", first_stop, next_stop);
	
	printf("timer alarm at: %hd\n", OCR1A);
	timer_alarm();
	printf("PORTD: %s\n", byte_to_binary(PORTD));

	printf("timer alarm at: %hd\n", OCR1A);
	timer_alarm();
	printf("PORTD: %s\n", byte_to_binary(PORTD));

	printf("plunk\n");
	return 0;
}

*/

static PyMethodDef module_methods[] = {
    {"power_init", pyt_power_init, METH_VARARGS, pyt_power_init_docstring},
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
pyt_power_init(PyObject *self, PyObject *args)
{
	power_init();
    return NULL;
}
