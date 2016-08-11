
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <node.h>

#include <nan.h>

#include <elastos.h>

#include "macros.h"

#include "elastos-ext.h"

#include "error.h"



using namespace node;

using namespace Nan;

using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

char const *Error::GetIntrinsicErrorName(int code) noexcept
{
    switch (code) {
    case NONE:
        return "NONE";

    case FAILED:
        return "FAILED";

    case INCOMPATIBLE_VERSION:
        return "INCOMPATIBLE_VERSION";

    case INVALID_RECEIVER:
        return "INVALID_RECEIVER";

    case INVALID_ARGUMENT:
        return "INVALID_ARGUMENT";

    case INVALID_FORMAT:
        return "INVALID_FORMAT";

    case NONENTITY:
        return "NONENTITY";

    case NO_MEMORY:
        return "NO_MEMORY";

    case CANCELLED:
        return "CANCELLED";

    case TIMED_OUT:
        return "TIMED_OUT";

    default:
        return "UNKNOWN";
    }
}

Error::Error(void) noexcept
{
    _type = TYPE_INTRINSIC;

    _code = NONE;

    _message[0] = '\0';
}

Error::Error(Error const &error) noexcept
{
    *this = error;
}

Error::Error(Error &&error) noexcept
{
    *this = error;
}

Error::Error(enum Type type, int code, char const *messageFormat, ...) noexcept
{
    va_list ap;

    va_start(ap, messageFormat);

    Set(type, code, messageFormat, ap);

    va_end(ap);
}

Error::Error(int code, char const *messageFormat, ...) noexcept
{
    va_list ap;

    va_start(ap, messageFormat);

    Set(code, messageFormat, ap);

    va_end(ap);
}

Error &Error::operator=(Error const &error) noexcept
{
    _type = error._type;

    _code = error._code;

    strcpy(_message, error._message);

    return *this;
}

Error &Error::operator=(Error &&error) noexcept
{
    *this = error;

    return *this;
}

void Error::Set(enum Type type, int code, char const *messageFormat, ...) noexcept
{
    va_list ap;

    va_start(ap, messageFormat);

    Set(type, code, messageFormat, ap);

    va_end(ap);
}

void Error::Set(int code, char const *messageFormat, ...) noexcept
{
    va_list ap;

    va_start(ap, messageFormat);

    Set(code, messageFormat, ap);

    va_end(ap);
}

void Error::Set(enum Type type, int code, char const *messageFormat, va_list ap) noexcept
{
    _type = type;

    _code = code;

    vsprintf(_message, messageFormat, ap);
}

void Error::Set(int code, char const *messageFormat, va_list ap) noexcept
{
    Set(TYPE_INTRINSIC, messageFormat, ap);
}

bool CanBeUsedAsError(Local<Value> value)
{
    return false;
}

void ToError(Error *error, Local<Value> value)
{
}

Local<Value> ToValue(Error const &error)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> _error;

    Error::Type type;

    int code;

    char const *_type;

    char const *name;

    _error = New<Object>();

    type = error.type();

    code = error.code();

    switch (type) {
    case Error::TYPE_INTRINSIC:
        _type = "INTRINSIC";

        name = Error::GetIntrinsicErrorName(code);

        break;

    case Error::TYPE_ELASTOS:
        _type = "ELASTOS";

        name = GetEName(code);

        break;

    default:
        abort();
    }

    ::Nan::Set(_error, New("type").ToLocalChecked(), New(_type).ToLocalChecked());

    ::Nan::Set(_error, New("name").ToLocalChecked(), New(name).ToLocalChecked());

    ::Nan::Set(_error, New("code").ToLocalChecked(), New<::v8::Int32>(code));

    ::Nan::Set(_error, New("message").ToLocalChecked(), New(error.message()).ToLocalChecked());

    return scope.Escape(_error);
}

ECode ToECode(Error const &error) noexcept
{
    int code;

    code = error.code();

    switch (error.type()) {
    case Error::TYPE_INTRINSIC:
        switch (code) {
        case Error::NONE:
            return NO_ERROR;

        case Error::FAILED:
            return E_FAILED;

        case Error::INCOMPATIBLE_VERSION:
            return E_FAILED;

        case Error::INVALID_RECEIVER:
            return E_INVALID_ARGUMENT;

        case Error::INVALID_ARGUMENT:
            return E_INVALID_ARGUMENT;

        case Error::INVALID_FORMAT:
            return E_FAILED;

        case Error::NONENTITY:
            return E_DOES_NOT_EXIST;

        case Error::NO_MEMORY:
            return E_OUT_OF_MEMORY;

        case Error::CANCELLED:
            return E_CALLBACK_CANCELED;

        case Error::TIMED_OUT:
            return E_TIMED_OUT;

        default:
            return E_FAILED;
        }

    case Error::TYPE_ELASTOS:
        return code;

    default:
        abort();
    }
}

CAR_BRIDGE_NAMESPACE_END

