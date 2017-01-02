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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDEXITDELEGATE_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDEXITDELEGATE_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Policy::IIKeyguardExitCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IOnKeyguardExitResult;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

class KeyguardExitDelegate
    : public Object
    , public IIKeyguardExitCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    KeyguardExitDelegate();

    CARAPI constructor(
        /* [in] */ IKeyguardServiceDelegate* host,
        /* [in] */ IOnKeyguardExitResult* onKeyguardExitResult);

    CARAPI OnKeyguardExitResult(
        /* [in] */ Boolean success);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    static const Boolean DEBUG;
    AutoPtr<IKeyguardServiceDelegate> mHost;
    AutoPtr<IOnKeyguardExitResult> mOnKeyguardExitResult;
};

} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDEXITDELEGATE_H__
