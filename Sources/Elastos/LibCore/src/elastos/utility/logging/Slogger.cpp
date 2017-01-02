//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

namespace Elastos {
namespace Utility {
namespace Logging {

extern ECode __PrintlnImpl(Int32 bufID, Int32 priority, const char * tag, const char *msgBuf);

#define MSG_BUF_SIZE    1024

ECode Slogger::D(const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(Logger::LOG_ID_SYSTEM, Logger::___DEBUG, tag, msgBuf);
}

/**
 * Send an ERROR log message.
 */
ECode Slogger::E(const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(Logger::LOG_ID_SYSTEM, Logger::ERROR, tag, msgBuf);
}

/**
 * Send an INFO log message.
 */
ECode Slogger::I(const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(Logger::LOG_ID_SYSTEM, Logger::INFO, tag, msgBuf);
}

/**
 * Send an VERBOSE log message.
 */
ECode Slogger::V(const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(Logger::LOG_ID_SYSTEM, Logger::VERBOSE, tag, msgBuf);
}

/**
 * Send a WARN log message.
 */
ECode Slogger::W(const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(Logger::LOG_ID_SYSTEM, Logger::WARN, tag, msgBuf);
}

/**
 * Low-level logging call.
 */
ECode Slogger::Println(Int32 priority, const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(Logger::LOG_ID_SYSTEM, priority, tag, msgBuf);
}

} // Logging
} // Utility
} // Elastos
