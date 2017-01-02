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

#ifndef __ELASTOS_DROID_MTP_CMTPSERVER_H__
#define __ELASTOS_DROID_MTP_CMTPSERVER_H__

#include "_Elastos_Droid_Mtp_CMtpServer.h"
#include <elastos/core/Object.h>

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpServer)
    , public Object
    , public IRunnable
    , public IMtpServer
{
public:
    CMtpServer();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IMtpDatabase* database,
        /* [in] */ Boolean usePtp);

    CARAPI Start();

    CARAPI Run();

    CARAPI SendObjectAdded(
        /* [in] */ Int32 handle);

    CARAPI SendObjectRemoved(
        /* [in] */ Int32 handle);

    CARAPI SendDevicePropertyChanged(
        /* [in] */ Int32 property);

    CARAPI AddStorage(
        /* [in] */ IMtpStorage* storage);

    CARAPI RemoveStorage(
        /* [in] */ IMtpStorage* storage);

private:
    CARAPI_(void) NativeSetup(
        /* [in] */ IMtpDatabase* database,
        /* [in] */ Boolean usePtp);

    CARAPI_(void) NativeRun();

    CARAPI_(void) NativeCleanup();

    CARAPI_(void) NativeSendObjectAdded(
        /* [in] */ Int32 handle);

    CARAPI_(void) NativeSendObjectRemoved(
        /* [in] */ Int32 handle);

    CARAPI_(void) NativeSendDevicePropertyChanged(
        /* [in] */ Int32 property);

    CARAPI_(void) NativeAddStorage(
        /* [in] */ IMtpStorage* storage);

    CARAPI_(void) NativeRemoveStorage(
        /* [in] */ Int32 storageId);

private:
    Int64 mNativeContext;

    Object mutex;
};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MTP_CMTPSERVER_H__
