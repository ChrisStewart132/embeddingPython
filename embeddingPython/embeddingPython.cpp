// embeddingPython.cpp : This file contains the 'main' function. Program execution begins and ends there.
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include <iostream>
#include <string>
#include <vector>
//
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
//
// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//python functions
static int numargs = 0;
static const char* argv_contents[32];
static std::vector<int> test;

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

static PyMethodDef EmbMethods[] = {
    {"numargs", emb_numargs, METH_VARARGS,
     "Return the number of arguments received by the process."},

    {"test", emb_test, METH_VARARGS,
     "test, test, test."},

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
                    return 1;
                }
                /* pValue reference stolen here: */
                PyTuple_SetItem(pArgs, i, pValue);
            }
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("Result of call: %ld\n", PyLong_AsLong(pValue));
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr, "Call failed\n");
                return 1;
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
        return 1;
    }
}



//help functions
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {//requires to be called with args, either in properties debug arguments, or in command name.exe pythonFunctions multiple 3 2...
    //args 0:exe, 1:pythonFunctions, 2:multiply, 3:argv[0]...
    std::string exeName = argv[0];
    std::string pythonFilename;//pythonFunctions
    std::string pythonFunctionName;//main
    std::vector<std::string> pythonFunctionArgs;//stores function argv[]

    //get args for python
    if (argc > 2) {
        pythonFilename = argv[1];
        pythonFunctionName = argv[2];
        for (int i = 3; i < argc; i++) {
            pythonFunctionArgs.push_back(argv[i]);
        }
    }
    else{
        /*
        std::cout << "enter python file name: ";      
        std::cin >> pythonFilename;
        std::cout << "enter python function name: ";
        std::cin >> pythonFunctionName;
        */
        pythonFilename = std::string("pythonFunctions");
        pythonFunctionName = std::string("main");
        std::cout << "enter arguments followed by 'end'...\n";
        int count = 0;
        while (1) {
            std::string input;
            std::cin >> input;
            if (input == std::string("end")) {
                break;
            }
            pythonFunctionArgs.push_back(input);
            count++;
        }
    }
    
    numargs = argc;
    for (int i = 0; i < numargs; i++) {
        argv_contents[i] = argv[i];
    }



    PyImport_AppendInittab("emb", &PyInit_emb);
    Py_Initialize();
 
    runPythonFunction(pythonFilename, pythonFunctionName, pythonFunctionArgs);

    if (Py_FinalizeEx() < 0) {
        return 120;
    }



    return 0;
}








////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int main1(int argc, char* argv[]) {//requires to be called with args, either in properties debug arguments, or in command name.exe pythonFunctions multiple 3 2...
    PyObject* pName, * pModule, * pFunc;
    PyObject* pArgs, * pValue;
    int i;

    if (argc < 3) {
        fprintf(stderr, "Usage: call pythonfile funcname [args]\n");
        return 1;
    }

    numargs = argc;
    for (int i = 0; i < numargs; i++) {
        argv_contents[i] = argv[i];
    }

    test.push_back(69);
    test.push_back(69);

    PyImport_AppendInittab("emb", &PyInit_emb);

    Py_Initialize();

    pName = PyUnicode_DecodeFSDefault(argv[1]);
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {//succesfully opened the python modle argv[1].py (pythonFunctions.py)
        pFunc = PyObject_GetAttrString(pModule, argv[2]);
        /* pFunc is a new reference */
        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(argc - 3);
            for (i = 0; i < argc - 3; ++i) {
                pValue = PyLong_FromLong(atoi(argv[i + 3]));
                if (!pValue) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return 1;
                }
                /* pValue reference stolen here: */
                PyTuple_SetItem(pArgs, i, pValue);
            }
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("Result of call: %ld\n", PyLong_AsLong(pValue));
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr, "Call failed\n");
                return 1;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
        return 1;
    }
    if (Py_FinalizeEx() < 0) {
        return 120;
    }
    return 0;
}


int main2(int argc, char* argv[])
{
    wchar_t* program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    Py_SetProgramName(program);  // optional but recommended 
    Py_Initialize();

    PyRun_SimpleString("from time import time,ctime\n"
        "print('Today is', ctime(time()))\n");

    std::string input;
    while (1) {
        std::cout << ">>> ";
        std::getline(std::cin, input);
        PyRun_SimpleString(input.c_str());
    }

    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
    PyMem_RawFree(program);

    std::cout << "end\n";
    std::cin >> input;
    return 0;
}


int main3(int argc, char* argv[]) {//requires to be called with args, either in properties debug arguments, or in command name.exe pythonFunctions multiple 3 2...
    PyObject* pName, * pModule, * pFunc;
    PyObject* pArgs, * pValue;
    int i;

    //args 0:exe, 1:pythonFunctions, 2:multiply, 3:argv[0]...
    std::string exeName = argv[0];
    std::string pythonFilename;//pythonFunctions
    std::string pythonFunctionName;//main
    std::vector<std::string> pythonFunctionArgs;//stores function argv[]

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
        std::cout << "enter arguments followed by 'end'...\n";
        int count = 0;
        while (1) {
            std::string input;
            std::cin >> input;
            if (input == std::string("end")) {
                break;
            }
            pythonFunctionArgs.push_back(input);
            count++;
        }
    }


    numargs = argc;
    for (int i = 0; i < numargs; i++) {
        argv_contents[i] = argv[i];
    }

    PyImport_AppendInittab("emb", &PyInit_emb);

    Py_Initialize();

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
                    return 1;
                }
                /* pValue reference stolen here: */
                PyTuple_SetItem(pArgs, i, pValue);
            }
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("Result of call: %ld\n", PyLong_AsLong(pValue));
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr, "Call failed\n");
                return 1;
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
        return 1;
    }


    if (Py_FinalizeEx() < 0) {
        return 120;
    }

    return 0;
}