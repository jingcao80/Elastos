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

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include "OpenSSLX509CRLEntry.h"
#include "NativeCrypto.h"
#include "elastos/utility/Arrays.h"

using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::Math::CBigInteger;
using Elastos::Utility::Arrays;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendarHelper;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLX509CRLEntry::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLX509CRLEntry, Object, IOpenSSLX509CRLEntry)

ECode OpenSSLX509CRLEntry::constructor(
    /* [in] */ Int64 ctx)
{
    mContext = ctx;
    return NOERROR;
}

ECode OpenSSLX509CRLEntry::GetCriticalExtensionOIDs(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > critOids;
    NativeCrypto::Get_X509_REVOKED_ext_oids(mContext,
                    INativeCrypto::EXTENSION_TYPE_CRITICAL,
                    (ArrayOf<String>**)&critOids);

    /*
     * This API has a special case that if there are no extensions, we
     * should return null. So if we have no critical extensions, we'll check
     * non-critical extensions.
     */
    AutoPtr<ArrayOf<String> > crit_non_Oids;
    NativeCrypto::Get_X509_REVOKED_ext_oids(mContext,
                    INativeCrypto::EXTENSION_TYPE_NON_CRITICAL,
                    (ArrayOf<String>**)&crit_non_Oids);
    if ((critOids->GetLength() == 0)
            && crit_non_Oids->GetLength() == 0) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IHashSet> res;
    AutoPtr<IList> oidsList;
    Arrays::AsList(critOids, (IList**)&oidsList);
    CHashSet::New(ICollection::Probe(oidsList), (IHashSet**)&res);
    *result = ISet::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CRLEntry::GetExtensionValue(
    /* [in] */ String oid,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::X509_REVOKED_get_ext_oid(mContext, oid, result);
}

ECode OpenSSLX509CRLEntry::GetNonCriticalExtensionOIDs(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > critOids;
    NativeCrypto::Get_X509_REVOKED_ext_oids(mContext,
            INativeCrypto::EXTENSION_TYPE_NON_CRITICAL,
            (ArrayOf<String>**)&critOids);

    /*
     * This API has a special case that if there are no extensions, we
     * should return null. So if we have no non-critical extensions, we'll
     * check critical extensions.
     */
    AutoPtr<ArrayOf<String> > oids;
    NativeCrypto::Get_X509_REVOKED_ext_oids(mContext,
            INativeCrypto::EXTENSION_TYPE_CRITICAL,
            (ArrayOf<String>**)&oids);
    if ((critOids->GetLength() == 0)
            && (oids->GetLength() == 0)) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IList> l;
    Arrays::AsList(critOids, (IList**)&l);
    AutoPtr<IHashSet> res;
    CHashSet::New(ICollection::Probe(l), (IHashSet**)&res);
    *result = ISet::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CRLEntry::HasUnsupportedCriticalExtension(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > criticalOids;
    NativeCrypto::Get_X509_REVOKED_ext_oids(mContext,
                INativeCrypto::EXTENSION_TYPE_CRITICAL,
                (ArrayOf<String>**)&criticalOids);
    for (Int32 i = 0; i < criticalOids->GetLength(); i++) {
        String oid = (*criticalOids)[i];
        Int64 extensionRef = 0;
        NativeCrypto::X509_REVOKED_get_ext(mContext, oid, &extensionRef);
        Int32 extension = 0;
        NativeCrypto::X509_supported_extension(extensionRef, &extension);
        if (extension != 1) {
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode OpenSSLX509CRLEntry::GetEncoded(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::I2d_X509_REVOKED(mContext, result);
}

ECode OpenSSLX509CRLEntry::GetSerialNumber(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > sn;
    NativeCrypto::X509_REVOKED_get_serialNumber(mContext, (ArrayOf<Byte>**)&sn);
    AutoPtr<IBigInteger> res;
    CBigInteger::New(*sn, (IBigInteger**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CRLEntry::GetRevocationDate(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ITimeZoneHelper> tzhlp;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzhlp);
    AutoPtr<ITimeZone> tz;
    tzhlp->GetTimeZone(String("UTC"), (ITimeZone**)&tz);
    AutoPtr<ICalendarHelper> chlp;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&chlp);
    AutoPtr<ICalendar> calendar;
    chlp->GetInstance(tz, (ICalendar**)&calendar);
    calendar->Set(ICalendar::MILLISECOND, 0);
    Int64 revocationdata = 0;
    NativeCrypto::Get_X509_REVOKED_revocationDate(mContext, &revocationdata);
    NativeCrypto::ASN1_TIME_to_Calendar(revocationdata,
            calendar);
    return calendar->GetTime(result);
}

ECode OpenSSLX509CRLEntry::HasExtensions(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > non_critical;
    NativeCrypto::Get_X509_REVOKED_ext_oids(mContext,
            INativeCrypto::EXTENSION_TYPE_NON_CRITICAL,
            (ArrayOf<String>**)&non_critical);
    AutoPtr<ArrayOf<String> > critical;
    NativeCrypto::Get_X509_REVOKED_ext_oids(mContext,
            INativeCrypto::EXTENSION_TYPE_CRITICAL,
            (ArrayOf<String>**)&critical);
    *result = (non_critical->GetLength() != 0)
            || (critical->GetLength() != 0);
    return NOERROR;
}

ECode OpenSSLX509CRLEntry::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IByteArrayOutputStream> os;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&os);
    Int64 bioCtx = 0;
    NativeCrypto::Create_BIO_OutputStream(IOutputStream::Probe(os), &bioCtx);
    NativeCrypto::X509_REVOKED_print(bioCtx, mContext);
    NativeCrypto::BIO_free_all(bioCtx);
    return os->ToString(result);
}

} // namespace Conscrypt
} // namespace Org
