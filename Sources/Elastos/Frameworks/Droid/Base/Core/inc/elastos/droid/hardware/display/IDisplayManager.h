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

#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_IDISPLAYMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_IDISPLAYMANAGER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Media::Projection::IIMediaProjection;
using Elastos::Droid::View::IDisplayInfo;
using Elastos::Droid::View::ISurface;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

class IDisplayManagerProxy
    : public Object
    , public IIDisplayManager
{
public:
    IDisplayManagerProxy(
        /* [in] */ android::sp<android::IBinder>& dm);

    CAR_INTERFACE_DECL();

    CARAPI GetDisplayInfo(
        /* [in] */ Int32 displayId,
        /* [out] */ IDisplayInfo** displayInfo);

    CARAPI GetDisplayIds(
        /* [out, callee] */ ArrayOf<Int32>** displayIds);

    CARAPI RegisterCallback(
        /* [in] */ IIDisplayManagerCallback* cb);

    CARAPI StartWifiDisplayScan();

    CARAPI StopWifiDisplayScan();

    CARAPI ConnectWifiDisplay(
        /* [in] */ const String& address);

    CARAPI DisconnectWifiDisplay();

    CARAPI RenameWifiDisplay(
        /* [in] */ const String& address,
        /* [in] */ const String& alias);

    CARAPI ForgetWifiDisplay(
        /* [in] */ const String& address);

    CARAPI PauseWifiDisplay();

    CARAPI ResumeWifiDisplay();

    CARAPI CreateVirtualDisplay(
        /* [in] */ IIVirtualDisplayCallback* clbk,
        /* [in] */ IIMediaProjection* projectionToken,
        /* [in] */ const String& packageName,
        /* [in] */ const String& name,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 densityDpi,
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 flags,
        /* [out] */ Int32* result);

    CARAPI ResizeVirtualDisplay(
        /* [in] */ IIVirtualDisplayCallback* token,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 densityDpi);

    CARAPI SetVirtualDisplaySurface(
        /* [in] */ IIVirtualDisplayCallback* token,
        /* [in] */ ISurface* surface);

    CARAPI ReleaseVirtualDisplay(
        /* [in] */ IIVirtualDisplayCallback* token);

    CARAPI GetWifiDisplayStatus(
        /* [out] */ IWifiDisplayStatus** wds);

private:
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_getDisplayInfo;
    static const Int32 TRANSACTION_registerCallback;

    android::sp<android::IBinder> mDm;
};

} // namespace Display
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_DISPLAY_IDISPLAYMANAGER_H__
