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

#ifndef __ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUPHELPER_H__
#define __ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUPHELPER_H__

#include "_Elastos_Droid_Provider_CLocalGroupsGroupHelper.h"
#include "_Elastos.Droid.Database.h"
#include "_Elastos.Droid.Content.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CLocalGroupsGroupHelper)
    , public Singleton
    , public ILocalGroupsGroupHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI RestoreGroup(
        /* [in] */ ICursor* cursor,
        /* [out] */ ILocalGroupsGroup** group);

    CARAPI RestoreGroupById(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 groupId,
        /* [out] */ ILocalGroupsGroup** group);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUPHELPER_H__
