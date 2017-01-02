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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UICCConfig.h"

#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                              UICCConfig
//=====================================================================
CAR_INTERFACE_IMPL(UICCConfig, Object, IUICCConfig);
UICCConfig::UICCConfig()
    : mMncLength(0)
{}

ECode UICCConfig::GetImsi(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (mImsi.IsNull()) {
        Logd(String("Getting IMSI: null"));
    }
    else {
        Logd(String("Getting IMSI: ") + mImsi);
    }
    *result = mImsi;
    return NOERROR;
}

ECode UICCConfig::SetImsi(
    /* [in] */ const String& lImsi)
{
    Logd(String("Setting IMSI: ") + lImsi);
    mImsi = lImsi;
    return NOERROR;
}

ECode UICCConfig::GetMncLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Logd(String("Getting MncLength: ") + StringUtils::ToString(mMncLength));
    *result = mMncLength;
    return NOERROR;
}

ECode UICCConfig::SetMncLength(
    /* [in] */ Int32 lMncLength)
{
    Logd(String("Setting MncLength: ") + StringUtils::ToString(lMncLength));
    mMncLength = lMncLength;
    return NOERROR;
}

void UICCConfig::Logd(
    /* [in] */ const String& sLog)
{
    if (LOG_DEBUG) {
        Logger::D(TAG, sLog);
    }
}

void UICCConfig::Loge(
    /* [in] */ const String& sLog)
{
    Logger::E(TAG, sLog);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
