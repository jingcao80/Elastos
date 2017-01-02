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

#include "elastos/droid/internal/telephony/cdma/EriInfo.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                               EriInfo
//=====================================================================
CAR_INTERFACE_IMPL(EriInfo, Object, IEriInfo);

EriInfo::EriInfo()
    : roamingIndicator(0)
    , iconIndex(0)
    , iconMode(0)
    , callPromptId(0)
    , alertId(0)
{
}

ECode EriInfo::constructor(
    /* [in] */ Int32 roamingIndicator,
    /* [in] */ Int32 iconIndex,
    /* [in] */ Int32 iconMode,
    /* [in] */ const String& eriText,
    /* [in] */ Int32 callPromptId,
    /* [in] */ Int32 alertId)
{

    this->roamingIndicator = roamingIndicator;
    this->iconIndex = iconIndex;
    this->iconMode = iconMode;
    this->eriText = eriText;
    this->callPromptId = callPromptId;
    this->alertId = alertId;
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
