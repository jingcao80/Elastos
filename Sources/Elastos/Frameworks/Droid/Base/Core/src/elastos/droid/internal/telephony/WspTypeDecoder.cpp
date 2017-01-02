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

#include "elastos/droid/internal/telephony/WspTypeDecoder.h"

#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  WspTypeDecoder::
//==============================================================
const Int32 WspTypeDecoder::WAP_PDU_SHORT_LENGTH_MAX = 30;
const Int32 WspTypeDecoder::WAP_PDU_LENGTH_QUOTE = 31;

static AutoPtr<IHashMap> InitTYPES()
{
    AutoPtr<IHashMap> hm;
    CHashMap::New((IHashMap**)&hm);
    hm->Put(CoreUtils::Convert(0x00), CoreUtils::Convert("*/*"));
    hm->Put(CoreUtils::Convert(0x01), CoreUtils::Convert("text/*"));
    hm->Put(CoreUtils::Convert(0x02), CoreUtils::Convert("text/html"));
    hm->Put(CoreUtils::Convert(0x03), CoreUtils::Convert("text/plain"));
    hm->Put(CoreUtils::Convert(0x04), CoreUtils::Convert("text/x-hdml"));
    hm->Put(CoreUtils::Convert(0x05), CoreUtils::Convert("text/x-ttml"));
    hm->Put(CoreUtils::Convert(0x06), CoreUtils::Convert("text/x-vCalendar"));
    hm->Put(CoreUtils::Convert(0x07), CoreUtils::Convert("text/x-vCard"));
    hm->Put(CoreUtils::Convert(0x08), CoreUtils::Convert("text/vnd.wap.wml"));
    hm->Put(CoreUtils::Convert(0x09), CoreUtils::Convert("text/vnd.wap.wmlscript"));
    hm->Put(CoreUtils::Convert(0x0A), CoreUtils::Convert("text/vnd.wap.wta-event"));
    hm->Put(CoreUtils::Convert(0x0B), CoreUtils::Convert("multipart/*"));
    hm->Put(CoreUtils::Convert(0x0C), CoreUtils::Convert("multipart/mixed"));
    hm->Put(CoreUtils::Convert(0x0D), CoreUtils::Convert("multipart/form-data"));
    hm->Put(CoreUtils::Convert(0x0E), CoreUtils::Convert("multipart/byterantes"));
    hm->Put(CoreUtils::Convert(0x0F), CoreUtils::Convert("multipart/alternative"));
    hm->Put(CoreUtils::Convert(0x10), CoreUtils::Convert("application/*"));
    hm->Put(CoreUtils::Convert(0x11), CoreUtils::Convert("application/java-vm"));
    hm->Put(CoreUtils::Convert(0x12), CoreUtils::Convert("application/x-www-form-urlencoded"));
    hm->Put(CoreUtils::Convert(0x13), CoreUtils::Convert("application/x-hdmlc"));
    hm->Put(CoreUtils::Convert(0x14), CoreUtils::Convert("application/vnd.wap.wmlc"));
    hm->Put(CoreUtils::Convert(0x15), CoreUtils::Convert("application/vnd.wap.wmlscriptc"));
    hm->Put(CoreUtils::Convert(0x16), CoreUtils::Convert("application/vnd.wap.wta-eventc"));
    hm->Put(CoreUtils::Convert(0x17), CoreUtils::Convert("application/vnd.wap.uaprof"));
    hm->Put(CoreUtils::Convert(0x18), CoreUtils::Convert("application/vnd.wap.wtls-ca-certificate"));
    hm->Put(CoreUtils::Convert(0x19), CoreUtils::Convert("application/vnd.wap.wtls-user-certificate"));
    hm->Put(CoreUtils::Convert(0x1A), CoreUtils::Convert("application/x-x509-ca-cert"));
    hm->Put(CoreUtils::Convert(0x1B), CoreUtils::Convert("application/x-x509-user-cert"));
    hm->Put(CoreUtils::Convert(0x1C), CoreUtils::Convert("image/*"));
    hm->Put(CoreUtils::Convert(0x1D), CoreUtils::Convert("image/gif"));
    hm->Put(CoreUtils::Convert(0x1E), CoreUtils::Convert("image/jpeg"));
    hm->Put(CoreUtils::Convert(0x1F), CoreUtils::Convert("image/tiff"));
    hm->Put(CoreUtils::Convert(0x20), CoreUtils::Convert("image/png"));
    hm->Put(CoreUtils::Convert(0x21), CoreUtils::Convert("image/vnd.wap.wbmp"));
    hm->Put(CoreUtils::Convert(0x22), CoreUtils::Convert("application/vnd.wap.multipart.*"));
    hm->Put(CoreUtils::Convert(0x23), CoreUtils::Convert("application/vnd.wap.multipart.mixed"));
    hm->Put(CoreUtils::Convert(0x24), CoreUtils::Convert("application/vnd.wap.multipart.form-data"));
    hm->Put(CoreUtils::Convert(0x25), CoreUtils::Convert("application/vnd.wap.multipart.byteranges"));
    hm->Put(CoreUtils::Convert(0x26), CoreUtils::Convert("application/vnd.wap.multipart.alternative"));
    hm->Put(CoreUtils::Convert(0x27), CoreUtils::Convert("application/xml"));
    hm->Put(CoreUtils::Convert(0x28), CoreUtils::Convert("text/xml"));
    hm->Put(CoreUtils::Convert(0x29), CoreUtils::Convert("application/vnd.wap.wbxml"));
    hm->Put(CoreUtils::Convert(0x2A), CoreUtils::Convert("application/x-x968-cross-cert"));
    hm->Put(CoreUtils::Convert(0x2B), CoreUtils::Convert("application/x-x968-ca-cert"));
    hm->Put(CoreUtils::Convert(0x2C), CoreUtils::Convert("application/x-x968-user-cert"));
    hm->Put(CoreUtils::Convert(0x2D), CoreUtils::Convert("text/vnd.wap.si"));
    hm->Put(CoreUtils::Convert(0x2E), CoreUtils::Convert("application/vnd.wap.sic"));
    hm->Put(CoreUtils::Convert(0x2F), CoreUtils::Convert("text/vnd.wap.sl"));
    hm->Put(CoreUtils::Convert(0x30), CoreUtils::Convert("application/vnd.wap.slc"));
    hm->Put(CoreUtils::Convert(0x31), CoreUtils::Convert("text/vnd.wap.co"));
    hm->Put(CoreUtils::Convert(0x32), CoreUtils::Convert("application/vnd.wap.coc"));
    hm->Put(CoreUtils::Convert(0x33), CoreUtils::Convert("application/vnd.wap.multipart.related"));
    hm->Put(CoreUtils::Convert(0x34), CoreUtils::Convert("application/vnd.wap.sia"));
    hm->Put(CoreUtils::Convert(0x35), CoreUtils::Convert("text/vnd.wap.connectivity-xml"));
    hm->Put(CoreUtils::Convert(0x36), CoreUtils::Convert("application/vnd.wap.connectivity-wbxml"));
    hm->Put(CoreUtils::Convert(0x37), CoreUtils::Convert("application/pkcs7-mime"));
    hm->Put(CoreUtils::Convert(0x38), CoreUtils::Convert("application/vnd.wap.hashed-certificate"));
    hm->Put(CoreUtils::Convert(0x39), CoreUtils::Convert("application/vnd.wap.signed-certificate"));
    hm->Put(CoreUtils::Convert(0x3A), CoreUtils::Convert("application/vnd.wap.cert-response"));
    hm->Put(CoreUtils::Convert(0x3B), CoreUtils::Convert("application/xhtml+xml"));
    hm->Put(CoreUtils::Convert(0x3C), CoreUtils::Convert("application/wml+xml"));
    hm->Put(CoreUtils::Convert(0x3D), CoreUtils::Convert("text/css"));
    hm->Put(CoreUtils::Convert(0x3E), CoreUtils::Convert("application/vnd.wap.mms-message"));
    hm->Put(CoreUtils::Convert(0x3F), CoreUtils::Convert("application/vnd.wap.rollover-certificate"));
    hm->Put(CoreUtils::Convert(0x40), CoreUtils::Convert("application/vnd.wap.locc+wbxml"));
    hm->Put(CoreUtils::Convert(0x41), CoreUtils::Convert("application/vnd.wap.loc+xml"));
    hm->Put(CoreUtils::Convert(0x42), CoreUtils::Convert("application/vnd.syncml.dm+wbxml"));
    hm->Put(CoreUtils::Convert(0x43), CoreUtils::Convert("application/vnd.syncml.dm+xml"));
    hm->Put(CoreUtils::Convert(0x44), CoreUtils::Convert("application/vnd.syncml.notification"));
    hm->Put(CoreUtils::Convert(0x45), CoreUtils::Convert("application/vnd.wap.xhtml+xml"));
    hm->Put(CoreUtils::Convert(0x46), CoreUtils::Convert("application/vnd.wv.csp.cir"));
    hm->Put(CoreUtils::Convert(0x47), CoreUtils::Convert("application/vnd.oma.dd+xml"));
    hm->Put(CoreUtils::Convert(0x48), CoreUtils::Convert("application/vnd.oma.drm.message"));
    hm->Put(CoreUtils::Convert(0x49), CoreUtils::Convert("application/vnd.oma.drm.content"));
    hm->Put(CoreUtils::Convert(0x4A), CoreUtils::Convert("application/vnd.oma.drm.rights+xml"));
    hm->Put(CoreUtils::Convert(0x4B), CoreUtils::Convert("application/vnd.oma.drm.rights+wbxml"));
    hm->Put(CoreUtils::Convert(0x4C), CoreUtils::Convert("application/vnd.wv.csp+xml"));
    hm->Put(CoreUtils::Convert(0x4D), CoreUtils::Convert("application/vnd.wv.csp+wbxml"));
    hm->Put(CoreUtils::Convert(0x4E), CoreUtils::Convert("application/vnd.syncml.ds.notification"));
    hm->Put(CoreUtils::Convert(0x4F), CoreUtils::Convert("audio/*"));
    hm->Put(CoreUtils::Convert(0x50), CoreUtils::Convert("video/*"));
    hm->Put(CoreUtils::Convert(0x51), CoreUtils::Convert("application/vnd.oma.dd2+xml"));
    hm->Put(CoreUtils::Convert(0x52), CoreUtils::Convert("application/mikey"));
    hm->Put(CoreUtils::Convert(0x53), CoreUtils::Convert("application/vnd.oma.dcd"));
    hm->Put(CoreUtils::Convert(0x54), CoreUtils::Convert("application/vnd.oma.dcdc"));

    hm->Put(CoreUtils::Convert(0x0201), CoreUtils::Convert("application/vnd.uplanet.cacheop-wbxml"));
    hm->Put(CoreUtils::Convert(0x0202), CoreUtils::Convert("application/vnd.uplanet.signal"));
    hm->Put(CoreUtils::Convert(0x0203), CoreUtils::Convert("application/vnd.uplanet.alert-wbxml"));
    hm->Put(CoreUtils::Convert(0x0204), CoreUtils::Convert("application/vnd.uplanet.list-wbxml"));
    hm->Put(CoreUtils::Convert(0x0205), CoreUtils::Convert("application/vnd.uplanet.listcmd-wbxml"));
    hm->Put(CoreUtils::Convert(0x0206), CoreUtils::Convert("application/vnd.uplanet.channel-wbxml"));
    hm->Put(CoreUtils::Convert(0x0207), CoreUtils::Convert("application/vnd.uplanet.provisioning-status-uri"));
    hm->Put(CoreUtils::Convert(0x0208), CoreUtils::Convert("x-wap.multipart/vnd.uplanet.header-set"));
    hm->Put(CoreUtils::Convert(0x0209), CoreUtils::Convert("application/vnd.uplanet.bearer-choice-wbxml"));
    hm->Put(CoreUtils::Convert(0x020A), CoreUtils::Convert("application/vnd.phonecom.mmc-wbxml"));
    hm->Put(CoreUtils::Convert(0x020B), CoreUtils::Convert("application/vnd.nokia.syncset+wbxml"));
    hm->Put(CoreUtils::Convert(0x020C), CoreUtils::Convert("image/x-up-wpng"));
    hm->Put(CoreUtils::Convert(0x0300), CoreUtils::Convert("application/iota.mmc-wbxml"));
    hm->Put(CoreUtils::Convert(0x0301), CoreUtils::Convert("application/iota.mmc-xml"));
    hm->Put(CoreUtils::Convert(0x0302), CoreUtils::Convert("application/vnd.syncml+xml"));
    hm->Put(CoreUtils::Convert(0x0303), CoreUtils::Convert("application/vnd.syncml+wbxml"));
    hm->Put(CoreUtils::Convert(0x0304), CoreUtils::Convert("text/vnd.wap.emn+xml"));
    hm->Put(CoreUtils::Convert(0x0305), CoreUtils::Convert("text/calendar"));
    hm->Put(CoreUtils::Convert(0x0306), CoreUtils::Convert("application/vnd.omads-email+xml"));
    hm->Put(CoreUtils::Convert(0x0307), CoreUtils::Convert("application/vnd.omads-file+xml"));
    hm->Put(CoreUtils::Convert(0x0308), CoreUtils::Convert("application/vnd.omads-folder+xml"));
    hm->Put(CoreUtils::Convert(0x0309), CoreUtils::Convert("text/directory;profile=vCard"));
    hm->Put(CoreUtils::Convert(0x030A), CoreUtils::Convert("application/vnd.wap.emn+wbxml"));
    hm->Put(CoreUtils::Convert(0x030B), CoreUtils::Convert("application/vnd.nokia.ipdc-purchase-response"));
    hm->Put(CoreUtils::Convert(0x030C), CoreUtils::Convert("application/vnd.motorola.screen3+xml"));
    hm->Put(CoreUtils::Convert(0x030D), CoreUtils::Convert("application/vnd.motorola.screen3+gzip"));
    hm->Put(CoreUtils::Convert(0x030E), CoreUtils::Convert("application/vnd.cmcc.setting+wbxml"));
    hm->Put(CoreUtils::Convert(0x030F), CoreUtils::Convert("application/vnd.cmcc.bombing+wbxml"));
    hm->Put(CoreUtils::Convert(0x0310), CoreUtils::Convert("application/vnd.docomo.pf"));
    hm->Put(CoreUtils::Convert(0x0311), CoreUtils::Convert("application/vnd.docomo.ub"));
    hm->Put(CoreUtils::Convert(0x0312), CoreUtils::Convert("application/vnd.omaloc-supl-init"));
    hm->Put(CoreUtils::Convert(0x0313), CoreUtils::Convert("application/vnd.oma.group-usage-list+xml"));
    hm->Put(CoreUtils::Convert(0x0314), CoreUtils::Convert("application/oma-directory+xml"));
    hm->Put(CoreUtils::Convert(0x0315), CoreUtils::Convert("application/vnd.docomo.pf2"));
    hm->Put(CoreUtils::Convert(0x0316), CoreUtils::Convert("application/vnd.oma.drm.roap-trigger+wbxml"));
    hm->Put(CoreUtils::Convert(0x0317), CoreUtils::Convert("application/vnd.sbm.mid2"));
    hm->Put(CoreUtils::Convert(0x0318), CoreUtils::Convert("application/vnd.wmf.bootstrap"));
    hm->Put(CoreUtils::Convert(0x0319), CoreUtils::Convert("application/vnc.cmcc.dcd+xml"));
    hm->Put(CoreUtils::Convert(0x031A), CoreUtils::Convert("application/vnd.sbm.cid"));
    hm->Put(CoreUtils::Convert(0x031B), CoreUtils::Convert("application/vnd.oma.bcast.provisioningtrigger"));
    return hm;
}
const AutoPtr<IHashMap> WspTypeDecoder::WELL_KNOWN_MIME_TYPES = InitTYPES();

