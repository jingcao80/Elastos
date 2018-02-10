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

#include "elastos/droid/media/CMetadata.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::CCollections;
using Elastos::Utility::CDate;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::ICollections;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_OBJECT_IMPL(CMetadata)

CAR_INTERFACE_IMPL(CMetadata, Object, IMetadata)

const String CMetadata::TAG("media.Metadata");

const Int32 CMetadata::LAST_SYSTEM = 31;
const Int32 CMetadata::FIRST_CUSTOM = 8192;

// FIXME: misses a type for shared heap is missing (MemoryFile).
// FIXME: misses a type for bitmaps.
const Int32 CMetadata::LAST_TYPE = 7;

const Int32 CMetadata::kInt32Size = 4;
const Int32 CMetadata::kMetaHeaderSize = 2 * kInt32Size; //  size + marker
const Int32 CMetadata::kRecordHeaderSize = 3 * kInt32Size; // size + id + type
const Int32 CMetadata::kMetaMarker = 0x4d455441;  // 'M' 'E' 'T' 'A'

static AutoPtr<ISet> InitMATCH_NONE()
{
    AutoPtr<ISet> none;
    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);
    c->GetEmptySet((ISet**)&none);
    return none;
}

static AutoPtr<ISet> InitMATCH_ALL()
{
    AutoPtr<ISet> all;
    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);
    c->Singleton(CoreUtils::Convert(IMetadata::ANY).Get(), (ISet**)&all);
    return all;
}

AutoPtr<ISet> CMetadata::MATCH_NONE = InitMATCH_NONE();
AutoPtr<ISet> CMetadata::MATCH_ALL = InitMATCH_ALL();

CMetadata::CMetadata()
{
}

ECode CMetadata::constructor()
{
    return NOERROR;
}

ECode CMetadata::Parse(
    /* [in] */ IParcel* parcel,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 avail;
    parcel->GetDataSize(&avail);
    if (avail < kMetaHeaderSize) {
        Slogger::E(TAG, "Not enough data %s", StringUtils::ToString(avail).string());
        return NOERROR;
    }

    Int32 pin;
    parcel->GetDataPosition(&pin);  // to roll back in case of errors.
    Int32 size;
    parcel->ReadInt32(&size);

    // The extra kInt32Size below is to account for the int32 'size' just read.
    if (avail + kInt32Size < size || size < kMetaHeaderSize) {
        Slogger::E(TAG, "Bad size %d, avail %d, position %d, ", size, avail, pin);
        parcel->SetDataPosition(pin);
        *result = FALSE;
        return NOERROR;
    }

    // Checks if the 'M' 'E' 'T' 'A' marker is present.
    Int32 kShouldBeMetaMarker;
    parcel->ReadInt32(&kShouldBeMetaMarker);
    if (kShouldBeMetaMarker != kMetaMarker ) {
        Slogger::E(TAG, "Marker missing %08x", kShouldBeMetaMarker);
        parcel->SetDataPosition(pin);
        *result = FALSE;
        return NOERROR;
    }

    // Scan the records to collect metadata ids and offsets.
    if (!ScanAllRecords(parcel, size - kMetaHeaderSize)) {
        parcel->SetDataPosition(pin);
        *result = FALSE;
        return NOERROR;
    }
    mParcel = parcel;
    *result = TRUE;
    return NOERROR;
}

ECode CMetadata::KeySet(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result);
    return mKeyToPosMap->GetKeySet(result);
}

ECode CMetadata::Has(
    /* [in] */ Int32 metadataId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (!CheckMetadataId(metadataId)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return mKeyToPosMap->ContainsKey(CoreUtils::Convert(metadataId).Get(), result);
}

ECode CMetadata::GetString(
    /* [in] */ Int32 key,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckType(key, STRING_VAL));
    mParcel->ReadString(result);
    return NOERROR;
}

ECode CMetadata::GetInt32(
    /* [in] */ Int32 key,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckType(key, INTEGER_VAL));
    mParcel->ReadInt32(result);
    return NOERROR;
}

ECode CMetadata::GetBoolean(
    /* [in] */ Int32 key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(CheckType(key, BOOLEAN_VAL));
    Int32 tempValue;
    mParcel->ReadInt32(&tempValue);
    if (tempValue == 1) {
        *result = TRUE;
    }
    return NOERROR;
}

ECode CMetadata::GetInt64(
    /* [in] */ Int32 key,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckType(key, LONG_VAL));
    mParcel->ReadInt64(result);
    return NOERROR;
}

ECode CMetadata::GetDouble(
    /* [in] */ Int32 key,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckType(key, DOUBLE_VAL));
    mParcel->ReadDouble(result);
    return NOERROR;
}

