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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/policy/impl/keyguard/KeyguardExitDelegate.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

CAR_INTERFACE_IMPL_2(KeyguardExitDelegate, Object, IBinder, IIKeyguardExitCallback)

const Boolean KeyguardExitDelegate::DEBUG = TRUE;

KeyguardExitDelegate::KeyguardExitDelegate()
{
}

ECode KeyguardExitDelegate::constructor(
    /* [in] */ IKeyguardServiceDelegate* host,
    /* [in] */ IOnKeyguardExitResult* onKeyguardExitResult)
{
    mHost = host;
    mOnKeyguardExitResult = onKeyguardExitResult;
    return NOERROR;
}

//@Override
ECode KeyguardExitDelegate::OnKeyguardExitResult(
    /* [in] */ Boolean success)
{
    if (DEBUG) Logger::V("KeyguardExitDelegate", "**** onKeyguardExitResult(%d) CALLED ****", success);
    if (mOnKeyguardExitResult != NULL) {
        return mOnKeyguardExitResult->OnKeyguardExitResult(success);
    }
    return NOERROR;
}

ECode KeyguardExitDelegate::ToString(
    /* [out] */ String* info)
{
    return Object::ToString(info);
}

} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
