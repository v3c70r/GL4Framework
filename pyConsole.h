#pragma once
#include <python2.7/Python.h>
#include "scene.h"
#include "importer.hpp"
#include <thread>
#include <functional>
#include <queue>

class PyConsole
{
private:
    Scene *scene;
    //Importer *importer;
    std::thread *t;
    std::queue<std::function<void()>> functionQueue;

    template<typename _Callable, typename... _Args>
    void QueueFunction(_Callable&& __f, _Args&&... __args)
    {
        std::function<void()> func = std::bind(std::forward<_Callable>(__f),
                std::forward<_Args>(__args)...);
        functionQueue.push(func);

        std::cout<<functionQueue.size()<<std::endl;
    }
    
public:
    //Singletone
    static PyConsole& getInstance()
    {
        static PyConsole instance;
        return instance;
    }
    PyConsole():
        scene(nullptr) {}
    PyConsole( PyConsole const&)        =    delete;
    void operator=(PyConsole const&)    =    delete;

    void setScene(Scene *s) {scene = s;}
    PyObject* import(PyObject* self, PyObject *args)
    {
        PyObject *path = nullptr;
        if (!PyArg_UnpackTuple(args, "import", 1, 1, &path))
        {
            std::cout<<"Wrong parameters\n";
        }
        else
        {
            auto func = [] (const std::string &fileName, Scene *s)
            {
                Importer importer;
                importer.setScene(s);
                importer.import(fileName );
            };
            QueueFunction(func,PyString_AsString(path), scene);
        }
        return Py_BuildValue("");
    }

    void runConsole();
    void startConsoleThread();

    //Need to be called in main thread
    void callFunctions()
    {
        while(!functionQueue.empty())
        {
            std::cout<<functionQueue.size()<<std::endl;
            std::function<void()> func = functionQueue.front();
            functionQueue.pop();
            func();
        }
    }


    ~PyConsole()
    {
        t->join();
        delete t;
    }
};