ECode CMetadata::GetByteArray(
    /* [in] */ Int32 key,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckType(key, BYTE_ARRAY_VAL));

    AutoPtr<ArrayOf<Byte> > array;
    mParcel->ReadArrayOf((HANDLE*)(&array));
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMetadata::GetDate(
    /* [in] */ Int32 key,
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckType(key, DATE_VAL));
    Int64 timeSinceEpoch;
    mParcel->ReadInt64(&timeSinceEpoch);
    String timeZone;
    mParcel->ReadString(&timeZone);

    if (timeZone.IsNullOrEmpty()) {
        AutoPtr<IDate> date;
        CDate::New(timeSinceEpoch, (IDate**)&date);
        *result = date;
    }
    else {
        AutoPtr<ITimeZoneHelper> timeZoneHelper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&timeZoneHelper);
        AutoPtr<ITimeZone> tz;
        timeZoneHelper->GetTimeZone(timeZone, (ITimeZone**)&tz);

        AutoPtr<ICalendarHelper> calendarHelper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calendarHelper);
        AutoPtr<ICalendar> cal;
        calendarHelper->GetInstance((ICalendar**)&cal);

        cal->SetTimeInMillis(timeSinceEpoch);
        cal->GetTime(result);
    }
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMetadata::RecycleParcel()
{
    if (mParcel != NULL) {
        assert(0);
        // TODO: mParcel.recycle();
    }
    return NOERROR;
}

/*static*/
ECode CMetadata::LastSytemId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = LAST_SYSTEM;
    return NOERROR;
}

/*static*/
ECode CMetadata::FirstCustomId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FIRST_CUSTOM;
    return NOERROR;
}

/*static*/
ECode CMetadata::LastType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = LAST_TYPE;
    return NOERROR;
}

Boolean CMetadata::ScanAllRecords(
    /* [in] */ IParcel* parcel,
    /* [in] */ Int32 bytesLeft)
{
    Int32 recCount = 0;
    Boolean error = FALSE;

    mKeyToPosMap->Clear();
    while (bytesLeft > kRecordHeaderSize) {
        Int32 start;
        parcel->GetDataPosition(&start);
        // Check the size.
        Int32 size;
        parcel->ReadInt32(&size);

        if (size <= kRecordHeaderSize) {  // at least 1 byte should be present.
            Slogger::E(TAG, "Record is too short");
            error = TRUE;
            break;
        }

        // Check the metadata key.
        Int32 metadataId;
        parcel->ReadInt32(&metadataId);
        if (!CheckMetadataId(metadataId)) {
            error = TRUE;
            break;
        }

        // Store the record offset which points to the type
        // field so we can later on read/unmarshall the record
        // payload.
        Boolean flag = FALSE;
        mKeyToPosMap->ContainsKey(CoreUtils::Convert(metadataId).Get(), &flag);
        if (flag) {
            Slogger::E(TAG, "Duplicate metadata ID found");
            error = TRUE;
            break;
        }

        Int32 tempValue;
        parcel->GetDataPosition(&tempValue);
        mKeyToPosMap->Put(CoreUtils::Convert(metadataId).Get(), CoreUtils::Convert(tempValue).Get());

        // Check the metadata type.
        Int32 metadataType;
        parcel->ReadInt32(&metadataType);
        if (metadataType <= 0 || metadataType > LAST_TYPE) {
            Slogger::E(TAG, "Invalid metadata type %d", metadataType);
            error = TRUE;
            break;
        }

        // Skip to the next one.
        parcel->SetDataPosition(start + size);
        bytesLeft -= size;
        ++recCount;
    }

    if (0 != bytesLeft || error) {
        Slogger::E(TAG, "Ran out of data or error on record %d", recCount);
        mKeyToPosMap->Clear();
        return FALSE;
    } else {
        return TRUE;
    }
}

Boolean CMetadata::CheckMetadataId(
    /* [in] */ Int32 val)
{
    if (val <= ANY || (LAST_SYSTEM < val && val < FIRST_CUSTOM)) {
        Slogger::E(TAG, "Invalid metadata ID %d", val);
        return FALSE;
    }
    return TRUE;
}

ECode CMetadata::CheckType(
    /* [in] */ Int32 key,
    /* [in] */ Int32 expectedType)
{
    AutoPtr<IInterface> obj;
    mKeyToPosMap->Get(CoreUtils::Convert(key).Get(), (IInterface**)&obj);
    AutoPtr<IInteger32> i = IInteger32::Probe(obj);
    Int32 pos;
    i->GetValue(&pos);

    mParcel->SetDataPosition(pos);

    Int32 type;
    mParcel->ReadInt32(&type);
    if (type != expectedType) {
        Slogger::E(TAG, "Wrong type %d but got %d", expectedType, type);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
