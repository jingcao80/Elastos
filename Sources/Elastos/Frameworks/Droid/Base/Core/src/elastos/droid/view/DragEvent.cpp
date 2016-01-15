
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/DragEvent.h"
#include "elastos/droid/content/CClipData.h"
#include "elastos/droid/content/CClipDescription.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::CClipData;
using Elastos::Droid::Content::IClipDescription;
using Elastos::Droid::Content::CClipDescription;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

const Int32 DragEvent::MAX_RECYCLED;
const Boolean DragEvent::TRACK_RECYCLED_LOCATION;
Mutex DragEvent::sRecyclerLock;
Int32 DragEvent::sRecyclerUsed = 0;
AutoPtr<DragEvent> DragEvent::sRecyclerTop;

CAR_INTERFACE_IMPL_2(DragEvent, Object, IDragEvent, IParcelable)

ECode DragEvent::GetAction(
    /* [out] */ Int32* action)
{
    VALIDATE_NOT_NULL(action);
    *action = mAction;
    return NOERROR;
}

ECode DragEvent::SetAction(
    /* [in] */ Int32 action)
{
    mAction = action;
    return NOERROR;
}

ECode DragEvent::GetX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);
    *x = mX;
    return NOERROR;
}

ECode DragEvent::GetY(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mY;
    return NOERROR;
}

ECode DragEvent::SetX(
    /* [in] */ Float x)
{
    mX = x;
    return NOERROR;
}

ECode DragEvent::SetY(
    /* [in] */ Float y)
{
    mY = y;
    return NOERROR;
}

ECode DragEvent::GetClipData(
    /* [out] */ IClipData** clipData)
{
    VALIDATE_NOT_NULL(clipData);
    *clipData = mClipData;
    REFCOUNT_ADD(*clipData);
    return NOERROR;
}

ECode DragEvent::GetClipDescription(
    /* [out] */ IClipDescription** description)
{
    VALIDATE_NOT_NULL(description);
    *description = mClipDescription;
    REFCOUNT_ADD(*description);
    return NOERROR;
}

ECode DragEvent::GetLocalState(
    /* [out] */ IInterface** localState)
{
    VALIDATE_NOT_NULL(localState);
    *localState = mLocalState;
    REFCOUNT_ADD(*localState);
    return NOERROR;
}

ECode DragEvent::SetLocalState(
    /* [in] */ IInterface* localState)
{
    mLocalState = localState;
    return NOERROR;
}

ECode DragEvent::GetResult(
    /* [out] */ Boolean* dragResult)
{
    VALIDATE_NOT_NULL(dragResult);
    *dragResult = mDragResult;
    return NOERROR;
}

ECode DragEvent::Recycle()
{
    // Ensure recycle is only called once!
    if (TRACK_RECYCLED_LOCATION) {
        if (mRecycledLocation) {
            //throw new RuntimeException(toString() + " recycled twice!", mRecycledLocation);
            Logger::E(String("DragEvent"), String("recycled twice!"));
            return E_RUNTIME_EXCEPTION;
        }
        mRecycledLocation = !mRecycledLocation;
        Logger::D(String("DragEvent"), String("Last recycled here!"));
    } else {
        if (mRecycled) {
            Logger::E("DragEvent", "recycled twice!");
            return E_RUNTIME_EXCEPTION;
        }
        mRecycled = TRUE;
    }

    mClipData = NULL;
    mClipDescription = NULL;
    mLocalState = NULL;

    Mutex::AutoLock lock(sRecyclerLock);
    if (sRecyclerUsed < MAX_RECYCLED) {
        sRecyclerUsed++;
        mNext = sRecyclerTop;
        sRecyclerTop = this;
    }

    return NOERROR;
}

ECode DragEvent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadInt32(&mAction))
    FAIL_RETURN(source->ReadFloat(&mX))
    FAIL_RETURN(source->ReadFloat(&mY))
    Int32 flag;
    FAIL_RETURN(source->ReadInt32(&flag))
    mDragResult = flag != 0;
    source->ReadInt32(&flag);
    if(flag != 0) {
        mClipData = NULL;
        CClipData::New((IClipData**)&mClipData);
        IParcelable::Probe(mClipData)->ReadFromParcel(source);
    }

    source->ReadInt32(&flag);
    if(flag != 0) {
        mClipDescription = NULL;
        CClipDescription::New((IClipDescription**)&mClipDescription);
        IParcelable::Probe(mClipDescription)->ReadFromParcel(source);
    }

    return NOERROR;
}