static AutoPtr<IHashMap> InitPARAMETERS()
{
    AutoPtr<IHashMap> hm;
    CHashMap::New((IHashMap**)&hm);
    hm->Put(CoreUtils::Convert(0x00), CoreUtils::Convert("Q"));
    hm->Put(CoreUtils::Convert(0x01), CoreUtils::Convert("Charset"));
    hm->Put(CoreUtils::Convert(0x02), CoreUtils::Convert("Level"));
    hm->Put(CoreUtils::Convert(0x03), CoreUtils::Convert("Type"));
    hm->Put(CoreUtils::Convert(0x07), CoreUtils::Convert("Differences"));
    hm->Put(CoreUtils::Convert(0x08), CoreUtils::Convert("Padding"));
    hm->Put(CoreUtils::Convert(0x09), CoreUtils::Convert("Type"));
    hm->Put(CoreUtils::Convert(0x0E), CoreUtils::Convert("Max-Age"));
    hm->Put(CoreUtils::Convert(0x10), CoreUtils::Convert("Secure"));
    hm->Put(CoreUtils::Convert(0x11), CoreUtils::Convert("SEC"));
    hm->Put(CoreUtils::Convert(0x12), CoreUtils::Convert("MAC"));
    hm->Put(CoreUtils::Convert(0x13), CoreUtils::Convert("Creation-date"));
    hm->Put(CoreUtils::Convert(0x14), CoreUtils::Convert("Modification-date"));
    hm->Put(CoreUtils::Convert(0x15), CoreUtils::Convert("Read-date"));
    hm->Put(CoreUtils::Convert(0x16), CoreUtils::Convert("Size"));
    hm->Put(CoreUtils::Convert(0x17), CoreUtils::Convert("Name"));
    hm->Put(CoreUtils::Convert(0x18), CoreUtils::Convert("Filename"));
    hm->Put(CoreUtils::Convert(0x19), CoreUtils::Convert("Start"));
    hm->Put(CoreUtils::Convert(0x1A), CoreUtils::Convert("Start-info"));
    hm->Put(CoreUtils::Convert(0x1B), CoreUtils::Convert("Comment"));
    hm->Put(CoreUtils::Convert(0x1C), CoreUtils::Convert("Domain"));
    hm->Put(CoreUtils::Convert(0x1D), CoreUtils::Convert("Path"));
    return hm;
}
const AutoPtr<IHashMap> WspTypeDecoder::WELL_KNOWN_PARAMETERS = InitPARAMETERS();

