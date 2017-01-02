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
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/InputEvent.h"
#include "elastos/droid/view/InputDevice.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/logging/Logger.h>

using namespace Elastos::Utility::Logging;

namespace Elastos {
namespace Droid {
namespace View {

const Int32 InputEvent::PARCEL_TOKEN_MOTION_EVENT;
const Int32 InputEvent::PARCEL_TOKEN_KEY_EVENT;

Int32 InputEvent::sNextSeq = 0;
Object InputEvent::sNextSeqLock;

const Boolean InputEvent::TRACK_RECYCLED_LOCATION;

CAR_INTERFACE_IMPL_2(InputEvent, Object, IInputEvent, IParcelable)

InputEvent::InputEvent()
    : mRecycled(FALSE)
{
    AutoLock lock(sNextSeqLock);
    mSeq = ++sNextSeq;
}

ECode InputEvent::GetDevice(
    /* [out] */ IInputDevice** device)
{
    VALIDATE_NOT_NULL(device)
    Int32 deviceId;
    GetDeviceId(&deviceId);
    return InputDevice::GetDevice(deviceId, device);
}

ECode InputEvent::IsFromSource(
        /* [in] */ Int32 source,
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 rSource;
    GetSource(&rSource);
    *result = (rSource & source) == source;
    return NOERROR;
}

ECode InputEvent::Recycle()
{
    // if (TRACK_RECYCLED_LOCATION) {
    //     if (mRecycledLocation != null) {
    //         throw new RuntimeException(toString() + " recycled twice!", mRecycledLocation);
    //     }
    //     mRecycledLocation = new RuntimeException("Last recycled here");
    // }
    // else {
        if (mRecycled) {
            Logger::E(String("InputEvent"), String("recycled twice!"));
            return E_RUNTIME_EXCEPTION;
            //throw new RuntimeException(toString() + " recycled twice!");
        }
        mRecycled = TRUE;
    //}

    return NOERROR;
}

ECode InputEvent::RecycleIfNeededAfterDispatch()
{
    return Recycle();
}

void InputEvent::PrepareForReuse()
{
    mRecycled = FALSE;
    AutoLock lock(sNextSeqLock);
    mSeq = ++sNextSeq;
}

ECode InputEvent::GetSequenceNumber(
    /* [out] */ Int32* seq)
{
    VALIDATE_NOT_NULL(seq)
    *seq = mSeq;

    return NOERROR;
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
