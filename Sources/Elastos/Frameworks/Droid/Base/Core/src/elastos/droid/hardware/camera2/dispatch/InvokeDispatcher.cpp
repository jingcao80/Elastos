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

#include "elastos/droid/hardware/camera2/dispatch/InvokeDispatcher.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CAR_INTERFACE_IMPL_2(InvokeDispatcher, Object, IInvokeDispatcher, IDispatchable)

const String InvokeDispatcher::TAG("InvokeDispatcher");
static const Boolean DEBUG = FALSE;

static String MethodToString(
    /* [in] */ IMethodInfo* method)
{
    String name;
    method->GetName(&name);
    Int32 count;
    method->GetParamCount(&count);

    StringBuilder sb("[methodName:");
    sb += name;
    sb += ", params(";
    sb += count;
    sb += ")";
    if (count > 0) {
        Int32 index;
        CarDataType carType;
        sb += "{";
        AutoPtr< ArrayOf<IParamInfo *> > infos = ArrayOf<IParamInfo *>::Alloc(count);
        method->GetAllParamInfos(infos);
        for (Int32 i = 0; i < count; ++i) {
            IParamInfo* info = (*infos)[i];
            info->GetName(&name);
            info->GetIndex(&index);
            if (i != 0) sb += "; ";
            sb += "\n    [";
            sb += index;
            sb += ": paramName:";
            sb += name;
            AutoPtr<IDataTypeInfo> typeInfo;
            info->GetTypeInfo((IDataTypeInfo**)&typeInfo);
            typeInfo->GetName(&name);
            typeInfo->GetDataType(&carType);
            sb += ", typeName:";
            sb += name;
            sb += ", carType:";
            sb += carType;
            sb += "]";
        }
        sb += "\n}\n";
    }
    sb += "]";

    return sb.ToString();
}

static String ArgumentsToString(
    /* [in] */ IArgumentList* args)
{
    AutoPtr<IFunctionInfo> funcInfo;
    args->GetFunctionInfo((IFunctionInfo**)&funcInfo);

    String name;
    funcInfo->GetName(&name);

    Int32 count;
    funcInfo->GetParamCount(&count);

    StringBuilder sb("[argument methodName:");
    sb += name;
    sb += ", params(";
    sb += count;
    sb += ")";
    if (count > 0) {
        Int32 index;
        CarDataType carType;
        sb += "{";
        AutoPtr< ArrayOf<IParamInfo *> > infos = ArrayOf<IParamInfo *>::Alloc(count);
        funcInfo->GetAllParamInfos(infos);
        for (Int32 i = 0; i < count; ++i) {
            IParamInfo* info = (*infos)[i];
            info->GetName(&name);
            info->GetIndex(&index);
            sb += "\n    [";
            sb += index;
            sb += ": paramName:";
            sb += name;
            AutoPtr<IDataTypeInfo> typeInfo;
            info->GetTypeInfo((IDataTypeInfo**)&typeInfo);
            typeInfo->GetName(&name);
            typeInfo->GetDataType(&carType);
            sb += ", typeName:";
            sb += name;
            sb += ", carType:";
            sb += carType;
            sb += "]";
        }
        sb += "\n}\n";
    }
    sb += "]";

    return sb.ToString();
}

InvokeDispatcher::InvokeDispatcher()
{
}

ECode InvokeDispatcher::constructor(
    /* [in] */ IInterface* target)
{
    FAIL_RETURN(Preconditions::CheckNotNull(target, String("target must not be null")))

    mTarget = target;
    return NOERROR;
}

ECode InvokeDispatcher::Dispatch(
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    //try {
    ECode ec = method->Invoke(mTarget, args);
    if (FAILED(ec) || DEBUG) {
        String methodStr = MethodToString(method);
        String argsStr = ArgumentsToString(args);

        if (FAILED(ec)) {
            Logger::E(TAG, "%s invoke method\n [%s]\n with params\n [%s]\n failed!",
                TO_CSTR(mTarget), methodStr.string(), argsStr.string());
        }
        else {
            Logger::E(TAG, "%s invoke method\n [%s]\n with params\n [%s]\n succeeded!",
                TO_CSTR(mTarget), methodStr.string(), argsStr.string());
        }
    }
    return ec;
}

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
