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

#ifndef __ELASTOS_DROID_SERVER_CWMSWALLPAPERCONNECTION_H__
#define __ELASTOS_DROID_SERVER_CWMSWALLPAPERCONNECTION_H__

#include "_Elastos_Droid_Server_CWMSWallpaperConnection.h"
#include "CWallpaperManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CWMSWallpaperConnection)
{
    friend class CWallpaperManagerService;
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IWallpaperInfo* info,
        /* [in] */ HANDLE wallpaper,
        /* [in] */ HANDLE owner);

    CARAPI OnServiceConnected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* service);

    CARAPI OnServiceDisconnected(
        /* [in] */ IComponentName* name);

    CARAPI AttachEngine(
        /* [in] */ IWallpaperEngine* engine);

    CARAPI EngineShown(
        /* [in] */ IWallpaperEngine* engine);

    CARAPI SetWallpaper(
        /* [in] */ const String& name,
        /* [out] */ IParcelFileDescriptor** result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IWallpaperInfo> mInfo;
    AutoPtr<IBinder> mToken;
    AutoPtr<IIWallpaperService> mService;
    AutoPtr<IWallpaperEngine> mEngine;
    AutoPtr<CWallpaperManagerService::WallpaperData> mWallpaper;
    AutoPtr<IRemoteCallback> mReply;
    CWallpaperManagerService* mOwner;
};

} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CWMSWALLPAPERCONNECTION_H__
