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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBINFORECORDUPDATERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBINFORECORDUPDATERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CSubInfoRecordUpdaterHelper.h"
#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSubInfoRecordUpdaterHelper)
    , public Singleton
    , public ISubInfoRecordUpdaterHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SetDisplayNameForNewSub(
    /* [in] */ const String& newSubName,
    /* [in] */ Int32 subId,
    /* [in] */ Int32 newNameSource);

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBINFORECORDUPDATERHELPER_H__
