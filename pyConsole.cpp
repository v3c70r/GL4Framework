#include "pyConsole.h"
PyMethodDef ConsoleMethods[] = {
    {"import", PyConsole::import, METH_VARARGS, "Import file into scene"},
    {NULL, NULL, 0, NULL}
};
Scene* PyConsole::scene=nullptr;
void PyConsole::runConsole()
{
    Py_SetProgramName("console");
    Py_Initialize();
    Py_InitModule("console", ConsoleMethods);
    PyRun_InteractiveLoop(stdin, "<stdin>");
    Py_Finalize();

}
