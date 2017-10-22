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

#include "_Elastos.Droid.Core.h"
#include "_Elastos.Droid.Text.h"
#include "_Elastos.Droid.View.h"
#include "_Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Text.h>
#include "elastos/droid/R.h"
#include "elastos/droid/net/http/SslCertificate.h"
#include "elastos/droid/net/http/CSslCertificate.h"
#include "elastos/droid/net/http/CSslCertificateDName.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/text/format/DateFormat.h"
#include "elastos/droid/view/LayoutInflater.h"
#include <elastos/core/IntegralToString.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::R;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::ITextView;

using Elastos::Core::IByte;
using Elastos::Core::IntegralToString;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IInputStream;
using Elastos::Math::IBigInteger;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::IPrincipal;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::IVector;

// using Org::Bouncycastle::Asn1::X509::CX509Name;
using Org::Bouncycastle::Asn1::X509::IX509Name;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

//===============================================================
// SslCertificateDName
//===============================================================
CAR_INTERFACE_IMPL(SslCertificateDName, Object, ISslCertificateDName)

ECode SslCertificateDName::constructor(
    /* [in] */ const String& dName)
{
    if (dName != NULL) {
        mDName = dName;

        AutoPtr<IX509Name> x509Name;
        // TODO: Waiting for X509Name
        assert(0);
        // CX509Name::New(dName, (IX509Name**)&x509Name);

        AutoPtr<IVector> val;
        // x509Name->GetValues((IVector**)&val);
        AutoPtr<IVector> oid;
        // x509Name->GetOIDs(&oid);

        Int32 size;
        oid->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
        //     if (oid->At(i)->Equals(IX509Name::CN)) {
        //         if (mCName.IsNull()) {
        //             mCName = (String) val->At(i);
        //         }
        //         continue;
        //     }

        //     if (oid->At(i).Equals(IX509Name::O)) {
        //         if (mOName.IsNull()) {
        //             mOName = (String) val->At(i);
        //         }
        //         continue;
        //     }

        //     if (oid->At(i).Equals(IX509Name::OU)) {
        //         if (mUName.IsNull()) {
        //             mUName = (String) val->At(i);
        //         }
        //         continue;
        //     }
        }
    }
    return NOERROR;
}

ECode SslCertificateDName::GetDName(
    /* [out] */ String* dName)
{
    VALIDATE_NOT_NULL(dName);

    if (mDName.IsNullOrEmpty()) {
        *dName = String("");
    } else {
        *dName = mDName;
    }
    return NOERROR;
}

ECode SslCertificateDName::GetCName(
    /* [out] */ String* cName)
{
    VALIDATE_NOT_NULL(cName);

    if (mCName.IsNullOrEmpty()) {
        *cName = String("");
    } else {
        *cName = mCName;
    }
    return NOERROR;
}

ECode SslCertificateDName::GetOName(
    /* [out] */ String* oName)
{
    VALIDATE_NOT_NULL(oName);

    if (mOName.IsNullOrEmpty()) {
        *oName = String("");
    } else {
        *oName = mOName;
    }
    return NOERROR;
}

ECode SslCertificateDName::GetUName(
    /* [out] */ String* uName)
{
    VALIDATE_NOT_NULL(uName);

    if (mUName.IsNullOrEmpty()) {
        *uName = String("");
    } else {
        *uName = mUName;
    }
    return NOERROR;
}

//=================================================================
// SslCertificate
//=================================================================
CAR_INTERFACE_IMPL(SslCertificate, Object, ISslCertificate)

String SslCertificate::sISO_8601_DATE_FORMAT("yyyy-MM-dd HH:mm:ssZ");
const String SslCertificate::ISSUED_TO("issued-to");
const String SslCertificate::ISSUED_BY("issued-by");
const String SslCertificate::VALID_NOT_BEFORE("valid-not-before");
const String SslCertificate::VALID_NOT_AFTER("valid-not-after");
const String SslCertificate::X509_CERTIFICATE("x509-certificate");

SslCertificate::SslCertificate()
{}

