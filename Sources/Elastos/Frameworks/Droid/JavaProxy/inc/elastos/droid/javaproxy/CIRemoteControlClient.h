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

#ifndef __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLCLIENT_H__
#define __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLCLIENT_H__

#include "_Elastos_Droid_JavaProxy_CIRemoteControlClient.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Media::IIRemoteControlClient;
using Elastos::Droid::Media::IIRemoteControlDisplay;
using Elastos::Droid::Media::IRating;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRemoteControlClient)
    , public Object
    , public IIRemoteControlClient
    , public IBinder
{
public:
    ~CIRemoteControlClient();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI OnInformationRequested(
        /* [in] */ Int32 generationId,
        /* [in] */ Int32 infoFlags);

    CARAPI InformationRequestForDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI SetCurrentClientGenerationId(
        /* [in] */ Int32 clientGeneration);

    CARAPI PlugRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI UnplugRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd);

    CARAPI SetBitmapSizeForDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI SetWantsSyncForDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Boolean wantsSync);

    CARAPI EnableRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Boolean enabled);

    CARAPI SeekTo(
        /* [in] */ Int32 clientGeneration,
        /* [in] */ Int64 timeMs);

    CARAPI UpdateMetadata(
        /* [in] */ Int32 clientGeneration,
        /* [in] */ Int32 key,
        /* [in] */ IRating* value);

    CARAPI SetPlayItem(
        /* [in] */ Int32 scope,
        /* [in] */ Int64 uid);

    CARAPI SetBrowsedPlayer();

    CARAPI GetNowPlayingEntries();

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLCLIENT_H__
