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

#include "CGuardedObject.h"

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CGuardedObject)
CAR_INTERFACE_IMPL_2(CGuardedObject, Object, IGuardedObject, ISerializable)
ECode CGuardedObject::constructor(
    /* [in] */ IInterface* object,
    /* [in] */ IGuard* guard)
{
    mObject = object;
    mGuard = guard;
    return NOERROR;
}

ECode CGuardedObject::GetObject(
    /* [out] */ IInterface** result) /*throws SecurityException*/
{
    if (mGuard != NULL) {
        FAIL_RETURN(mGuard->CheckGuard(mObject));
    }
    *result = mObject;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGuardedObject::WriteObject(
    /* [in] */ /*java.io.*/IObjectOutputStream* out) /*throws IOException*/
{
    if (mGuard != NULL) {
        FAIL_RETURN(mGuard->CheckGuard(mObject));
    }
    return out->DefaultWriteObject();
}

} // namespace Security
} // namespace Elastos