ECode SslCertificate::SaveState(
    /* [in] */ ISslCertificate* certificate,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)

    if (certificate == NULL) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IBundle> localbundle;
    CBundle::New((IBundle**)&localbundle);
    AutoPtr<ISslCertificateDName> dNameTo;
    certificate->GetIssuedTo((ISslCertificateDName**)&dNameTo);
    String strTo;
    dNameTo->GetDName(&strTo);
    localbundle->PutString(CSslCertificate::ISSUED_TO, strTo);
    AutoPtr<ISslCertificateDName> dNameBy;
    certificate->GetIssuedTo((ISslCertificateDName**)&dNameBy);
    String strBy;
    dNameBy->GetDName(&strBy);
    localbundle->PutString(CSslCertificate::ISSUED_BY, strBy);
    String strNotBefore;
    certificate->GetValidNotBefore(&strNotBefore);
    localbundle->PutString(CSslCertificate::VALID_NOT_BEFORE, strNotBefore);
    String strNotAfter;
    certificate->GetValidNotAfter(&strNotAfter);
    localbundle->PutString(CSslCertificate::VALID_NOT_AFTER, strNotAfter);
    AutoPtr<IX509Certificate> x509Certificate = ((CSslCertificate*)certificate)->mX509Certificate;
    if (x509Certificate != NULL) {
        AutoPtr<ArrayOf<Byte> > bytesEncoded;
        ICertificate::Probe(x509Certificate)->GetEncoded((ArrayOf<Byte>**)&bytesEncoded);
        localbundle->PutByteArray(CSslCertificate::X509_CERTIFICATE, bytesEncoded);

    }
    *result = localbundle;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SslCertificate::RestoreState(
    /* [in] */ IBundle* bundle,
    /* [out] */ ISslCertificate** result)
{
    VALIDATE_NOT_NULL(result);

    if (bundle == NULL) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IX509Certificate> x509Certificate;
    AutoPtr<ArrayOf<Byte> > bytes;
    bundle->GetByteArray(CSslCertificate::X509_CERTIFICATE, (ArrayOf<Byte>**)&bytes);
    if (bytes != NULL) {
        AutoPtr<ICertificateFactoryHelper> helper;
        CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
        AutoPtr<ICertificateFactory> certFactory;
        helper->GetInstance(String("X.509"), (ICertificateFactory**)&certFactory);
        AutoPtr<ICertificate> cert;
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(bytes, (IByteArrayInputStream**)&stream);
        if (!FAILED(certFactory->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&cert))) {
            x509Certificate = IX509Certificate::Probe(cert);
        }
    }

    String issuedTo;
    bundle->GetString(CSslCertificate::ISSUED_TO, &issuedTo);
    String issuedBy;
    bundle->GetString(CSslCertificate::ISSUED_BY, &issuedBy);
    String strNotBefore;
    bundle->GetString(CSslCertificate::VALID_NOT_BEFORE, &issuedBy);
    AutoPtr<IDate> dateNOtBefore = CSslCertificate::ParseDate(strNotBefore);
    String strNotAfter;
    bundle->GetString(CSslCertificate::VALID_NOT_AFTER, &strNotAfter);
    AutoPtr<IDate> dateNOtAfter = CSslCertificate::ParseDate(strNotBefore);

    CSslCertificate* cert = new CSslCertificate();
    cert->constructor(issuedTo, issuedBy, dateNOtBefore, dateNOtAfter, x509Certificate);
    *result = ISslCertificate::Probe(cert);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SslCertificate::constructor(
    /* [in] */ const String& issuedTo,
    /* [in] */ const String& issuedBy,
    /* [in] */ const String& validNotBefore,
    /* [in] */ const String& validNotAfter)
{
    AutoPtr<IDate> notBefore = ParseDate(validNotBefore);
    AutoPtr<IDate> notAfter = ParseDate(validNotAfter);

    return constructor(issuedTo, issuedBy, notBefore, notAfter, NULL);
}

ECode SslCertificate::constructor(
    /* [in] */ const String& issuedTo,
    /* [in] */ const String& issuedBy,
    /* [in] */ IDate* validNotBefore,
    /* [in] */ IDate* validNotAfter)
{
    return constructor(issuedTo, issuedBy, validNotBefore, validNotAfter, NULL);
}

ECode SslCertificate::constructor(
    /* [in] */ IX509Certificate* certificate)
{
    AutoPtr<IPrincipal> subDN;
    certificate->GetSubjectDN((IPrincipal**)&subDN);
    String issuedTo;
    subDN->GetName(&issuedTo);
    AutoPtr<IPrincipal> issbDN;
    certificate->GetSubjectDN((IPrincipal**)&issbDN);
    String issuedBy;
    issbDN->GetName(&issuedBy);
    AutoPtr<IDate> notBefore;
    certificate->GetNotBefore((IDate**)&notBefore);
    AutoPtr<IDate> notAfter;
    certificate->GetNotAfter((IDate**)&notAfter);

    return constructor(issuedTo, issuedBy, notBefore, notAfter, certificate);
}

