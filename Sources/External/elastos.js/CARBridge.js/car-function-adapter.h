#ifndef __CAR_BRIDGE_CAR_FUNCTION_ADAPTER_H
# define __CAR_BRIDGE_CAR_FUNCTION_ADAPTER_H

# include <cstdarg>
# include <cstddef>

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

class CARFunctionAdapter {
public:
    static _ELASTOS ECode Call(::v8::Local<::v8::Function> function, ::v8::Local<::v8::Value> receiver,
            IFunctionInfo const *functionInfo, ...) noexcept;

    static _ELASTOS ECode Call(::v8::Local<::v8::Function> function, ::v8::Local<::v8::Value> receiver,
            IFunctionInfo const *functionInfo, va_list ap) noexcept;

    CARFunctionAdapter(IFunctionInfo const *functionInfo,
            ::v8::Local<::v8::Function> function, ::v8::Local<::v8::Value> receiver = ::v8::Local<::v8::Value>());

    ~CARFunctionAdapter();

    _ELASTOS ECode operator()(void) noexcept;

    template<class T>
    _ELASTOS ECode operator()(T firstArgument, ...) noexcept;

    IFunctionInfo const *functionInfo(void) const noexcept
    {
        return _functionInfo;
    }

    ::v8::Local<::v8::Function> function(void) const
    {
        return *_function;
    }

    ::v8::Local<::v8::Value> receiver(void) const
    {
        return ::Nan::New(_receiver);
    }

    _ELASTOS ECode Call(void) noexcept;

    template<class T>
    _ELASTOS ECode Call(T firstArgument, ...) noexcept;

    template<class T>
    _ELASTOS ECode Call(T firstArgument, va_list ap) noexcept;

    _ELASTOS ECode Call(va_list ap) noexcept;

private:
    _ELASTOS AutoPtr<IFunctionInfo const> _functionInfo;
    _ELASTOS AutoPtr<_ELASTOS ArrayOf<IParamInfo const *> const> _paramInfos;

    ::Nan::Callback _function;

    ::Nan::Persistent<::v8::Value> _receiver;

    static void SetArgumentOf(IParamInfo const *paramInfo,
            size_t argc, ::v8::Local<::v8::Value> argv[],
            size_t index,
            va_list ap);

    static void SetArgumentOf(IParamInfo const *paramInfo,
            size_t argc, ::v8::Local<::v8::Value> argv[],
            size_t index,
            void *ap);

    CARFunctionAdapter(CARFunctionAdapter const &carFunctionAdapter) = delete;

    CARFunctionAdapter(CARFunctionAdapter &&carFunctionAdapter) = delete;

    CARFunctionAdapter &operator=(CARFunctionAdapter const &carFunctionAdapter) = delete;

    CARFunctionAdapter &operator=(CARFunctionAdapter &&carFunctionAdapter) = delete;

    ::v8::Local<::v8::Value> CallFunction(size_t argc, ::v8::Local<::v8::Value> argv[]);
};

CAR_BRIDGE_NAMESPACE_END

# include "car-function-adapter-cc.h"

#endif
