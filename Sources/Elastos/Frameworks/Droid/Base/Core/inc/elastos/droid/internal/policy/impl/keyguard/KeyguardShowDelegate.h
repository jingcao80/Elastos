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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDSHOWDELEGATE_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDSHOWDELEGATE_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Policy::IIKeyguardShowCallback;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

class KeyguardShowDelegate
    : public Object
    , public IBinder
    , public IIKeyguardShowCallback
{
public:
    CAR_INTERFACE_DECL()

    KeyguardShowDelegate();

    CARAPI constructor(
        /* [in] */ IKeyguardServiceDelegate* host,
        /* [in] */ IKeyguardServiceDelegateShowListener* showListener);

    CARAPI OnShown(
        /* [in] */ IBinder* windowToken);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    static const Boolean DEBUG;
    IKeyguardServiceDelegate* mHost;//TODO
    AutoPtr<IKeyguardServiceDelegateShowListener> mShowListener;
};

} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDSHOWDELEGATE_H__
