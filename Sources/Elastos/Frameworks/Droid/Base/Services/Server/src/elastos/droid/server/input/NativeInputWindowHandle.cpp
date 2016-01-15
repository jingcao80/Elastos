
#include "input/NativeInputWindowHandle.h"
#include "input/InputWindowHandle.h"
#include "elastos/droid/view/NativeInputChannel.h"

using Elastos::Droid::View::NativeInputChannel;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

NativeInputWindowHandle::NativeInputWindowHandle(
    /* [in] */ const android::sp<android::InputApplicationHandle>& inputApplicationHandle,
    /* [in] */ Input::InputWindowHandle* obj)
    : InputWindowHandle(inputApplicationHandle)
    , mObject(obj)
{}

NativeInputWindowHandle::~NativeInputWindowHandle()
{
    mObject = NULL;
}

Input::InputWindowHandle* NativeInputWindowHandle::getInputWindowHandleObj()
{
    return mObject;
}

bool NativeInputWindowHandle::updateInfo()
{
    if (!mObject) {
        releaseInfo();
        return false;
    }

    if (!mInfo) {
        mInfo = new android::InputWindowInfo();
    }

    IInputChannel* inputChannelObj = mObject->mInputChannel;
    if (inputChannelObj) {
        NativeInputChannel* nativeInputChannel;
        inputChannelObj->GetNativeInputChannel((Handle32*)&nativeInputChannel);
        mInfo->inputChannel = nativeInputChannel != NULL ? nativeInputChannel->getInputChannel() : NULL;
    }
    else {
        mInfo->inputChannel.clear();
    }

    if (!mObject->mName.IsNull()) {
        mInfo->name.setTo((const char*)mObject->mName);
    }
    else {
        mInfo->name.setTo("<null>");
    }

    mInfo->layoutParamsFlags = mObject->mLayoutParamsFlags;
    mInfo->layoutParamsType = mObject->mLayoutParamsType;
    mInfo->dispatchingTimeout = mObject->mDispatchingTimeoutNanos;
    mInfo->frameLeft = mObject->mFrameLeft;
    mInfo->frameTop = mObject->mFrameTop;
    mInfo->frameRight = mObject->mFrameRight;
    mInfo->frameBottom = mObject->mFrameBottom;
    mInfo->scaleFactor = mObject->mScaleFactor;

    IRegion* regionObj = mObject->mTouchableRegion;
    if (regionObj) {
        SkRegion* region;
        regionObj->GetNativeRegion((Handle32*)&region);
        mInfo->touchableRegion.set(*region);
    }
    else {
        mInfo->touchableRegion.setEmpty();
    }

    mInfo->visible = mObject->mVisible;
    mInfo->canReceiveKeys = mObject->mCanReceiveKeys;
    mInfo->hasFocus = mObject->mHasFocus;
    mInfo->hasWallpaper = mObject->mHasWallpaper;
    mInfo->paused = mObject->mPaused;
    mInfo->layer = mObject->mLayer;
    mInfo->ownerPid = mObject->mOwnerPid;
    mInfo->ownerUid = mObject->mOwnerUid;
    mInfo->inputFeatures = mObject->mInputFeatures;
    mInfo->displayId = mObject->mDisplayId;

    return true;
}

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos
