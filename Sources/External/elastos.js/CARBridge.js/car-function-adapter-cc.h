#ifndef __CAR_BRIDGE_CAR_FUNCTION_ADAPTER_CC_H
# define __CAR_BRIDGE_CAR_FUNCTION_ADAPTER_CC_H

# include <cstdarg>
# include <cstddef>

# include <memory>
# include <new>

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"

# include "elastos-ext.h"

# include "error.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class T>
_ELASTOS ECode CARFunctionAdapter::operator()(T firstArgument, ...) noexcept
{
    va_list ap;

    _ELASTOS ECode ec;

    va_start(ap, firstArgument);

    ec = Call(firstArgument, ap);

    va_end(ap);

    return ec;
}

template<class T>
_ELASTOS ECode CARFunctionAdapter::Call(T firstArgument, ...) noexcept
{
    va_list ap;

    _ELASTOS ECode ec;

    va_start(ap, firstArgument);

    ec = Call(firstArgument, ap);

    va_end(ap);

    return ec;
}

template<class T>
_ELASTOS ECode CARFunctionAdapter::Call(T firstArgument, va_list ap) noexcept
{
    try {
        ::Nan::HandleScope scope;

        size_t argc;

        argc = _paramInfos->GetLength();
        ::std::unique_ptr<::v8::Local<::v8::Value> []> argv(new(::std::nothrow) ::v8::Local<::v8::Value>[argc]);
        if (argv == nullptr)
            throw Error(Error::NO_MEMORY, "");

        SetArgumentOf((*_paramInfos)[0], argc, argv.get(), 0, &firstArgument);
        for (size_t i = 1; i < argc; ++i)
            SetArgumentOf((*_paramInfos)[i], argc, argv.get(), i, ap);

        CallFunction(argc, argv.get());
    } catch (Error const &error) {
        return ToECode(error);
    } catch (...) {
        return E_FAILED;
    }

    return NO_ERROR;
}

CAR_BRIDGE_NAMESPACE_END

#endif
