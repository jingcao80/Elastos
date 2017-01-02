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

#include "_Elastos_Droid_View_CDisplayManagerAw.h"

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CDisplayManagerAw)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDisplayManagerAw();

    CARAPI constructor();

    CARAPI GetDisplayCount(
        /* [out] */ Int32* result);

    CARAPI GetDisplayOpenStatus(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Boolean* result);

    CARAPI GetDisplayHotPlugStatus(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayOutputType(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayOutputFormat(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayWidth(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayHeight(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayPixelFormat(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI SetDisplayParameter(
        /* [in] */ Int32 mDisplay,
        /* [in] */ Int32 param0,
        /* [in] */ Int32 param1,
        /* [out] */ Int32* result);

    CARAPI SetDisplayMode(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* result);

    CARAPI GetDisplayMode(
        /* [out] */ Int32* result);

    CARAPI SetDisplayOutputType(
        /* [in] */ Int32 mDisplay,
        /* [in] */ Int32 type,
        /* [in] */ Int32 format,
        /* [out] */ Int32* result);

    CARAPI OpenDisplay(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI CloseDisplay(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI SetDisplayBacklightMode(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* result);

    CARAPI SetDisplayMaster(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayMaster(
        /* [out] */ Int32* result);

    CARAPI GetMaxWidthDisplay(
        /* [out] */ Int32* result);

    CARAPI GetMaxHdmiMode(
        /* [out] */ Int32* result);

    CARAPI GetDisplayBacklightMode(
        /* [out] */ Int32* result);

    CARAPI IsSupportHdmiMode(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* result);

    CARAPI IsSupport3DMode(
        /* [out] */ Int32* result);

    CARAPI GetHdmiHotPlugStatus(
        /* [out] */ Int32* result);

    CARAPI GetTvHotPlugStatus(
        /* [out] */ Int32* result);

    CARAPI SetDisplayAreaPercent(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 percent,
        /* [out] */ Int32* result);

    CARAPI GetDisplayAreaPercent(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* result);

    CARAPI SetDisplayBright(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 bright,
        /* [out] */ Int32* result);

    CARAPI GetDisplayBright(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* result);

    CARAPI SetDisplayContrast(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 contrast,
        /* [out] */ Int32* result);

    CARAPI GetDisplayContrast(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* result);

    CARAPI SetDisplaySaturation(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 saturation,
        /* [out] */ Int32* result);

    CARAPI GetDisplaySaturation(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* result);

    CARAPI SetDisplayHue(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 hue,
        /* [out] */ Int32* result);

    CARAPI GetDisplayHue(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* result);

    CARAPI Set3DLayerOffset(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [out] */ Int32* result);
private:

    CARAPI_(AutoPtr<IIDisplayManagerAw>) GetService();
    const static String Tab;
    AutoPtr<IIDisplayManagerAw> mService;
    AutoPtr<IBinder> mToken;

};

}// namespace Elastos
}// namespace Droid
}// namespace View
