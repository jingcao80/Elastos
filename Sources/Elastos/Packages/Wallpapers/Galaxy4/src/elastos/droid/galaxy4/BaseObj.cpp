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

#include "BaseObj.h"
#include "RenderScript.h"

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

ECode BaseObj::constructor(
    /* [in] */ Int64 id,
    /* [in] */ RenderScript* rs)
{
    FAIL_RETURN(rs->Validate());
    mRS = rs;
    mID = id;
    mDestroyed = FALSE;
    return NOERROR;
}

ECode BaseObj::SetID(
    /* [in] */ Int64 id)
{
    if (mID != 0) {
        // throw new RSRuntimeException("Internal Error, reset of object ID.");
        return E_RUNTIME_EXCEPTION;
    }
    mID = id;
    return NOERROR;
}

ECode BaseObj::GetID(
    /* [in] */ RenderScript* rs,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = -1;
    FAIL_RETURN(mRS->Validate());
    if (mDestroyed) {
        // throw new RSInvalidStateException("using a destroyed object.");
        return E_RUNTIME_EXCEPTION;
    }
    if (mID == 0) {
        // throw new RSRuntimeException("Internal error: Object id 0.");
        return E_RUNTIME_EXCEPTION;
    }
    if ((rs != NULL) && (rs != mRS)) {
        // throw new RSInvalidStateException("using object with mismatched context.");
        return E_RUNTIME_EXCEPTION;
    }
    *id = mID;
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