const Int32 WspTypeDecoder::Q_VALUE = 0x00;

CAR_INTERFACE_IMPL(WspTypeDecoder, Object, IWspTypeDecoder)

ECode WspTypeDecoder::constructor(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    mWspData = pdu;
    return NOERROR;
}

ECode WspTypeDecoder::DecodeTextString(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 index = startIndex;
    while ((*mWspData)[index] != 0) {
        index++;
    }
    mDataLength = index - startIndex + 1;
    if ((*mWspData)[startIndex] == 127) {
        mStringValue = String(*mWspData, startIndex + 1, mDataLength - 2);
    }
    else {
        mStringValue = String(*mWspData, startIndex, mDataLength - 1);
    }
    *result = TRUE;
    return NOERROR;
}

ECode WspTypeDecoder::DecodeTokenText(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 index = startIndex;
    while ((*mWspData)[index] != 0) {
        index++;
    }
    mDataLength = index - startIndex + 1;
    mStringValue = String(*mWspData, startIndex, mDataLength - 1);

    *result = TRUE;
    return NOERROR;
}

ECode WspTypeDecoder::DecodeShortInteger(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (((*mWspData)[startIndex] & 0x80) == 0) {
        *result = FALSE;
        return NOERROR;
    }
    mUnsigned32bit = (*mWspData)[startIndex] & 0x7f;
    mDataLength = 1;
    *result = TRUE;
    return NOERROR;
}

ECode WspTypeDecoder::DecodeLongInteger(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 lengthMultiOctet = (*mWspData)[startIndex] & 0xff;

    if (lengthMultiOctet > WAP_PDU_SHORT_LENGTH_MAX) {
        *result = FALSE;
        return NOERROR;
    }
    mUnsigned32bit = 0;
    for (Int32 i = 1; i <= lengthMultiOctet; i++) {
        mUnsigned32bit = (mUnsigned32bit << 8) | ((*mWspData)[startIndex + i] & 0xff);
    }
    mDataLength = 1 + lengthMultiOctet;
    *result = TRUE;
    return NOERROR;
}

ECode WspTypeDecoder::DecodeIntegerValue(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b = FALSE;
    DecodeShortInteger(startIndex, &b);
    if (b == TRUE) {
        *result = TRUE;
        return NOERROR;
    }
    return DecodeLongInteger(startIndex, result);
}

ECode WspTypeDecoder::DecodeUintvarInteger(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 index = startIndex;

    mUnsigned32bit = 0;
    while (((*mWspData)[index] & 0x80) != 0) {
        if ((index - startIndex) >= 4) {
            *result = FALSE;
            return NOERROR;
        }
        mUnsigned32bit = (mUnsigned32bit << 7) | ((*mWspData)[index] & 0x7f);
        index++;
    }
    mUnsigned32bit = (mUnsigned32bit << 7) | ((*mWspData)[index] & 0x7f);
    mDataLength = index - startIndex + 1;
    *result = TRUE;
    return NOERROR;
}

