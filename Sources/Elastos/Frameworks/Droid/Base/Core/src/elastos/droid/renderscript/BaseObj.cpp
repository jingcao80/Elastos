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

#include "elastos/droid/RenderScript/BaseObj.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Droid::RenderScript::EIID_IBaseObj;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace RenderScript {

BaseObj::BaseObj()
    : mID(-1)
    , mDestroyed(FALSE)
{}

ECode BaseObj::constructor(
    /* [in] */ Int64 id,
    /* [in] */ RenderScript* rs)
{
    mID = id;
    rs->Validate();
    mRS = rs;
    return NOERROR;
}

BaseObj::~BaseObj()
{
    HelpDestroy();
}

CAR_INTERFACE_IMPL(BaseObj, Object, IBaseObj)

ECode BaseObj::SetID(
    /* [in] */ Int64 id)
{
    if (mID != 0) {
        Slogger::E("BaseObj", "Internal Error, reset of object ID.");
        return E_RS_RUNTIME_EXCEPTION;
    }
    mID = id;
    return NOERROR;
}

ECode BaseObj::GetID(
    /* [in] */ RenderScript* rs,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    *id = -1;
    FAIL_RETURN(mRS->Validate())
    if (mDestroyed) {
        Slogger::E("BaseObj", "using a destroyed object.");
        return E_RS_INVALID_STATE_EXCEPTION;
    }
    if (mID == 0) {
        Slogger::E("BaseObj", "Internal error: Object id 0.");
        return E_RS_RUNTIME_EXCEPTION;
    }
    if ((rs != NULL) && (rs != mRS)) {
        Slogger::E("BaseObj", "using object with mismatched context.");
        return E_RS_INVALID_STATE_EXCEPTION;
    }
    *id = mID;
    return NOERROR;
}

ECode BaseObj::CheckValid()
{
    if (mID == 0) {
        Slogger::E("BaseObj", "Invalid object.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode BaseObj::SetName(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        Slogger::E("BaseObj", "setName requires a string of non-zero length.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if(name.GetLength() < 1) {
        Slogger::E("BaseObj", "setName does not accept a zero length string.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if(!mName.IsNull()) {
        Slogger::E("BaseObj", "setName object already has a name.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    AutoPtr<ArrayOf<Byte> > bytes = name.GetBytes(/*"UTF-8"*/);
    mRS->NAssignName(mID, bytes);
    mName = name;
    // } catch (java.io.UnsupportedEncodingException e) {
    //     throw new RuntimeException(e);
    // }
    return NOERROR;
}

ECode BaseObj::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = name;
    return NOERROR;
}

void BaseObj::HelpDestroy()
{
    Boolean shouldDestroy = FALSE;
    {    AutoLock syncLock(this);
        if (!mDestroyed) {
            shouldDestroy = TRUE;
            mDestroyed = TRUE;
        }
    }

    if (shouldDestroy) {
        // must include nObjDestroy in the critical section
        AutoPtr<ILock> rlock;
        mrs->mRWLock->ReadLock((ILock**)&rlock);
        rlock->Lock();
        // AllocationAdapters are BaseObjs with an ID of 0 but should not be passed to nObjDestroy
        if(mRS->IsAlive() && mID != 0) {
            mRS->NObjDestroy(mID);
        }
        rlock->Unlock();
        mRS = NULL;
        mID = 0;
    }
}

ECode BaseObj::Destroy()
{
    if(mDestroyed) {
        Slogger::E("BaseObj", "Object already destroyed.");
        return E_RS_INVALID_STATE_EXCEPTION;
    }
    HelpDestroy();
    return NOERROR;
}

void BaseObj::UpdateFromNative()
{
    mRS->Validate();
    mName = mRS->NGetName(GetID(mRS));
}

ECode BaseObj::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = (Int32)((mID & 0xfffffff) ^ (mID >> 32));
    return NOERROR;
}

ECode BaseObj::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal)
    // Early-out check to see if both BaseObjs are actually the same
    if ((IInterface*)(IObject*)this == obj) {
        *equal = TRUE;
        return NOERROR;
    }

    if (obj == NULL) {
        *equal = FALSE;
        return NOERROR;
    }

    EGuid clsId, objClsId;
    GetClassID(&clsId);
    IObject::Probe(obj)->GetClassID(&objClsId);
    if (clsId.mUunm != objClsId.mUunm) {
        *equal = FALSE;
        return NOERROR;
    }

    BaseObj* b = (BaseObj*)IBaseObj::Probe(obj);
    *equal = (mID == b->mID);
    return NOERROR;
}

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos
