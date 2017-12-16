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

#include "elastos/droid/view/CWindowManagerGlobalSessionCallback.h"
#include "elastos/droid/animation/CValueAnimatorHelper.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL_2(CWindowManagerGlobalSessionCallback, Object, IIWindowSessionCallback, IBinder)

CAR_OBJECT_IMPL(CWindowManagerGlobalSessionCallback)

CWindowManagerGlobalSessionCallback::CWindowManagerGlobalSessionCallback()
    : mBBinder(NULL)
{}

ECode CWindowManagerGlobalSessionCallback::constructor()
{
    return NOERROR;
}

ECode CWindowManagerGlobalSessionCallback::OnAnimatorScaleChanged(
    /* [in] */ Float scale)
{
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    helper->SetDurationScale(scale);
    return NOERROR;
}

ECode CWindowManagerGlobalSessionCallback::ToString(
    /* [out] */ String* info)
{
    return Object::ToString(info);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos