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

#include "elastos/droid/internal/telephony/CSubInfoRecordUpdaterHelper.h"
#include "elastos/droid/internal/telephony/SubInfoRecordUpdater.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CSubInfoRecordUpdaterHelper, Object, ISubInfoRecordUpdaterHelper)

CAR_SINGLETON_IMPL(CSubInfoRecordUpdaterHelper)

ECode CSubInfoRecordUpdaterHelper::SetDisplayNameForNewSub(
    /* [in] */ const String& newSubName,
    /* [in] */ Int32 subId,
    /* [in] */ Int32 newNameSource)
{
    return SubInfoRecordUpdater::SetDisplayNameForNewSub(newSubName, subId, newNameSource);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
