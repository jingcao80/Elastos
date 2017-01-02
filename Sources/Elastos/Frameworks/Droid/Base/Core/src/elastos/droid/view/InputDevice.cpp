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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"

#include "elastos/droid/os/NullVibrator.h"
#include "elastos/droid/view/InputDevice.h"
#include "elastos/droid/view/CKeyCharacterMap.h"
#include "elastos/droid/view/MotionEvent.h"
#include "elastos/droid/hardware/input/CInputManager.h"
#include "elastos/droid/hardware/input/CInputDeviceIdentifier.h"
#include "elastos/droid/hardware/input/CInputManagerHelper.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;
using Elastos::Droid::Os::NullVibrator;
using Elastos::Droid::Hardware::Input::CInputManager;
using Elastos::Droid::Hardware::Input::CInputDeviceIdentifier;
using Elastos::Droid::Hardware::Input::IInputManagerHelper;
using Elastos::Droid::Hardware::Input::CInputManagerHelper;
using Elastos::Droid::Hardware::Input::IInputManager;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(InputDevice::MotionRange, Object, IMotionRange)
CAR_INTERFACE_IMPL_2(InputDevice, Object, IInputDevice, IParcelable)

InputDevice::MotionRange::MotionRange(
    /* [in] */ Int32 axis,
    /* [in] */ Int32 source,
    /* [in] */ Float min,
    /* [in] */ Float max,
    /* [in] */ Float flat,
    /* [in] */ Float fuzz,
    /* [in] */ Float resolution)
    : mAxis(axis)
    , mSource(source)
    , mMin(min)
    , mMax(max)
    , mFlat(flat)
    , mFuzz(fuzz)
    , mResolution(resolution)
{
}

ECode InputDevice::MotionRange::GetAxis(
    /* [out] */ Int32* axis)
{
    VALIDATE_NOT_NULL(axis)
    *axis = mAxis;
    return NOERROR;
}

ECode InputDevice::MotionRange::GetSource(
    /* [out] */ Int32* source)
{
    VALIDATE_NOT_NULL(source)
    *source = mSource;
    return NOERROR;
}

ECode InputDevice::MotionRange::IsFromSource(
    /* [in] */ Int32 source,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = (mSource & source) == source;
    return NOERROR;
}

ECode InputDevice::MotionRange::GetMin(
    /* [out] */ Float* minimum)
{
    VALIDATE_NOT_NULL(minimum);
    *minimum = mMin;
    return NOERROR;
}

ECode InputDevice::MotionRange::GetMax(
    /* [out] */ Float* maximum)
{
    VALIDATE_NOT_NULL(maximum);
    *maximum = mMax;
    return NOERROR;
}

ECode InputDevice::MotionRange::GetRange(
    /* [out] */ Float* range)
{
    VALIDATE_NOT_NULL(range);
    *range = mMax - mMin;
    return NOERROR;
}

ECode InputDevice::MotionRange::GetFlat(
    /* [out] */ Float* flat)
{
    VALIDATE_NOT_NULL(flat);
    *flat = mFlat;
    return NOERROR;
}

ECode InputDevice::MotionRange::GetFuzz(
    /* [out] */ Float* fuzz)
{
    VALIDATE_NOT_NULL(fuzz);
    *fuzz = mFuzz;
    return NOERROR;
}

ECode InputDevice::MotionRange::GetResolution(
    /* [out] */ Float* resolution)
{
    VALIDATE_NOT_NULL(resolution);
    *resolution = mResolution;
    return NOERROR;
}

InputDevice::InputDevice()
    : mId(0)
    , mGeneration(0)
    , mControllerNumber(0)
    , mVendorId(0)
    , mProductId(0)
    , mIsExternal(FALSE)
    , mSources(0)
    , mKeyboardType(0)
    , mHasVibrator(FALSE)
    , mHasButtonUnderPad(FALSE)
{
}

InputDevice::~InputDevice()
{
}

ECode InputDevice::constructor()
{
    CArrayList::New((IArrayList**)&mMotionRanges);
    return NOERROR;
}

ECode InputDevice::constructor(
    /* [in] */ Int32 id,
    /* [in] */ Int32 generation,
    /* [in] */ Int32 controllerNumber,
    /* [in] */ const String& name,
    /* [in] */ Int32 vendorId,
    /* [in] */ Int32 productId,
    /* [in] */ const String& descriptor,
    /* [in] */ Boolean isExternal,
    /* [in] */ Int32 sources,
    /* [in] */ Int32 keyboardType,
    /* [in] */ IKeyCharacterMap* keyCharacterMap,
    /* [in] */ Boolean hasVibrator,
    /* [in] */ Boolean hasButtonUnderPad)
{
    CArrayList::New((IArrayList**)&mMotionRanges);

    mId = id;
    mGeneration = generation;
    mControllerNumber = controllerNumber;
    mName = name;
    mVendorId = vendorId;
    mProductId = productId;
    mDescriptor = descriptor;
    mIsExternal = isExternal;
    mSources = sources;
    mKeyboardType = keyboardType;
    mKeyCharacterMap = keyCharacterMap;
    mHasVibrator = hasVibrator;
    mHasButtonUnderPad = hasButtonUnderPad;
    CInputDeviceIdentifier::New(descriptor, vendorId, productId, (IInputDeviceIdentifier**)&mIdentifier);
    return NOERROR;
}

