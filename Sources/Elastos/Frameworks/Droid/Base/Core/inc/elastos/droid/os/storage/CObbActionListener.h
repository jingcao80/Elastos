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

#ifndef __ELASTOS_DROID_OS_STORAGE_COBBACTIONLISTENER_H__
#define __ELASTOS_DROID_OS_STORAGE_COBBACTIONLISTENER_H__

#include "_Elastos_Droid_Os_Storage_CObbActionListener.h"
#include "elastos/droid/os/storage/CStorageManager.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CObbActionListener)
    , public Object
    , public IIObbActionListener
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IStorageManager* host);

    CARAPI OnObbResult(
        /* [in] */ const String& filename,
        /* [in] */ Int32 nonce,
        /* [in] */ Int32 status);

    Int32 AddListener(
        /* [in] */ IOnObbStateChangeListener* listener);

private:
    CStorageManager* mHost;

    HashMap<Int32, AutoPtr<CStorageManager::ObbListenerDelegate> > mListeners;

    Object mListenersLock;
};

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_STORAGE_COBBACTIONLISTENER_H__