ECode WspTypeDecoder::DecodeValueLength(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (((*mWspData)[startIndex] & 0xff) > WAP_PDU_LENGTH_QUOTE) {
        *result = FALSE;
        return NOERROR;
    }
    if ((*mWspData)[startIndex] < WAP_PDU_LENGTH_QUOTE) {
        mUnsigned32bit = (*mWspData)[startIndex];
        mDataLength = 1;
    }
    else {
        Boolean res = FALSE;
        DecodeUintvarInteger(startIndex + 1, &res);
        mDataLength++;
    }
    *result = TRUE;
    return NOERROR;
}

ECode WspTypeDecoder::DecodeExtensionMedia(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 index = startIndex;
    mDataLength = 0;
    mStringValue = NULL;
    Int32 length = mWspData->GetLength();
    Boolean rtrn = index < length;

    while (index < length && (*mWspData)[index] != 0) {
        index++;
    }

    mDataLength = index - startIndex + 1;
    mStringValue = String(*mWspData, startIndex, mDataLength - 1);

    *result = rtrn;
    return NOERROR;
}

ECode WspTypeDecoder::DecodeConstrainedEncoding(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b = FALSE;
    DecodeShortInteger(startIndex, &b);
    if (b == TRUE) {
        mStringValue = String(NULL);
        *result = TRUE;
        return NOERROR;
    }
    return DecodeExtensionMedia(startIndex, result);
}