ECode InputDevice::GetDevice(
    /* [in] */ Int32 id,
    /* [out] */ IInputDevice** device)
{
    return CInputManager::GetInstance()->GetInputDevice(id, device);
}

ECode InputDevice::GetDeviceIds(
    /* [out, callee] */ ArrayOf<Int32>** deviceIds)
{
    return CInputManager::GetInstance()->GetInputDeviceIds(deviceIds);
}

ECode InputDevice::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode InputDevice::GetControllerNumber(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = mControllerNumber;
    return NOERROR;
}

ECode InputDevice::GetIdentifier(
    /* [out] */ IInputDeviceIdentifier** identifier)
{
    VALIDATE_NOT_NULL(identifier)
    *identifier = mIdentifier;
    REFCOUNT_ADD(*identifier)
    return NOERROR;
}

ECode InputDevice::GetGeneration(
    /* [out] */ Int32* generation)
{
    VALIDATE_NOT_NULL(generation);
    *generation = mGeneration;
    return NOERROR;
}

ECode InputDevice::GetVendorId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mVendorId;
    return NOERROR;
}

ECode InputDevice::GetProductId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mProductId;
    return NOERROR;
}

ECode InputDevice::GetDescriptor(
    /* [out] */ String* descriptor)
{
    VALIDATE_NOT_NULL(descriptor);
    *descriptor = mDescriptor;
    return NOERROR;
}

ECode InputDevice::IsVirtual(
    /* [out] */ Boolean* isVirtual)
{
     VALIDATE_NOT_NULL(isVirtual);
     *isVirtual = mId < 0;
     return NOERROR;
}

ECode InputDevice::IsExternal(
    /* [out] */ Boolean* isExternal)
{
    VALIDATE_NOT_NULL(isExternal);
    *isExternal = mIsExternal;
    return NOERROR;
}

ECode InputDevice::IsFullKeyboard(
    /* [out] */ Boolean* isFullKeyboard)
{
    VALIDATE_NOT_NULL(isFullKeyboard);
    *isFullKeyboard = (mSources & SOURCE_KEYBOARD) == SOURCE_KEYBOARD
        && mKeyboardType == KEYBOARD_TYPE_ALPHABETIC;
    return NOERROR;
}

ECode InputDevice::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode InputDevice::GetSources(
    /* [out] */ Int32* sources)
{
    VALIDATE_NOT_NULL(sources);
    *sources = mSources;
    return NOERROR;
}

ECode InputDevice::SupportsSource(
    /* [in] */ Int32 source,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);
    *rst = (mSources & source) == source;
    return NOERROR;
}

ECode InputDevice::GetKeyboardType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mKeyboardType;
    return NOERROR;
}

ECode InputDevice::GetKeyCharacterMap(
    /* [out] */ IKeyCharacterMap** keyCharacterMap)
{
    VALIDATE_NOT_NULL(keyCharacterMap);
    *keyCharacterMap = mKeyCharacterMap;
    REFCOUNT_ADD(*keyCharacterMap);
    return NOERROR;
}

ECode InputDevice::GetHasVibrator(
    /* [out] */ Boolean* hasVibrator)
{
    VALIDATE_NOT_NULL(hasVibrator)
    *hasVibrator = mHasVibrator;
    return NOERROR;
}

ECode InputDevice::GetHasButtonUnderPad(
    /* [out] */ Boolean* hasButtonUnderPad)
{
    VALIDATE_NOT_NULL(hasButtonUnderPad)
    *hasButtonUnderPad = mHasButtonUnderPad;
    return NOERROR;
}

ECode InputDevice::HasKeys(
    /* [in] */ ArrayOf<Int32>* keys,
    /* [out, calleee] */ ArrayOf<Boolean>** rsts)
{
    VALIDATE_NOT_NULL(rsts)
    AutoPtr<IInputManagerHelper> helper;
    CInputManagerHelper::AcquireSingleton((IInputManagerHelper**)&helper);
    AutoPtr<IInputManager> manager;
    helper->GetInstance((IInputManager**)&manager);
    AutoPtr<ArrayOf<Int32> > param = ArrayOf<Int32>::Alloc(keys->GetLength() + 1);
    param->Set(0, mId);
    param->Copy(1, keys);
    return manager->DeviceHasKeys(*param, rsts);
}

