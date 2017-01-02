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

#include "elastos/droid/hardware/camera2/dispatch/ArgumentReplacingDispatcher.h"
#include "elastos/droid/internal/utility/Preconditions.h"

using Elastos::Droid::Internal::Utility::Preconditions;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CAR_INTERFACE_IMPL_2(ArgumentReplacingDispatcher, Object, IArgumentReplacingDispatcher, IDispatchable)

ArgumentReplacingDispatcher::ArgumentReplacingDispatcher()
{
}

ECode ArgumentReplacingDispatcher::constructor(
    /* [in] */ IDispatchable* target,
    /* [in] */ Int32 argumentIndex,
    /* [in] */ IInterface* replaceWith)
{
    FAIL_RETURN(Preconditions::CheckNotNull(target, String("target must not be null")))
    FAIL_RETURN(Preconditions::CheckArgumentNonnegative(argumentIndex,
            String("argumentIndex must not be negative")))
    FAIL_RETURN(Preconditions::CheckNotNull(replaceWith, String("replaceWith must not be null")))

    mTarget = target;
    mArgumentIndex = argumentIndex;
    mReplaceWith = replaceWith;
    return NOERROR;
}

ECode ArgumentReplacingDispatcher::Dispatch(
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    Int32 argumentCount;
    method->GetParamCount(&argumentCount);
    if (argumentCount > mArgumentIndex) {
        String methodName, paraName, name;
        method->GetName(&methodName);

        AutoPtr<IParamInfo> paramInfo;
        method->GetParamInfoByIndex(mArgumentIndex, (IParamInfo**)&paramInfo);
        AutoPtr<IDataTypeInfo> typeInfo;
        paramInfo->GetTypeInfo((IDataTypeInfo**)&typeInfo);
        paramInfo->GetName(&paraName);
        CarDataType dataType;
        typeInfo->GetName(&name);
        typeInfo->GetDataType(&dataType);
        Logger::V("ArgumentReplacingDispatcher", " >> replace parament %s[type: %s, CarDataType: %d] at %d with %s, "
            "dataType: %d in method [%s, parament count: %d].",
            paraName.string(), name.string(), mArgumentIndex, TO_CSTR(mReplaceWith),
            dataType, methodName.string(), argumentCount);
        assert(dataType == CarDataType_Interface);

        //TODO clone args luo.zhaohui
        // AutoPtr<IArgumentList> cloneArgs;
        // ECode ec = method->CreateArgumentList((IArgumentList**)&cloneArgs);

        // don't change in-place since it can affect upstream dispatches
        args->SetInputArgumentOfObjectPtr(mArgumentIndex, mReplaceWith);
    }
    return mTarget->Dispatch(method, args);
}

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