ECode SslCertificate::constructor(
    /* [in] */ const String& issuedTo,
    /* [in] */ const String& issuedBy,
    /* [in] */ IDate* validNotBefore,
    /* [in] */ IDate* validNotAfter,
    /* [in] */ IX509Certificate* x509Certificate)
{
    CSslCertificateDName::New(issuedTo, (ISslCertificateDName**)&mIssuedTo);
    CSslCertificateDName::New(issuedBy, (ISslCertificateDName**)&mIssuedBy);
    mValidNotBefore = CloneDate(validNotBefore);
    mValidNotAfter  = CloneDate(validNotAfter);
    mX509Certificate = x509Certificate;

    return NOERROR;
}

ECode SslCertificate::GetValidNotBeforeDate(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IDate> d = CloneDate(mValidNotBefore);
    *result = d;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SslCertificate::GetValidNotBefore(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FormatDate(mValidNotBefore);
    return NOERROR;
}

ECode SslCertificate::GetValidNotAfterDate(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IDate> d = CloneDate(mValidNotAfter);
    *result = d;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SslCertificate::GetValidNotAfter(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FormatDate(mValidNotAfter);
    return NOERROR;
}

ECode SslCertificate::GetIssuedTo(
    /* [out] */ ISslCertificateDName** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIssuedTo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SslCertificate::GetIssuedBy(
    /* [out] */ ISslCertificateDName** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIssuedBy;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

String SslCertificate::GetSerialNumber(
    /* [in] */ IX509Certificate* x509Certificate)
{
    if (x509Certificate == NULL) {
        return String("");
    }
    AutoPtr<IBigInteger> serialNumber;
    x509Certificate->GetSerialNumber((IBigInteger**)&serialNumber);
    if (serialNumber == NULL) {
        return String("");
    }
    AutoPtr<ArrayOf<Byte> > serArray;
    serialNumber->ToByteArray((ArrayOf<Byte>**)&serArray);
    return Fingerprint(serArray);
}

String SslCertificate::GetDigest(
    /* [in] */ IX509Certificate* x509Certificate,
    /* [in] */ const String& algorithm)
{
    if (x509Certificate == NULL) {
        return String("");
    }

    AutoPtr<ArrayOf<Byte> >bytes;
    ICertificate::Probe(x509Certificate)->GetEncoded((ArrayOf<Byte> **)&bytes);
    AutoPtr<IMessageDigestHelper> helper;
    AutoPtr<IMessageDigest> md;
    if(FAILED(helper->GetInstance(algorithm, (IMessageDigest**)&md))) {
        return String("");
    }
    AutoPtr<ArrayOf<Byte> >digest;
    if(FAILED(md->Digest(bytes, (ArrayOf<Byte>**)&digest))) {
        return String("");
    }

    return Fingerprint(digest);
}

String SslCertificate::Fingerprint(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    if (bytes == NULL) {
        return String("");
    }
    AutoPtr<StringBuilder> sb = new StringBuilder();
    for (Int32 i = 0; i < bytes->GetLength(); i++) {
        Byte b = (*bytes)[i];
        IntegralToString::AppendByteAsHex(*sb, b, TRUE);
        if (i + 1 != bytes->GetLength()) {
            sb->AppendChar(':');
        }
    }
    return sb->ToString();
}

ECode SslCertificate::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String issuedTo;
    mIssuedTo->GetDName(&issuedTo);
    String issuedBy;
    mIssuedBy->GetDName(&issuedBy);

    *result = String("Issued to: ") + issuedTo + String(";\n")
        + String("Issued by: ") + issuedBy + String(";\n");
    return NOERROR;
}

AutoPtr<IDate> SslCertificate::ParseDate(
    /* [in] */ const String& string)
{
    AutoPtr<ISimpleDateFormat> format;
    CSimpleDateFormat::New(sISO_8601_DATE_FORMAT, (ISimpleDateFormat**)&format);
    AutoPtr<IDate> date;
    if(!FAILED(Elastos::Text::IDateFormat::Probe(format)->Parse(string, (IDate**)&date))) {
        return date;
    }

    return NULL;
}

String SslCertificate::FormatDate(
    /* [in] */ IDate* date)
{
    AutoPtr<ISimpleDateFormat> format;
    CSimpleDateFormat::New(sISO_8601_DATE_FORMAT, (ISimpleDateFormat**)&format);
    String strDate;
    Elastos::Text::IDateFormat::Probe(format)->Format(date, &strDate);

    return strDate;
}

AutoPtr<IDate> SslCertificate::CloneDate(
    /* [in] */ IDate* date)
{
    if (date == NULL) {
        return NULL;
    }
    AutoPtr<IInterface> newDate;
    Elastos::Core::ICloneable::Probe(date)->Clone((IInterface**)&newDate);

    return IDate::Probe(newDate);
}

ECode SslCertificate::InflateCertificateView(
    /* [in] */ IContext* context,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ILayoutInflater> factory;
    LayoutInflater::From(context, (ILayoutInflater**)&factory);

    AutoPtr<IView> certificateView;
    factory->Inflate(R::layout::ssl_certificate, NULL, (IView**)&certificateView);

    // issued to:
    AutoPtr<ISslCertificateDName> issuedTo;
    GetIssuedTo((ISslCertificateDName**)&issuedTo);
    AutoPtr<IView> view;
    if (issuedTo != NULL) {
        certificateView->FindViewById(R::id::to_common, (IView**)&view);
        AutoPtr<ITextView> commonView = ITextView::Probe(view);
        String sCName;
        issuedTo->GetCName(&sCName);
        commonView->SetText(StringUtils::ParseCharSequence(sCName));

        view = NULL;
        certificateView->FindViewById(R::id::to_org, (IView**)&view);
        AutoPtr<ITextView> orgView = ITextView::Probe(view);
        String sOName;
        issuedTo->GetCName(&sOName);
        orgView->SetText(StringUtils::ParseCharSequence(sOName));

        view = NULL;
        certificateView->FindViewById(R::id::to_org_unit, (IView**)&view);
        AutoPtr<ITextView> unitView = ITextView::Probe(view);
        String sUName;
        issuedTo->GetUName(&sUName);
        unitView->SetText(StringUtils::ParseCharSequence(sUName));
    }
    // serial number:
    view = NULL;
    certificateView->FindViewById(R::id::serial_number, (IView**)&view);
    AutoPtr<ITextView> serView = ITextView::Probe(view);
    String sSerNum = GetSerialNumber(mX509Certificate);
    serView->SetText(StringUtils::ParseCharSequence(sSerNum));

    // issued by:
    AutoPtr<ISslCertificateDName> issuedBy;
    GetIssuedBy((ISslCertificateDName**)&issuedBy);
    if (issuedBy != NULL) {
        view = NULL;
        certificateView->FindViewById(R::id::by_common, (IView**)&view);
        AutoPtr<ITextView> commonView = ITextView::Probe(view);
        String sCName;
        issuedBy->GetCName(&sCName);
        commonView->SetText(StringUtils::ParseCharSequence(sCName));

        view = NULL;
        certificateView->FindViewById(R::id::by_org, (IView**)&view);
        AutoPtr<ITextView> orgView = ITextView::Probe(view);
        String sOName;
        issuedBy->GetCName(&sOName);
        orgView->SetText(StringUtils::ParseCharSequence(sOName));

        view = NULL;
        certificateView->FindViewById(R::id::by_org_unit, (IView**)&view);
        AutoPtr<ITextView> unitView = ITextView::Probe(view);
        String sUName;
        issuedBy->GetUName(&sUName);
        unitView->SetText(StringUtils::ParseCharSequence(sUName));
    }

    // issued on:
    AutoPtr<IDate> beforeDate;
    GetValidNotBeforeDate((IDate**)&beforeDate);
    String issuedOn = FormatCertificateDate(context, beforeDate);
    view = NULL;
    certificateView->FindViewById(R::id::issued_on, (IView**)&view);
    AutoPtr<ITextView> isOnView = ITextView::Probe(view);
    isOnView->SetText(StringUtils::ParseCharSequence(issuedOn));

    // expires on:
    AutoPtr<IDate> afterDate;
    GetValidNotAfterDate((IDate**)&afterDate);
    String expiresOn = FormatCertificateDate(context, afterDate);
    view = NULL;
    certificateView->FindViewById(R::id::issued_on, (IView**)&view);
    AutoPtr<ITextView> expOnView = ITextView::Probe(view);
    expOnView->SetText(StringUtils::ParseCharSequence(expiresOn));

    // fingerprints:
    view = NULL;
    certificateView->FindViewById(R::id::sha256_fingerprint, (IView**)&view);
    AutoPtr<ITextView> sha256View = ITextView::Probe(view);
    String sSha256Digest = GetDigest(mX509Certificate, String("SHA256"));
    sha256View->SetText(StringUtils::ParseCharSequence(sSha256Digest));

    view = NULL;
    certificateView->FindViewById(R::id::sha1_fingerprint, (IView**)&view);
    AutoPtr<ITextView> sha1View = ITextView::Probe(view);
    String sSha1Digest = GetDigest(mX509Certificate, String("SHA1"));
    sha1View->SetText(StringUtils::ParseCharSequence(sSha1Digest));

    *result = IView::Probe(certificateView);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

String SslCertificate::FormatCertificateDate(
    /* [in] */ IContext* context,
    /* [in] */ IDate* certificateDate)
{
    if (certificateDate == NULL) {
        return String("");
    }
    String sdate;
    DateFormat::GetDateFormat(context)->Format(certificateDate, &sdate);

    return sdate;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
