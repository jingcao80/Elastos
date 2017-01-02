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

#ifndef __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGERHELPER_H__
#define __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGERHELPER_H__

#include "_Elastos_Droid_Os_Storage_CStorageManagerHelper.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Droid::Content::IContext;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CStorageManagerHelper)
    , public Singleton
    , public IStorageManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI From(
        /* [in] */IContext* context,
        /* [out] */ IStorageManager** manager);

    CARAPI GetPrimaryVolume(
        /* [in] */ ArrayOf<IStorageVolume*>* volumes,
        /* [out] */ IStorageVolume** result );

    CARAPI GetNoEmulatedVolume(
        /* [in] */ ArrayOf<IStorageVolume*>* volumes,
        /* [out] */ IStorageVolume** result);

    CARAPI GetPhysicalExternalVolume(
        /* [in] */ ArrayOf<IStorageVolume*>* volumesphy,
        /* [out] */ IArrayList** result);
};

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGERHELPER_H__