ECode WspTypeDecoder::DecodeContentType(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 mediaPrefixLength = 0;
    CHashMap::New((IHashMap**)&mContentParameters);

    // try {
    Boolean bLength = FALSE;
    DecodeValueLength(startIndex, &bLength);
    if (bLength == FALSE) {
        Boolean found = FALSE;
        DecodeConstrainedEncoding(startIndex, &found);
        if (found) {
            ExpandWellKnownMimeType();
        }
        *result = found;
        return NOERROR;
    }
    Int32 headersLength = (Int32) mUnsigned32bit;
    GetDecodedDataLength(&mediaPrefixLength);
    Boolean bIntegerValue = FALSE;
    DecodeIntegerValue(startIndex + mediaPrefixLength, &bIntegerValue);
    if (bIntegerValue == TRUE) {
        mDataLength += mediaPrefixLength;
        Int32 readLength = mDataLength;
        mStringValue = String(NULL);
        ExpandWellKnownMimeType();
        Int64 wellKnownValue = mUnsigned32bit;
        String mimeType = mStringValue;
        if (ReadContentParameters(startIndex + mDataLength,
                (headersLength - (mDataLength - mediaPrefixLength)), 0)) {
            mDataLength += readLength;
            mUnsigned32bit = wellKnownValue;
            mStringValue = mimeType;
            *result = TRUE;
            return NOERROR;
        }
        *result = FALSE;
        return NOERROR;
    }
    Boolean bExtMedia = FALSE;
    DecodeExtensionMedia(startIndex + mediaPrefixLength, &bExtMedia);
    if (bExtMedia == TRUE) {
        mDataLength += mediaPrefixLength;
        Int32 readLength = mDataLength;
        ExpandWellKnownMimeType();
        Int64 wellKnownValue = mUnsigned32bit;
        String mimeType = mStringValue;
        if (ReadContentParameters(startIndex + mDataLength,
                (headersLength - (mDataLength - mediaPrefixLength)), 0)) {
            mDataLength += readLength;
            mUnsigned32bit = wellKnownValue;
            mStringValue = mimeType;
            *result = TRUE;
            return NOERROR;
        }
    }
    // } Catch (ArrayIndexOutOfBoundsException e) {
    //     //something doesn't add up
    //     return FALSE;
    // }
    *result = FALSE;
    return NOERROR;
}

Boolean WspTypeDecoder::ReadContentParameters(
    /* [in] */ Int32 startIndex,
    /* [in] */ Int32 leftToRead,
    /* [in] */ Int32 accumulator)
{
    Int32 totalRead = 0;

    if (leftToRead > 0) {
        Byte nextByte = (*mWspData)[startIndex];
        String value(NULL);
        String param(NULL);
        if ((nextByte & 0x80) == 0x00 && nextByte > 31) { // untyped
            Boolean b = FALSE;
            DecodeTokenText(startIndex, &b);
            param = mStringValue;
            totalRead += mDataLength;
        }
        else { // typed
            Boolean bIntegerValue = FALSE;
            DecodeIntegerValue(startIndex, &bIntegerValue);
            if (bIntegerValue) {
                totalRead += mDataLength;
                Int32 wellKnownParameterValue = (Int32) mUnsigned32bit;
                AutoPtr<IInterface> pVal;
                WELL_KNOWN_PARAMETERS->Get(CoreUtils::Convert(wellKnownParameterValue), (IInterface**)&pVal);
                param = CoreUtils::Unbox(ICharSequence::Probe(pVal));
                if (param == NULL) {
                    param = "unassigned/0x";
                    param += StringUtils::ToHexString(wellKnownParameterValue);
                }
                // special case for the "Q" parameter, value is a uintvar
                if (wellKnownParameterValue == Q_VALUE) {
                    Boolean bInteger = FALSE;
                    DecodeUintvarInteger(startIndex + totalRead, &bInteger);
                    if (bInteger) {
                        totalRead += mDataLength;
                        value = StringUtils::ToString(mUnsigned32bit);
                        mContentParameters->Put(CoreUtils::Convert(param), CoreUtils::Convert(value));
                        return ReadContentParameters(startIndex + totalRead, leftToRead
                                                        - totalRead, accumulator + totalRead);
                    }
                    else {
                        return FALSE;
                    }
                }
            }
            else {
                return FALSE;
            }
        }

        Boolean b = FALSE;
        if (DecodeNoValue(startIndex + totalRead)) {
            totalRead += mDataLength;
            value = String(NULL);
        }
        else if ((DecodeIntegerValue(startIndex + totalRead, &b), b)) {
            totalRead += mDataLength;
            Int32 intValue = (Int32) mUnsigned32bit;
            value = StringUtils::ToString(intValue);
        }
        else {
            Boolean b = FALSE;
            DecodeTokenText(startIndex + totalRead, &b);
            totalRead += mDataLength;
            value = mStringValue;
            if (value.StartWith("\"")) {
                // quoted string, so remove the quote
                value = value.Substring(1);
            }
        }
        mContentParameters->Put(CoreUtils::Convert(param), CoreUtils::Convert(value));
        return ReadContentParameters(startIndex + totalRead, leftToRead - totalRead,
                                        accumulator + totalRead);
    }
    else {
        mDataLength = accumulator;
        return TRUE;
    }
}

Boolean WspTypeDecoder::DecodeNoValue(
    /* [in] */ Int32 startIndex)
{
    if ((*mWspData)[startIndex] == 0) {
        mDataLength = 1;
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void WspTypeDecoder::ExpandWellKnownMimeType()
{
    if (mStringValue.IsNull()) {
        Int32 binaryContentType = (Int32) mUnsigned32bit;
        AutoPtr<IInterface> p;
        WELL_KNOWN_MIME_TYPES->Get(CoreUtils::Convert(binaryContentType), (IInterface**)&p);
        ICharSequence::Probe(p)->ToString(&mStringValue);
    }
    else {
        mUnsigned32bit = -1;
    }
}

ECode WspTypeDecoder::DecodeContentLength(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return DecodeIntegerValue(startIndex, result);
}

ECode WspTypeDecoder::DecodeContentLocation(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return DecodeTextString(startIndex, result);
}

ECode WspTypeDecoder::DecodeXWapApplicationId(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b = FALSE;
    DecodeIntegerValue(startIndex, &b);
    if (b == TRUE) {
        mStringValue = NULL;
        *result = TRUE;
        return NOERROR;
    }
    return DecodeTextString(startIndex, result);
}

ECode WspTypeDecoder::SeekXWapApplicationId(
    /* [in] */ Int32 startIndex,
    /* [in] */ Int32 endIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 index = startIndex;

    // try {
    for (index = startIndex; index <= endIndex; ) {
        /**
         * 8.4.1.1  Field name
         * Field name is integer or text.
         */
        Boolean b = FALSE;
        DecodeIntegerValue(index, &b);
        if (b) {
            Int64 val = 0;
            GetValue32(&val);
            Int32 fieldValue = (Int32) val;

            if (fieldValue == PARAMETER_ID_X_WAP_APPLICATION_ID) {
                mUnsigned32bit = index + 1;
                *result = TRUE;
                return NOERROR;
            }
        }
        else {
            Boolean bString = FALSE;
            DecodeTextString(index, &bString);
            if (!bString) {
                *result = FALSE;
                return NOERROR;
            }
        }
        Int32 length = 0;
        GetDecodedDataLength(&length);
        index += length;
        if (index > endIndex) {
            *result = FALSE;
            return NOERROR;
        }

        /**
         * 8.4.1.2 Field values
         * Value Interpretation of First Octet
         * 0 - 30 This octet is followed by the indicated Number (0 - 30)
                of data octets
         * 31 This octet is followed by a uintvar, which indicates the number
         *      of data octets after it
         * 32 - 127 The value is a text string, terminated by a zero octet
                (NUL character)
         * 128 - 255 It is an encoded 7-bit value; this header has no more data
         */
        Byte val = (*mWspData)[index];
        if (0 <= val && val <= WAP_PDU_SHORT_LENGTH_MAX) {
            index += (*mWspData)[index] + 1;
        }
        else if (val == WAP_PDU_LENGTH_QUOTE) {
            if (index + 1 >= endIndex) {
                *result = FALSE;
                return NOERROR;
            }
            index++;
            Boolean bInteger = FALSE;
            DecodeUintvarInteger(index, &bInteger);
            if (!bInteger) {
                *result = FALSE;
                return NOERROR;
            }
            Int32 length = 0;
            GetDecodedDataLength(&length);
            index += length;
        }
        else if (WAP_PDU_LENGTH_QUOTE < val && val <= 127) {
            Boolean bString = FALSE;
            DecodeTextString(index, &bString);
            if (!bString) {
                *result = FALSE;
                return NOERROR;
            }
            Int32 length = 0;
            GetDecodedDataLength(&length);
            index += length;
        }
        else {
            index++;
        }
    }
    // } Catch (ArrayIndexOutOfBoundsException e) {
    //     //seek application ID failed. WSP header might be corrupted
    //     return FALSE;
    // }
    *result = FALSE;
    return NOERROR;
}

ECode WspTypeDecoder::DecodeXWapContentURI(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return DecodeTextString(startIndex, result);
}

ECode WspTypeDecoder::DecodeXWapInitiatorURI(
    /* [in] */ Int32 startIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return DecodeTextString(startIndex, result);
}

ECode WspTypeDecoder::GetDecodedDataLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDataLength;
    return NOERROR;
}

ECode WspTypeDecoder::GetValue32(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUnsigned32bit;
    return NOERROR;
}

ECode WspTypeDecoder::GetValueString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStringValue;
    return NOERROR;
}

ECode WspTypeDecoder::GetContentParameters(
    /* [out] */ IHashMap** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mContentParameters;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
