#ifndef __CAR_BRIDGE_ERROR_H
# define __CAR_BRIDGE_ERROR_H

# include <cstdarg>

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

class Error {
public:
    enum Type {
        TYPE_INTRINSIC,
        TYPE_ELASTOS
    };

    enum {
        NONE = 0,
        FAILED,
        INCOMPATIBLE_VERSION,
        NO_MATCHING_FUNCTION_FOR_CALL,
        AMBIGUOUS_CALL_OF_OVERLOADED_FUNCTION,
        INVALID_RECEIVER,
        INVALID_ARGUMENT,
        INVALID_FORMAT,
        NONENTITY,
        NO_MEMORY,
        CANCELLED,
        TIMED_OUT
    };

    static int const MAX_MESSAGE_LENGTH = 256;

    static char const *GetIntrinsicErrorName(int code) noexcept;

    Error(void) noexcept;

    Error(Error const &error) noexcept;

    Error(Error &&error) noexcept;

    Error(enum Type type, int code, char const *messageFormat, ...) noexcept;

    Error(int code, char const *messageFormat, ...) noexcept;

    Error &operator=(Error const &error) noexcept;

    Error &operator=(Error &&error) noexcept;

    operator enum Type() const noexcept
    {
        return _type;
    }

    operator int() const noexcept
    {
        return _code;
    }

    operator char const *() const noexcept
    {
        return _message;
    }

    void Set(enum Type type, int code, char const *messageFormat, ...) noexcept;

    void Set(int code, char const *messageFormat, ...) noexcept;

    void Set(enum Type type, int code, char const *messageFormat, va_list ap) noexcept;

    void Set(int code, char const *messageFormat, va_list ap) noexcept;

    enum Type type() const noexcept
    {
        return _type;
    }

    int code() const noexcept
    {
        return _code;
    }

    char const *message() const noexcept
    {
        return _message;
    }

private:
    enum Type _type;

    int _code;

    char _message[MAX_MESSAGE_LENGTH];
};

extern bool CanBeUsedAsError(::v8::Local<::v8::Value> value);

extern void ToError(Error *error, ::v8::Local<::v8::Value> value);

extern ::v8::Local<::v8::Value> ToValue(Error const &error);

extern _ELASTOS ECode ToECode(Error const &error) noexcept;

CAR_BRIDGE_NAMESPACE_END

#endif
