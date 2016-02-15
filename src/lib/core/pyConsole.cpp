#include "pyConsole.h"
/*
 * Get callback functions
 * Maybe I can modify them in App class
 */
PyObject* call_import(PyObject* self, PyObject *args)
{
    return PyConsole::getInstance().import(self, args);
}

PyObject* call_getRendererInfo(PyObject* self, PyObject *args)
{
    return PyConsole::getInstance().getRendererInfo(self, args);
}

void call_startConsoleThread()
{
    PyConsole::getInstance().startConsoleThread();
}




/*Construct Python methods*/
PyMethodDef ConsoleMethods[] = {
    {"importScene", call_import, METH_VARARGS, "Import file into scene"},
    {"getRendererInfo", call_getRendererInfo, METH_NOARGS, "Get renderer info from scene"},
    {NULL, NULL, 0, NULL}
};

void PyConsole::runConsole()
{
    t = new std::thread(call_startConsoleThread);
}
void PyConsole::startConsoleThread()
{
    const char* consoleName = "console";
    Py_SetProgramName((char*)consoleName);
    Py_Initialize();
    Py_InitModule("console", ConsoleMethods);
    //PyRun_InteractiveLoop(stdin, "<stdin>");
    PyRun_SimpleString("execfile('scripts/do.py')");
    Py_Finalize();
};
