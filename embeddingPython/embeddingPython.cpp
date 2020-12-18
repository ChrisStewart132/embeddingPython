//program that links C functions usable to python, and allows python functions in the accompanied .py file to run and return values back to the main program.
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <windows.h>
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//python functions

//variables that the python functions can read/write
static int numargs = 0;
static const char* argv_contents[32];
static int numfish = 1;
//static std::vector<int> test;

/* Return the number of arguments of the application command line */
static PyObject* emb_numargs(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, ":numargs"))
        return NULL;
    return PyLong_FromLong(numargs);
}

static PyObject* emb_test(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, ":test"))
        return NULL;
    //PyObject* list = Py_BuildValue("[iis]", 1, 2, "three");
    Py_ssize_t n;
    n = numargs;//
    PyObject* list;
    list = PyList_New(n);
    for (int i = 0; i < n; i++) {
        PyList_SetItem(list, i, PyUnicode_FromString(argv_contents[i]));
        Py_DECREF(PyLong_FromLong(i));
    }
    return list;
}

static PyObject* emb_numfish(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, ":emb_numfish"))
        return NULL;
    return PyLong_FromLong(numfish);
}

static PyObject* emb_setFish(PyObject* self, PyObject* args)
{
    /*
    const char* s;
    if (!PyArg_ParseTuple(args, "s", &s))
        return NULL;
    printf("%s\n", s);*/
    if (!PyArg_ParseTuple(args, "i", &numfish))
        return NULL;
    return PyLong_FromLong(numfish);
}

static PyMethodDef EmbMethods[] = {
    {"numargs", emb_numargs, METH_VARARGS,
     "Return the number of arguments received by the process."},

    {"test", emb_test, METH_VARARGS,
     "test, test, test."},

     {"numfish", emb_numfish, METH_VARARGS,
     "get numfish."},

     {"setFish", emb_setFish, METH_VARARGS,
     "set numfish."},

    {NULL, NULL, 0, NULL}
};

static PyModuleDef EmbModule = {
    PyModuleDef_HEAD_INIT, "emb", NULL, -1, EmbMethods,
    NULL, NULL, NULL, NULL
};

static PyObject*
PyInit_emb(void)
{
    return PyModule_Create(&EmbModule);
}

//python functions
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//help functions

int runPythonFunction(std::string pythonFilename, std::string pythonFunctionName, std::vector<std::string> pythonFunctionArgs) {
    PyObject* pName, * pModule, * pFunc;
    PyObject* pArgs, * pValue;
    int i;
    pName = PyUnicode_DecodeFSDefault(pythonFilename.c_str());
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {//succesfully opened the python modle argv[1].py (pythonFunctions.py)
        pFunc = PyObject_GetAttrString(pModule, pythonFunctionName.c_str());
        /* pFunc is a new reference */
        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(pythonFunctionArgs.size());

            for (i = 0; i < pythonFunctionArgs.size(); i++) {
                pValue = PyLong_FromLong(atoi(pythonFunctionArgs[i].c_str()));
                if (!pValue) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return -1;
                }
                /* pValue reference stolen here: */
                PyTuple_SetItem(pArgs, i, pValue);
            }
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                int value = PyLong_AsLong(pValue);
                printf("Result of call: %ld\n", PyLong_AsLong(pValue));
                Py_DECREF(pValue);
                return value;
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr, "Call failed\n");
                return -1;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", pythonFunctionName);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", pythonFilename);
        return -1;
    }
}

//help functions
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {//requires to be called with args, either in properties debug arguments, or in command name.exe pythonFunctions multiple 3 2
    numargs = argc;
    for (int i = 0; i < numargs; i++) {
        argv_contents[i] = argv[i];
    }
 
    PyImport_AppendInittab("emb", &PyInit_emb);
    Py_Initialize();
    
    std::string exeName = std::string(argv[0]);
    std::string pythonFilename;//pythonFunctions
    std::string pythonFunctionName;//main
    std::vector<std::string> pythonFunctionArgs;//stores function argv[]

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //main loop
    while (1) {
        static int frame;
        frame++;
        
        for (int i = 0; i < numfish; i++) {
            //std::cout << "<>< ";
        }
        std::cout << "\n";
        pythonFunctionArgs.clear();
        //get args for python
        if (argc > 2) {
            pythonFilename = argv[1];
            pythonFunctionName = argv[2];
            for (int i = 3; i < argc; i++) {
                pythonFunctionArgs.push_back(argv[i]);
            }
        }
        else {
            /*
            std::cout << "enter python file name: ";
            std::cin >> pythonFilename;
            std::cout << "enter python function name: ";
            std::cin >> pythonFunctionName;
            */
            pythonFilename = std::string("pythonFunctions");
            pythonFunctionName = std::string("main");
            /*
            std::cout << "enter each argument (entering individually) for " << pythonFunctionName << "() followed by entering '-'...\n";
            int count = 0;
                       
            while (1) {
                std::string input;
                std::cin >> input;
                if (input == std::string("-")) {
                    break;
                }
                pythonFunctionArgs.push_back(input);
                count++;
            }
            */
            pythonFunctionArgs.push_back(std::to_string(frame));
            pythonFunctionArgs.push_back("1");

            argv_contents[0] = exeName.c_str();
            argv_contents[1] = pythonFilename.c_str();
            argv_contents[2] = pythonFunctionName.c_str();
            for (int i = 0; i < pythonFunctionArgs.size(); i++) {
                argv_contents[i + 3] = pythonFunctionArgs[i].c_str();
            }
            numargs = pythonFunctionArgs.size() + 3;
        }
        system("CLS");
        //numfish = runPythonFunction(pythonFilename, pythonFunctionName, pythonFunctionArgs);
        int temp = runPythonFunction(pythonFilename, pythonFunctionName, pythonFunctionArgs);
        numfish = stoi(pythonFunctionArgs[0]);
        Sleep(20);
    }
    //main loop
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
 
    if (Py_FinalizeEx() < 0) {
        return 120;
    }

    return 0;
}