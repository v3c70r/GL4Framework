/*
 * Log stuff;
 */
#pragma once
#define GL_LOG_FILE "gl.log"

namespace LOG
{
    bool writeLog (const char* message, ...);
    bool writeLogErr (const char* message, ...);
    bool restartLog();
}

