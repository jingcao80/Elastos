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

#ifndef __ELASTOS_DROID_DATABASE_CCONTENTOBSERVERTRANSPORT_H__
#define __ELASTOS_DROID_DATABASE_CCONTENTOBSERVERTRANSPORT_H__

#include "_Elastos_Droid_Database_CContentObserverTransport.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Database.h>
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Database::IIContentObserver;

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CContentObserverTransport)
    , public Object
    , public IContentObserverTransport
    , public IIContentObserver
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentObserverTransport();

    ~CContentObserverTransport();

    CARAPI OnChange(
        /* [in] */ Boolean selfchange,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userId);

    CARAPI ReleaseContentObserver();

    CARAPI constructor(
        /* [in] */ IContentObserver* contentObserver);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    android::IBinder* mBBinder;

private:
    AutoPtr<IWeakReference> mWeakContentObserver;   // IContentObserver
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CCONTENTOBSERVERTRANSPORT_H__
