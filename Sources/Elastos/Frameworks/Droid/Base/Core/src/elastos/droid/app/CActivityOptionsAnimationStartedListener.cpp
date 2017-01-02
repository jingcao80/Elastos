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

#include "elastos/droid/app/CActivityOptionsAnimationStartedListener.h"
#include "elastos/droid/app/CActivityOptions.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Os::EIID_IIRemoteCallback;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActivityOptionsAnimationStartedListener, Object, IIRemoteCallback, IBinder)

CAR_OBJECT_IMPL(CActivityOptionsAnimationStartedListener)

ECode CActivityOptionsAnimationStartedListener::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener)
{
    mHandler = handler;
    mListener = listener;
    return NOERROR;
}

ECode CActivityOptionsAnimationStartedListener::SendResult(
    /* [in] */ IBundle* data)
{
    if (mHandler != NULL) {
        Boolean result;
        AutoPtr<IRunnable> r = new AnimationStartedListenerRunnable(mListener);
        mHandler->Post(r, &result);
    }
    return NOERROR;
}

ECode CActivityOptionsAnimationStartedListener::ToString(
    /* [out]*/ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("CActivityOptionsAnimationStartedListener");
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
