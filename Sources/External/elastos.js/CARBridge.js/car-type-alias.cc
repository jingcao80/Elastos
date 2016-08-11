
#include <map>

#include <node.h>

#include <nan.h>

#include <elastos.h>

#include "macros.h"

#include "nan-ext.h"

#include "car-data-type.h"
#include "error.h"
#include "js-2-car.h"



using namespace std;

using namespace node;

using namespace Nan;

using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

static map<AutoPtr<ITypeAliasInfo const>, CopyablePersistent<Object>> _mapTypeAliasInfoToCARTypeAlias;

Local<Object> CARTypeAlias(ITypeAliasInfo const *typeAliasInfo)
{
    ::Nan::EscapableHandleScope scope;

    Local<Object> typeAlias;

    ECode ec;

    _ELASTOS String name;

    _ELASTOS Boolean isDummy;

    AutoPtr<IDataTypeInfo const> typeInfo;
    IDataTypeInfo *_typeInfo;

    auto &_typeAlias = _mapTypeAliasInfoToCARTypeAlias[typeAliasInfo];
    if (!_typeAlias.IsEmpty())
        return scope.Escape(New(_typeAlias));

    typeAlias = New<Object>();

    DefineOwnProperty(typeAlias,
            New("$what").ToLocalChecked(),
            New("CARTypeAlias").ToLocalChecked(),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = typeAliasInfo->GetName(&name);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(typeAlias,
            New("$name").ToLocalChecked(),
            ToValue(name),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = typeAliasInfo->IsDummy(&isDummy);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    DefineOwnProperty(typeAlias,
            New("isDummy").ToLocalChecked(),
            ToValueFromBoolean(isDummy),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));

    ec = typeAliasInfo->GetTypeInfo(&_typeInfo);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    typeInfo = _typeInfo, _typeInfo->Release();

    DefineOwnProperty(typeAlias,
            New("realType").ToLocalChecked(),
            CARDataType(typeInfo),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));

    _typeAlias.Reset(typeAlias);

    return scope.Escape(typeAlias);
}

CAR_BRIDGE_NAMESPACE_END

