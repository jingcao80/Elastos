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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_POLICY_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_POLICY_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Policy::IIPolicy;
using Elastos::Droid::View::IFallbackEventHandler;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerPolicy;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

/**
    * {@hide}
      */
// Simple implementation of the policy interface that spawns the right
// set of objects
class Policy
    : public Object
    , public IIPolicy
{
public:
    CAR_INTERFACE_DECL()

    Policy();

    CARAPI constructor();

    CARAPI MakeNewWindow(
        /* [in] */ IContext* context,
        /* [out] */ IWindow** window);

    CARAPI MakeNewLayoutInflater(
        /* [in] */ IContext* context,
        /* [out] */ ILayoutInflater** inflater);

    CARAPI MakeNewWindowManager(
        /* [out] */ IWindowManagerPolicy** wm);

    CARAPI MakeNewFallbackEventHandler(
        /* [in] */ IContext* context,
        /* [out] */ IFallbackEventHandler** handler);
private:
    static const String TAG;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_POLICY_H__
