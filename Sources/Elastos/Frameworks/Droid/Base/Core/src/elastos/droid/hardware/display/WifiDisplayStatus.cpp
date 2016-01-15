
#include "elastos/droid/hardware/display/WifiDisplayStatus.h"
#include "elastos/droid/hardware/display/WifiDisplay.h"
#include "elastos/droid/hardware/display/WifiDisplaySessionInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CAR_INTERFACE_IMPL_2(WifiDisplayStatus, Object, IWifiDisplayStatus, IParcelable)

WifiDisplayStatus::WifiDisplayStatus()
    : mFeatureState(0)
    , mScanState(0)
    , mActiveDisplayState(0)
{
}

ECode WifiDisplayStatus::constructor()
{
    return constructor(FEATURE_STATE_UNAVAILABLE, SCAN_STATE_NOT_SCANNING,
            DISPLAY_STATE_NOT_CONNECTED, NULL,
            WifiDisplay::EMPTY_ARRAY, NULL);
}

ECode WifiDisplayStatus::constructor(
    /* [in] */ Int32 featureState,
    /* [in] */ Int32 scanState,
    /* [in] */ Int32 activeDisplayState,
    /* [in] */ IWifiDisplay* activeDisplay,
    /* [in] */ ArrayOf<IWifiDisplay*>* displays,
    /* [in] */ IWifiDisplaySessionInfo* sessionInfo)
{
    if (displays == NULL) {
        Logger::E("WifiDisplayStatus", "displays must not be null");
    }

    mFeatureState = featureState;
    mScanState = scanState;
    mActiveDisplayState = activeDisplayState;
    mActiveDisplay = activeDisplay;
    mDisplays = displays;

    if (sessionInfo != NULL) {
        mSessionInfo = sessionInfo;
    }
    else {
        mSessionInfo = new WifiDisplaySessionInfo();
    }
    return NOERROR;
}

ECode WifiDisplayStatus::GetFeatureState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);

    *state = mFeatureState;
    return NOERROR;
}

ECode WifiDisplayStatus::GetScanState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);

    *state = mScanState;
    return NOERROR;
}

ECode WifiDisplayStatus::GetActiveDisplayState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);

    *state = mActiveDisplayState;
    return NOERROR;
}

ECode WifiDisplayStatus::GetActiveDisplay(
    /* [out] */ IWifiDisplay** display)
{
    VALIDATE_NOT_NULL(display)

    *display = mActiveDisplay;
    REFCOUNT_ADD(*display);

    return NOERROR;
}

ECode WifiDisplayStatus::GetDisplays(
    /* [out, callee] */ ArrayOf<IWifiDisplay*>** displays)
{
    VALIDATE_NOT_NULL(displays);

    *displays = mDisplays;
    REFCOUNT_ADD(*displays);

    return NOERROR;
}

ECode WifiDisplayStatus::GetSessionInfo(
    /* [out] */ IWifiDisplaySessionInfo** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSessionInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiDisplayStatus::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadInt32(&mFeatureState));
    FAIL_RETURN(source->ReadInt32(&mScanState));
    FAIL_RETURN(source->ReadInt32(&mActiveDisplayState));

    mActiveDisplay = NULL;
    Int32 active;
    FAIL_RETURN(source->ReadInt32(&active));
    if (1 == active) {
        mActiveDisplay = new WifiDisplay();
        IParcelable::Probe(mActiveDisplay)->ReadFromParcel(source);
    }

    mDisplays = NULL;
    Int32 length;;
    FAIL_RETURN(source->ReadInt32(&length));
    mDisplays = ArrayOf<IWifiDisplay*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IWifiDisplay> temp;
        temp = new WifiDisplay();
        FAIL_RETURN(IParcelable::Probe(temp)->ReadFromParcel(source));
        mDisplays->Set(i, temp);
    }

    return IParcelable::Probe(mSessionInfo)->ReadFromParcel(source);
}

ECode WifiDisplayStatus::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteInt32(mFeatureState));
    FAIL_RETURN(dest->WriteInt32(mScanState));
    FAIL_RETURN(dest->WriteInt32(mActiveDisplayState));

    if (mActiveDisplay != NULL) {
        FAIL_RETURN(dest->WriteInt32(1));
        FAIL_RETURN(IParcelable::Probe(mActiveDisplay)->WriteToParcel(dest));
    }
    else {
        FAIL_RETURN(dest->WriteInt32(0));
    }

    Int32 length = mDisplays->GetLength();
    FAIL_RETURN(dest->WriteInt32(length));
    for (Int32 i = 0; i < length; i++) {
        FAIL_RETURN(IParcelable::Probe((*mDisplays)[i])->WriteToParcel(dest));
    }

    return IParcelable::Probe(mSessionInfo)->WriteToParcel(dest);
}

ECode WifiDisplayStatus::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);

    StringBuilder sb("WifiDisplayStatus{featureState=");
    sb += mFeatureState;
    sb += ", scanState=";
    sb += mScanState;
    sb += ", activeDisplayState=";
    sb += mActiveDisplayState;
    sb += ", activeDisplay=";
    String temp("NULL");
    if (mActiveDisplay != NULL) {
        mActiveDisplay->ToString(&temp);
    }
    sb += temp;

    Int32 size = mDisplays ? mDisplays->GetLength() : 0;
    sb += ", displays={size:";
    sb += size;
    sb += " ";
    for (Int32 i = 0; i < size; ++i) {
        if (i != 0) {
            sb += ", ";
        }
        sb += (i + 1);
        sb += " ";
        (*mDisplays)[i]->ToString(&temp);
        sb += temp;
    }

    sb += "}, sessionInfo=";
    mSessionInfo->ToString(&temp);
    sb += temp;
    sb += "}";

    *info = sb.ToString();
    return NOERROR;
}

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
