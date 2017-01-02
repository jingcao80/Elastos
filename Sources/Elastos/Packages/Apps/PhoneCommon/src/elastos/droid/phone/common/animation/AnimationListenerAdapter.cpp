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

#include "elastos/droid/phone/common/animation/AnimationListenerAdapter.h"

using Elastos::Droid::View::Animation::EIID_IAnimationAnimationListener;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Animation {

CAR_INTERFACE_IMPL(AnimationListenerAdapter, Object, IAnimationAnimationListener);

// @Override
ECode AnimationListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

// @Override
ECode AnimationListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

// @Override
ECode AnimationListenerAdapter::OnAnimationRepeat(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

} // namespace Animation
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos
