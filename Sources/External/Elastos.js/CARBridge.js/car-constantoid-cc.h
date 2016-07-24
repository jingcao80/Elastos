#ifndef __CAR_BRIDGE_CAR_CONSTANTOID_CC_H
# define __CAR_BRIDGE_CAR_CONSTANTOID_CC_H

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"

# include "nan-ext.h"

# include "error.h"
# include "js-2-car.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class ConstantoidInfo>
::v8::Local<::v8::NumberObject> CARConstantoid(ConstantoidInfo const *constantoidInfo, char const *what)
{
    ::Nan::EscapableHandleScope scope;

    _ELASTOS ECode ec;

    _ELASTOS Int32 value;

    ::v8::Local<::v8::NumberObject> constantoid;

    _ELASTOS String name;

    ec = constantoidInfo->GetValue(&value);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    constantoid = ::Nan::New<::v8::NumberObject>(value);

    DefineOwnProperty(constantoid,
            ::Nan::New("$what").ToLocalChecked(),
            ::Nan::New(what).ToLocalChecked(),
            static_cast<enum ::v8::PropertyAttribute>(::v8::ReadOnly | ::v8::DontDelete | ::v8::DontEnum));

    ec = constantoidInfo->GetName(&name);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(constantoid,
            ::Nan::New("$name").ToLocalChecked(),
            ToValue(name),
            static_cast<enum ::v8::PropertyAttribute>(::v8::ReadOnly | ::v8::DontDelete | ::v8::DontEnum));

    return scope.Escape(constantoid);
}

CAR_BRIDGE_NAMESPACE_END

#endif
