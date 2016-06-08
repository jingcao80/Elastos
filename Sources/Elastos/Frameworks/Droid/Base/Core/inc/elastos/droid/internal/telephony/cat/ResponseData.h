#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RESPONSEDATA_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RESPONSEDATA_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import com.android.internal.telephony.EncodeException;
// import com.android.internal.telephony.GsmAlphabet;
// import java.util.TimeZone;
// import android.os.SystemProperties;
// import android.text.TextUtils;
// import com.android.internal.telephony.cat.AppInterface.CommandType;
// import java.io.UnsupportedEncodingException;

using Elastos::IO::IByteArrayOutputStream;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

class ResponseData
    : public Object
{
public:
    /**
      * Format the data appropriate for TERMINAL RESPONSE and write it into
      * the ByteArrayOutputStream object.
      */
    virtual CARAPI Format(
        /* [in] */ IByteArrayOutputStream* buf) = 0;

    static CARAPI WriteLength(
        /* [in] */ IByteArrayOutputStream* buf,
        /* [in] */ Int32 length);
};

class SelectItemResponseData
    : public ResponseData
{
public:
    SelectItemResponseData(
        /* [in] */ Int32 id);

    // @Override
    CARAPI Format(
        /* [in] */ IByteArrayOutputStream* buf);

private:
    // members
    Int32 mId;
};

class GetInkeyInputResponseData
    : public ResponseData
{
public:
    GetInkeyInputResponseData(
        /* [in] */ const String& inData,
        /* [in] */ Boolean ucs2,
        /* [in] */ Boolean packed);

    GetInkeyInputResponseData(
        /* [in] */ Boolean yesNoResponse);

    // @Override
    CARAPI Format(
        /* [in] */ IByteArrayOutputStream* buf);

public:
    String mInData;

protected:
    // GetInKey Yes/No response characters constants.
    static const Byte GET_INKEY_YES;
    static const Byte GET_INKEY_NO;

private:
    // members
    Boolean mIsUcs2;
    Boolean mIsPacked;
    Boolean mIsYesNo;
    Boolean mYesNoResponse;
};

// For "PROVIDE LOCAL INFORMATION" command.
// See TS 31.111 section 6.4.15/ETSI TS 102 223
// TS 31.124 section 27.22.4.15 for test spec
class LanguageResponseData
    : public ResponseData
{
public:
    LanguageResponseData(
        /* [in] */ const String& lang);

    // @Override
    CARAPI Format(
        /* [in] */ IByteArrayOutputStream* buf);

private:
    String mLang;
};

// For "PROVIDE LOCAL INFORMATION" command.
// See TS 31.111 section 6.4.15/ETSI TS 102 223
// TS 31.124 section 27.22.4.15 for test spec
class DTTZResponseData
    : public ResponseData
{
public:
    DTTZResponseData(
        /* [in] */ ICalendar* cal);

    // @Override
    CARAPI Format(
        /* [in] */ IByteArrayOutputStream* buf);

private:
    CARAPI_(Byte) ByteToBCD(
        /* [in] */ Int32 value);

    CARAPI_(Byte) GetTZOffSetByte(
        /* [in] */ Int64 offSetVal);

private:
    AutoPtr<ICalendar> mCalendar;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RESPONSEDATA_H__
