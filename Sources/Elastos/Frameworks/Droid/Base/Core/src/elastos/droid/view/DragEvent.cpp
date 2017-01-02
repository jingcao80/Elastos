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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/DragEvent.h"
#include "elastos/droid/view/CDragEvent.h"
#include "elastos/droid/content/CClipData.h"
#include "elastos/droid/content/CClipDescription.h"
#include <elastos/core/AutoLock.h>
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
Object DragEvent::sRecyclerLock;
Int32 DragEvent::sRecyclerUsed = 0;
AutoPtr<IDragEvent> DragEvent::sRecyclerTop;

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

    AutoLock lock(sRecyclerLock);
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


AutoPtr<IDragEvent> DragEvent::Obtain()
{
    return Obtain(0, 0.0f, 0.0f, NULL, NULL, NULL, FALSE);
}

AutoPtr<IDragEvent> DragEvent::Obtain(
    /* [in] */ Int32 action,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IInterface * localState,
    /* [in] */ IClipDescription * description,
    /* [in] */ IClipData * data,
    /* [in] */ Boolean result)
{
    AutoPtr<CDragEvent> ev;
    {
        AutoLock lock(sRecyclerLock);
        if (sRecyclerTop == NULL) {
            CDragEvent::NewByFriend((CDragEvent**)&ev);
            ev->Init(action, x, y, description, data, localState, result);
            return (IDragEvent*)ev.Get();
        }
        ev = (CDragEvent*)sRecyclerTop.Get();
        sRecyclerTop = ev->mNext;
        sRecyclerUsed -= 1;
    }
    ev->mRecycledLocation = FALSE;
    ev->mRecycled = FALSE;
    ev->mNext = NULL;

    ev->Init(action, x, y, description, data, localState, result);

    return (IDragEvent*)ev.Get();
}

AutoPtr<IDragEvent> DragEvent::Obtain(
    /* [in] */ IDragEvent* source)
{
    Int32 action;
    source->GetAction(&action);
    Float x, y;
    source->GetX(&x);
    source->GetY(&y);
    AutoPtr<IInterface> localState;
    source->GetLocalState((IInterface**)&localState);
    AutoPtr<IClipDescription> clipDescription;
    source->GetClipDescription((IClipDescription**)&clipDescription);
    AutoPtr<IClipData> clipData;
    source->GetClipData((IClipData**)&clipData);
    Boolean result;
    source->GetResult(&result);
    return Obtain(action, x, y, localState, clipDescription, clipData, result);
}

} // View
} // Droid
} // Elastos

