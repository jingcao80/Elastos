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

#include "elastos/droid/internal/telephony/CSmsUsageMonitorHelper.h"
#include "elastos/droid/internal/telephony/SmsUsageMonitor.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CSmsUsageMonitorHelper, Object, ISmsUsageMonitorHelper)

CAR_SINGLETON_IMPL(CSmsUsageMonitorHelper)

ECode CSmsUsageMonitorHelper::MergeShortCodeCategories(
    /* [in] */ Int32 type1,
    /* [in] */ Int32 type2,
    /* [in] */ Int32* result)
{
    return SmsUsageMonitor::MergeShortCodeCategories(type1, type2, result);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
