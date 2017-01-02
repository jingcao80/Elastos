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

#ifndef __ELASTOS_DROID_OS_STORAGE_CMOUNTSERVICEBINDERLISTENER_H__
#define __ELASTOS_DROID_OS_STORAGE_CMOUNTSERVICEBINDERLISTENER_H__

#include "_Elastos_Droid_Os_Storage_CMountServiceBinderListener.h"
#include "elastos/droid/os/storage/CStorageManager.h"
#include "Elastos.Droid.Os.h"

#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CMountServiceBinderListener)
    , public Object
    , public IIMountServiceListener
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IStorageManager* host);

    CARAPI OnUsbMassStorageConnectionChanged(
        /* [in] */ Boolean connected);

    CARAPI OnStorageStateChanged(
        /* [in] */ const String& path,
        /* [in] */ const String& oldState,
        /* [in] */ const String& newState);

private:
    CStorageManager* mHost;
};

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_STORAGE_CMOUNTSERVICEBINDERLISTENER_H__
