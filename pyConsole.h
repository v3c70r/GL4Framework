#pragma once
#include <python2.7/Python.h>
#include "scene.h"
#include "importer.hpp"


class PyConsole
{
private:
    static Scene *scene;
public:
    static void setScene(Scene *s) {scene = s;}

    static PyObject* import(PyObject* self, PyObject *args)
    {
        PyObject *path = nullptr;
        if (!PyArg_UnpackTuple(args, "import", 1, 1, &path))
        {
            std::cout<<"Wrong parameters\n";
        }
        else
        {
            Importer importer;
            importer.setScene(scene);
            importer.import( PyString_AsString(path));
        }
        return Py_BuildValue("");
    }

    static void runConsole();
};


