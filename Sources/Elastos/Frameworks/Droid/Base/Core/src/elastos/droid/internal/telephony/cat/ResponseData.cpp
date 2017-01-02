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

#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/internal/telephony/CGsmAlphabet.h"
#include "elastos/droid/internal/telephony/cat/ResponseData.h"
#include "elastos/droid/internal/telephony/cat/CatLog.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/text/TextUtils.h"

#include <elastos/core/StringUtils.h>

using Elastos::Droid::Internal::Telephony::CGsmAlphabet;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Text::TextUtils;

using Elastos::Core::StringUtils;
using Elastos::IO::IOutputStream;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                             ResponseData
//=====================================================================
ECode ResponseData::WriteLength(
    /* [in] */ IByteArrayOutputStream* buf,
    /* [in] */ Int32 length)
{
    // As per ETSI 102.220 Sec7.1.2, if the total length is greater
    // than 0x7F, it should be coded in two bytes and the first byte
    // should be 0x81.
    if (length > 0x7F) {
        IOutputStream::Probe(buf)->Write(0x81);
    }
    IOutputStream::Probe(buf)->Write(length);
    return NOERROR;
}

//=====================================================================
//                        SelectItemResponseData
//=====================================================================
SelectItemResponseData::SelectItemResponseData(
    /* [in] */ Int32 id)
{
    mId = id;
}

ECode SelectItemResponseData::Format(
    /* [in] */ IByteArrayOutputStream* buf)
{
    // Item identifier object
    Int32 tag = 0x80 | ComprehensionTlvTag_ITEM_ID;
    IOutputStream::Probe(buf)->Write(tag); // tag
    IOutputStream::Probe(buf)->Write(1); // length
    IOutputStream::Probe(buf)->Write(mId); // identifier of item chosen
    return NOERROR;
}

//=====================================================================
//                      GetInkeyInputResponseData
//=====================================================================
const Byte GetInkeyInputResponseData::GET_INKEY_YES = 0x01;
const Byte GetInkeyInputResponseData::GET_INKEY_NO = 0x00;

GetInkeyInputResponseData::GetInkeyInputResponseData(
    /* [in] */ const String& inData,
    /* [in] */ Boolean ucs2,
    /* [in] */ Boolean packed)
{
    mIsUcs2 = ucs2;
    mIsPacked = packed;
    mInData = inData;
    mIsYesNo = FALSE;
}

GetInkeyInputResponseData::GetInkeyInputResponseData(
    /* [in] */ Boolean yesNoResponse)
{
    mIsUcs2 = FALSE;
    mIsPacked = FALSE;
    mInData = "";
    mIsYesNo = TRUE;
    mYesNoResponse = yesNoResponse;
}

ECode GetInkeyInputResponseData::Format(
    /* [in] */ IByteArrayOutputStream* buf)
{
    if (buf == NULL) {
        return NOERROR;
    }

    // Text string object
    Int32 tag = 0x80 | ComprehensionTlvTag_TEXT_STRING;
    IOutputStream::Probe(buf)->Write(tag); // tag

    AutoPtr<ArrayOf<Byte> > data;

    if (mIsYesNo) {
        data = ArrayOf<Byte>::Alloc(1);
        (*data)[0] = mYesNoResponse ? GET_INKEY_YES : GET_INKEY_NO;
    }
    else if (mInData != NULL && mInData.GetLength() > 0) {
        // try {
            // ETSI TS 102 223 8.15, should use the same format as in SMS messages
            // on the network.
            AutoPtr<IGsmAlphabet> gsm;
            CGsmAlphabet::AcquireSingleton((IGsmAlphabet**)&gsm);
            if (mIsUcs2) {
                // ucs2 is by definition big endian.
                assert(0 && "TODO string getbytes UTF-16BE");
                data = mInData.GetBytes(); // "UTF-16BE"
            }
            else if (mIsPacked) {
                Int32 size = mInData.GetLength();

                AutoPtr<ArrayOf<Byte> > tempData;
                gsm->StringToGsm7BitPacked(mInData, 0, 0, (ArrayOf<Byte>**)&tempData);
                data = ArrayOf<Byte>::Alloc(size);
                // Since stringToGsm7BitPacked() set byte 0 in the
                // returned byte array to the count of septets used...
                // copy to a new array without byte 0.
                // System::Arraycopy(tempData, 1, data, 0, size);
                data->Copy(0, tempData, 1, size);
            }
            else {
                gsm->StringToGsm8BitPacked(mInData, (ArrayOf<Byte>**)&data);
            }
        // } catch (UnsupportedEncodingException e) {
        //     data = new byte[0];
        // } catch (EncodeException e) {
        //     data = new byte[0];
        // }
    }
    else {
        assert(0 && "TODO");
        // data = new byte[0];
    }

    // length - one more for data coding scheme.
    WriteLength(buf, data->GetLength() + 1);

    // data coding scheme
    if (mIsUcs2) {
        IOutputStream::Probe(buf)->Write(0x08); // UCS2
    }
    else if (mIsPacked) {
        IOutputStream::Probe(buf)->Write(0x00); // 7 bit packed
    }
    else {
        IOutputStream::Probe(buf)->Write(0x04); // 8 bit unpacked
    }

    for (Int32 i = 0; i < data->GetLength(); i++) {
        Byte b = (*data)[i];
        IOutputStream::Probe(buf)->Write(b);
    }
    return NOERROR;
}

//=====================================================================
//                         LanguageResponseData
//=====================================================================
LanguageResponseData::LanguageResponseData(
    /* [in] */ const String& lang)
{
    mLang = lang;
}

ECode LanguageResponseData::Format(
    /* [in] */ IByteArrayOutputStream* buf)
{
    if (buf == NULL) {
        return NOERROR;
    }

    // Text string object
    Int32 tag = 0x80 | ComprehensionTlvTag_LANGUAGE;
    IOutputStream::Probe(buf)->Write(tag); // tag

    AutoPtr<ArrayOf<Byte> > data;

    if (!mLang.IsNull() && mLang.GetLength() > 0) {
        AutoPtr<IGsmAlphabet> gsm;
        CGsmAlphabet::AcquireSingleton((IGsmAlphabet**)&gsm);
        gsm->StringToGsm8BitPacked(mLang, (ArrayOf<Byte>**)&data);
    }
    else {
        assert(0 && "TODO");
        // data = new byte[0];
    }

    IOutputStream::Probe(buf)->Write(data->GetLength());

    for (Int32 i = 0; i < data->GetLength(); i++) {
        Byte b = (*data)[i];
        IOutputStream::Probe(buf)->Write(b);
    }
    return NOERROR;
}

//=====================================================================
//                           DTTZResponseData
//=====================================================================
DTTZResponseData::DTTZResponseData(
    /* [in] */ ICalendar* cal)
{
    mCalendar = cal;
}

ECode DTTZResponseData::Format(
    /* [in] */ IByteArrayOutputStream* buf)
{
    if (buf == NULL) {
        return NOERROR;
    }

    // DTTZ object
    Int32 tag = 0x80 | PROVIDE_LOCAL_INFORMATION;
    IOutputStream::Probe(buf)->Write(tag); // tag

    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(8);

    (*data)[0] = 0x07; // Write length of DTTZ data

    if (mCalendar == NULL) {
        AutoPtr<ICalendarHelper> hlp;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&hlp);
        hlp->GetInstance((ICalendar**)&mCalendar);
    }
    // Fill year byte
    Int32 year = 0;
    mCalendar->Get(ICalendar::YEAR, &year);
    (*data)[1] = ByteToBCD(year % 100);

    // Fill month byte
    Int32 month = 0;
    mCalendar->Get(ICalendar::MONTH, &month);
    (*data)[2] = ByteToBCD(month + 1);

    // Fill day byte
    Int32 date = 0;
    mCalendar->Get(ICalendar::DATE, &date);
    (*data)[3] = ByteToBCD(date);

    // Fill hour byte
    Int32 hour = 0;
    mCalendar->Get(ICalendar::HOUR_OF_DAY, &hour);
    (*data)[4] = ByteToBCD(hour);

    // Fill minute byte
    Int32 minute = 0;
    mCalendar->Get(ICalendar::MINUTE, &minute);
    (*data)[5] = ByteToBCD(minute);

    // Fill second byte
    Int32 second = 0;
    mCalendar->Get(ICalendar::SECOND, &second);
    (*data)[6] = ByteToBCD(second);

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String tz;
    sp->Get(String("persist.sys.timezone"), String(""), &tz);
    if (TextUtils::IsEmpty(tz)) {
        (*data)[7] = (Byte) 0xFF;    // set FF in terminal response
    }
    else {
        AutoPtr<ITimeZoneHelper> tzhlp;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzhlp);
        AutoPtr<ITimeZone> zone;
        tzhlp->GetTimeZone(tz, (ITimeZone**)&zone);
        Int32 rawOff = 0, saving = 0;
        zone->GetRawOffset(&rawOff);
        zone->GetDSTSavings(&saving);
        Int32 zoneOffset = rawOff + saving;
        (*data)[7] = GetTZOffSetByte(zoneOffset);
    }

    for (Int32 i = 0; i < data->GetLength(); i++) {
        Byte b = (*data)[i];
        IOutputStream::Probe(buf)->Write(b);
    }
    return NOERROR;
}

Byte DTTZResponseData::ByteToBCD(
    /* [in] */ Int32 value)
{
    if (value < 0 && value > 99) {
        CatLog::D((IInterface*)(IObject*)this, String("Err: byteToBCD conversion Value is ") + StringUtils::ToString(value) +
                       String(" Value has to be between 0 and 99"));
        return 0;
    }

    return (Byte) ((value / 10) | ((value % 10) << 4));
}

Byte DTTZResponseData::GetTZOffSetByte(
    /* [in] */ Int64 offSetVal)
{
    Boolean isNegative = (offSetVal < 0);

    /*
     * The 'offSetVal' is in milliseconds. Convert it to hours and compute
     * offset While sending T.R to UICC, offset is expressed is 'quarters of
     * hours'
     */
    Int64 tzOffset = offSetVal / (15 * 60 * 1000);
    tzOffset = (isNegative ? -1 : 1) * tzOffset;
    Byte bcdVal = ByteToBCD((Int32) tzOffset);
    // For negative offsets, put '1' in the msb
    return isNegative ? (bcdVal |= 0x08) : bcdVal;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
