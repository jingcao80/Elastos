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
#include "elastos/droid/view/MotionEvent.h"
#include "elastos/droid/view/CMotionEvent.h"
#include "elastos/droid/view/InputDevice.h"
#include "elastos/droid/view/KeyEvent.h"
#include "elastos/droid/view/CPointerCoords.h"
#include "elastos/droid/view/CPointerProperties.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <input/Input.h>
#include <skia/core/SkMatrix.h>
#include <utils/BitSet.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

static ECode ValidatePointerCount(
    /* [in] */ Int32 pointerCount)
{
    if (pointerCount < 1) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //         "pointerCount must be at least 1");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

static ECode ValidatePointerPropertiesArray(
    /* [in] */ ArrayOf<IPointerProperties*>* pointerPropertiesObjArray,
    /* [in] */ Int32 pointerCount)
{
    if (!pointerPropertiesObjArray) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //         "pointerProperties array must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (pointerPropertiesObjArray->GetLength() < pointerCount) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //         "pointerProperties array must be large enough to hold all pointers");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

static ECode ValidatePointerCoordsObjArray(
    /* [in] */ ArrayOf<IPointerCoords*>* pointerCoordsObjArray,
    /* [in] */ Int32 pointerCount)
{
    if (!pointerCoordsObjArray) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //         "pointerCoords array must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (pointerCoordsObjArray->GetLength() < pointerCount) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //         "pointerCoords array must be large enough to hold all pointers");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

static ECode ValidatePointerIndex(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ size_t pointerCount)
{
    if (pointerIndex < 0 || size_t(pointerIndex) >= pointerCount) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //         "pointerIndex out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

static ECode ValidateHistoryPos(
    /* [in] */ Int32 historyPos,
    /* [in] */ size_t historySize)
{
    if (historyPos < 0 || size_t(historyPos) >= historySize) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //         "historyPos out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

static ECode ValidatePointerCoords(
    /* [in] */ IPointerCoords* pointerCoordsObj)
{
    if (!pointerCoordsObj) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //         "pointerCoords must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

static ECode ValidatePointerProperties(
    /* [in] */ IPointerProperties* pointerPropertiesObj)
{
    if (!pointerPropertiesObj) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //         "pointerProperties must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

static void PointerCoordsToNative(
    /* [in] */ IPointerCoords* coords,
    /* [in] */ Float xOffset,
    /* [in] */ Float yOffset,
    /* [in] */ android::PointerCoords* outRawPointerCoords)
{
    MotionEvent::PointerCoords* pointerCoordsObj = (MotionEvent::PointerCoords*)coords;
    outRawPointerCoords->clear();
    outRawPointerCoords->setAxisValue(AMOTION_EVENT_AXIS_X, pointerCoordsObj->mX - xOffset);
    outRawPointerCoords->setAxisValue(AMOTION_EVENT_AXIS_Y, pointerCoordsObj->mY - yOffset);
    outRawPointerCoords->setAxisValue(AMOTION_EVENT_AXIS_PRESSURE, pointerCoordsObj->mPressure);
    outRawPointerCoords->setAxisValue(AMOTION_EVENT_AXIS_SIZE, pointerCoordsObj->mSize);
    outRawPointerCoords->setAxisValue(AMOTION_EVENT_AXIS_TOUCH_MAJOR, pointerCoordsObj->mTouchMajor);
    outRawPointerCoords->setAxisValue(AMOTION_EVENT_AXIS_TOUCH_MINOR, pointerCoordsObj->mTouchMinor);
    outRawPointerCoords->setAxisValue(AMOTION_EVENT_AXIS_TOOL_MAJOR, pointerCoordsObj->mToolMajor);
    outRawPointerCoords->setAxisValue(AMOTION_EVENT_AXIS_TOOL_MINOR, pointerCoordsObj->mToolMinor);
    outRawPointerCoords->setAxisValue(AMOTION_EVENT_AXIS_ORIENTATION, pointerCoordsObj->mOrientation);

    android::BitSet64 bits = android::BitSet64(pointerCoordsObj->mPackedAxisBits);
    if (!bits.isEmpty()) {
        ArrayOf<Float>* valuesArray = pointerCoordsObj->mPackedAxisValues;
        if (valuesArray) {
            Float* values = valuesArray->GetPayload();

            uint32_t index = 0;
            do {
                uint32_t axis = bits.clearFirstMarkedBit();
                outRawPointerCoords->setAxisValue(axis, values[index++]);
            } while (!bits.isEmpty());
        }
    }
}

static AutoPtr<ArrayOf<Float> > ObtainPackedAxisValuesArray(
    /* [in] */ uint32_t minSize,
    /* [in, out] */ IPointerCoords* coords)
{
    MotionEvent::PointerCoords* outPointerCoordsObj = (MotionEvent::PointerCoords*)coords;
    AutoPtr<ArrayOf<Float> > outValuesArray = outPointerCoordsObj->mPackedAxisValues;
    if (outValuesArray) {
        uint32_t size = outValuesArray->GetLength();
        if (minSize <= size) {
            return outValuesArray;
        }
    }
    uint32_t size = 8;
    while (size < minSize) {
        size *= 2;
    }
    outValuesArray = ArrayOf<Float>::Alloc(size);
    outPointerCoordsObj->mPackedAxisValues = outValuesArray;
    return outValuesArray;
}

static void PointerCoordsFromNative(
    /* [in] */ const android::PointerCoords* rawPointerCoords,
    /* [in] */ Float xOffset,
    /* [in] */ Float yOffset,
    /* [in] */ IPointerCoords* coords)
{
    coords->SetX(rawPointerCoords->getAxisValue(AMOTION_EVENT_AXIS_X) + xOffset);
    coords->SetY(rawPointerCoords->getAxisValue(AMOTION_EVENT_AXIS_Y) + yOffset);
    coords->SetPressure(rawPointerCoords->getAxisValue(AMOTION_EVENT_AXIS_PRESSURE));
    coords->SetSize(rawPointerCoords->getAxisValue(AMOTION_EVENT_AXIS_SIZE));
    coords->SetTouchMajor(rawPointerCoords->getAxisValue(AMOTION_EVENT_AXIS_TOUCH_MAJOR));
    coords->SetTouchMinor(rawPointerCoords->getAxisValue(AMOTION_EVENT_AXIS_TOUCH_MINOR));
    coords->SetToolMajor(rawPointerCoords->getAxisValue(AMOTION_EVENT_AXIS_TOOL_MAJOR));
    coords->SetToolMinor(rawPointerCoords->getAxisValue(AMOTION_EVENT_AXIS_TOOL_MINOR));
    coords->SetOrientation(rawPointerCoords->getAxisValue(AMOTION_EVENT_AXIS_ORIENTATION));

    uint64_t outBits = 0;
    android::BitSet64 bits = android::BitSet64(rawPointerCoords->bits);
    bits.clearBit(AMOTION_EVENT_AXIS_X);
    bits.clearBit(AMOTION_EVENT_AXIS_Y);
    bits.clearBit(AMOTION_EVENT_AXIS_PRESSURE);
    bits.clearBit(AMOTION_EVENT_AXIS_SIZE);
    bits.clearBit(AMOTION_EVENT_AXIS_TOUCH_MAJOR);
    bits.clearBit(AMOTION_EVENT_AXIS_TOUCH_MINOR);
    bits.clearBit(AMOTION_EVENT_AXIS_TOOL_MAJOR);
    bits.clearBit(AMOTION_EVENT_AXIS_TOOL_MINOR);
    bits.clearBit(AMOTION_EVENT_AXIS_ORIENTATION);
    if (!bits.isEmpty()) {
        uint32_t packedAxesCount = bits.count();
        AutoPtr<ArrayOf<Float> > outValuesArray = ObtainPackedAxisValuesArray(packedAxesCount, coords);
        if (!outValuesArray) {
            return; // OOM
        }

        Float* outValues = outValuesArray->GetPayload();

        // const float* values = rawPointerCoords->values;
        uint32_t index = 0;
        do {
            uint32_t axis = bits.clearFirstMarkedBit();
            outBits |= android::BitSet64::valueForBit(axis);
            outValues[index++] = rawPointerCoords->getAxisValue(axis);
        } while (!bits.isEmpty());
    }

    MotionEvent::PointerCoords* outPointerCoordsObj = (MotionEvent::PointerCoords*)coords;
    outPointerCoordsObj->mPackedAxisBits = outBits;
}

static void PointerPropertiesToNative(
    /* [in] */ IPointerProperties* pointerPropertiesObj,
    /* [in] */ android::PointerProperties* outPointerProperties)
{
    outPointerProperties->clear();
    pointerPropertiesObj->GetId(&outPointerProperties->id);
    pointerPropertiesObj->GetToolType(&outPointerProperties->toolType);
}

static void PointerPropertiesFromNative(
    /* [in] */ const android::PointerProperties* pointerProperties,
    /* [in] */ IPointerProperties* outPointerPropertiesObj)
{
    outPointerPropertiesObj->SetId(pointerProperties->id);
    outPointerPropertiesObj->SetToolType(pointerProperties->toolType);
}

const Int64 MotionEvent::NS_PER_MS = 1000000;
const String MotionEvent::LABEL_PREFIX("AXIS_");

static AutoPtr<ISparseArray> InitAXIS_SYMBOLIC_NAMES()
{
    AutoPtr<CSparseArray> array;
    CSparseArray::NewByFriend((CSparseArray**)&array);
    AutoPtr<ISparseArray> names = (ISparseArray*)array.Get();

    names->Append(IMotionEvent::AXIS_X, CoreUtils::Convert("AXIS_X"));
    names->Append(IMotionEvent::AXIS_Y, CoreUtils::Convert("AXIS_Y"));
    names->Append(IMotionEvent::AXIS_PRESSURE, CoreUtils::Convert("AXIS_PRESSURE"));
    names->Append(IMotionEvent::AXIS_SIZE, CoreUtils::Convert("AXIS_SIZE"));
    names->Append(IMotionEvent::AXIS_TOUCH_MAJOR, CoreUtils::Convert("AXIS_TOUCH_MAJOR"));
    names->Append(IMotionEvent::AXIS_TOUCH_MINOR, CoreUtils::Convert("AXIS_TOUCH_MINOR"));
    names->Append(IMotionEvent::AXIS_TOOL_MAJOR, CoreUtils::Convert("AXIS_TOOL_MAJOR"));
    names->Append(IMotionEvent::AXIS_TOOL_MINOR, CoreUtils::Convert("AXIS_TOOL_MINOR"));
    names->Append(IMotionEvent::AXIS_ORIENTATION, CoreUtils::Convert("AXIS_ORIENTATION"));
    names->Append(IMotionEvent::AXIS_VSCROLL, CoreUtils::Convert("AXIS_VSCROLL"));
    names->Append(IMotionEvent::AXIS_HSCROLL, CoreUtils::Convert("AXIS_HSCROLL"));
    names->Append(IMotionEvent::AXIS_Z, CoreUtils::Convert("AXIS_Z"));
    names->Append(IMotionEvent::AXIS_RX, CoreUtils::Convert("AXIS_RX"));
    names->Append(IMotionEvent::AXIS_RY, CoreUtils::Convert("AXIS_RY"));
    names->Append(IMotionEvent::AXIS_RZ, CoreUtils::Convert("AXIS_RZ"));
    names->Append(IMotionEvent::AXIS_HAT_X, CoreUtils::Convert("AXIS_HAT_X"));
    names->Append(IMotionEvent::AXIS_HAT_Y, CoreUtils::Convert("AXIS_HAT_Y"));
    names->Append(IMotionEvent::AXIS_LTRIGGER, CoreUtils::Convert("AXIS_LTRIGGER"));
    names->Append(IMotionEvent::AXIS_RTRIGGER, CoreUtils::Convert("AXIS_RTRIGGER"));
    names->Append(IMotionEvent::AXIS_THROTTLE, CoreUtils::Convert("AXIS_THROTTLE"));
    names->Append(IMotionEvent::AXIS_RUDDER, CoreUtils::Convert("AXIS_RUDDER"));
    names->Append(IMotionEvent::AXIS_WHEEL, CoreUtils::Convert("AXIS_WHEEL"));
    names->Append(IMotionEvent::AXIS_GAS, CoreUtils::Convert("AXIS_GAS"));
    names->Append(IMotionEvent::AXIS_BRAKE, CoreUtils::Convert("AXIS_BRAKE"));
    names->Append(IMotionEvent::AXIS_DISTANCE, CoreUtils::Convert("AXIS_DISTANCE"));
    names->Append(IMotionEvent::AXIS_TILT, CoreUtils::Convert("AXIS_TILT"));
    names->Append(IMotionEvent::AXIS_GENERIC_1, CoreUtils::Convert("AXIS_GENERIC_1"));
    names->Append(IMotionEvent::AXIS_GENERIC_2, CoreUtils::Convert("AXIS_GENERIC_2"));
    names->Append(IMotionEvent::AXIS_GENERIC_3, CoreUtils::Convert("AXIS_GENERIC_3"));
    names->Append(IMotionEvent::AXIS_GENERIC_4, CoreUtils::Convert("AXIS_GENERIC_4"));
    names->Append(IMotionEvent::AXIS_GENERIC_5, CoreUtils::Convert("AXIS_GENERIC_5"));
    names->Append(IMotionEvent::AXIS_GENERIC_6, CoreUtils::Convert("AXIS_GENERIC_6"));
    names->Append(IMotionEvent::AXIS_GENERIC_7, CoreUtils::Convert("AXIS_GENERIC_7"));
    names->Append(IMotionEvent::AXIS_GENERIC_8, CoreUtils::Convert("AXIS_GENERIC_8"));
    names->Append(IMotionEvent::AXIS_GENERIC_9, CoreUtils::Convert("AXIS_GENERIC_9"));
    names->Append(IMotionEvent::AXIS_GENERIC_10, CoreUtils::Convert("AXIS_GENERIC_10"));
    names->Append(IMotionEvent::AXIS_GENERIC_11, CoreUtils::Convert("AXIS_GENERIC_11"));
    names->Append(IMotionEvent::AXIS_GENERIC_12, CoreUtils::Convert("AXIS_GENERIC_12"));
    names->Append(IMotionEvent::AXIS_GENERIC_13, CoreUtils::Convert("AXIS_GENERIC_13"));
    names->Append(IMotionEvent::AXIS_GENERIC_14, CoreUtils::Convert("AXIS_GENERIC_14"));
    names->Append(IMotionEvent::AXIS_GENERIC_15, CoreUtils::Convert("AXIS_GENERIC_15"));
    names->Append(IMotionEvent::AXIS_GENERIC_16, CoreUtils::Convert("AXIS_GENERIC_16"));

    return names;
}
const AutoPtr<ISparseArray> MotionEvent::AXIS_SYMBOLIC_NAMES = InitAXIS_SYMBOLIC_NAMES();

const String MotionEvent::BUTTON_SYMBOLIC_NAMES[] = {
    String("BUTTON_PRIMARY"),
    String("BUTTON_SECONDARY"),
    String("BUTTON_TERTIARY"),
    String("BUTTON_BACK"),
    String("BUTTON_FORWARD"),
    String("0x00000020"),
    String("0x00000040"),
    String("0x00000080"),
    String("0x00000100"),
    String("0x00000200"),
    String("0x00000400"),
    String("0x00000800"),
    String("0x00001000"),
    String("0x00002000"),
    String("0x00004000"),
    String("0x00008000"),
    String("0x00010000"),
    String("0x00020000"),
    String("0x00040000"),
    String("0x00080000"),
    String("0x00100000"),
    String("0x00200000"),
    String("0x00400000"),
    String("0x00800000"),
    String("0x01000000"),
    String("0x02000000"),
    String("0x04000000"),
    String("0x08000000"),
    String("0x10000000"),
    String("0x20000000"),
    String("0x40000000"),
    String("0x80000000"),
};

static AutoPtr<ISparseArray> InitTOOL_TYPE_SYMBOLIC_NAMES()
{
    AutoPtr<CSparseArray> array;
    CSparseArray::NewByFriend((CSparseArray**)&array);
    AutoPtr<ISparseArray> names = (ISparseArray*)array.Get();

    names->Append(IMotionEvent::TOOL_TYPE_UNKNOWN, CoreUtils::Convert("TOOL_TYPE_UNKNOWN"));
    names->Append(IMotionEvent::TOOL_TYPE_FINGER, CoreUtils::Convert("TOOL_TYPE_FINGER"));
    names->Append(IMotionEvent::TOOL_TYPE_STYLUS, CoreUtils::Convert("TOOL_TYPE_STYLUS"));
    names->Append(IMotionEvent::TOOL_TYPE_MOUSE, CoreUtils::Convert("TOOL_TYPE_MOUSE"));
    names->Append(IMotionEvent::TOOL_TYPE_ERASER, CoreUtils::Convert("TOOL_TYPE_ERASER"));

    return names;
}
const AutoPtr<ISparseArray> MotionEvent::TOOL_TYPE_SYMBOLIC_NAMES = InitTOOL_TYPE_SYMBOLIC_NAMES();

const Int32 MotionEvent::HISTORY_CURRENT = -0x80000000;
const Int32 MotionEvent::MAX_RECYCLED = 10;

Object MotionEvent::sRecyclerLock;
Int32 MotionEvent::sRecyclerUsed = 0;
AutoPtr<MotionEvent> MotionEvent::sRecyclerTop;

Object MotionEvent::sSharedTempLock;
AutoPtr<ArrayOf<IPointerCoords*> > MotionEvent::sSharedTempPointerCoords;
AutoPtr<ArrayOf<IPointerProperties*> > MotionEvent::sSharedTempPointerProperties;
AutoPtr<ArrayOf<Int32> > MotionEvent::sSharedTempPointerIndexMap;
const Int32 MotionEvent::PointerCoords::INITIAL_PACKED_AXIS_VALUES = 8;

/*   MotionEvent::PointerCoords   */

CAR_INTERFACE_IMPL(MotionEvent::PointerCoords, Object, IPointerCoords);

MotionEvent::PointerCoords::PointerCoords()
    : mX(0.0f)
    , mY(0.0f)
    , mPressure(0.0f)
    , mSize(0.0f)
    , mTouchMajor(0.0f)
    , mTouchMinor(0.0f)
    , mToolMajor(0.0f)
    , mToolMinor(0.0f)
    , mOrientation(0.0f)
    , mPackedAxisBits(0)
{
}

ECode MotionEvent::PointerCoords::constructor()
{
    return NOERROR;
}

ECode MotionEvent::PointerCoords::constructor(
    /* [in] */ IPointerCoords* other)
{
    return CopyFrom(other);
}

AutoPtr< ArrayOf<IPointerCoords*> > MotionEvent::PointerCoords::CreateArray(
    /* [in] */ Int32 size)
{
    AutoPtr< ArrayOf<IPointerCoords*> > array = ArrayOf<IPointerCoords*>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IPointerCoords> coords;
        CPointerCoords::New((IPointerCoords**)&coords);
        array->Set(i, coords);
    }
    return array;
}

ECode MotionEvent::PointerCoords::SetX(
    /* [in] */ Float value)
{
    mX = value;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::GetX(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mX;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::SetY(
    /* [in] */ Float value)
{
    mY = value;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::GetY(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mY;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::SetPressure(
    /* [in] */ Float value)
{
    mPressure = value;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::GetPressure(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mPressure;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::SetSize(
    /* [in] */ Float value)
{
    mSize = value;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::GetSize(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mSize;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::SetTouchMajor(
    /* [in] */ Float value)
{
    mTouchMajor = value;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::GetTouchMajor(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mTouchMajor;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::SetTouchMinor(
    /* [in] */ Float value)
{
    mTouchMinor = value;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::GetTouchMinor(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mTouchMinor;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::SetToolMajor(
    /* [in] */ Float value)
{
    mToolMajor = value;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::GetToolMajor(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mToolMajor;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::SetToolMinor(
    /* [in] */ Float value)
{
    mToolMinor = value;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::GetToolMinor(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mToolMinor;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::SetOrientation(
    /* [in] */ Float value)
{
    mOrientation = value;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::GetOrientation(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mOrientation;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::Clear()
{
    mPackedAxisBits = 0;

    mX = 0;
    mY = 0;
    mPressure = 0;
    mSize = 0;
    mTouchMajor = 0;
    mTouchMinor = 0;
    mToolMajor = 0;
    mToolMinor = 0;
    mOrientation = 0;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::CopyFrom(
    /* [in] */ IPointerCoords* otherI)
{
    VALIDATE_NOT_NULL(otherI);

    AutoPtr<PointerCoords> other = (PointerCoords*)otherI;

    const Int64 bits = other->mPackedAxisBits;
    mPackedAxisBits = bits;
    if (bits != 0) {
        AutoPtr<ArrayOf<Float> > otherValues = other->mPackedAxisValues;
        const Int32 count = Elastos::Core::Math::BitCount(bits);
        AutoPtr<ArrayOf<Float> > values = mPackedAxisValues;
        if (values == NULL || count > values->GetLength()) {
            values = ArrayOf<Float>::Alloc(otherValues->GetLength());
            mPackedAxisValues = values;
        }

        values->Copy(otherValues, 0, count);
    }

    mX = other->mX;
    mY = other->mY;
    mPressure = other->mPressure;
    mSize = other->mSize;
    mTouchMajor = other->mTouchMajor;
    mTouchMinor = other->mTouchMinor;
    mToolMajor = other->mToolMajor;
    mToolMinor = other->mToolMinor;
    mOrientation = other->mOrientation;
    return NOERROR;
}

ECode MotionEvent::PointerCoords::GetAxisValue(
    /* [in] */ Int32 axis,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    switch (axis) {
        case IMotionEvent::AXIS_X:
            *value = mX;
            break;
        case IMotionEvent::AXIS_Y:
            *value = mY;
            break;
        case IMotionEvent::AXIS_PRESSURE:
            *value = mPressure;
            break;
        case IMotionEvent::AXIS_SIZE:
            *value = mSize;
            break;
        case IMotionEvent::AXIS_TOUCH_MAJOR:
            *value = mTouchMajor;
            break;
        case IMotionEvent::AXIS_TOUCH_MINOR:
            *value = mTouchMinor;
            break;
        case IMotionEvent::AXIS_TOOL_MAJOR:
            *value = mToolMajor;
            break;
        case IMotionEvent::AXIS_TOOL_MINOR:
            *value = mToolMinor;
            break;
        case IMotionEvent::AXIS_ORIENTATION:
            *value = mOrientation;
            break;

        default: {
            if (axis < 0 || axis > 63) {
                Logger::E("MotionEvent", "Axis out of range.");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            const Int64 bits = mPackedAxisBits;
            const Int64 axisBit = (UInt64)0x8000000000000000LL >> (axis & 0x3F);
            if ((bits & axisBit) == 0) {
                *value = 0;
                break;
            }
            Int64 data = (UInt64)0xFFFFFFFFFFFFFFFFLL >> (axis & 0x3F);
            const Int32 index = Elastos::Core::Math::BitCount(bits & ~data);
            *value = (*mPackedAxisValues)[index];
        }
    }

    return NOERROR;
}

ECode MotionEvent::PointerCoords::SetAxisValue(
    /* [in] */ Int32 axis,
    /* [in] */ Float value)
{
    switch (axis) {
        case IMotionEvent::AXIS_X:
            mX = value;
            break;
        case IMotionEvent::AXIS_Y:
            mY = value;
            break;
        case IMotionEvent::AXIS_PRESSURE:
            mPressure = value;
            break;
        case IMotionEvent::AXIS_SIZE:
            mSize = value;
            break;
        case IMotionEvent::AXIS_TOUCH_MAJOR:
            mTouchMajor = value;
            break;
        case IMotionEvent::AXIS_TOUCH_MINOR:
            mTouchMinor = value;
            break;
        case IMotionEvent::AXIS_TOOL_MAJOR:
            mToolMajor = value;
            break;
        case IMotionEvent::AXIS_TOOL_MINOR:
            mToolMinor = value;
            break;
        case IMotionEvent::AXIS_ORIENTATION:
            mOrientation = value;
            break;
        default: {
            if (axis < 0 || axis > 63) {
                Logger::E("MotionEvent", "Axis out of range.");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            const Int64 bits = mPackedAxisBits;
            const Int64 axisBit = (UInt64)0x8000000000000000LL >> (axis & 0x3F);
            Int64 data = (UInt64)0xFFFFFFFFFFFFFFFFLL >> (axis & 0x3F);
            const Int32 index = Elastos::Core::Math::BitCount(bits & ~data);
            AutoPtr<ArrayOf<Float> > values = mPackedAxisValues;
            if ((bits & axisBit) == 0) {
                if (values == NULL) {
                    values = ArrayOf<Float>::Alloc(INITIAL_PACKED_AXIS_VALUES);
                    mPackedAxisValues = values;
                }
                else {
                    const Int32 count = Elastos::Core::Math::BitCount(bits);
                    if (count < values->GetLength()) {
                        if (index != count) {
                            values->Copy(index + 1, values, index, count - index);
                        }
                    }
                    else {
                        AutoPtr<ArrayOf<Float> > newValues = ArrayOf<Float>::Alloc(count * 2);
                        newValues->Copy(values, 0, index);
                        newValues->Copy(index + 1, values, index, count - index);
                        values = newValues;
                        mPackedAxisValues = values;
                    }
                }
                mPackedAxisBits = bits | axisBit;
            }
            (*values)[index] = value;
        }
    }

    return NOERROR;
}

/*   MotionEvent::PointerProperties   */

CAR_INTERFACE_IMPL(MotionEvent::PointerProperties, Object, IPointerProperties);

MotionEvent::PointerProperties::PointerProperties()
    : mId(0)
    , mToolType(0)
{
}

ECode MotionEvent::PointerProperties::constructor()
{
    Clear();
    return NOERROR;
}

ECode MotionEvent::PointerProperties::constructor(
    /* [in] */ IPointerProperties* other)
{
    VALIDATE_NOT_NULL(other);
    other->GetId(&mId);
    other->GetToolType(&mToolType);
    return NOERROR;
}

AutoPtr< ArrayOf<IPointerProperties*> > MotionEvent::PointerProperties::CreateArray(
    /* [in] */ Int32 size)
{
    AutoPtr< ArrayOf<IPointerProperties*> > array = ArrayOf<IPointerProperties*>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IPointerProperties> properties;
        CPointerProperties::New((IPointerProperties**)&properties);
        array->Set(i, properties);
    }
    return array;
}

ECode MotionEvent::PointerProperties::SetId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

ECode MotionEvent::PointerProperties::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode MotionEvent::PointerProperties::SetToolType(
    /* [in] */ Int32 type)
{
    mToolType = type;
    return NOERROR;
}

ECode MotionEvent::PointerProperties::GetToolType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mToolType;
    return NOERROR;
}

ECode MotionEvent::PointerProperties::Clear()
{
    mId = IMotionEvent::INVALID_POINTER_ID;
    mToolType = IMotionEvent::TOOL_TYPE_UNKNOWN;
    return NOERROR;
}

ECode MotionEvent::PointerProperties::CopyFrom(
    /* [in] */ IPointerProperties* other)
{
    VALIDATE_NOT_NULL(other);
    other->GetId(&mId);
    other->GetToolType(&mToolType);
    return NOERROR;
}

ECode MotionEvent::PointerProperties::Equals(
    /* [in] */ IPointerProperties* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (other == NULL) {
        return NOERROR;
    }

    Int32 id, toolType;
    other->GetId(&id);
    other->GetToolType(&toolType);
    *result = (mId == id && mToolType == toolType);
    return NOERROR;
}

ECode MotionEvent::PointerProperties::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (IPointerProperties::Probe(other)) {
        return Equals(IPointerProperties::Probe(other), result);
    }
    return NOERROR;
}

ECode MotionEvent::PointerProperties::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = (mId | (mToolType << 8));
    return NOERROR;
}

/*   MotionEvent   */

CAR_INTERFACE_IMPL(MotionEvent, InputEvent, IMotionEvent);

MotionEvent::MotionEvent()
    : mNativePtr(0)
{}

MotionEvent::~MotionEvent()
{
    if (mNativePtr != 0) {
        NativeDispose(mNativePtr);
        mNativePtr = 0;
    }
}

ECode MotionEvent::constructor()
{
    return NOERROR;
}

void MotionEvent::EnsureSharedTempPointerCapacity(
    /* [in] */ Int32 desiredCapacity)
{
    if (sSharedTempPointerCoords == NULL
            || sSharedTempPointerCoords->GetLength() < desiredCapacity) {
        Int32 capacity = sSharedTempPointerCoords != NULL ? sSharedTempPointerCoords->GetLength() : 8;
        while (capacity < desiredCapacity) {
            capacity *= 2;
        }

        sSharedTempPointerCoords = MotionEvent::PointerCoords::CreateArray(capacity);
        sSharedTempPointerProperties = MotionEvent::PointerProperties::CreateArray(capacity);
        sSharedTempPointerIndexMap = ArrayOf<Int32>::Alloc(capacity);
    }
}

AutoPtr<MotionEvent> MotionEvent::Obtain()
{
    AutoPtr<MotionEvent> ev;

    {    AutoLock syncLock(sRecyclerLock);
        ev = sRecyclerTop;
        if (ev == NULL) {
            AutoPtr<IMotionEvent> event;
            CMotionEvent::New((IMotionEvent**)&event);
            return (MotionEvent*)event.Get();
        }
        sRecyclerTop = ev->mNext;
        sRecyclerUsed -= 1;
    }

    ev->mNext = NULL;
    ev->PrepareForReuse();
    return ev;
}

ECode MotionEvent::Obtain(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 pointerCount,
    /* [in] */ ArrayOf<IPointerProperties*>* pointerProperties,
    /* [in] */ ArrayOf<IPointerCoords*>* pointerCoords,
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 buttonState,
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 edgeFlags,
    /* [in] */ Int32 source,
    /* [in] */ Int32 flags,
    /* [out] */ IMotionEvent** event)
{
    VALIDATE_NOT_NULL(event);
    *event = NULL;

    AutoPtr<MotionEvent> ev = Obtain();

    Int64 ptr;
    FAIL_RETURN(NativeInitialize(ev->mNativePtr,
            deviceId, source, action, flags, edgeFlags, metaState, buttonState,
            0, 0, xPrecision, yPrecision,
            downTime * NS_PER_MS, eventTime * NS_PER_MS,
            pointerCount, pointerProperties, pointerCoords, &ptr));
    ev->mNativePtr = ptr;
    *event = (IMotionEvent*)ev.Get();
    REFCOUNT_ADD(*event);
    return NOERROR;
}

ECode MotionEvent::Obtain(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 pointerCount,
    /* [in] */ ArrayOf<Int32>* pointerIds,
    /* [in] */ ArrayOf<IPointerCoords*>* pointerCoords,
    /* [in] */ Int32 metaState,
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 edgeFlags,
    /* [in] */ Int32 source,
    /* [in] */ Int32 flags,
    /* [out] */ IMotionEvent** event)
{
    VALIDATE_NOT_NULL(event);

    {    AutoLock syncLock(sSharedTempLock);
        EnsureSharedTempPointerCapacity(pointerCount);
        ArrayOf<IPointerProperties*>* pp = sSharedTempPointerProperties;
        for (Int32 i = 0; i < pointerCount; i++) {
            AutoPtr<IPointerProperties> obj = (*pp)[i];
            obj->Clear();
            obj->SetId((*pointerIds)[i]);
        }
        return Obtain(downTime, eventTime, action, pointerCount, pp,
                pointerCoords, metaState, 0, xPrecision, yPrecision, deviceId,
                edgeFlags, source, flags, event);
    }

    return NOERROR;
}

ECode MotionEvent::Obtain(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float pressure,
    /* [in] */ Float size,
    /* [in] */ Int32 metaState,
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 edgeFlags,
    /* [out] */ IMotionEvent** event)
{
    VALIDATE_NOT_NULL(event);
    *event = NULL;

    AutoPtr<MotionEvent> ev = Obtain();

    {    AutoLock syncLock(sSharedTempLock);
        EnsureSharedTempPointerCapacity(1);

        AutoPtr< ArrayOf<IPointerProperties*> > pp = sSharedTempPointerProperties;
        (*pp)[0]->Clear();
        (*pp)[0]->SetId(0);

        AutoPtr< ArrayOf<IPointerCoords*> > pc = sSharedTempPointerCoords;
        (*pc)[0]->Clear();
        (*pc)[0]->SetX(x);
        (*pc)[0]->SetY(y);
        (*pc)[0]->SetPressure(pressure);
        (*pc)[0]->SetSize(size);

        Int64 ptr;
        FAIL_RETURN(NativeInitialize(ev->mNativePtr,
                deviceId, IInputDevice::SOURCE_UNKNOWN, action, 0, edgeFlags, metaState, 0,
                0, 0, xPrecision, yPrecision, downTime * NS_PER_MS,
                eventTime * NS_PER_MS, 1, pp, pc, &ptr));
        ev->mNativePtr = ptr;
        *event = (IMotionEvent*)ev.Get();
        REFCOUNT_ADD(*event);
    }

    return NOERROR;
}

ECode MotionEvent::Obtain(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 pointers,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float pressure,
    /* [in] */ Float size,
    /* [in] */ Int32 metaState,
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 edgeFlags,
    /* [out] */ IMotionEvent** event)
{
    VALIDATE_NOT_NULL(event);
    return Obtain(downTime, eventTime, action, x, y, pressure, size,
            metaState, xPrecision, yPrecision, deviceId, edgeFlags, event);
}

ECode MotionEvent::Obtain(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 metaState,
    /* [out] */ IMotionEvent** event)
{
    VALIDATE_NOT_NULL(event);
    return Obtain(downTime, eventTime, action, x, y, 1.0f, 1.0f,
            metaState, 1.0f, 1.0f, 0, 0, event);
}

ECode MotionEvent::Obtain(
    /* [in] */ IMotionEvent* other,
    /* [out] */ IMotionEvent** event)
{
    VALIDATE_NOT_NULL(event);
    *event = NULL;
    if (other == NULL) {
        Logger::E("MotionEvent", "other motion event must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<MotionEvent> ev = Obtain();
    AutoPtr<MotionEvent> othObject = (MotionEvent*)other;

    ev->mNativePtr = NativeCopy(ev->mNativePtr, othObject->mNativePtr, TRUE /*keepHistory*/);
    *event = (IMotionEvent*)ev.Get();
    REFCOUNT_ADD(*event);
    return NOERROR;
}

ECode MotionEvent::ObtainNoHistory(
    /* [in] */ IMotionEvent* other,
    /* [out] */ IMotionEvent** event)
{
    VALIDATE_NOT_NULL(event);
    *event = NULL;
    if (other == NULL) {
        Logger::E("MotionEvent", "other motion event must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<MotionEvent> ev = Obtain();
    AutoPtr<MotionEvent> othObject = (MotionEvent*)other;

    ev->mNativePtr = NativeCopy(ev->mNativePtr, othObject->mNativePtr, FALSE /*keepHistory*/);
    *event = (IMotionEvent*)ev.Get();
    REFCOUNT_ADD(*event);
    return NOERROR;
}

ECode MotionEvent::Copy(
    /* [out] */ IInputEvent** event)
{
    VALIDATE_NOT_NULL(event);
    AutoPtr<IMotionEvent> motionEvent;
    FAIL_RETURN(Obtain(this, (IMotionEvent**)&motionEvent));
    *event = IInputEvent::Probe(motionEvent);
    REFCOUNT_ADD(*event);

    return NOERROR;
}

ECode MotionEvent::Recycle()
{
    InputEvent::Recycle();

    {    AutoLock syncLock(sRecyclerLock);
        if (sRecyclerUsed < MAX_RECYCLED) {
            sRecyclerUsed++;
            mNext = sRecyclerTop;
            sRecyclerTop = this;
        }
    }

    return NOERROR;
}

ECode MotionEvent::Scale(
    /* [in] */ Float scale)
{
    if (scale != 1.0f) {
        NativeScale(mNativePtr, scale);
    }

    return NOERROR;
}

ECode MotionEvent::GetDeviceId(
    /* [out] */ Int32* deviceId)
{
    VALIDATE_NOT_NULL(deviceId);

    *deviceId = NativeGetDeviceId(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::GetSource(
    /* [out] */ Int32* source)
{
    VALIDATE_NOT_NULL(source);

    *source = NativeGetSource(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::SetSource(
    /* [in] */ Int32 source)
{
    NativeSetSource(mNativePtr, source);

    return NOERROR;
}

ECode MotionEvent::GetAction(
    /* [out] */ Int32* action)
{
    VALIDATE_NOT_NULL(action);

    *action = NativeGetAction(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::GetActionMasked(
    /* [out] */ Int32* actionMasked)
{
    VALIDATE_NOT_NULL(actionMasked);

    *actionMasked = NativeGetAction(mNativePtr) & ACTION_MASK;

    return NOERROR;
}

ECode MotionEvent::GetActionIndex(
    /* [out] */ Int32* actionIndex)
{
    VALIDATE_NOT_NULL(actionIndex);

    *actionIndex = (NativeGetAction(mNativePtr) & ACTION_POINTER_INDEX_MASK)
            >> ACTION_POINTER_INDEX_SHIFT;

    return NOERROR;
}

ECode MotionEvent::IsTouchEvent(
    /* [out] */ Boolean* isTouchEvent)
{
    VALIDATE_NOT_NULL(isTouchEvent);

    *isTouchEvent = NativeIsTouchEvent(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);

    *flags = NativeGetFlags(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::IsTainted(
    /* [out] */ Boolean* isTainted)
{
    VALIDATE_NOT_NULL(isTainted);
    Int32 flags;
    GetFlags(&flags);
    *isTainted = (flags & FLAG_TAINTED) != 0;

    return NOERROR;
}

ECode MotionEvent::SetTainted(
    /* [in] */ Boolean tainted)
{
    Int32 flags;
    GetFlags(&flags);
    NativeSetFlags(mNativePtr, tainted ? flags | FLAG_TAINTED : flags & ~FLAG_TAINTED);

    return NOERROR;
}

ECode MotionEvent::GetDownTime(
    /* [out] */ Int64* downTime)
{
    VALIDATE_NOT_NULL(downTime);

    *downTime = NativeGetDownTimeNanos(mNativePtr) / NS_PER_MS;

    return NOERROR;
}

ECode MotionEvent::SetDownTime(
    /* [in] */ Int64 downTime)
{
    NativeSetDownTimeNanos(mNativePtr, downTime * NS_PER_MS);

    return NOERROR;
}

ECode MotionEvent::GetEventTime(
    /* [out] */ Int64* eventTime)
{
    VALIDATE_NOT_NULL(eventTime);
    *eventTime = 0;

    Int64 id;
    FAIL_RETURN(NativeGetEventTimeNanos(mNativePtr, HISTORY_CURRENT, &id));

    *eventTime = id / NS_PER_MS;
    return NOERROR;
}

ECode MotionEvent::GetEventTimeNano(
    /* [out] */ Int64* eventTimeNano)
{
    VALIDATE_NOT_NULL(eventTimeNano);

    return NativeGetEventTimeNanos(mNativePtr, HISTORY_CURRENT, eventTimeNano);
}

ECode MotionEvent::GetX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);

    return NativeGetAxisValue(mNativePtr, AXIS_X, 0, HISTORY_CURRENT, x);
}

ECode MotionEvent::GetY(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);

    return NativeGetAxisValue(mNativePtr, AXIS_Y, 0, HISTORY_CURRENT, y);
}

ECode MotionEvent::GetPressure(
    /* [out] */ Float* pressure)
{
    VALIDATE_NOT_NULL(pressure);

    return NativeGetAxisValue(mNativePtr, AXIS_PRESSURE, 0, HISTORY_CURRENT, pressure);
}

ECode MotionEvent::GetSize(
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size);

    return NativeGetAxisValue(mNativePtr, AXIS_SIZE, 0, HISTORY_CURRENT, size);
}

ECode MotionEvent::GetTouchMajor(
    /* [out] */ Float* touchMajor)
{
    VALIDATE_NOT_NULL(touchMajor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOUCH_MAJOR, 0, HISTORY_CURRENT, touchMajor);
}

ECode MotionEvent::GetTouchMinor(
    /* [out] */ Float* touchMinor)
{
    VALIDATE_NOT_NULL(touchMinor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOUCH_MINOR, 0, HISTORY_CURRENT, touchMinor);
}

ECode MotionEvent::GetToolMajor(
    /* [out] */ Float* toolMajor)
{
    VALIDATE_NOT_NULL(toolMajor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOOL_MAJOR, 0, HISTORY_CURRENT, toolMajor);
}

ECode MotionEvent::GetToolMinor(
    /* [out] */ Float* toolMinor)
{
    VALIDATE_NOT_NULL(toolMinor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOOL_MINOR, 0, HISTORY_CURRENT, toolMinor);
}

ECode MotionEvent::GetOrientation(
    /* [out] */ Float* orientation)
{
    VALIDATE_NOT_NULL(orientation);

    return NativeGetAxisValue(mNativePtr, AXIS_ORIENTATION, 0, HISTORY_CURRENT, orientation);
}

ECode MotionEvent::GetAxisValue(
    /* [in] */ Int32 axis,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    return NativeGetAxisValue(mNativePtr, axis, 0, HISTORY_CURRENT, value);
}

ECode MotionEvent::GetPointerCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    *count = NativeGetPointerCount(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::GetPointerId(
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Int32* pointerId)
{
    VALIDATE_NOT_NULL(pointerId);

    return NativeGetPointerId(mNativePtr, pointerIndex, pointerId);
}

ECode MotionEvent::GetToolType(
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Int32* toolType)
{
    VALIDATE_NOT_NULL(toolType);
    return NativeGetToolType(mNativePtr, pointerIndex, toolType);
}

ECode MotionEvent::FindPointerIndex(
    /* [in] */ Int32 pointerId,
    /* [out] */ Int32* pointerIndex)
{
    VALIDATE_NOT_NULL(pointerIndex);

    *pointerIndex = NativeFindPointerIndex(mNativePtr, pointerId);

    return NOERROR;
}

ECode MotionEvent::GetX(
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);

    return NativeGetAxisValue(mNativePtr, AXIS_X, pointerIndex, HISTORY_CURRENT, x);
}

ECode MotionEvent::GetY(
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);

    return NativeGetAxisValue(mNativePtr, AXIS_Y, pointerIndex, HISTORY_CURRENT, y);
}

ECode MotionEvent::GetPressure(
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* pressure)
{
    VALIDATE_NOT_NULL(pressure);

    return NativeGetAxisValue(mNativePtr, AXIS_PRESSURE, pointerIndex, HISTORY_CURRENT, pressure);
}

ECode MotionEvent::GetSize(
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size);

    return NativeGetAxisValue(mNativePtr, AXIS_SIZE, pointerIndex, HISTORY_CURRENT, size);
}

ECode MotionEvent::GetTouchMajor(
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* touchMajor)
{
    VALIDATE_NOT_NULL(touchMajor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOUCH_MAJOR, pointerIndex, HISTORY_CURRENT, touchMajor);
}

ECode MotionEvent::GetTouchMinor(
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* touchMinor)
{
    VALIDATE_NOT_NULL(touchMinor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOUCH_MINOR, pointerIndex, HISTORY_CURRENT, touchMinor);
}

ECode MotionEvent::GetToolMajor(
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* toolMajor)
{
    VALIDATE_NOT_NULL(toolMajor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOOL_MAJOR, pointerIndex, HISTORY_CURRENT, toolMajor);
}

ECode MotionEvent::GetToolMinor(
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* toolMinor)
{
    VALIDATE_NOT_NULL(toolMinor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOOL_MINOR, pointerIndex, HISTORY_CURRENT, toolMinor);
}

ECode MotionEvent::GetOrientation(
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* orientation)
{
    VALIDATE_NOT_NULL(orientation);

    return NativeGetAxisValue(mNativePtr, AXIS_ORIENTATION, pointerIndex, HISTORY_CURRENT, orientation);
}

ECode MotionEvent::GetAxisValue(
    /* [in] */ Int32 axis,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    return NativeGetAxisValue(mNativePtr, axis, pointerIndex, HISTORY_CURRENT, value);
}

ECode MotionEvent::GetPointerCoords(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ IPointerCoords* outPointerCoords)
{
    return NativeGetPointerCoords(mNativePtr, pointerIndex, HISTORY_CURRENT, outPointerCoords);
}

ECode MotionEvent::GetPointerProperties(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ IPointerProperties* outPointerProperties)
{
    return NativeGetPointerProperties(mNativePtr, pointerIndex, outPointerProperties);
}

ECode MotionEvent::GetMetaState(
    /* [out] */ Int32* metaState)
{
    VALIDATE_NOT_NULL(metaState);

    *metaState = NativeGetMetaState(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::GetButtonState(
    /* [out] */ Int32* buttonState)
{
    VALIDATE_NOT_NULL(buttonState);

    *buttonState = NativeGetButtonState(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::GetRawX(
    /* [out] */ Float* rawX)
{
    VALIDATE_NOT_NULL(rawX);

    return NativeGetRawAxisValue(mNativePtr, AXIS_X, 0, HISTORY_CURRENT, rawX);
}

ECode MotionEvent::GetRawY(
    /* [out] */ Float* rawY)
{
    VALIDATE_NOT_NULL(rawY);

    return NativeGetRawAxisValue(mNativePtr, AXIS_Y, 0, HISTORY_CURRENT, rawY);
}

ECode MotionEvent::GetXPrecision(
    /* [out] */ Float* xPrecision)
{
    VALIDATE_NOT_NULL(xPrecision);

    *xPrecision = NativeGetXPrecision(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::GetYPrecision(
    /* [out] */ Float* yPrecision)
{
    VALIDATE_NOT_NULL(yPrecision);

    *yPrecision = NativeGetYPrecision(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::GetHistorySize(
    /* [out] */ Int32* historySize)
{
    VALIDATE_NOT_NULL(historySize);

    *historySize = NativeGetHistorySize(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::GetHistoricalEventTime(
    /* [in] */ Int32 pos,
    /* [out] */ Int64* hEventTime)
{
    VALIDATE_NOT_NULL(hEventTime);
    *hEventTime = 0;

    Int64 id;
    FAIL_RETURN(NativeGetEventTimeNanos(mNativePtr, pos, &id));
    *hEventTime = id / NS_PER_MS;

    return NOERROR;
}

ECode MotionEvent::GetHistoricalEventTimeNano(
    /* [in] */ Int32 pos,
    /* [out] */ Int64* eventTimeNano)
{
    VALIDATE_NOT_NULL(eventTimeNano);

    return NativeGetEventTimeNanos(mNativePtr, pos, eventTimeNano);
}

ECode MotionEvent::GetHistoricalX(
    /* [in] */ Int32 pos,
    /* [out] */ Float* hX)
{
    VALIDATE_NOT_NULL(hX);

    return NativeGetAxisValue(mNativePtr, AXIS_X, 0, pos, hX);
}

ECode MotionEvent::GetHistoricalY(
    /* [in] */ Int32 pos,
    /* [out] */ Float* hY)
{
    VALIDATE_NOT_NULL(hY);

    return NativeGetAxisValue(mNativePtr, AXIS_Y, 0, pos, hY);
}

ECode MotionEvent::GetHistoricalPressure(
    /* [in] */ Int32 pos,
    /* [out] */ Float* hPressure)
{
    VALIDATE_NOT_NULL(hPressure);

    return NativeGetAxisValue(mNativePtr, AXIS_PRESSURE, 0, pos, hPressure);
}

ECode MotionEvent::GetHistoricalSize(
    /* [in] */ Int32 pos,
    /* [out] */ Float* hSize)
{
    VALIDATE_NOT_NULL(hSize);

    return NativeGetAxisValue(mNativePtr, AXIS_SIZE, 0, pos, hSize);
}

ECode MotionEvent::GetHistoricalTouchMajor(
    /* [in] */ Int32 pos,
    /* [out] */ Float* hTouchMajor)
{
    VALIDATE_NOT_NULL(hTouchMajor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOUCH_MAJOR, 0, pos, hTouchMajor);
}

ECode MotionEvent::GetHistoricalTouchMinor(
    /* [in] */ Int32 pos,
    /* [out] */ Float* hTouchMinor)
{
    VALIDATE_NOT_NULL(hTouchMinor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOUCH_MINOR, 0, pos, hTouchMinor);
}

ECode MotionEvent::GetHistoricalToolMajor(
    /* [in] */ Int32 pos,
    /* [out] */ Float* hToolMajor)
{
    VALIDATE_NOT_NULL(hToolMajor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOOL_MAJOR, 0, pos, hToolMajor);
}

ECode MotionEvent::GetHistoricalToolMinor(
    /* [in] */ Int32 pos,
    /* [out] */ Float* hToolMinor)
{
    VALIDATE_NOT_NULL(hToolMinor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOOL_MINOR, 0, pos, hToolMinor);
}

ECode MotionEvent::GetHistoricalOrientation(
    /* [in] */ Int32 pos,
    /* [out] */ Float* hOrientation)
{
    VALIDATE_NOT_NULL(hOrientation);

    return NativeGetAxisValue(mNativePtr, AXIS_ORIENTATION, 0, pos, hOrientation);
}

ECode MotionEvent::GetHistoricalAxisValue(
    /* [in] */ Int32 axis,
    /* [in] */ Int32 pos,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    return NativeGetAxisValue(mNativePtr, axis, 0, pos, value);
}

ECode MotionEvent::GetHistoricalX(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 pos,
    /* [out] */ Float* hX)
{
    VALIDATE_NOT_NULL(hX);

    return NativeGetAxisValue(mNativePtr, AXIS_X, pointerIndex, pos, hX);
}

ECode MotionEvent::GetHistoricalY(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 pos,
    /* [out] */ Float* hY)
{
    VALIDATE_NOT_NULL(hY);

    return NativeGetAxisValue(mNativePtr, AXIS_Y, pointerIndex, pos, hY);
}

ECode MotionEvent::GetHistoricalPressure(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 pos,
    /* [out] */ Float* hPressure)
{
    VALIDATE_NOT_NULL(hPressure);

    return NativeGetAxisValue(mNativePtr, AXIS_PRESSURE, pointerIndex, pos, hPressure);
}

ECode MotionEvent::GetHistoricalSize(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 pos,
    /* [out] */ Float* hSize)
{
    VALIDATE_NOT_NULL(hSize);

    return NativeGetAxisValue(mNativePtr, AXIS_SIZE, pointerIndex, pos, hSize);
}

ECode MotionEvent::GetHistoricalTouchMajor(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 pos,
    /* [out] */ Float* hTouchMajor)
{
    VALIDATE_NOT_NULL(hTouchMajor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOUCH_MAJOR, pointerIndex, pos, hTouchMajor);
}

ECode MotionEvent::GetHistoricalTouchMinor(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 pos,
    /* [out] */ Float* hTouchMinor)
{
    VALIDATE_NOT_NULL(hTouchMinor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOUCH_MINOR, pointerIndex, pos, hTouchMinor);
}

ECode MotionEvent::GetHistoricalToolMajor(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 pos,
    /* [out] */ Float* hToolMajor)
{
    VALIDATE_NOT_NULL(hToolMajor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOOL_MAJOR, pointerIndex, pos, hToolMajor);
}

ECode MotionEvent::GetHistoricalToolMinor(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 pos,
    /* [out] */ Float* hToolMinor)
{
    VALIDATE_NOT_NULL(hToolMinor);

    return NativeGetAxisValue(mNativePtr, AXIS_TOOL_MINOR, pointerIndex, pos, hToolMinor);
}

ECode MotionEvent::GetHistoricalOrientation(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 pos,
    /* [out] */ Float* hOrientation)
{
    VALIDATE_NOT_NULL(hOrientation);

    return NativeGetAxisValue(mNativePtr, AXIS_ORIENTATION, pointerIndex, pos, hOrientation);
}

ECode MotionEvent::GetHistoricalAxisValue(
    /* [in] */ Int32 axis,
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 pos,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    return NativeGetAxisValue(mNativePtr, axis, pointerIndex, pos, value);
}

ECode MotionEvent::GetHistoricalPointerCoords(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 pos,
    /* [in] */ IPointerCoords* outPointerCoords)
{
    VALIDATE_NOT_NULL(outPointerCoords);

    return NativeGetPointerCoords(mNativePtr, pointerIndex, pos, outPointerCoords);
}

ECode MotionEvent::GetEdgeFlags(
    /* [out] */ Int32* edgeFlags)
{
    VALIDATE_NOT_NULL(edgeFlags);

    *edgeFlags = NativeGetEdgeFlags(mNativePtr);

    return NOERROR;
}

ECode MotionEvent::SetEdgeFlags(
    /* [in] */ Int32 flags)
{
    NativeSetEdgeFlags(mNativePtr, flags);

    return NOERROR;
}

ECode MotionEvent::SetAction(
    /* [in] */ Int32 action)
{
    NativeSetAction(mNativePtr, action);

    return NOERROR;
}

ECode MotionEvent::OffsetLocation(
    /* [in] */ Float deltaX,
    /* [in] */ Float deltaY)
{
    NativeOffsetLocation(mNativePtr, deltaX, deltaY);

    return NOERROR;
}

ECode MotionEvent::SetLocation(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Float oldX, oldY;
    GetX(&oldX);
    GetY(&oldY);
    NativeOffsetLocation(mNativePtr, x - oldX, y - oldY);

    return NOERROR;
}

ECode MotionEvent::Transform(
    /* [in] */ IMatrix* matrix)
{
    if (matrix == NULL) {
        Logger::E("MotionEvent", "matrix must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    NativeTransform(mNativePtr, matrix);

    return NOERROR;
}

ECode MotionEvent::AddBatch(
    /* [in] */ Int64 eventTime,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float pressure,
    /* [in] */ Float size,
    /* [in] */ Int32 metaState)
{
    {    AutoLock syncLock(sSharedTempLock);
        EnsureSharedTempPointerCapacity(1);

        AutoPtr< ArrayOf<IPointerCoords*> > pc = sSharedTempPointerCoords;
        (*pc)[0]->Clear();
        (*pc)[0]->SetX(x);
        (*pc)[0]->SetY(y);
        (*pc)[0]->SetPressure(pressure);
        (*pc)[0]->SetSize(size);

        FAIL_RETURN(NativeAddBatch(mNativePtr, eventTime * NS_PER_MS, pc, metaState));
    }

    return NOERROR;
}

ECode MotionEvent::AddBatch(
    /* [in] */ Int64 eventTime,
    /* [in] */ ArrayOf<IPointerCoords*>* pointerCoords,
    /* [in] */ Int32 metaState)
{
    return NativeAddBatch(mNativePtr, eventTime * NS_PER_MS, pointerCoords, metaState);
}

ECode MotionEvent::AddBatch(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    AutoPtr<MotionEvent> eventNative = (MotionEvent*)event;

    const Int32 action = NativeGetAction(mNativePtr);
    if (action != ACTION_MOVE && action != ACTION_HOVER_MOVE) {
        return NOERROR;
    }
    if (action != NativeGetAction(eventNative->mNativePtr)) {
        return NOERROR;
    }

    if (NativeGetDeviceId(mNativePtr) != NativeGetDeviceId(eventNative->mNativePtr)
        || NativeGetSource(mNativePtr) != NativeGetSource(eventNative->mNativePtr)
        || NativeGetFlags(mNativePtr) != NativeGetFlags(eventNative->mNativePtr)) {
        return NOERROR;
    }

    const Int32 pointerCount = NativeGetPointerCount(mNativePtr);
    if (pointerCount != NativeGetPointerCount(eventNative->mNativePtr)) {
        return NOERROR;
    }

    {    AutoLock syncLock(sSharedTempLock);
        AutoLock lock(sSharedTempLock);
        EnsureSharedTempPointerCapacity(Elastos::Core::Math::Max(pointerCount, 2));

        ArrayOf<IPointerProperties*>* pp = sSharedTempPointerProperties;
        ArrayOf<IPointerCoords*>* pc = sSharedTempPointerCoords;

        for (Int32 i = 0; i < pointerCount; i++) {
            NativeGetPointerProperties(mNativePtr, i, (*pp)[0]);
            NativeGetPointerProperties(eventNative->mNativePtr, i, (*pp)[1]);
            Boolean isEquals;
            (*pp)[0]->Equals((*pp)[1], &isEquals);
            if (!isEquals) {
                return NOERROR;
            }
        }

        const Int32 metaState = NativeGetMetaState(eventNative->mNativePtr);
        const Int32 historySize = NativeGetHistorySize(eventNative->mNativePtr);
        for (Int32 h = 0; h <= historySize; h++) {
            const Int32 historyPos = (h == historySize ? HISTORY_CURRENT : h);

            for (Int32 i = 0; i < pointerCount; i++) {
                NativeGetPointerCoords(eventNative->mNativePtr, i, historyPos, (*pc)[i]);
            }

            Int64 eventTimeNanos;
            FAIL_RETURN(NativeGetEventTimeNanos(eventNative->mNativePtr, historyPos, &eventTimeNanos));
            FAIL_RETURN(NativeAddBatch(mNativePtr, eventTimeNanos, pc, metaState));
        }
    }
    *res = TRUE;
    return NOERROR;
}

ECode MotionEvent::IsWithinBoundsNoHistory(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    const Int32 pointerCount = NativeGetPointerCount(mNativePtr);
    for (Int32 i = 0; i < pointerCount; i++) {
        Float x;
        FAIL_RETURN(NativeGetAxisValue(mNativePtr, AXIS_X, i, HISTORY_CURRENT, &x));
        Float y;
        FAIL_RETURN(NativeGetAxisValue(mNativePtr, AXIS_Y, i, HISTORY_CURRENT, &y));
        if (x < left || x > right || y < top || y > bottom) {
            *res = FALSE;
            return NOERROR;
        }
    }
    *res = TRUE;
    return NOERROR;
}

Float MotionEvent::Clamp(
    /* [in] */ Float value,
    /* [in] */ Float low,
    /* [in] */ Float high)
{
    if (value < low) {
        return low;
    }
    else if (value > high) {
        return high;
    }
    return value;
}

ECode MotionEvent::ClampNoHistory(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [out] */ IMotionEvent** event)
{
    VALIDATE_NOT_NULL(event);

    AutoPtr<MotionEvent> ev = Obtain();

    {    AutoLock syncLock(sSharedTempLock);
        const Int32 pointerCount = NativeGetPointerCount(mNativePtr);

        EnsureSharedTempPointerCapacity(pointerCount);
        ArrayOf<IPointerProperties*>* pp = sSharedTempPointerProperties;
        ArrayOf<IPointerCoords*>* pc = sSharedTempPointerCoords;
        Float x, y;
        for (Int32 i = 0; i < pointerCount; i++) {
            NativeGetPointerProperties(mNativePtr, i, (*pp)[i]);
            AutoPtr<IPointerCoords> coords = (*pc)[i];
            NativeGetPointerCoords(mNativePtr, i, HISTORY_CURRENT, coords);

            coords->GetX(&x);
            coords->GetY(&y);
            coords->SetX(Clamp(x, left, right));
            coords->SetY(Clamp(y, top, bottom));
        }

        Int64 id;
        FAIL_RETURN(NativeGetEventTimeNanos(mNativePtr, HISTORY_CURRENT, &id));
        Int64 ptr;
        FAIL_RETURN(NativeInitialize(ev->mNativePtr,
                NativeGetDeviceId(mNativePtr), NativeGetSource(mNativePtr),
                NativeGetAction(mNativePtr), NativeGetFlags(mNativePtr),
                NativeGetEdgeFlags(mNativePtr), NativeGetMetaState(mNativePtr),
                NativeGetButtonState(mNativePtr),
                NativeGetXOffset(mNativePtr), NativeGetYOffset(mNativePtr),
                NativeGetXPrecision(mNativePtr), NativeGetYPrecision(mNativePtr),
                NativeGetDownTimeNanos(mNativePtr),
                id, pointerCount, pp, pc, &ptr));
        ev->mNativePtr = ptr;
        *event = (IMotionEvent*)ev.Get();
        REFCOUNT_ADD(*event);
    }

    return NOERROR;
}

ECode MotionEvent::GetPointerIdBits(
    /* [out] */ Int32* idBits)
{
    VALIDATE_NOT_NULL(idBits);

    *idBits = 0;
    const Int32 pointerCount = NativeGetPointerCount(mNativePtr);
    for (Int32 i = 0; i < pointerCount; i++) {
        Int32 id;
        FAIL_RETURN(NativeGetPointerId(mNativePtr, i, &id));
        *idBits |= 1 << id;
    }
    return NOERROR;
}

ECode MotionEvent::Split(
    /* [in] */ Int32 idBits,
    /* [out] */ IMotionEvent** event)
{
    VALIDATE_NOT_NULL(event);
    *event = NULL;

    AutoPtr<MotionEvent> ev = Obtain();

    {    AutoLock syncLock(sSharedTempLock);
        const Int32 oldPointerCount = NativeGetPointerCount(mNativePtr);
        EnsureSharedTempPointerCapacity(oldPointerCount);
        ArrayOf<IPointerProperties*>* pp = sSharedTempPointerProperties;
        ArrayOf<IPointerCoords*>* pc = sSharedTempPointerCoords;
        ArrayOf<Int32>* map = sSharedTempPointerIndexMap;

        const Int32 oldAction = NativeGetAction(mNativePtr);
        const Int32 oldActionMasked = oldAction & ACTION_MASK;
        const Int32 oldActionPointerIndex = (oldAction & ACTION_POINTER_INDEX_MASK)
                >> ACTION_POINTER_INDEX_SHIFT;
        Int32 newActionPointerIndex = -1;
        Int32 newPointerCount = 0;
        Int32 newIdBits = 0;
        Int32 id;
        for (Int32 i = 0; i < oldPointerCount; i++) {
            AutoPtr<IPointerProperties> ppObj = (*pp)[newPointerCount];
            NativeGetPointerProperties(mNativePtr, i, ppObj);
            ppObj->GetId(&id);
            const Int32 idBit = 1 << id;
            if ((idBit & idBits) != 0) {
                if (i == oldActionPointerIndex) {
                    newActionPointerIndex = newPointerCount;
                }
                (*map)[newPointerCount] = i;
                newPointerCount += 1;
                newIdBits |= idBit;
            }
        }

        if (newPointerCount == 0) {
            Logger::E("MotionEvent", "idBits did not match any ids in the event");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        Int32 newAction;
        if (oldActionMasked == ACTION_POINTER_DOWN || oldActionMasked == ACTION_POINTER_UP) {
            if (newActionPointerIndex < 0) {
                // An unrelated pointer changed.
                newAction = ACTION_MOVE;
            }
            else if (newPointerCount == 1) {
                // The first/last pointer went down/up.
                newAction = oldActionMasked == ACTION_POINTER_DOWN
                        ? ACTION_DOWN : ACTION_UP;
            }
            else {
                // A secondary pointer went down/up.
                newAction = oldActionMasked
                        | (newActionPointerIndex << ACTION_POINTER_INDEX_SHIFT);
            }
        }
        else {
            // Simple up/down/cancel/move or other motion action.
            newAction = oldAction;
        }

        const Int32 historySize = NativeGetHistorySize(mNativePtr);
        for (Int32 h = 0; h <= historySize; h++) {
            const Int32 historyPos = h == historySize ? HISTORY_CURRENT : h;

            for (Int32 i = 0; i < newPointerCount; i++) {
                NativeGetPointerCoords(mNativePtr, (*map)[i], historyPos, (*pc)[i]);
            }

            Int64 eventTimeNanos;
            FAIL_RETURN(NativeGetEventTimeNanos(mNativePtr, historyPos, &eventTimeNanos));

            if (h == 0) {
                Int64 ptr;
                FAIL_RETURN(NativeInitialize(ev->mNativePtr,
                        NativeGetDeviceId(mNativePtr), NativeGetSource(mNativePtr),
                        newAction, NativeGetFlags(mNativePtr),
                        NativeGetEdgeFlags(mNativePtr), NativeGetMetaState(mNativePtr),
                        NativeGetButtonState(mNativePtr),
                        NativeGetXOffset(mNativePtr), NativeGetYOffset(mNativePtr),
                        NativeGetXPrecision(mNativePtr), NativeGetYPrecision(mNativePtr),
                        NativeGetDownTimeNanos(mNativePtr), eventTimeNanos,
                        newPointerCount, pp, pc, &ptr));
                ev->mNativePtr = ptr;
            }
            else {
                FAIL_RETURN(NativeAddBatch(ev->mNativePtr, eventTimeNanos, pc, 0));
            }
        }
        *event = (IMotionEvent*)ev.Get();
        REFCOUNT_ADD(*event);
    }

    return NOERROR;
}

ECode MotionEvent::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder msg;
    msg.Append("MotionEvent { action=");
    Int32 action;
    GetAction(&action);
    msg.Append(ActionToString(action));

    Int32 pointerCount;
    GetPointerCount(&pointerCount);
    for (Int32 i = 0; i < pointerCount; i++) {
        msg.Append(", id[");
        msg.Append(i);
        msg.Append("]=");
        Int32 id;
        GetPointerId(i, &id);
        msg.Append(id);
        msg.Append(", x[");
        msg.Append(i);
        msg.Append("]=");
        Float x, y;
        GetX(i, &x);
        GetY(i, &y);
        msg.Append(x);
        msg.Append(", y[");
        msg.Append(i);
        msg.Append("]=");
        msg.Append(y);
        msg.Append(", toolType[");
        msg.Append(i);
        msg.Append("]=");
        Int32 toolType;
        GetToolType(i, &toolType);
        msg.Append(ToolTypeToString(toolType));
    }

    msg.Append(", buttonState=");
    Int32 buttonState;
    GetButtonState(&buttonState);
    msg.Append(ButtonStateToString(buttonState));
    msg.Append(", metaState=");
    Int32 metaState;
    GetMetaState(&metaState);
    msg.Append(KeyEvent::MetaStateToString(metaState));
    msg.Append(", flags=0x");
    Int32 flags;
    GetFlags(&flags);
    msg.Append(StringUtils::ToHexString(flags));
    msg.Append(", edgeFlags=0x");
    GetEdgeFlags(&flags);
    msg.Append(StringUtils::ToHexString(flags));
    msg.Append(", pointerCount=");
    msg.Append(pointerCount);
    msg.Append(", historySize=");
    Int32 size;
    GetHistorySize(&size);
    msg.Append(size);
    msg.Append(", eventTime=");
    Int64 eventTime;
    GetEventTime(&eventTime);
    msg.Append(eventTime);
    msg.Append(", downTime=");
    GetDownTime(&eventTime);
    msg.Append(eventTime);
    msg.Append(", deviceId=");
    Int32 id;
    GetDeviceId(&id);
    msg.Append(id);
    msg.Append(", source=0x");
    Int32 source;
    GetSource(&source);
    msg.Append(StringUtils::ToHexString(source));
    msg.Append(" }");

    *str = msg.ToString();
    return NOERROR;
}

String MotionEvent::ActionToString(
    /* [in] */ Int32 action)
{
    switch (action) {
        case ACTION_DOWN:
            return String("ACTION_DOWN");
        case ACTION_UP:
            return String("ACTION_UP");
        case ACTION_CANCEL:
            return String("ACTION_CANCEL");
        case ACTION_OUTSIDE:
            return String("ACTION_OUTSIDE");
        case ACTION_MOVE:
            return String("ACTION_MOVE");
        case ACTION_HOVER_MOVE:
            return String("ACTION_HOVER_MOVE");
        case ACTION_SCROLL:
            return String("ACTION_SCROLL");
        case ACTION_HOVER_ENTER:
            return String("ACTION_HOVER_ENTER");
        case ACTION_HOVER_EXIT:
            return String("ACTION_HOVER_EXIT");
    }
    Int32 index = (action & ACTION_POINTER_INDEX_MASK) >> ACTION_POINTER_INDEX_SHIFT;
    switch (action & ACTION_MASK) {
        case ACTION_POINTER_DOWN: {
            StringBuilder builder;
            builder += "ACTION_POINTER_DOWN()";
            builder += index;
            builder += ")";
            return builder.ToString();
        }
        case ACTION_POINTER_UP: {
            StringBuilder builder;
            builder += "ACTION_POINTER_UP()";
            builder += index;
            builder += ")";
            return builder.ToString();
        }
        default:
            return StringUtils::ToString(action);
    }
}

String MotionEvent::AxisToString(
    /* [in] */ Int32 axis)
{
    String symbolicName = NativeAxisToString(axis);

    return symbolicName.IsNull() ? StringUtils::ToString(axis) : LABEL_PREFIX + symbolicName;
}

ECode MotionEvent::AxisFromString(
    /* [in] */ const String& symbolicName,
    /* [out] */ Int32* axis)
{
    VALIDATE_NOT_NULL(axis);
    *axis = -1;

    String str = symbolicName;
    if (symbolicName.StartWith(LABEL_PREFIX)) {
        str = symbolicName.Substring(LABEL_PREFIX.GetLength());
        Int32 a = NativeAxisFromString(str);
        if (a >= 0) {
            *axis = a;
            return NOERROR;
        }
    }

    ECode ec = StringUtils::Parse(str, 10, axis);
    if (FAILED(ec)) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    return NOERROR;
}

String MotionEvent::ButtonStateToString(
    /* [in] */ Int32 buttonState)
{
    if (buttonState == 0) {
        return String("0");
    }
    AutoPtr<StringBuilder> result;
    Int32 i = 0;
    while (buttonState != 0) {
        Boolean isSet = (buttonState & 1) != 0;
        buttonState = ((UInt32)buttonState) >> 1; // unsigned shift!
        if (isSet) {
            String name = BUTTON_SYMBOLIC_NAMES[i];
            if (result == NULL) {
                if (buttonState == 0) {
                    return name;
                }
                result = new StringBuilder(name);
            }
            else {
                result->Append("|");
                result->Append(name);
            }
        }
        i += 1;
    }
    return result->ToString();
}

String MotionEvent::ToolTypeToString(
    /* [in] */ Int32 toolType)
{
    AutoPtr<IInterface> obj;
    TOOL_TYPE_SYMBOLIC_NAMES->Get(toolType, (IInterface**)&obj);
    String symbolicName;
    ICharSequence::Probe(obj)->ToString(&symbolicName);

    return symbolicName.IsNull() ? StringUtils::ToString(toolType) : symbolicName;
}

ECode MotionEvent::IsButtonPressed(
    /* [in] */ Int32 button,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (button == 0) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 state;
    GetButtonState(&state);
    *result = (state & button) == button;
    return NOERROR;
}

ECode MotionEvent::CreateFromParcelBody(
    /* [in] */ IParcel* in,
    /* [out] */ IMotionEvent** event)
{
    VALIDATE_NOT_NULL(event);

    AutoPtr<MotionEvent> ev = Obtain();
    Int64 ptr;
    FAIL_RETURN(NativeReadFromParcel(ev->mNativePtr, in, &ptr));
    ev->mNativePtr = ptr;
    *event = (IMotionEvent*)ev.Get();
    REFCOUNT_ADD(*event);
    return NOERROR;
}

ECode MotionEvent::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(PARCEL_TOKEN_MOTION_EVENT);
    NativeWriteToParcel(mNativePtr, out);

    return NOERROR;
}

ECode MotionEvent::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    Int32 parcelToken;
    source->ReadInt32(&parcelToken); // skip token, we already know this is a MotionEvent
    FAIL_RETURN(NativeReadFromParcel(mNativePtr, source, &mNativePtr));

    return NOERROR;
}

ECode MotionEvent::GetNative(
    /* [out] */ Handle64* native)
{
    VALIDATE_NOT_NULL(native);
    *native = (Handle64)mNativePtr;

    return NOERROR;
}

ECode MotionEvent::SetNative(
    /* [in] */ Handle64 native)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(mNativePtr);
    if (event) {
        delete event;
    }
    mNativePtr = native;

    return NOERROR;
}

Int64 MotionEvent::GetNativePtr()
{
    return mNativePtr;
}

ECode MotionEvent::NativeInitialize(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 source,
    /* [in] */ Int32 action,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 edgeFlags,
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 buttonState,
    /* [in] */ Float xOffset,
    /* [in] */ Float yOffset,
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision,
    /* [in] */ Int64 downTimeNanos,
    /* [in] */ Int64 eventTimeNanos,
    /* [in] */ Int32 pointerCount,
    /* [in] */ ArrayOf<IPointerProperties*>* pointerPropertiesObjArray,
    /* [in] */ ArrayOf<IPointerCoords*>* pointerCoordsObjArray,
    /* [out] */ Int64* resultPtr)
{
    VALIDATE_NOT_NULL(resultPtr);
    *resultPtr = 0;

    FAIL_RETURN(ValidatePointerCount(pointerCount));
    FAIL_RETURN(ValidatePointerPropertiesArray(pointerPropertiesObjArray, pointerCount));
    FAIL_RETURN(ValidatePointerCoordsObjArray(pointerCoordsObjArray, pointerCount));

    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    if (!event) {
        event = new android::MotionEvent();
    }

    android::PointerProperties pointerProperties[pointerCount];
    android::PointerCoords rawPointerCoords[pointerCount];

    for (Int32 i = 0; i < pointerCount; i++) {
        AutoPtr<IPointerProperties> pointerPropertiesObj = (*pointerPropertiesObjArray)[i];
        if (!pointerPropertiesObj) {
            goto Error;
        }
        PointerPropertiesToNative(pointerPropertiesObj, &pointerProperties[i]);

        AutoPtr<IPointerCoords> pointerCoordsObj = (*pointerCoordsObjArray)[i];
        if (!pointerCoordsObj) {
            // jniThrowNullPointerException(env, "pointerCoords");
            goto Error;
        }
        PointerCoordsToNative(pointerCoordsObj, xOffset, yOffset, &rawPointerCoords[i]);
    }

    event->initialize(deviceId, source, action, flags, edgeFlags, metaState, buttonState,
            xOffset, yOffset, xPrecision, yPrecision,
            downTimeNanos, eventTimeNanos, pointerCount, pointerProperties, rawPointerCoords);

    *resultPtr = reinterpret_cast<Int64>(event);
    return NOERROR;

Error:
    if (!nativePtr) {
        delete event;
    }
    *resultPtr = 0;
    return NOERROR;
}

Int64 MotionEvent::NativeCopy(
    /* [in] */ Int64 destNativePtr,
    /* [in] */ Int64 sourceNativePtr,
    /* [in] */ Boolean keepHistory)
{
    android::MotionEvent* destEvent = reinterpret_cast<android::MotionEvent*>(destNativePtr);
    if (!destEvent) {
        destEvent = new android::MotionEvent();
    }
    android::MotionEvent* sourceEvent = reinterpret_cast<android::MotionEvent*>(sourceNativePtr);
    destEvent->copyFrom(sourceEvent, keepHistory);
    return reinterpret_cast<Int64>(destEvent);
}

void MotionEvent::NativeDispose(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    delete event;
}

ECode MotionEvent::NativeAddBatch(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int64 eventTimeNanos,
    /* [in] */ ArrayOf<IPointerCoords*>* pointerCoordsObjArray,
    /* [in] */ Int32 metaState)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    size_t pointerCount = event->getPointerCount();
    FAIL_RETURN(ValidatePointerCoordsObjArray(pointerCoordsObjArray, pointerCount));

    android::PointerCoords rawPointerCoords[pointerCount];

    for (size_t i = 0; i < pointerCount; i++) {
        AutoPtr<IPointerCoords> pointerCoordsObj = (*pointerCoordsObjArray)[i];
        if (!pointerCoordsObj) {
            // jniThrowNullPointerException(env, "pointerCoords");
            return E_NULL_POINTER_EXCEPTION;
        }
        PointerCoordsToNative(pointerCoordsObj,
                event->getXOffset(), event->getYOffset(), &rawPointerCoords[i]);
    }

    event->addSample(eventTimeNanos, rawPointerCoords);
    event->setMetaState(event->getMetaState() | metaState);
    return NOERROR;
}

Int32 MotionEvent::NativeGetDeviceId(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getDeviceId();
}

Int32 MotionEvent::NativeGetSource(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getSource();
}

void MotionEvent::NativeSetSource(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 source)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    event->setSource(source);
}

Int32 MotionEvent::NativeGetAction(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getAction();
}

void MotionEvent::NativeSetAction(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 action)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    event->setAction(action);
}

Boolean MotionEvent::NativeIsTouchEvent(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->isTouchEvent();
}

Int32 MotionEvent::NativeGetFlags(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getFlags();
}

void MotionEvent::NativeSetFlags(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 flags)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    event->setFlags(flags);
}

Int32 MotionEvent::NativeGetEdgeFlags(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getEdgeFlags();
}

void MotionEvent::NativeSetEdgeFlags(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 edgeFlags)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    event->setEdgeFlags(edgeFlags);
}

Int32 MotionEvent::NativeGetMetaState(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getMetaState();
}

Int32 MotionEvent::NativeGetButtonState(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getButtonState();
}

void MotionEvent::NativeOffsetLocation(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Float deltaX,
    /* [in] */ Float deltaY)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->offsetLocation(deltaX, deltaY);
}

Float MotionEvent::NativeGetXOffset(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getXOffset();
}

Float MotionEvent::NativeGetYOffset(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getYOffset();
}

Float MotionEvent::NativeGetXPrecision(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getXPrecision();
}

Float MotionEvent::NativeGetYPrecision(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getYPrecision();
}

Int64 MotionEvent::NativeGetDownTimeNanos(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return event->getDownTime();
}

void MotionEvent::NativeSetDownTimeNanos(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int64 downTimeNanos)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    event->setDownTime(downTimeNanos);
}

Int32 MotionEvent::NativeGetPointerCount(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return Int32(event->getPointerCount());
}

ECode MotionEvent::NativeGetPointerId(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = -1;

    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    size_t pointerCount = event->getPointerCount();
    FAIL_RETURN(ValidatePointerIndex(pointerIndex, pointerCount));

    *id = event->getPointerId(pointerIndex);
    return NOERROR;
}

ECode MotionEvent::NativeGetToolType(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = -1;

    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    size_t pointerCount = event->getPointerCount();
    FAIL_RETURN(ValidatePointerIndex(pointerIndex, pointerCount));

    *id = event->getToolType(pointerIndex);
    return NOERROR;
}

Int32 MotionEvent::NativeFindPointerIndex(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 pointerId)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return Int32(event->findPointerIndex(pointerId));
}

Int32 MotionEvent::NativeGetHistorySize(
    /* [in] */ Int64 nativePtr)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    return Int32(event->getHistorySize());
}

ECode MotionEvent::NativeGetEventTimeNanos(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 historyPos,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0;

    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    if (historyPos == HISTORY_CURRENT) {
        *id = event->getEventTime();
        return NOERROR;
    }
    else {
        size_t historySize = event->getHistorySize();
        FAIL_RETURN(ValidateHistoryPos(historyPos, historySize));

        *id = event->getHistoricalEventTime(historyPos);
        return NOERROR;
    }
}

ECode MotionEvent::NativeGetRawAxisValue(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 axis,
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 historyPos,
    /* [out] */ Float* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0;

    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    size_t pointerCount = event->getPointerCount();
    FAIL_RETURN(ValidatePointerIndex(pointerIndex, pointerCount));

    if (historyPos == HISTORY_CURRENT) {
        *id = event->getRawAxisValue(axis, pointerIndex);
        return NOERROR;
    }
    else {
        size_t historySize = event->getHistorySize();
        FAIL_RETURN(ValidateHistoryPos(historyPos, historySize));

        *id = event->getHistoricalRawAxisValue(axis, pointerIndex, historyPos);
        return NOERROR;
    }
}

ECode MotionEvent::NativeGetAxisValue(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 axis,
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 historyPos,
    /* [out] */ Float* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0;

    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    size_t pointerCount = event->getPointerCount();
    FAIL_RETURN(ValidatePointerIndex(pointerIndex, pointerCount));

    if (historyPos == HISTORY_CURRENT) {
        *id = event->getAxisValue(axis, pointerIndex);
        return NOERROR;
    }
    else {
        size_t historySize = event->getHistorySize();
        FAIL_RETURN(ValidateHistoryPos(historyPos, historySize));

        *id = event->getHistoricalAxisValue(axis, pointerIndex, historyPos);
        return NOERROR;
    }
}

ECode MotionEvent::NativeGetPointerCoords(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 pointerIndex,
    /* [in] */ Int32 historyPos,
    /* [in, out] */ IPointerCoords* outPointerCoordsObj)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    size_t pointerCount = event->getPointerCount();
    FAIL_RETURN(ValidatePointerIndex(pointerIndex, pointerCount));
    FAIL_RETURN(ValidatePointerCoords(outPointerCoordsObj));

    const android::PointerCoords* rawPointerCoords;
    if (historyPos == HISTORY_CURRENT) {
        rawPointerCoords = event->getRawPointerCoords(pointerIndex);
    }
    else {
        size_t historySize = event->getHistorySize();
        FAIL_RETURN(ValidateHistoryPos(historyPos, historySize));

        rawPointerCoords = event->getHistoricalRawPointerCoords(pointerIndex, historyPos);
    }
    PointerCoordsFromNative(rawPointerCoords, event->getXOffset(), event->getYOffset(),
            outPointerCoordsObj);
    return NOERROR;
}

ECode MotionEvent::NativeGetPointerProperties(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Int32 pointerIndex,
    /* [in, out] */ IPointerProperties* outPointerPropertiesObj)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    size_t pointerCount = event->getPointerCount();
    FAIL_RETURN(ValidatePointerIndex(pointerIndex, pointerCount));
    FAIL_RETURN(ValidatePointerProperties(outPointerPropertiesObj));

    const android::PointerProperties* pointerProperties = event->getPointerProperties(pointerIndex);
    PointerPropertiesFromNative(pointerProperties, outPointerPropertiesObj);
    return NOERROR;
}

void MotionEvent::NativeScale(
    /* [in] */ Int64 nativePtr,
    /* [in] */ Float scale)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    event->scale(scale);
}

void MotionEvent::NativeTransform(
    /* [in] */ Int64 nativePtr,
    /* [in] */ IMatrix* matrixObj)
{
    SkMatrix* matrix = (SkMatrix*)((CMatrix*)matrixObj)->mNativeMatrix;
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);

    float m[9];
    m[0] = SkScalarToFloat(matrix->get(SkMatrix::kMScaleX));
    m[1] = SkScalarToFloat(matrix->get(SkMatrix::kMSkewX));
    m[2] = SkScalarToFloat(matrix->get(SkMatrix::kMTransX));
    m[3] = SkScalarToFloat(matrix->get(SkMatrix::kMSkewY));
    m[4] = SkScalarToFloat(matrix->get(SkMatrix::kMScaleY));
    m[5] = SkScalarToFloat(matrix->get(SkMatrix::kMTransY));
    m[6] = SkScalarToFloat(matrix->get(SkMatrix::kMPersp0));
    m[7] = SkScalarToFloat(matrix->get(SkMatrix::kMPersp1));
    m[8] = SkScalarToFloat(matrix->get(SkMatrix::kMPersp2));
    event->transform(m);
}

ECode MotionEvent::NativeReadFromParcel(
    /* [in] */ Int64 nativePtr,
    /* [in] */ IParcel* parcelObj,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    if (!event) {
        event = new android::MotionEvent();
    }

    android::Parcel* parcel;
    parcelObj->GetDataPayload((Handle32*)&parcel);

    android::status_t status = event->readFromParcel(parcel);
    if (status) {
        if (!nativePtr) {
            delete event;
        }
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = reinterpret_cast<Int64>(event);
    return NOERROR;
}

ECode MotionEvent::NativeWriteToParcel(
    /* [in] */ Int64 nativePtr,
    /* [in] */ IParcel* parcelObj)
{
    android::MotionEvent* event = reinterpret_cast<android::MotionEvent*>(nativePtr);
    android::Parcel* parcel;
    parcelObj->GetDataPayload((Handle32*)&parcel);

    android::status_t status = event->writeToParcel(parcel);
    if (status) {
       // jniThrowRuntimeException(env, "Failed to write MotionEvent parcel.");
       return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

String MotionEvent::NativeAxisToString(
    /* [in] */ Int32 axis)
{
    return String(android::MotionEvent::getLabel(static_cast<int32_t>(axis)));
}

Int32 MotionEvent::NativeAxisFromString(
    /* [in] */ const String& label)
{
    return static_cast<Int32>(android::MotionEvent::getAxisFromLabel(label.string()));
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