ECode InputDevice::GetMotionRange(
    /* [in] */ Int32 axis,
    /* [out] */ IMotionRange** montionRange)
{
    VALIDATE_NOT_NULL(montionRange);

    Int32 size;
    mMotionRanges->GetSize(&size);

    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> tmp;
        mMotionRanges->Get(i, (IInterface**)&tmp);
        AutoPtr<IMotionRange> rangeItf = IMotionRange::Probe(tmp);
        MotionRange* range = (MotionRange*)(rangeItf.Get());
        if (range->mAxis == axis) {
            *montionRange = range;
            REFCOUNT_ADD(*montionRange);
            return NOERROR;
        }
    }
    *montionRange = NULL;

    return NOERROR;
}

ECode InputDevice::GetMotionRange(
    /* [in] */ Int32 axis,
    /* [in] */ Int32 source,
    /* [out] */ IMotionRange** montionRange)
{
    VALIDATE_NOT_NULL(montionRange);

    Int32 size;
    mMotionRanges->GetSize(&size);

    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> tmp;
        mMotionRanges->Get(i, (IInterface**)&tmp);
        AutoPtr<IMotionRange> rangeItf = IMotionRange::Probe(tmp);
        MotionRange* range = (MotionRange*)(rangeItf.Get());
        if (range->mAxis == axis && range->mSource == source) {
            *montionRange = range;
            REFCOUNT_ADD(*montionRange);
            return NOERROR;
        }
    }
    *montionRange = NULL;

    return NOERROR;
}

ECode InputDevice::GetMotionRanges(
    /* [out] */ IList** motionRanges)
{
    VALIDATE_NOT_NULL(motionRanges);
    *motionRanges = IList::Probe(mMotionRanges);
    REFCOUNT_ADD(*motionRanges)
    return NOERROR;
}

ECode InputDevice::AddMotionRange(
    /* [in] */ Int32 axis,
    /* [in] */ Int32 source,
    /* [in] */ Float min,
    /* [in] */ Float max,
    /* [in] */ Float flat,
    /* [in] */ Float fuzz,
    /* [in] */ Float resolution)
{
    AutoPtr<MotionRange> range = new MotionRange(axis, source, min, max, flat, fuzz, resolution);
    mMotionRanges->Add((IMotionRange*)range.Get());
    return NOERROR;
}

ECode InputDevice::GetVibrator(
    /* [out] */ IVibrator** vibrator)
{
    VALIDATE_NOT_NULL(vibrator);

    ISynchronize* sync = ISynchronize::Probe(mMotionRanges);
    AutoLock lock(sync);
    if (mVibrator == NULL) {
        if (mHasVibrator) {
            CInputManager::GetInstance()->GetInputDeviceVibrator(
                mId, (IVibrator**)&mVibrator);
        }
        else {
            mVibrator = (IVibrator*)NullVibrator::GetInstance().Get();
        }
    }
    *vibrator = mVibrator;
    REFCOUNT_ADD(*vibrator);

    return NOERROR;
}

ECode InputDevice::HasButtonUnderPad(
    /* [in] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = mHasButtonUnderPad;
    return NOERROR;
}

ECode InputDevice::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mId);
    in->ReadInt32(&mGeneration);
    in->ReadInt32(&mControllerNumber);
    in->ReadString(&mName);
    in->ReadInt32(&mVendorId);
    in->ReadInt32(&mProductId);
    in->ReadString(&mDescriptor);
    in->ReadBoolean(&mIsExternal);
    in->ReadInt32(&mSources);
    in->ReadInt32(&mKeyboardType);

    CKeyCharacterMap::New((IKeyCharacterMap**)&mKeyCharacterMap);
    IParcelable::Probe(mKeyCharacterMap)->ReadFromParcel(in);

    in->ReadBoolean(&mHasVibrator);
    in->ReadBoolean(&mHasButtonUnderPad);
    CInputDeviceIdentifier::New(mDescriptor, mVendorId, mProductId, (IInputDeviceIdentifier**)&mIdentifier);

    Int32 size;
    in->ReadInt32(&size);
    for (Int32 i = 0; i < size; ++i) {
        Int32 axis, source;
        Float min, max, flat, fuzz, resolution;
        in->ReadInt32(&axis);
        in->ReadInt32(&source);
        in->ReadFloat(&min);
        in->ReadFloat(&max);
        in->ReadFloat(&flat);
        in->ReadFloat(&fuzz);
        in->ReadFloat(&resolution);
        AddMotionRange(axis, source, min, max, flat, fuzz, resolution);
    }

    return NOERROR;
}

ECode InputDevice::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(mId);
    out->WriteInt32(mGeneration);
    out->WriteInt32(mControllerNumber);
    out->WriteString(mName);
    out->WriteInt32(mVendorId);
    out->WriteInt32(mProductId);
    out->WriteString(mDescriptor);
    out->WriteBoolean(mIsExternal);
    out->WriteInt32(mSources);
    out->WriteInt32(mKeyboardType);
    IParcelable::Probe(mKeyCharacterMap)->WriteToParcel(out);
    out->WriteBoolean(mHasVibrator);
    out->WriteBoolean(mHasButtonUnderPad);

    Int32 size;
    mMotionRanges->GetSize(&size);
    out->WriteInt32(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> tmp;
        mMotionRanges->Get(i, (IInterface**)&tmp);
        AutoPtr<IMotionRange> rangeItf = IMotionRange::Probe(tmp);
        MotionRange* range = (MotionRange*)(rangeItf.Get());
        assert(range != NULL);
        out->WriteInt32(range->mAxis);
        out->WriteInt32(range->mSource);
        out->WriteFloat(range->mMin);
        out->WriteFloat(range->mMax);
        out->WriteFloat(range->mFlat);
        out->WriteFloat(range->mFuzz);
        out->WriteFloat(range->mResolution);
    }

    return NOERROR;
}


ECode InputDevice::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder description;
    ((((description += "Input Device ") += mId) += ": ") += mName) += "\n";
    ((description += "  Descriptor: ") += mDescriptor) += "\n";
    ((description += "  Generation: ") += "  Generation: ") += "\n";
    ((description += "  Generation: ") += mGeneration) += "\n";
    ((description += "  Location: ") += mIsExternal ? "external" : "built-in") += "\n";
    description += "  Keyboard Type: ";
    switch (mKeyboardType) {
        case IInputDevice::KEYBOARD_TYPE_NONE:
            description += "none";
            break;
        case IInputDevice::KEYBOARD_TYPE_NON_ALPHABETIC:
            description += "non-alphabetic";
            break;
        case IInputDevice::KEYBOARD_TYPE_ALPHABETIC:
            description += "alphabetic";
            break;
    }
    description += "\n";
    ((description += "  Has Vibrator: ") += "  Has Vibrator: ") += "\n";
    ((description += "  Sources: 0x") += StringUtils::ToString(mSources, 16)) += " (";
    AppendSourceDescriptionIfApplicable(description, IInputDevice::SOURCE_KEYBOARD, String("keyboard"));
    AppendSourceDescriptionIfApplicable(description, IInputDevice::SOURCE_DPAD, String("dpad"));
    AppendSourceDescriptionIfApplicable(description, IInputDevice::SOURCE_TOUCHSCREEN, String("touchscreen"));
    AppendSourceDescriptionIfApplicable(description, IInputDevice::SOURCE_MOUSE, String("mouse"));
    AppendSourceDescriptionIfApplicable(description, IInputDevice::SOURCE_STYLUS, String("stylus"));
    AppendSourceDescriptionIfApplicable(description, IInputDevice::SOURCE_TRACKBALL, String("trackball"));
    AppendSourceDescriptionIfApplicable(description, IInputDevice::SOURCE_TOUCHPAD, String("touchpad"));
    AppendSourceDescriptionIfApplicable(description, IInputDevice::SOURCE_JOYSTICK, String("joystick"));
    AppendSourceDescriptionIfApplicable(description, IInputDevice::SOURCE_GAMEPAD, String("gamepad"));
    AppendSourceDescriptionIfApplicable(description, IInputDevice::SOURCE_GESTURE_SENSOR, String("gesture"));
    description += " )\n";
    Int32 size;
    mMotionRanges->GetSize(&size);

    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> tmp;
        mMotionRanges->Get(i, (IInterface**)&tmp);
        AutoPtr<IMotionRange> rangeItf = IMotionRange::Probe(tmp);
        MotionRange* range = (MotionRange*)(rangeItf.Get());
        (description += "    ") += MotionEvent::AxisToString(range->mAxis);
        (description += ": source=0x") += StringUtils::ToString(range->mSource, 16);
        (description += " min=") += range->mMin;
        (description += " max=") += range->mMax;
        (description += " flat=") += range->mFlat;
        (description += " fuzz=") += range->mFuzz;
        (description += " resolution=") += range->mResolution;
        description += "\n";
    }
    *str = description.ToString();
    return NOERROR;
}

ECode InputDevice::AppendSourceDescriptionIfApplicable(
    /* [in] */ StringBuilder& description,
    /* [in] */ Int32 source,
    /* [in] */ const String& sourceName)
{
    if ((mSources & source) == source) {
        description += " ";
        description += sourceName;
    }
    return NOERROR;
}


}   //namespace View
}   //namespace Droid
}   //namespace Elastos
