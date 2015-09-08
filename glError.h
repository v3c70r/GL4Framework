#pragma once
/* http://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/
 * This simple tool is used to find an OpenGL Error*/
void _check_gl_error(const char *file, int line);
 
///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)
