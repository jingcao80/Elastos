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

#include "elastos/droid/server/input/NativeInputWindowHandle.h"
#include "elastos/droid/view/NativeInputChannel.h"
#include "elastos/droid/view/InputChannel.h"
#include <skia/core/SkRegion.h>

using Elastos::Droid::View::NativeInputChannel;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

NativeInputWindowHandle::NativeInputWindowHandle(
    /* [in] */ const android::sp<android::InputApplicationHandle>& inputApplicationHandle,
    /* [in] */ IWeakReference* objWeak)
    : InputWindowHandle(inputApplicationHandle)
    , mObjWeak(objWeak)
{}

NativeInputWindowHandle::~NativeInputWindowHandle()
{
    mObjWeak = NULL;
}

AutoPtr<Elastos::Droid::Server::Input::InputWindowHandle> NativeInputWindowHandle::getInputWindowHandleObj()
{
    AutoPtr<IInputWindowHandle> obj;
    mObjWeak->Resolve(EIID_IInputWindowHandle, (IInterface**)&obj);
    return (Elastos::Droid::Server::Input::InputWindowHandle*)obj.Get();
}

bool NativeInputWindowHandle::updateInfo()
{
    AutoPtr<IInputWindowHandle> obj;
    mObjWeak->Resolve(EIID_IInputWindowHandle, (IInterface**)&obj);
    if (!obj) {
        releaseInfo();
        return false;
    }

    if (!mInfo) {
        mInfo = new android::InputWindowInfo();
    }
    else {
        mInfo->touchableRegion.clear();
    }

    Elastos::Droid::Server::Input::InputWindowHandle* handle =
            (Elastos::Droid::Server::Input::InputWindowHandle*)obj.Get();
    AutoPtr<IInputChannel> inputChannelObj = handle->mInputChannel;
    if (inputChannelObj) {
        Handle64 ptr;
        inputChannelObj->GetNativeInputChannel(&ptr);
        NativeInputChannel* nativeInputChannel = reinterpret_cast<NativeInputChannel*>(ptr);
        mInfo->inputChannel = nativeInputChannel != NULL ? nativeInputChannel->getInputChannel() : NULL;
    }
    else {
        mInfo->inputChannel.clear();
    }

    if (!handle->mName.IsNull()) {
        mInfo->name.setTo(handle->mName.string());
    }
    else {
        mInfo->name.setTo("<null>");
    }

    mInfo->layoutParamsFlags = handle->mLayoutParamsFlags;
    mInfo->layoutParamsType = handle->mLayoutParamsType;
    mInfo->dispatchingTimeout = handle->mDispatchingTimeoutNanos;
    mInfo->frameLeft = handle->mFrameLeft;
    mInfo->frameTop = handle->mFrameTop;
    mInfo->frameRight = handle->mFrameRight;
    mInfo->frameBottom = handle->mFrameBottom;
    mInfo->scaleFactor = handle->mScaleFactor;

    AutoPtr<IRegion> regionObj = handle->mTouchableRegion;
    if (regionObj) {
        Int64 regionHandle;
        regionObj->GetNativeRegion((Handle64*)&regionHandle);
        SkRegion* region = reinterpret_cast<SkRegion*>(regionHandle);
        SkASSERT(region != NULL);

        for (SkRegion::Iterator it(*region); !it.done(); it.next()) {
            const SkIRect& rect = it.rect();
            mInfo->addTouchableRegion(android::Rect(rect.fLeft, rect.fTop, rect.fRight, rect.fBottom));
        }
    }

    mInfo->visible = handle->mVisible;
    mInfo->canReceiveKeys = handle->mCanReceiveKeys;
    mInfo->hasFocus = handle->mHasFocus;
    mInfo->hasWallpaper = handle->mHasWallpaper;
    mInfo->paused = handle->mPaused;
    mInfo->layer = handle->mLayer;
    mInfo->ownerPid = handle->mOwnerPid;
    mInfo->ownerUid = handle->mOwnerUid;
    mInfo->inputFeatures = handle->mInputFeatures;
    mInfo->displayId = handle->mDisplayId;

    return true;
}

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos
