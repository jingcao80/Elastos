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

#include "elastos/droid/internal/telephony/cdma/CSignalToneUtilHelper.h"
#include "elastos/droid/internal/telephony/cdma/SignalToneUtil.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CSignalToneUtilHelper, Singleton, ISignalToneUtilHelper)
CAR_SINGLETON_IMPL(CSignalToneUtilHelper)

ECode CSignalToneUtilHelper::GetAudioToneFromSignalInfo(
    /* [in] */ Int32 signalType,
    /* [in] */ Int32 alertPitch,
    /* [in] */ Int32 signal,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SignalToneUtil::GetAudioToneFromSignalInfo(signalType, alertPitch, signal);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
