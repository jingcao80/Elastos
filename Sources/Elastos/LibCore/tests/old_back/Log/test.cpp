#include <elastos/Logger.h>
#include <elastos/Slogger.h>

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <elstring.h>
#include <elcstring.h>

using namespace Elastos;
using namespace Elastos::Utility::Logging;

int create_dir(const char * name) {
    if (access(name, F_OK) == 0)
        return 0;
    return (mkdir(name, 0755) < 0) ? -1 : 0;
}

int create_file(const char * name) {
    if (access(name, F_OK) == 0)
        return 0;
    return (open(name, O_CREAT | O_WRONLY | O_TRUNC) < 0) ? -1 : 0;
}

int create_log_files() {
    if (create_dir("/elastos/log") < 0)
        return -1;
    if (create_file("/elastos/log/main") < 0)
        return -1;
    if (create_file("/elastos/log/radio") < 0)
        return -1;
    if (create_file("/elastos/log/events") < 0)
        return -1;
    if (create_file("/elastos/log/system") < 0)
        return -1;
    return 0;
}

int main() {
    if (create_log_files() < 0) {
        printf("<err> create log files failed.\n");
        return -1;
    }

    // Test Logger
    Logger::D("Logger::D", "debug message.\n");
    Logger::D("Logger::D", "debug format message: %d %c %s.\n", 1, 'A',
            "DEBUG");
    Logger::E("Logger::E", "error message.\n");
    Logger::E("Logger::E", "error format message: %d %c %s.\n", 2, 'B',
            "ERROR");
    Logger::I("Logger::I", "information message.\n");
    Logger::I("Logger::I", "information format message: %d %c %s.\n", 3, 'C',
            "INFO");
    Logger::V("Logger::V", "verbose message.\n");
    Logger::V("Logger::V", "verbose format message: %d %c %s.\n", 4, 'D',
            "VERBOSE");
    Logger::W("Logger::W", "warning message.\n");
    Logger::W("Logger::W", "warning format message: %d %c %s.\n", 5, 'E',
            "WARN");

    Slogger::D("Slogger::D", "system debug message.\n");
    Slogger::D("Slogger::D", "system debug format message: %d %c %s.\n", 1, 'A',
            "DEBUG");
    Slogger::E("Slogger::E", "system error message.\n");
    Slogger::E("Slogger::E", "system error format message: %d %c %s.\n", 2, 'B',
            "ERROR");
    Slogger::I("Slogger::I", "system information message.\n");
    Slogger::I("Slogger::I", "system information format message: %d %c %s.\n",
            3, 'C', "INFO");
    Slogger::V("Slogger::V", "system verbose message.\n");
    Slogger::V("Slogger::V", "system verbose format message: %d %c %s.\n", 4,
            'D', "VERBOSE");
    Slogger::W("Slogger::W", "system warning message.\n");
    Slogger::W("Slogger::W", "system warning format message: %d %c %s.\n", 5,
            'E', "WARN");

    String str("A String message.\n");
    CString cstr("A CString message.\n");
    Logger::D("String", str);
    Slogger::D("CString", cstr);

    LOGD("LOGD", " Test macro LogD without format.\n");
    LOGD("LOGD", " >> test macro %s\n", "LOGD");
    SLOGD("SLOGD", " Test macro SLogD without format.\n");
    SLOGD("SLOGD", " >> test macro %s\n", "SLOGD");

    printf(">>>>> Test Logger ok.\n");
    return 0;
}
