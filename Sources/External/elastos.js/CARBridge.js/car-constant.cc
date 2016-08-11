
#include <map>

#include <node.h>

#include <nan.h>

#include <elastos.h>

#include "macros.h"

#include "nan-ext.h"

#include "car-constantoid.h"



using namespace std;

using namespace node;

using namespace Nan;

using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

static map<AutoPtr<IConstantInfo const>, CopyablePersistent<NumberObject>> _mapConstantInfoToCARConstant;

Local<NumberObject> CARConstant(IConstantInfo const *constantInfo)
{
    Local<NumberObject> constant;

    auto &_constant = _mapConstantInfoToCARConstant[constantInfo];
    if (!_constant.IsEmpty())
        return New(_constant);

    constant = CARConstantoid(constantInfo, "CARConstant");

    _constant.Reset(constant);

    return constant;
}

CAR_BRIDGE_NAMESPACE_END

