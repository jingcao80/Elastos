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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/AsyncResult.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(AsyncResult, Object, IAsyncResult)

/** please note, this sets m.obj to be this */
AsyncResult::AsyncResult(
    /* [in] */ IInterface* uo,
    /* [in] */ IInterface* r,
    /* [in] */ IThrowable* ex)
    : mUserObj(uo)
    , mException(ex)
    , mResult(r)
{
}

/** Saves and sets m.obj */
AutoPtr<AsyncResult> AsyncResult::ForMessage(
    /* [in] */ IMessage* m,
    /* [in] */ IInterface* r,
    /* [in] */ IThrowable* ex)
{
    AutoPtr<IInterface> obj;
    m->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> ret = new AsyncResult(obj, r, ex);

    m->SetObj(TO_IINTERFACE(ret));

    return ret;
}

/** Saves and sets m.obj */
AutoPtr<AsyncResult> AsyncResult::ForMessage(
    /* [in] */ IMessage* m)
{
    AutoPtr<IInterface> obj;
    m->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> ret = new AsyncResult(obj, NULL, NULL);

    m->SetObj(TO_IINTERFACE(ret));

    return ret;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
