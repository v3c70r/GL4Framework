/*
 * Log stuff;
 */
#pragma once
#define GL_LOG_FILE "gl.log"
#include <string>

namespace LOG
{
    bool writeLog (const char* message, ...);
    bool writeLog (const std::string message, ...);

    bool writeLogErr (const char* message, ...);
    bool writeLogErr (const std::string message, ...);
    bool restartLog();
}