/**
 * Creates a {@link android.os.Parcel} object from this DragEvent object.
 * @param dest A {@link android.os.Parcel} object in which to put the DragEvent object.
 * @param flags Flags to store in the Parcel.
 */
ECode DragEvent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    FAIL_RETURN(dest->WriteInt32(mAction));
    FAIL_RETURN(dest->WriteFloat(mX));
    FAIL_RETURN(dest->WriteFloat(mY));
    FAIL_RETURN(dest->WriteInt32(mDragResult ? 1 : 0));
    if (NULL == mClipData) {
        FAIL_RETURN(dest->WriteInt32(0));
    }
    else {
        FAIL_RETURN(dest->WriteInt32(1));
        assert(IParcelable::Probe(mClipData) != NULL);
        IParcelable::Probe(mClipData)->WriteToParcel(dest);
    }
    if (NULL == mClipDescription) {
        FAIL_RETURN(dest->WriteInt32(0));
    }
    else {
        FAIL_RETURN(dest->WriteInt32(1));
        IParcelable::Probe(mClipDescription)->WriteToParcel(dest);
    }
    return NOERROR;
}

DragEvent::DragEvent()
    : mRecycledLocation(FALSE)
    , mRecycled(FALSE)
{}

ECode DragEvent::constructor()
{
    return NOERROR;
}

ECode DragEvent::Init(
        /* [in] */ Int32 action,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IClipDescription * description,
        /* [in] */ IClipData* data,
        /* [in] */ IInterface* localState,
        /* [in] */ Boolean result)
{
    mAction = action;
    mX = x;
    mY = y;
    mClipDescription = description;
    mClipData = data;
    mLocalState = localState;
    mDragResult = result;
    return NOERROR;
}


ECode DragEvent::Obtain(
    /* [out] */ IDragEvent** event)
{
    VALIDATE_NOT_NULL(event);
    return Obtain(0, 0.0f, 0.0f, NULL, NULL, NULL, FALSE, event);
}

ECode DragEvent::Obtain(
    /* [in] */ Int32 action,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IInterface * pLocalState,
    /* [in] */ IClipDescription * pDescription,
    /* [in] */ IClipData * pData,
    /* [in] */ Boolean result,
    /* [out] */ IDragEvent ** ppEvent)
{
    AutoPtr<DragEvent> event= Obtain(action, x, y, pLocalState, pDescription, pData, result);
    *ppEvent = event.Get();
    REFCOUNT_ADD(*ppEvent)
    return NOERROR;
}


ECode DragEvent::Obtain(
    /* [in] */ IDragEvent* source,
    /* [out] */ IDragEvent** event)
{
    if (source == NULL) {
        Logger::E("DragEvent", "other drag event must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<DragEvent> sourceParam = (DragEvent*)source;
    return(Obtain(sourceParam->mAction, sourceParam->mX, sourceParam->mY,
        sourceParam->mLocalState, sourceParam->mClipDescription, sourceParam->mClipData,
        sourceParam->mDragResult, event));
}


AutoPtr<DragEvent> DragEvent::Obtain(
    /* [in] */ Int32 action,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IInterface* localState,
    /* [in] */ IClipDescription* description,
    /* [in] */ IClipData* data,
    /* [in] */ Boolean result)
{
    AutoPtr<DragEvent> event;
    {
        Mutex::AutoLock lock(sRecyclerLock);
        if (sRecyclerTop == NULL) {
            event = new DragEvent();
            event->Init(action, x, y, description, data, localState, result);
            return event;

        }
        event = sRecyclerTop;
        sRecyclerTop = event->mNext;
        sRecyclerUsed -= 1;
    }
    event->mRecycledLocation = FALSE;
    event->mRecycled = FALSE;
    event->mNext = NULL;
    event->Init(action, x, y, description, data, localState, result);
    return event;
}

} // View
} // Droid
} // Elastos

