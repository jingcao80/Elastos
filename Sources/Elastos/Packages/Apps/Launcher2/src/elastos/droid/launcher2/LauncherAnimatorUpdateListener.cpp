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

#include "elastos/droid/launcher2/LauncherAnimatorUpdateListener.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Core.h"

using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Core::IFloat;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(LauncherAnimatorUpdateListener, Object, IAnimatorUpdateListener);

ECode LauncherAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> fObj = IFloat::Probe(obj);
    Float b;
    fObj->GetValue(&b);
    Float a = 1.0f - b;
    return OnAnimationUpdate(a, b);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos