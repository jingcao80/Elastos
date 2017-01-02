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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/gfx/DeviceDisplayInfo.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/api/DeviceDisplayInfo_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IPixelFormatHelper;
using Elastos::Droid::Graphics::CPixelFormatHelper;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::CPixelFormat;
using Elastos::Droid::Os::Build;
using Elastos::Droid::View::ISurface;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Gfx {

//=====================================================================
//                          DeviceDisplayInfo
//=====================================================================
Int32 DeviceDisplayInfo::GetDisplayHeight()
{
    // ==================before translated======================
    // getDisplay().getSize(mTempPoint);
    // return mTempPoint.y;

    GetDisplay()->GetSize(mTempPoint);
    Int32 y = 0;
    mTempPoint->GetY(&y);
    return y;
}

Int32 DeviceDisplayInfo::GetDisplayWidth()
{
    // ==================before translated======================
    // getDisplay().getSize(mTempPoint);
    // return mTempPoint.x;

    GetDisplay()->GetSize(mTempPoint);
    Int32 x = 0;
    mTempPoint->GetX(&x);
    return x;
}

Int32 DeviceDisplayInfo::GetPhysicalDisplayHeight()
{
    // ==================before translated======================
    // if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN_MR1) {
    //     return 0;
    // }
    // getDisplay().getRealSize(mTempPoint);
    // return mTempPoint.y;

    if (Build::VERSION::SDK_INT < Build::VERSION_CODES::JELLY_BEAN_MR1) {
        return 0;
    }
    GetDisplay()->GetRealSize(mTempPoint);
    Int32 y = 0;
    mTempPoint->GetY(&y);
    return y;
}

Int32 DeviceDisplayInfo::GetPhysicalDisplayWidth()
{
    // ==================before translated======================
    // if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN_MR1) {
    //     return 0;
    // }
    // getDisplay().getRealSize(mTempPoint);
    // return mTempPoint.x;

    if (Build::VERSION::SDK_INT < Build::VERSION_CODES::JELLY_BEAN_MR1) {
        return 0;
    }
    GetDisplay()->GetRealSize(mTempPoint);
    Int32 x = 0;
    mTempPoint->GetX(&x);
    return x;
}

Int32 DeviceDisplayInfo::GetBitsPerPixel()
{
    Int32 format = GetPixelFormat();
    AutoPtr<IPixelFormat> info;
    CPixelFormat::New((IPixelFormat**)&info);

    AutoPtr<IPixelFormatHelper> helper;
    CPixelFormatHelper::AcquireSingleton((IPixelFormatHelper**)&helper);
    helper->GetPixelFormatInfo(format, info);
    Int32 bitsPerPixel = 0;
    info->GetBitsPerPixel(&bitsPerPixel);
    return bitsPerPixel;
}

Int32 DeviceDisplayInfo::GetBitsPerComponent()
{
    // ==================before translated======================
    // int format = getPixelFormat();
    // switch (format) {
    //     case PixelFormat.RGBA_4444:
    //         return 4;
    //
    //     case PixelFormat.RGBA_5551:
    //         return 5;
    //
    //     case PixelFormat.RGBA_8888:
    //     case PixelFormat.RGBX_8888:
    //     case PixelFormat.RGB_888:
    //         return 8;
    //
    //     case PixelFormat.RGB_332:
    //         return 2;
    //
    //     case PixelFormat.RGB_565:
    //         return 5;
    //
    //     // Non-RGB formats.
    //     case PixelFormat.A_8:
    //     case PixelFormat.LA_88:
    //     case PixelFormat.L_8:
    //         return 0;
    //
    //     // Unknown format. Use 8 as a sensible default.
    //     default:
    //         return 8;
    // }

    Int32 format = GetPixelFormat();
    switch (format) {
        case IPixelFormat::RGBA_4444:
            return 4;
        case IPixelFormat::RGBA_5551:
            return 5;
        case IPixelFormat::RGBA_8888:
        case IPixelFormat::RGBX_8888:
        case IPixelFormat::RGB_888:
            return 8;
        case IPixelFormat::RGB_332:
            return 2;
        case IPixelFormat::RGB_565:
            return 5;
        case IPixelFormat::A_8:
        case IPixelFormat::LA_88:
        case IPixelFormat::L_8:
            return 0;
        default:
            return 8;
    }
}

Double DeviceDisplayInfo::GetDIPScale()
{
    // ==================before translated======================
    // getDisplay().getMetrics(mTempMetrics);
    // return mTempMetrics.density;

    GetDisplay()->GetMetrics(mTempMetrics);
    Float density = 0.0f;
    mTempMetrics->GetDensity(&density);
    return (Double)density;
}

ECode DeviceDisplayInfo::UpdateNativeSharedDisplayInfo()
{
    // ==================before translated======================
    // nativeUpdateSharedDeviceDisplayInfo(
    //         getDisplayHeight(), getDisplayWidth(),
    //         getPhysicalDisplayHeight(), getPhysicalDisplayWidth(),
    //         getBitsPerPixel(), getBitsPerComponent(),
    //         getDIPScale(), getSmallestDIPWidth(), getRotationDegrees());

    NativeUpdateSharedDeviceDisplayInfo(
        GetDisplayHeight(), GetDisplayWidth(),
        GetPhysicalDisplayHeight(), GetPhysicalDisplayWidth(),
        GetBitsPerPixel(), GetBitsPerComponent(),
        GetDIPScale(), GetSmallestDIPWidth(), GetRotationDegrees());
    return NOERROR;
}

AutoPtr<DeviceDisplayInfo> DeviceDisplayInfo::Create(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // return new DeviceDisplayInfo(context);

    AutoPtr<DeviceDisplayInfo> deviceDisplayInfo = new DeviceDisplayInfo(context);
    return deviceDisplayInfo;
}

DeviceDisplayInfo::DeviceDisplayInfo(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // mAppContext = context.getApplicationContext();
    // mWinManager = (WindowManager) mAppContext.getSystemService(Context.WINDOW_SERVICE);

    AutoPtr<IContext> applicationContext;
    context->GetApplicationContext((IContext**)&applicationContext);
    mAppContext = applicationContext;

    AutoPtr<IInterface> interfaceTmp;
    mAppContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&interfaceTmp);
    mWinManager = IWindowManager::Probe(interfaceTmp);
}

Int32 DeviceDisplayInfo::GetPixelFormat()
{
    // ==================before translated======================
    // if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN_MR1) {
    //     return getDisplay().getPixelFormat();
    // }
    // // JellyBean MR1 and later always uses RGBA_8888.
    // return PixelFormat.RGBA_8888;

    if (Build::VERSION::SDK_INT < Build::VERSION_CODES::JELLY_BEAN_MR1) {
        Int32 pixelFormat = 0;
        GetDisplay()->GetPixelFormat(&pixelFormat);
        return pixelFormat;
    }
    return IPixelFormat::RGBA_8888;
}

Int32 DeviceDisplayInfo::GetSmallestDIPWidth()
{
    // ==================before translated======================
    // return mAppContext.getResources().getConfiguration().smallestScreenWidthDp;

    AutoPtr<IResources> resources;
    mAppContext->GetResources((IResources**)&resources);

    AutoPtr<IConfiguration> configuration;
    resources->GetConfiguration((IConfiguration**)&configuration);

    Int32 screenWidthDp = 0;
    configuration->GetSmallestScreenWidthDp(&screenWidthDp);
    return screenWidthDp;
}

Int32 DeviceDisplayInfo::GetRotationDegrees()
{
    // ==================before translated======================
    // switch (getDisplay().getRotation()) {
    //     case Surface.ROTATION_0:
    //         return 0;
    //     case Surface.ROTATION_90:
    //         return 90;
    //     case Surface.ROTATION_180:
    //         return 180;
    //     case Surface.ROTATION_270:
    //         return 270;
    // }
    //
    // // This should not happen.
    // assert false;
    // return 0;

    Int32 rotation = 0;
    GetDisplay()->GetRotation(&rotation);
    switch (rotation) {
        case ISurface::ROTATION_0:
            return 0;
        case ISurface::ROTATION_90:
            return 90;
        case ISurface::ROTATION_180:
            return 180;
        case ISurface::ROTATION_270:
            return 270;
    }
    assert (FALSE);
    return 0;
}

AutoPtr<IDisplay> DeviceDisplayInfo::GetDisplay()
{
    // ==================before translated======================
    // return mWinManager.getDefaultDisplay();

    AutoPtr<IDisplay> display;
    mWinManager->GetDefaultDisplay((IDisplay**)&display);
    return display;
}

ECode DeviceDisplayInfo::NativeUpdateSharedDeviceDisplayInfo(
    /* [in] */ Int32 displayHeight,
    /* [in] */ Int32 displayWidth,
    /* [in] */ Int32 physicalDisplayHeight,
    /* [in] */ Int32 physicalDisplayWidth,
    /* [in] */ Int32 bitsPerPixel,
    /* [in] */ Int32 bitsPerComponent,
    /* [in] */ Double dipScale,
    /* [in] */ Int32 smallestDIPWidth,
    /* [in] */ Int32 rotationDegrees)
{
    Elastos_DeviceDisplayInfo_nativeUpdateSharedDeviceDisplayInfo(TO_IINTERFACE(this), displayHeight, displayWidth, physicalDisplayHeight, physicalDisplayWidth, bitsPerPixel, bitsPerComponent, dipScale, smallestDIPWidth, rotationDegrees);
    return NOERROR;
}

Int32 DeviceDisplayInfo::GetDisplayHeight(
    /* [in] */ IInterface* obj)
{
    DeviceDisplayInfo* mObj = (DeviceDisplayInfo*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("DeviceDisplayInfo", "DeviceDisplayInfo::GetDisplayHeight, mObj is NULL");
        return 0;
    }
    return mObj->GetDisplayHeight();
}

Int32 DeviceDisplayInfo::GetDisplayWidth(
    /* [in] */ IInterface* obj)
{
    DeviceDisplayInfo* mObj = (DeviceDisplayInfo*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("DeviceDisplayInfo", "DeviceDisplayInfo::GetDisplayWidth, mObj is NULL");
        return 0;
    }
    return mObj->GetDisplayWidth();
}

Int32 DeviceDisplayInfo::GetPhysicalDisplayHeight(
    /* [in] */ IInterface* obj)
{
    DeviceDisplayInfo* mObj = (DeviceDisplayInfo*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("DeviceDisplayInfo", "DeviceDisplayInfo::GetPhysicalDisplayHeight, mObj is NULL");
        return 0;
    }
    return mObj->GetPhysicalDisplayHeight();
}

Int32 DeviceDisplayInfo::GetPhysicalDisplayWidth(
    /* [in] */ IInterface* obj)
{
    DeviceDisplayInfo* mObj = (DeviceDisplayInfo*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("DeviceDisplayInfo", "DeviceDisplayInfo::GetPhysicalDisplayWidth, mObj is NULL");
        return 0;
    }
    return mObj->GetPhysicalDisplayWidth();
}

Int32 DeviceDisplayInfo::GetBitsPerPixel(
    /* [in] */ IInterface* obj)
{
    DeviceDisplayInfo* mObj = (DeviceDisplayInfo*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("DeviceDisplayInfo", "DeviceDisplayInfo::GetBitsPerPixel, mObj is NULL");
        return 0;
    }
    return mObj->GetBitsPerPixel();
}

Int32 DeviceDisplayInfo::GetBitsPerComponent(
    /* [in] */ IInterface* obj)
{
    DeviceDisplayInfo* mObj = (DeviceDisplayInfo*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("DeviceDisplayInfo", "DeviceDisplayInfo::GetBitsPerComponent, mObj is NULL");
        return 0;
    }
    return mObj->GetBitsPerComponent();
}

Double DeviceDisplayInfo::GetDIPScale(
    /* [in] */ IInterface* obj)
{
    DeviceDisplayInfo* mObj = (DeviceDisplayInfo*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("DeviceDisplayInfo", "DeviceDisplayInfo::GetDIPScale, mObj is NULL");
        return 0;
    }
    return mObj->GetDIPScale();
}

Int32 DeviceDisplayInfo::GetSmallestDIPWidth(
    /* [in] */ IInterface* obj)
{
    DeviceDisplayInfo* mObj = (DeviceDisplayInfo*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("DeviceDisplayInfo", "DeviceDisplayInfo::GetSmallestDIPWidth, mObj is NULL");
        return 0;
    }
    return mObj->GetSmallestDIPWidth();
}

Int32 DeviceDisplayInfo::GetRotationDegrees(
    /* [in] */ IInterface* obj)
{
    DeviceDisplayInfo* mObj = (DeviceDisplayInfo*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("DeviceDisplayInfo", "DeviceDisplayInfo::GetRotationDegrees, mObj is NULL");
        return 0;
    }
    return mObj->GetRotationDegrees();
}

AutoPtr<IInterface> DeviceDisplayInfo::Create(
    /* [in] */ IInterface* context)
{
    IContext* c = IContext::Probe(context);
    return TO_IINTERFACE(Create(c));
}

} // namespace Gfx
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


