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

#ifndef __ELASTOS_DROID_JAVAPROXY_CIFULLBACKUPRESTOREOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIFULLBACKUPRESTOREOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIFullBackupRestoreObserverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::Backup::IIFullBackupRestoreObserver;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIFullBackupRestoreObserverNative)
    , public Object
    , public IIFullBackupRestoreObserver
    , public IBinder
{
public:
    ~CIFullBackupRestoreObserverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnStartBackup();

    CARAPI OnBackupPackage(
        /* [in] */ const String& name);

    CARAPI OnEndBackup();

    CARAPI OnEndBackupWithResult(
        /* [in] */ Int32 result);

    CARAPI OnStartRestore();

    CARAPI OnRestorePackage(
        /* [in] */ const String& name);

    CARAPI OnEndRestore();

    CARAPI OnEndRestoreWithResult(
        /* [in] */ Int32 result);

    CARAPI OnTimeout();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CIFULLBACKUPRESTOREOBSERVERNATIVE_H__
