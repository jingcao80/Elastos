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

#include "elastos/droid/net/nsd/NsdServiceInfo.h"
#include "elastos/droid/net/nsd/DnsSdTxtRecord.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/utility/CArrayMap.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::CString;
using Elastos::Core::CArrayOf;
using Elastos::Core::CByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::Charset::CStandardCharsets;
using Elastos::IO::Charset::IStandardCharsets;
using Elastos::Net::EIID_IInetAddress;
using Elastos::Net::IInetAddress;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Nsd {

CAR_INTERFACE_IMPL_2(NsdServiceInfo, Object, IParcelable, INsdServiceInfo)

const String NsdServiceInfo::TAG("NsdServiceInfo");

NsdServiceInfo::NsdServiceInfo()
    : mPort(0)
{
    CArrayMap::New((IArrayMap**)&mTxtRecord);
}

ECode NsdServiceInfo::constructor()
{
    return NOERROR;
}

ECode NsdServiceInfo::constructor(
    /* [in] */ const String& sn,
    /* [in] */ const String& rt)
{
    mServiceName = sn;
    mServiceType = rt;
    return NOERROR;
}

ECode NsdServiceInfo::GetServiceName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mServiceName;
    return NOERROR;
}

ECode NsdServiceInfo::SetServiceName(
    /* [in] */ const String& s)
{
    mServiceName = s;
    return NOERROR;
}

ECode NsdServiceInfo::GetServiceType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mServiceType;
    return NOERROR;
}

ECode NsdServiceInfo::SetServiceType(
    /* [in] */ const String& s)
{
    mServiceType = s;
    return NOERROR;
}

ECode NsdServiceInfo::GetHost(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHost;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NsdServiceInfo::SetHost(
    /* [in] */ IInetAddress* s)
{
    mHost = s;
    return NOERROR;
}

ECode NsdServiceInfo::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);
    *port = mPort;
    return NOERROR;
}

ECode NsdServiceInfo::SetPort(
    /* [in] */ Int32 p)
{
    mPort = p;
    return NOERROR;
}

