
#include "elastos/droid/internal/telephony/DriverCall.h"
#include "elastos/droid/internal/telephony/ATResponseParser.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String DriverCall::TAG("DriverCall");
CAR_INTERFACE_IMPL_2(DriverCall, Object, IComparable, IDriverCall)
/** returns null on error */
AutoPtr<DriverCall> DriverCall::FromCLCCLine(
    /* [in] */ const String& line)
{
    AutoPtr<DriverCall> ret = new DriverCall();

    //+CLCC: 1,0,2,0,0,\"+18005551212\",145
    //     mIndex,mIsMT,state,mode,isMpty(,number,TOA)?
    AutoPtr<ATResponseParser> p = new ATResponseParser(line);
    ECode ec = NOERROR;
    do {
        if (FAILED(ec = p->NextInt32(&ret->mIndex))) {
            break;
        }

        if (FAILED(ec = p->NextBoolean(&ret->mIsMT))) {
            break;
        }

        Int32 v = 0;
        if (FAILED(ec = p->NextInt32(&v))) {
            break;
        }
        if (FAILED(ec = StateFromCLCC(v, &ret->mState))) {
            break;
        }

        if (FAILED(ec = p->NextInt32(&v))) {
            break;
        }
        ret->mIsVoice = (0 == v);
        if (FAILED(ec = p->NextBoolean(&ret->mIsMpty))) {
            break;
        }

        // use ALLOWED as default presentation while parsing CLCC
        ret->mNumberPresentation = IPhoneConstants::PRESENTATION_ALLOWED;

        Boolean has = FALSE;
        if (p->HasMore(&has), has) {
            // Some lame implementations return strings
            // like "NOT AVAILABLE" in the CLCC line
            String s;
            if (FAILED(ec = p->NextString(&s))) {
                break;
            }
            PhoneNumberUtils::ExtractNetworkPortionAlt(s, &ret->mNumber);

            if (ret->mNumber.GetLength() == 0) {
                ret->mNumber = String(NULL);
            }

            if (FAILED(ec = p->NextInt32(&ret->mTOA))) {
                break;
            }

            // Make sure there's a leading + on addresses with a TOA
            // of 145
            PhoneNumberUtils::StringFromStringAndTOA(
                            ret->mNumber, ret->mTOA, &ret->mNumber);
        }
    } while (0);

    if (FAILED(ec)) {
        Logger::E(TAG,"Invalid CLCC line: '%s'", line.string());
        return NULL;
    }

    return ret;
}

DriverCall::DriverCall()
    : mIndex(0)
    , mIsMT(FALSE)
    // , mState(-1)
    , mIsMpty(FALSE)
    , mTOA(0)
    , mIsVoice(FALSE)
    , mIsVoicePrivacy(FALSE)
    , mAls(0)
    , mNumberPresentation(0)
    , mNamePresentation(0)
{
}

ECode DriverCall::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("id=");
    sb.Append(mIndex);
    sb.Append(",");
    sb.Append(mState);
    sb.Append(",");
    sb.Append("toa=");
    sb.Append(mTOA);
    sb.Append(",");
    sb.Append((mIsMpty ? "conf" : "norm"));
    sb.Append(",");
    sb.Append((mIsMT ? "mt" : "mo"));
    sb.Append(",");
    sb.Append(mAls);
    sb.Append(",");
    sb.Append((mIsVoice ? "voc" : "nonvoc"));
    sb.Append(",");
    sb.Append(mIsVoicePrivacy ? "evp" : "noevp");
    sb.Append(",");
    sb.Append("number=");
    sb.Append(mNumber);
    sb.Append(",cli=");
    sb.Append(mNumberPresentation);
    sb.Append(",");
    sb.Append("name=");
    sb.Append(mName);
    sb.Append(",");
    sb.Append(mNamePresentation);
    return sb.ToString(str);
}

ECode DriverCall::StateFromCLCC(
    /* [in] */ Int32 state,
    /* [out] */ IDriverCallState* result) /*throws ATParseEx*/
{
    VALIDATE_NOT_NULL(result);
    switch(state) {
        case 0: {
            *result = DriverCallState_ACTIVE;
            return NOERROR;
        }
        case 1: {
            *result = DriverCallState_HOLDING;
            return NOERROR;
        }
        case 2: {
            *result = DriverCallState_DIALING;
            return NOERROR;
        }
        case 3: {
            *result = DriverCallState_ALERTING;
            return NOERROR;
        }
        case 4: {
            *result = DriverCallState_INCOMING;
            return NOERROR;
        }
        case 5: {
            *result = DriverCallState_WAITING;
            return NOERROR;
        }
    }
    return E_TELEPHONEY_AT_PARSE_EXCEPTION;
}

ECode DriverCall::PresentationFromCLIP(
    /* [in] */ Int32 cli,
    /* [out] */ Int32* result) /*throws ATParseEx*/
{
    VALIDATE_NOT_NULL(result);
    switch(cli) {
        case 0: {
            *result = IPhoneConstants::PRESENTATION_ALLOWED;
            return NOERROR;
        }
        case 1: {
            *result = IPhoneConstants::PRESENTATION_RESTRICTED;
            return NOERROR;
        }
        case 2: {
            *result = IPhoneConstants::PRESENTATION_UNKNOWN;
            return NOERROR;
        }
        case 3: {
            *result = IPhoneConstants::PRESENTATION_PAYPHONE;
            return NOERROR;
        }
        // default:
        //     throw new ATParseEx("illegal presentation " + cli);
    }
    *result = -1;
    return E_TELEPHONEY_AT_PARSE_EXCEPTION;
}

ECode DriverCall::CompareTo(
    /* [in] */ IInterface* _dc,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    DriverCall* dc = (DriverCall*)IComparable::Probe(_dc);
    if (mIndex < dc->mIndex) {
        *result = -1;
        return NOERROR;
    }
    else if (mIndex == dc->mIndex) {
        *result = 0;
        return NOERROR;
    }

    /*mIndex > dc.mIndex*/
    *result = 1;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
