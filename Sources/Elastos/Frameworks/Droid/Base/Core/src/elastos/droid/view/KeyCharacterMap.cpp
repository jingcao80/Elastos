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

#include <Elastos.CoreLibrary.Text.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/KeyCharacterMap.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/hardware/input/CInputManager.h"
#include "elastos/droid/utility/CSparseInt32Array.h"
#include <elastos/core/Character.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <binder/Parcel.h>
#include <input/KeyCharacterMap.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Hardware::Input::CInputManager;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Core::Character;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Text::INormalizer;
using Elastos::Text::CNormalizerHelper;
using Elastos::Text::NormalizerForm_NFC;
using Elastos::Utility::Etl::Vector;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {

const String KeyCharacterMap::TAG("CKeyCharacterMap");
const Int32 KeyCharacterMap::ACCENT_ACUTE = 0x00B4;
const Int32 KeyCharacterMap::ACCENT_BREVE = 0x02D8;
const Int32 KeyCharacterMap::ACCENT_CARON = 0x02C7;
const Int32 KeyCharacterMap::ACCENT_CEDILLA = 0x00B8;
const Int32 KeyCharacterMap::ACCENT_CIRCUMFLEX = 0x02C6;
const Int32 KeyCharacterMap::ACCENT_COMMA_ABOVE = 0x1FBD;
const Int32 KeyCharacterMap::ACCENT_COMMA_ABOVE_RIGHT = 0x02BC;
const Int32 KeyCharacterMap::ACCENT_DOT_ABOVE = 0x02D9;
const Int32 KeyCharacterMap::ACCENT_DOT_BELOW = '.';
const Int32 KeyCharacterMap::ACCENT_DOUBLE_ACUTE = 0x02DD;
const Int32 KeyCharacterMap::ACCENT_GRAVE = 0x02CB;
const Int32 KeyCharacterMap::ACCENT_HOOK_ABOVE = 0x02C0;
const Int32 KeyCharacterMap::ACCENT_HORN = '\'';
const Int32 KeyCharacterMap::ACCENT_MACRON = 0x00AF;
const Int32 KeyCharacterMap::ACCENT_MACRON_BELOW = 0x02CD;
const Int32 KeyCharacterMap::ACCENT_OGONEK = 0x02DB;
const Int32 KeyCharacterMap::ACCENT_REVERSED_COMMA_ABOVE = 0x02BD;
const Int32 KeyCharacterMap::ACCENT_RING_ABOVE = 0x02DA;
const Int32 KeyCharacterMap::ACCENT_STROKE = '-';
const Int32 KeyCharacterMap::ACCENT_TILDE = 0x02DC;
const Int32 KeyCharacterMap::ACCENT_TURNED_COMMA_ABOVE = 0x02BB;
const Int32 KeyCharacterMap::ACCENT_UMLAUT = 0x00A8;
const Int32 KeyCharacterMap::ACCENT_VERTICAL_LINE_ABOVE = 0x02C8;
const Int32 KeyCharacterMap::ACCENT_VERTICAL_LINE_BELOW = 0x02CC;

const Int32 KeyCharacterMap::ACCENT_GRAVE_LEGACY = '`';
const Int32 KeyCharacterMap::ACCENT_CIRCUMFLEX_LEGACY = '^';
const Int32 KeyCharacterMap::ACCENT_TILDE_LEGACY = '~';
const Int32 KeyCharacterMap::CHAR_SPACE = ' ';

static AutoPtr<ISparseInt32Array> InitSparse()
{
    AutoPtr<ISparseInt32Array> array;
    CSparseInt32Array::New((ISparseInt32Array**)&array);
    return array.Get();
}

AutoPtr<ISparseInt32Array> KeyCharacterMap::sCombiningToAccent = InitSparse();
AutoPtr<ISparseInt32Array> KeyCharacterMap::sAccentToCombining = InitSparse();
AutoPtr<ISparseInt32Array> KeyCharacterMap::sDeadKeyCache = InitSparse();

Object KeyCharacterMap::sDeadKeyCacheLock;

static AutoPtr<IKeyEvent> CreateKeyEventFromNative(
    /* [in] */ const android::KeyEvent* event)
{
    AutoPtr<IKeyEvent> eventObj;
    ECode ec = CKeyEvent::New(nanoseconds_to_milliseconds(event->getDownTime()),
            nanoseconds_to_milliseconds(event->getEventTime()),
            event->getAction(),
            event->getKeyCode(),
            event->getRepeatCount(),
            event->getMetaState(),
            event->getDeviceId(),
            event->getScanCode(),
            event->getFlags(),
            event->getSource(),
            (IKeyEvent**)&eventObj);
    if (FAILED(ec)) {
//        LOGE("An exception occurred while obtaining a key event.");
        return NULL;
    }
    return eventObj;
}

static AutoPtr<IStringBuilder> CreateStringBuilder()
{
    AutoPtr<IStringBuilder> stringBuilder = new StringBuilder();

    return stringBuilder;
}

AutoPtr<IStringBuilder> KeyCharacterMap::sDeadKeyBuilder = CreateStringBuilder();

const Boolean KeyCharacterMap::mIsStaticInited = KeyCharacterMap::InitStatic();

Boolean KeyCharacterMap::InitStatic()
{
    AddCombining(0x0300, ACCENT_GRAVE);
    AddCombining(0x0301, ACCENT_ACUTE);
    AddCombining(0x0302, ACCENT_CIRCUMFLEX);
    AddCombining(0x0303, ACCENT_TILDE);
    AddCombining(0x0304, ACCENT_MACRON);
    AddCombining(0x0306, ACCENT_BREVE);
    AddCombining(0x0307, ACCENT_DOT_ABOVE);
    AddCombining(0x0308, ACCENT_UMLAUT);
    AddCombining(0x0309, ACCENT_HOOK_ABOVE);
    AddCombining(0x030A, ACCENT_RING_ABOVE);
    AddCombining(0x030B, ACCENT_DOUBLE_ACUTE);
    AddCombining(0x030C, ACCENT_CARON);
    AddCombining(0x030D, ACCENT_VERTICAL_LINE_ABOVE);
    //AddCombining(0x030E, ACCENT_DOUBLE_VERTICAL_LINE_ABOVE);
    //AddCombining(0x030F, ACCENT_DOUBLE_GRAVE);
    //AddCombining(0x0310, ACCENT_CANDRABINDU);
    //AddCombining(0x0311, ACCENT_INVERTED_BREVE);
    AddCombining(0x0312, ACCENT_TURNED_COMMA_ABOVE);
    AddCombining(0x0313, ACCENT_COMMA_ABOVE);
    AddCombining(0x0314, ACCENT_REVERSED_COMMA_ABOVE);
    AddCombining(0x0315, ACCENT_COMMA_ABOVE_RIGHT);
    AddCombining(0x031B, ACCENT_HORN);
    AddCombining(0x0323, ACCENT_DOT_BELOW);
    //AddCombining(0x0326, ACCENT_COMMA_BELOW);
    AddCombining(0x0327, ACCENT_CEDILLA);
    AddCombining(0x0328, ACCENT_OGONEK);
    AddCombining(0x0329, ACCENT_VERTICAL_LINE_BELOW);
    AddCombining(0x0331, ACCENT_MACRON_BELOW);
    AddCombining(0x0335, ACCENT_STROKE);
    //AddCombining(0x0342, ACCENT_PERISPOMENI);
    //AddCombining(0x0344, ACCENT_DIALYTIKA_TONOS);
    //AddCombining(0x0345, ACCENT_YPOGEGRAMMENI);

    // One-way mappings to equivalent preferred accents.
    sCombiningToAccent->Append(0x0340, ACCENT_GRAVE);
    sCombiningToAccent->Append(0x0341, ACCENT_ACUTE);
    sCombiningToAccent->Append(0x0343, ACCENT_COMMA_ABOVE);

    // One-way legacy mappings to preserve compatibility with older applications.
    sAccentToCombining->Append(ACCENT_GRAVE_LEGACY, 0x0300);
    sAccentToCombining->Append(ACCENT_CIRCUMFLEX_LEGACY, 0x0302);
    sAccentToCombining->Append(ACCENT_TILDE_LEGACY, 0x0303);

    // Non-standard decompositions.
    // Stroke modifier for Finnish multilingual keyboard and others.
    AddDeadKey(ACCENT_STROKE, 'D', 0x0110);
    AddDeadKey(ACCENT_STROKE, 'G', 0x01e4);
    AddDeadKey(ACCENT_STROKE, 'H', 0x0126);
    AddDeadKey(ACCENT_STROKE, 'I', 0x0197);
    AddDeadKey(ACCENT_STROKE, 'L', 0x0141);
    AddDeadKey(ACCENT_STROKE, 'O', 0x00d8);
    AddDeadKey(ACCENT_STROKE, 'T', 0x0166);
    AddDeadKey(ACCENT_STROKE, 'd', 0x0111);
    AddDeadKey(ACCENT_STROKE, 'g', 0x01e5);
    AddDeadKey(ACCENT_STROKE, 'h', 0x0127);
    AddDeadKey(ACCENT_STROKE, 'i', 0x0268);
    AddDeadKey(ACCENT_STROKE, 'l', 0x0142);
    AddDeadKey(ACCENT_STROKE, 'o', 0x00f8);
    AddDeadKey(ACCENT_STROKE, 't', 0x0167);

    return TRUE;
}

void KeyCharacterMap::AddCombining(
    /* [in] */ Int32 combining,
    /* [in] */ Int32 accent)
{
    sCombiningToAccent->Append(combining, accent);
    sAccentToCombining->Append(accent, combining);
}

void KeyCharacterMap::AddDeadKey(
    /* [in] */ Int32 accent,
    /* [in] */ Int32 c,
    /* [in] */ Int32 result)
{
    Int32 combining;
    sAccentToCombining->Get(accent, &combining);
    if (combining == 0) {
        ALOGE("Invalid dead key declaration.");
        assert(0);
        //throw new IllegalStateException("Invalid dead key declaration.");
    }
    Int32 combination = (combining << 16) | c;
    sDeadKeyCache->Put(combination, result);
}

/*   KeyCharacterMap::FallbackAction   */

Int32 KeyCharacterMap::FallbackAction::MAX_RECYCLED = 10;
Object KeyCharacterMap::FallbackAction::sRecycleLock;
AutoPtr<KeyCharacterMap::FallbackAction> KeyCharacterMap::FallbackAction::sRecycleBin;
Int32 KeyCharacterMap::FallbackAction::sRecycledCount = 0;

CAR_INTERFACE_IMPL(KeyCharacterMap::FallbackAction, Object, IFallbackAction);

KeyCharacterMap::FallbackAction::FallbackAction()
{
}

ECode KeyCharacterMap::FallbackAction::constructor()
{
    return NOERROR;
}

AutoPtr<KeyCharacterMap::FallbackAction> KeyCharacterMap::FallbackAction::Obtain()
{
    AutoPtr<FallbackAction> target;
    {    AutoLock syncLock(sRecycleLock);
        if (sRecycleBin == NULL) {
            target = new FallbackAction();
        }
        else {
            target = sRecycleBin;
            sRecycleBin = target->mNext;
            sRecycledCount--;
            target->mNext = NULL;
        }
    }

    return target;
}

KeyCharacterMap::FallbackAction::Recycle()
{
    {    AutoLock syncLock(sRecycleLock);
        if (sRecycledCount < MAX_RECYCLED) {
            mNext = sRecycleBin;
            sRecycleBin = this;
            sRecycledCount++;
        }
        else {
            mNext = NULL;
        }
    }

    return NOERROR;
}

ECode KeyCharacterMap::FallbackAction::SetKeyCode(
    /* [in] */ Int32 keyCode)
{
    mKeyCode = keyCode;
    return NOERROR;
}

ECode KeyCharacterMap::FallbackAction::GetKeyCode(
    /* [out] */ Int32* keyCode)
{
    VALIDATE_NOT_NULL(keyCode);
    *keyCode = mKeyCode;
    return NOERROR;
}

ECode KeyCharacterMap::FallbackAction::SetMetaState(
    /* [in] */ Int32 metaState)
{
    mMetaState = metaState;
    return NOERROR;
}

ECode KeyCharacterMap::FallbackAction::GetMetaState(
    /* [out] */ Int32* metaState)
{
    VALIDATE_NOT_NULL(metaState);
    *metaState = mMetaState;
    return NOERROR;
}

/*   KeyCharacterMap::KeyData   */

CAR_INTERFACE_IMPL(KeyCharacterMap::KeyData, Object, IKeyData);

KeyCharacterMap::KeyData::KeyData()
{
    mMeta = ArrayOf<Char32>::Alloc(META_LENGTH);
}

KeyCharacterMap::KeyData::~KeyData()
{}

ECode KeyCharacterMap::KeyData::constructor()
{
    return NOERROR;
}

ECode KeyCharacterMap::KeyData::SetDisplayLabel(
    /* [in] */ Char32 displayLabel)
{
    mDisplayLabel = displayLabel;
    return NOERROR;
}

ECode KeyCharacterMap::KeyData::GetDisplayLabel(
    /* [out] */ Char32* displayLabel)
{
    VALIDATE_NOT_NULL(displayLabel);
    *displayLabel = mDisplayLabel;
    return NOERROR;
}

ECode KeyCharacterMap::KeyData::SetNumber(
    /* [in] */ Char32 number)
{
    mNumber = number;
    return NOERROR;
}

ECode KeyCharacterMap::KeyData::GetNumber(
    /* [out] */ Char32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mNumber;
    return NOERROR;
}

ECode KeyCharacterMap::KeyData::SetMeta(
    /* [in] */ ArrayOf<Char32>* meta)
{
    mMeta = meta;
    return NOERROR;
}

ECode KeyCharacterMap::KeyData::GetMeta(
    /* [out, callee] */ ArrayOf<Char32>** meta)
{
    VALIDATE_NOT_NULL(meta);
    *meta = mMeta;
    REFCOUNT_ADD(*meta);
    return NOERROR;
}

/*   KeyCharacterMap   */
class NativeKeyCharacterMap
{
public:
    NativeKeyCharacterMap(int32_t deviceId, const android::sp<android::KeyCharacterMap>& map) :
        mDeviceId(deviceId), mMap(map)
    {
    }

    ~NativeKeyCharacterMap() {
    }

    inline int32_t getDeviceId() const {
        return mDeviceId;
    }

    inline const android::sp<android::KeyCharacterMap>& getMap() const {
        return mMap;
    }

private:
    int32_t mDeviceId;
    android::sp<android::KeyCharacterMap> mMap;
};

CAR_INTERFACE_IMPL_2(KeyCharacterMap, Object, IKeyCharacterMap, IParcelable);

KeyCharacterMap::KeyCharacterMap()
    : mPtr(0)
{}

KeyCharacterMap::~KeyCharacterMap()
{
    if (mPtr != 0) {
        NativeKeyCharacterMap* map = reinterpret_cast<NativeKeyCharacterMap*>(mPtr);
        delete map;
        mPtr = 0;
    }
}

ECode KeyCharacterMap::constructor()
{
    return NOERROR;
}

ECode KeyCharacterMap::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    if (in == NULL) {
        Slogger::E(TAG, String("parcel must not be NULL"));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mPtr = NativeReadFromParcel(in);
    if (mPtr == 0) {
        Slogger::E(TAG, String("Could not read KeyCharacterMap from parcel."));
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode KeyCharacterMap::constructor(
    /* [in] */ Int64 ptr)
{
    mPtr = ptr;
    return NOERROR;
}

ECode KeyCharacterMap::Load(
    /* [in] */ Int32 deviceId,
    /* [out] */ IKeyCharacterMap** kcm)
{
    VALIDATE_NOT_NULL(kcm);

    AutoPtr<IInputManager> im = CInputManager::GetInstance();
    AutoPtr<IInputDevice> inputDevice;
    im->GetInputDevice(deviceId, (IInputDevice**)&inputDevice);
    if (inputDevice == NULL) {
        im->GetInputDevice(IKeyCharacterMap::VIRTUAL_KEYBOARD, (IInputDevice**)&inputDevice);
        if (inputDevice == NULL) {
            Slogger::E(TAG, String("Could not load key character map for device ")
                + StringUtils::ToString(deviceId));
            return E_UNAVAILABLE_EXCEPTION;
        }
    }
    return inputDevice->GetKeyCharacterMap(kcm);
}

ECode KeyCharacterMap::Get(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 metaState,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    metaState = KeyEvent::NormalizeMetaState(metaState);
    Char32 ch = NativeGetCharacter(mPtr, keyCode, metaState);

    Int32 map;
    sCombiningToAccent->Get(ch, &map);
    if (map != 0) {
        *value = map | IKeyCharacterMap::COMBINING_ACCENT;
    }
    else {
        *value = ch;
    }

    return NOERROR;
}

ECode KeyCharacterMap::GetFallbackAction(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 metaState,
    /* [out] */ IFallbackAction** fallbackAction)
{
    VALIDATE_NOT_NULL(fallbackAction);

    AutoPtr<FallbackAction> action = FallbackAction::Obtain();
    metaState = KeyEvent::NormalizeMetaState(metaState);
    if (NativeGetFallbackAction(mPtr, keyCode, metaState, action)) {
        action->mMetaState = KeyEvent::NormalizeMetaState(action->mMetaState);
        *fallbackAction = (IFallbackAction*)action.Get();
        REFCOUNT_ADD(*fallbackAction);
        return NOERROR;
    }

    action->Recycle();
    *fallbackAction = NULL;

    return NOERROR;
}

ECode KeyCharacterMap::GetNumber(
    /* [in] */ Int32 keyCode,
    /* [out] */ Char32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = NativeGetNumber(mPtr, keyCode);

    return NOERROR;
}

ECode KeyCharacterMap::GetMatch(
    /* [in] */ Int32 keyCode,
    /* [in] */ ArrayOf<Char32>* chars,
    /* [out] */ Char32* match)
{
    VALIDATE_NOT_NULL(match);
    return GetMatch(keyCode, chars, 0, match);
}

ECode KeyCharacterMap::GetMatch(
    /* [in] */ Int32 keyCode,
    /* [in] */ ArrayOf<Char32>* chars,
    /* [in] */ Int32 metaState,
    /* [out] */ Char32* match)
{
    VALIDATE_NOT_NULL(chars);
    VALIDATE_NOT_NULL(match);
    metaState = KeyEvent::NormalizeMetaState(metaState);
    *match = NativeGetMatch(mPtr, keyCode, *chars, metaState);

    return NOERROR;
}

ECode KeyCharacterMap::GetDisplayLabel(
    /* [in] */ Int32 keyCode,
    /* [out] */ Char32* label)
{
    VALIDATE_NOT_NULL(label);
    *label = NativeGetDisplayLabel(mPtr, keyCode);

    return NOERROR;
}

Int32 KeyCharacterMap::GetDeadChar(
    /* [in] */ Int32 accent,
    /* [in] */ Int32 c)
{
    if (c == accent || CHAR_SPACE == c) {
        // The same dead character typed twice or a dead character followed by a
        // space should both produce the non-combining version of the combining char.
        // In this case we don't even need to compute the combining character.
        return accent;
    }

    Int32 combining;
    sAccentToCombining->Get(accent, &combining);
    if (combining == 0) {
        return 0;
    }

    Int32 combination = (combining << 16) | c;
    Int32 combined;
    {    AutoLock syncLock(sDeadKeyCacheLock);
        sDeadKeyCache->Get(combination, -1, &combined);
        if (combined == -1) {
            sDeadKeyBuilder->SetLength(0);
            sDeadKeyBuilder->AppendChar((Char32)c);
            sDeadKeyBuilder->AppendChar((Char32)combining);
            AutoPtr<INormalizer> helper;
            CNormalizerHelper::AcquireSingleton((INormalizer**)&helper);
            String result;
            helper->Normalize(ICharSequence::Probe(sDeadKeyBuilder), NormalizerForm_NFC, &result);
            combined = result.GetLength() == 1 ? result.GetChar(0) : 0;
            sDeadKeyCache->Put(combination, combined);
        }
    }

    return combined;
}

ECode KeyCharacterMap::GetKeyData(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyData* results,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    KeyData* keyData = (KeyData*)results;
    if (keyData->mMeta->GetLength() < IKeyData::META_LENGTH) {
        Slogger::E(TAG, String("results.meta.length must be >= ")
                + StringUtils::ToString(IKeyData::META_LENGTH));
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Char32 displayLabel = NativeGetDisplayLabel(mPtr, keyCode);
    if (displayLabel == 0) {
        *res = FALSE;
        return NOERROR;
    }

    keyData->mDisplayLabel = displayLabel;
    keyData->mNumber = NativeGetNumber(mPtr, keyCode);
    (*keyData->mMeta)[0] = NativeGetCharacter(mPtr, keyCode, 0);
    (*keyData->mMeta)[1] = NativeGetCharacter(mPtr, keyCode, IKeyEvent::META_SHIFT_ON);
    (*keyData->mMeta)[2] = NativeGetCharacter(mPtr, keyCode, IKeyEvent::META_ALT_ON);
    (*keyData->mMeta)[3] = NativeGetCharacter(mPtr, keyCode,
            IKeyEvent::META_ALT_ON | IKeyEvent::META_SHIFT_ON);
    *res = TRUE;
    return NOERROR;
}

ECode KeyCharacterMap::GetEvents(
    /* [in] */ ArrayOf<Char32>* chars,
    /* [out, callee] */ ArrayOf<IKeyEvent*>** keyEvents)
{
    VALIDATE_NOT_NULL(chars);
    VALIDATE_NOT_NULL(keyEvents);
    AutoPtr<ArrayOf<IKeyEvent*> > temp = NativeGetEvents(mPtr, *chars);
    *keyEvents = temp;
    REFCOUNT_ADD(*keyEvents);

    return NOERROR;
}

ECode KeyCharacterMap::IsPrintingKey(
    /* [in] */ Int32 keyCode,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 type = Character::GetType(NativeGetDisplayLabel(mPtr, keyCode));

    switch (type) {
        case Character::SPACE_SEPARATOR:
        case Character::LINE_SEPARATOR:
        case Character::PARAGRAPH_SEPARATOR:
        case Character::CONTROL:
        case Character::FORMAT:
            *res = FALSE;
            break;
        default:
            *res = TRUE;
            break;
    }

    return NOERROR;
}

ECode KeyCharacterMap::GetKeyboardType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = NativeGetKeyboardType(mPtr);

    return NOERROR;
}

ECode KeyCharacterMap::GetModifierBehavior(
    /* [out] */ Int32* behavior)
{
    VALIDATE_NOT_NULL(behavior);
    Int32 type;
    GetKeyboardType(&type);
    switch (type) {
        case IKeyCharacterMap::FULL:
        case IKeyCharacterMap::SPECIAL_FUNCTION:
            *behavior = IKeyCharacterMap::MODIFIER_BEHAVIOR_CHORDED;
            break;
        default:
            *behavior = IKeyCharacterMap::MODIFIER_BEHAVIOR_CHORDED_OR_TOGGLED;
            break;
    }

    return NOERROR;
}

ECode KeyCharacterMap::DeviceHasKey(
    /* [in] */ Int32 keyCode,
    /* [out] */ Boolean* hasKey)
{
    VALIDATE_NOT_NULL(hasKey);
    AutoPtr< ArrayOf<Int32> > keyCodes = ArrayOf<Int32>::Alloc(1);
    (*keyCodes)[0] = keyCode;

    AutoPtr<ArrayOf<Boolean> > hasKeys;
    FAIL_RETURN(CInputManager::GetInstance()->DeviceHasKeys(
            *keyCodes, (ArrayOf<Boolean>**)&hasKeys));
    *hasKey = (*hasKeys)[0];

    return NOERROR;
}

ECode KeyCharacterMap::DeviceHasKeys(
    /* [in] */ ArrayOf<Int32>* keyCodes,
    /* [out, callee]] */ ArrayOf<Boolean>** hasKeys)
{
    VALIDATE_NOT_NULL(keyCodes);
    VALIDATE_NOT_NULL(hasKeys);
    return CInputManager::GetInstance()->DeviceHasKeys(*keyCodes, hasKeys);
}

ECode KeyCharacterMap::WriteToParcel(
    /* [in] */ IParcel* out)
{
    if (out == NULL) {
        Slogger::E(TAG, String("parcel must not be NULL"));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    NativeWriteToParcel(mPtr, out);

    return NOERROR;
}

ECode KeyCharacterMap::GetNative(
    /* [out] */ Handle64* native)
{
    VALIDATE_NOT_NULL(native);
    *native = (Handle64)mPtr;

    return NOERROR;
}

ECode KeyCharacterMap::SetNative(
    /* [in] */ Handle64 native)
{
    android::KeyCharacterMap* event = reinterpret_cast<android::KeyCharacterMap*>(mPtr);
    if (event) {
        event = NULL;
    }
    mPtr = native;

    return NOERROR;
}

ECode KeyCharacterMap::GetMap(
    /* [in] */ Int64* map)
{
    VALIDATE_NOT_NULL(map);

    NativeKeyCharacterMap* nMap = reinterpret_cast<NativeKeyCharacterMap*>(mPtr);
    *map = reinterpret_cast<Int64>(
            new NativeKeyCharacterMap(nMap->getDeviceId(), nMap->getMap()));
    return NOERROR;
}

Int64 KeyCharacterMap::NativeReadFromParcel(
    /* [in] */ IParcel* in)
{
    Handle32 data;
    in->GetElementPayload(&data);
    android::Parcel* parcel = reinterpret_cast<android::Parcel*>(data);
    if (!parcel) {
        return 0;
    }

    int32_t deviceId = parcel->readInt32();
    if (parcel->errorCheck()) {
        return 0;
    }

    android::sp<android::KeyCharacterMap> kcm =
            android::KeyCharacterMap::readFromParcel(parcel);
    if (!kcm.get()) {
        return 0;
    }

    NativeKeyCharacterMap* map = new NativeKeyCharacterMap(deviceId, kcm);
    return reinterpret_cast<Int64>(map);
}

void KeyCharacterMap::NativeWriteToParcel(
    /* [in] */ Int64 ptr,
    /* [in] */ IParcel* out)
{
    NativeKeyCharacterMap* map = reinterpret_cast<NativeKeyCharacterMap*>(ptr);

    Handle32 data;
    out->GetElementPayload(&data);
    android::Parcel* parcel = reinterpret_cast<android::Parcel*>(data);
    if (parcel) {
        parcel->writeInt32(map->getDeviceId());
        map->getMap()->writeToParcel(parcel);
    }
}

void KeyCharacterMap::NativeDispose(
    /* [in] */ Int64 ptr)
{
    NativeKeyCharacterMap* map = reinterpret_cast<NativeKeyCharacterMap*>(ptr);
    delete map;
}

Char32 KeyCharacterMap::NativeGetCharacter(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 metaState)
{
    NativeKeyCharacterMap* map = reinterpret_cast<NativeKeyCharacterMap*>(ptr);
    return map->getMap()->getCharacter(keyCode, metaState);
}

Boolean KeyCharacterMap::NativeGetFallbackAction(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 metaState,
    /* [in] */ FallbackAction* outFallbackAction)
{
    NativeKeyCharacterMap* map = reinterpret_cast<NativeKeyCharacterMap*>(ptr);
    android::KeyCharacterMap::FallbackAction fallbackAction;

    Boolean result = map->getMap()->getFallbackAction(keyCode, metaState, &fallbackAction);
    if (result) {
        outFallbackAction->mKeyCode = fallbackAction.keyCode;
        outFallbackAction->mMetaState = fallbackAction.metaState;
    }
    return result;
}

Char32 KeyCharacterMap::NativeGetNumber(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 keyCode)
{
    NativeKeyCharacterMap* map = reinterpret_cast<NativeKeyCharacterMap*>(ptr);
    return map->getMap()->getNumber(keyCode);
}

Char32 KeyCharacterMap::NativeGetMatch(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 keyCode,
    /* [in] */ const ArrayOf<Char32>& chars,
    /* [in] */ Int32 metaState)
{
    NativeKeyCharacterMap* map = reinterpret_cast<NativeKeyCharacterMap*>(ptr);
    Int32 numChars = chars.GetLength();
    AutoPtr<ArrayOf<char16_t> > array = ArrayOf<char16_t>::Alloc(numChars);
    for (Int32 i = 0; i < numChars; i++) {
        (*array)[i] = (char16_t)chars[i];
    }

    return map->getMap()->getMatch(keyCode, array->GetPayload(), size_t(numChars), metaState);
}

Char32 KeyCharacterMap::NativeGetDisplayLabel(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 keyCode)
{
    NativeKeyCharacterMap* map = reinterpret_cast<NativeKeyCharacterMap*>(ptr);
    return map->getMap()->getDisplayLabel(keyCode);
}

Int32 KeyCharacterMap::NativeGetKeyboardType(
    /* [in] */ Int64 ptr)
{
    NativeKeyCharacterMap* map = reinterpret_cast<NativeKeyCharacterMap*>(ptr);
    return map->getMap()->getKeyboardType();
}

AutoPtr<ArrayOf<IKeyEvent*> > KeyCharacterMap::NativeGetEvents(
    /* [in] */ Int64 ptr,
    /* [in] */ const ArrayOf<Char32>& chars)
{
    NativeKeyCharacterMap* map = reinterpret_cast<NativeKeyCharacterMap*>(ptr);
    Int32 numChars = chars.GetLength();
    AutoPtr<ArrayOf<char16_t> > array = ArrayOf<char16_t>::Alloc(numChars);
    for (Int32 i = 0; i < numChars; i++) {
        (*array)[i] = (char16_t)chars[i];
    }

    android::Vector<android::KeyEvent> events;
    AutoPtr<ArrayOf<IKeyEvent*> > result;
    if (map->getMap()->getEvents(map->getDeviceId(), array->GetPayload(), size_t(numChars), events)) {
        result = ArrayOf<IKeyEvent*>::Alloc(events.size());
        if (result != NULL) {
            for (size_t i = 0; i < events.size(); i++) {
                AutoPtr<IKeyEvent> keyEvent = CreateKeyEventFromNative(&events.itemAt(i));
                if (keyEvent == NULL)
                    break; // threw OOM exception
                result->Set(i, keyEvent);
            }
        }
    }

    return result;
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
