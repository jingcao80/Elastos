
#ifndef __ELASTOS_UTILITY_LOGGING_SLOGGER_H__
#define __ELASTOS_UTILITY_LOGGING_SLOGGER_H__

#include <elastos.h>

#ifndef __ELASTOS_UTILITY_LOGGING_SLOGGER_MACRO__
#define __ELASTOS_UTILITY_LOGGING_SLOGGER_MACRO__
#define SLOGGERD(tag, ...)  Elastos::Utility::Logging::Slogger::D(tag, __VA_ARGS__);
#define SLOGGERE(tag, ...)  Elastos::Utility::Logging::Slogger::E(tag, __VA_ARGS__);
#define SLOGGERI(tag, ...)  Elastos::Utility::Logging::Slogger::I(tag, __VA_ARGS__);
#define SLOGGERV(tag, ...)  Elastos::Utility::Logging::Slogger::V(tag, __VA_ARGS__);
#define SLOGGERW(tag, ...)  Elastos::Utility::Logging::Slogger::W(tag, __VA_ARGS__);
#endif //__ELASTOS_UTILITY_LOGGING_SLOGGER_MACRO__

namespace Elastos {
namespace Utility {
namespace Logging {

class ECO_PUBLIC Slogger
{
public:
    /*!
     * @brief Send a DEBUG log message.
     * @param tag
     * @param msg
     */
    static CARAPI D(const char * tag, const char *fmt, ...);

    /*!
     * @brief Send an ERROR log message.
     * @param tag
     * @param msg
     */
    static CARAPI E(const char * tag, const char *fmt, ...);

    /*!
     * @brief Send an INFO log message.
     * @param tag
     * @param msg
     */
    static CARAPI I(const char * tag, const char *fmt, ...);

    /*!
     * @brief Send an VERBOSE log message.
     * @param tag
     * @param msg
     */
    static CARAPI V(const char * tag, const char *fmt, ...);

    /*!
     * @brief Send a WARN log message.
     * @param tag
     * @param msg
     */
    static CARAPI W(const char * tag, const char *fmt, ...);

    /*!
     * @brief Low-level logging call.
     * @param priority
     * @param tag
     * @param msg
     */
    static CARAPI Println(Int32 priority, const char * tag, const char *fmt, ...);

private:
    ECO_LOCAL Slogger();
    ECO_LOCAL Slogger(const Slogger&);
};

} // Logging
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_LOGGING_ELASTOS_UTILITY_SLOGGER_H__
