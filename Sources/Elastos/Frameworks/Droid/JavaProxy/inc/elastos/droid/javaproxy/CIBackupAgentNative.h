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

#ifndef __ELASTOS_DROID_JAVAPROXY_CIBACKUPAGENTNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIBACKUPAGENTNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIBackupAgentNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::IIBackupAgent;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIBackupAgentNative)
    , public Object
    , public IIBackupAgent
    , public IBinder
{
public:
    ~CIBackupAgentNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI DoBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ IParcelFileDescriptor* newState,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoRestore(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int32 appVersionCode,
        /* [in] */ IParcelFileDescriptor* newState,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoFullBackup(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoRestoreFile(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int64 size,
        /* [in] */ Int32 type,
        /* [in] */ const String& domainName,
        /* [in] */ const String& path,
        /* [in] */ Int64 mode,
        /* [in] */ Int64 mtime,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoRestoreFinished(
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI Fail(
        /* [in] */ const String& message);

    CARAPI DoBackupFiles(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int32 token,
        /* [in] */ ArrayOf<String>* domainTokens,
        /* [in] */ const String& excludeFilesRegex,
        /* [in] */ IIBackupManager* callbackBinder);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIBACKUPAGENTNATIVE_H__
