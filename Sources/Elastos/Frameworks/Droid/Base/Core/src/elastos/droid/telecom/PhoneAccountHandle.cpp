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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/PhoneAccountHandle.h"
#include "elastos/droid/content/CComponentNameHelper.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// PhoneAccountHandle::
//===============================================================
CAR_INTERFACE_IMPL_2(PhoneAccountHandle, Object, IPhoneAccountHandle, IParcelable)

ECode PhoneAccountHandle::constructor()
{
    return NOERROR;
}

ECode PhoneAccountHandle::constructor(
    /* [in] */ IComponentName* componentName,
    /* [in] */const String& id)
{
    mComponentName = componentName;
    mId = id;
    return NOERROR;
}

ECode PhoneAccountHandle::GetComponentName(
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mComponentName;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccountHandle::GetId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode PhoneAccountHandle::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 hc = 0;
    IObject::Probe(mComponentName)->GetHashCode(&hc);
    Int32 idHc = mId.GetHashCode();
    *result = hc + idHc;
    return NOERROR;
}

ECode PhoneAccountHandle::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb;
    sb.Append(mComponentName);
    sb.Append(", ");
    sb.Append(mId);
    return sb.ToString(result);
}

ECode PhoneAccountHandle::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (other != NULL) {
        if (IPhoneAccountHandle::Probe(other) != NULL) {
            AutoPtr<IPhoneAccountHandle> pOther = IPhoneAccountHandle::Probe(other);
            AutoPtr<IComponentName> otherCN, conName;
            pOther->GetComponentName((IComponentName**)&otherCN);
            GetComponentName((IComponentName**)&conName);
            String otherId, id;
            pOther->GetId(&otherId);
            GetId(&id);
            if (Object::Equals(otherCN, conName) && otherId.Equals(id)) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode PhoneAccountHandle::WriteToParcel(
    /* [in] */ IParcel* out)
{
    if (mComponentName != NULL) {
        out->WriteInt32(1);
        //IParcelable::Probe(mComponentName)->WriteToParcel(out);
        AutoPtr<IComponentNameHelper> helper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
        helper->WriteToParcel(mComponentName, out);
    }
    else {
        out->WriteInt32(0);
    }
    out->WriteString(mId);
    return NOERROR;
}

ECode PhoneAccountHandle::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    mComponentName = NULL;
    Int32 value = 0;
    in->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IComponentNameHelper> helper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
        helper->ReadFromParcel(in, (IComponentName**)&mComponentName);
    }
    in->ReadString(&mId);
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