ECode NsdServiceInfo::SetAttribute(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* value)
{
    // Key must be printable US-ASCII, excluding =.
    for (Int32 i = 0; i < key.GetLength(); ++i) {
        Char32 character = key.GetChar(i);
        if (character < 0x20 || character > 0x7E) {
            Logger::E(TAG, "Key strings must be printable US-ASCII");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else if (character == 0x3D) {
            Logger::E(TAG, "Key strings must not include '='");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    // Key length + value length must be < 255.
    if (key.GetLength() + (value == NULL ? 0 : value->GetLength()) >= 255) {
        Logger::E(TAG, "Key length + value length must be < 255 bytes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Warn if key is > 9 characters, as recommended by RFC 6763 section 6.4.
    if (key.GetLength() > 9) {
        Logger::W(TAG, "Key lengths > 9 are discouraged: %s", key.string());
    }
    // Check against total TXT record size limits.
    // Arbitrary 400 / 1300 byte limits taken from RFC 6763 section 6.2.
    Int32 txtRecordSize;
    GetTxtRecordSize(&txtRecordSize);
    Int32 futureSize = txtRecordSize + key.GetLength() + (value == NULL ? 0 : value->GetLength()) + 2;
    if (futureSize > 1300) {
        Logger::E(TAG, "Total length of attributes must be < 1300 bytes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    } else if (futureSize > 400) {
            Logger::W(TAG, "Total length of all attributes exceeds 400 bytes; truncation may occur");
    }
    AutoPtr<ICharSequence> csq;
    CString::New(key, (ICharSequence**)&csq);
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IByte, value->GetLength(), (IArrayOf**)&array);
    for (Int32 i = 0; i < value->GetLength(); ++i) {
        AutoPtr<IByte> b;
        CByte::New((*value)[i], (IByte**)&b);
        array->Set(i, b);
    }
    mTxtRecord->Put(csq, array);
    return NOERROR;
}

ECode NsdServiceInfo::SetAttribute(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
        // try {
    ECode ec = SetAttribute(key, value.IsNull() ? ArrayOf<Byte>::Alloc(0) : value.GetBytes(/* "UTF-8" */).Get());
        // } catch (UnsupportedEncodingException e) {
    if (FAILED(ec)) {
        Logger::E(TAG, "Value must be UTF-8");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
        // }
    return NOERROR;
}

ECode NsdServiceInfo::RemoveAttribute(
    /* [in] */ const String& key)
{
    AutoPtr<ICharSequence> csq;
    CString::New(key, (ICharSequence**)&csq);
    mTxtRecord->Remove(IInterface::Probe(csq));
    return NOERROR;
}

ECode NsdServiceInfo::GetAttributes(
    /* [out, callee] */ IMap** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->UnmodifiableMap(IMap::Probe(mTxtRecord), result);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode NsdServiceInfo::GetTxtRecordSize(
    /* [out] */ Int32* result)
{
    Int32 txtRecordSize = 0;
    AutoPtr<ISet> entrySet;
    mTxtRecord->GetEntrySet((ISet**)&entrySet);
    AutoPtr<ArrayOf<IInterface*> > array;
    entrySet->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        AutoPtr<IMapEntry> entry;
        entry = IMapEntry::Probe((*array)[i]);
        txtRecordSize += 2;  // One for the length byte, one for the = between key and value.
        AutoPtr<IInterface> key;
        entry->GetKey((IInterface**)&key);
        Int32 length;
        ICharSequence::Probe(key)->GetLength(&length);
        txtRecordSize += length;
        AutoPtr<IInterface> obj;
        entry->GetValue((IInterface**)&obj);
        AutoPtr<IArrayOf> value = IArrayOf::Probe(obj);
        length = 0;
        if (value != NULL)
            value->GetLength(&length);
        txtRecordSize += length;
    }
    *result = txtRecordSize;
    return NOERROR;
}

ECode NsdServiceInfo::GetTxtRecord(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    Int32 txtRecordSize;
    GetTxtRecordSize(&txtRecordSize);
    if (txtRecordSize == 0) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > txtRecord = ArrayOf<Byte>::Alloc(txtRecordSize);
    Int32 ptr = 0;
    AutoPtr<ISet> entrySet;
    mTxtRecord->GetEntrySet((ISet**)&entrySet);
    AutoPtr<ArrayOf<IInterface*> > array;
    entrySet->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        AutoPtr<IMapEntry> entry;
        entry = IMapEntry::Probe((*array)[i]);
        AutoPtr<IInterface> iKey;
        entry->GetKey((IInterface**)&iKey);
        String key;
        ICharSequence::Probe(iKey)->ToString(&key);
        AutoPtr<IInterface> iValue;
        entry->GetValue((IInterface**)&iValue);
        AutoPtr<IArrayOf> value = IArrayOf::Probe(iValue);
        // One byte to record the length of this key/value pair.
        Int32 length = 0;
        if (value != NULL) {
            value->GetLength(&length);
        }
        (*txtRecord)[ptr++] = (Byte) (key.GetLength() + length + 1);
        // The key, in US-ASCII.
        // Note: use the StandardCharsets const here because it doesn't raise exceptions and we
        // already know the key is ASCII at this point.
        // System.arraycopy(key.getBytes(StandardCharsets.US_ASCII), 0, txtRecord, ptr, key.length());
        txtRecord->Copy(ptr, key.GetBytes(), 0, key.GetLength());
        ptr += key.GetLength();
        // US-ASCII '=' character.
        (*txtRecord)[ptr++] = (Byte)'=';
        // The value, as any raw bytes.
        if (value != NULL) {
                // System.arraycopy(value, 0, txtRecord, ptr, value.length);
            Int32 length;
            value->GetLength(&length);
            AutoPtr<ArrayOf<Byte> > aValue = ArrayOf<Byte>::Alloc(length);
            for (Int32 i = 0; i < length; ++i) {
                AutoPtr<IInterface> obj;
                value->Get(i, (IInterface**)&obj);
                Byte bv;
                IByte::Probe(obj)->GetValue(&bv);
                (*aValue)[i] = bv;
            }
            txtRecord->Copy(ptr, aValue, 0, length);
            ptr += length;
        }
    }
    *result = txtRecord;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode NsdServiceInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb;

    sb += "name: ";
    sb += mServiceName;
    sb += ", type: ";
    sb += mServiceType;

    sb += ", host: ";
    String temp;
    IObject::Probe(mHost)->ToString(&temp);
    sb += temp;

    sb += ", port: ";
    sb += StringUtils::ToString(mPort);
    sb += "txtRecord: ";
    IObject::Probe(mTxtRecord)->ToString(&temp);
    sb += temp;

    AutoPtr<ArrayOf<Byte> > txtRecord;
    GetTxtRecord((ArrayOf<Byte>**)&txtRecord);
    if (txtRecord != NULL) {
        sb += ", txtRecord: ";
        String s(*txtRecord/*, IStandardCharsets::UTF_8*/);
        sb += s;
    }

    *result = sb.ToString();
    return NOERROR;
}

ECode NsdServiceInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mServiceName);
    dest->WriteString(mServiceType);
    dest->WriteInterfacePtr(mTxtRecord.Get());
    dest->WriteInterfacePtr(mHost.Get());
    dest->WriteInt32(mPort);

    return NOERROR;
}

ECode NsdServiceInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadString(&mServiceName);
    source->ReadString(&mServiceType);
    source->ReadInterfacePtr((HANDLE*)&obj);
    mTxtRecord = IArrayMap::Probe(obj);

    obj = NULL;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mHost = IInetAddress::Probe(obj);

    source->ReadInt32(&mPort);

    return NOERROR;
}

} // namespace Nsd
} // namespace Net
} // namespace Droid
} // namespace Elastos
