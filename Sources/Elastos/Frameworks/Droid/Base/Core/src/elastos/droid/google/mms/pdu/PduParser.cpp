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

#include "Elastos.Droid.Google.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/pdu/PduParser.h"
#include "elastos/droid/google/mms/pdu/CSendReq.h"
#include "elastos/droid/google/mms/pdu/CSendConf.h"
#include "elastos/droid/google/mms/pdu/CNotificationInd.h"
#include "elastos/droid/google/mms/pdu/CNotifyRespInd.h"
#include "elastos/droid/google/mms/pdu/CRetrieveConf.h"
#include "elastos/droid/google/mms/pdu/CDeliveryInd.h"
#include "elastos/droid/google/mms/pdu/CAcknowledgeInd.h"
#include "elastos/droid/google/mms/pdu/CReadOrigInd.h"
#include "elastos/droid/google/mms/pdu/CReadRecInd.h"
#include "elastos/droid/google/mms/pdu/PduHeaders.h"
#include "elastos/droid/google/mms/pdu/CPduBody.h"
#include "elastos/droid/google/mms/pdu/CPduPart.h"
#include "elastos/droid/google/mms/pdu/PduPart.h"
#include "elastos/droid/google/mms/pdu/CEncodedStringValue.h"
#include "elastos/droid/google/mms/pdu/PduContentTypes.h"
#include "elastos/droid/google/mms/pdu/Base64.h"
#include "elastos/droid/google/mms/pdu/QuotedPrintable.h"
#include "elastos/droid/google/mms/pdu/CCharacterSetsHelper.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/R.h"

#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::R;

using Elastos::Core::CoreUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::Arrays;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                              PduParser
//=====================================================================
CAR_INTERFACE_IMPL(PduParser, Object, IPduParser);

const Int32 PduParser::QUOTE;
const Int32 PduParser::LENGTH_QUOTE;
const Int32 PduParser::TEXT_MIN;
const Int32 PduParser::TEXT_MAX;
const Int32 PduParser::SHORT_INTEGER_MAX;
const Int32 PduParser::SHORT_LENGTH_MAX;
const Int32 PduParser::LONG_INTEGER_LENGTH_MAX;
const Int32 PduParser::QUOTED_STRING_FLAG;
const Int32 PduParser::END_STRING_FLAG;
const Int32 PduParser::TYPE_TEXT_STRING;
const Int32 PduParser::TYPE_QUOTED_STRING;
const Int32 PduParser::TYPE_TOKEN_STRING;
const Int32 PduParser::THE_FIRST_PART;
const Int32 PduParser::THE_LAST_PART;
AutoPtr<ArrayOf<Byte> > PduParser::mTypeParam = NULL;
AutoPtr<ArrayOf<Byte> > PduParser::mStartParam = NULL;
const String PduParser::LOGTAG("PduParser");
const Boolean PduParser::DEBUG = FALSE;
const Boolean PduParser::LOCAL_LOGV = FALSE;

PduParser::PduParser()
{
}

ECode PduParser::constructor(
    /* [in] */ ArrayOf<Byte>* pduDataStream)
{
    CByteArrayInputStream::New(pduDataStream, (IByteArrayInputStream**)&mPduDataStream);
    return NOERROR;
}

ECode PduParser::Parse(
    /* [out] */ IGenericPdu** result)
{
    VALIDATE_NOT_NULL(result);
    if (mPduDataStream == NULL) {
        *result = NULL;
        return NOERROR;
    }

    /* parse headers */
    mHeaders = ParseHeaders(mPduDataStream);
    if (NULL == mHeaders) {
        // Parse headers failed.
        *result = NULL;
        return NOERROR;
    }

    /* get the message type */
    Int32 messageType = 0;
    mHeaders->GetOctet(IPduHeaders::MESSAGE_TYPE, &messageType);

    /* check mandatory header fields */
    if (FALSE == CheckMandatoryHeader(mHeaders)) {
        Log(String("check mandatory headers failed!"));
        *result = NULL;
        return NOERROR;
    }

    if ((IPduHeaders::MESSAGE_TYPE_SEND_REQ == messageType) ||
            (IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF == messageType)) {
        /* need to parse the parts */
        mBody = ParseParts(mPduDataStream);
        if (NULL == mBody) {
            // Parse parts failed.
            *result = NULL;
            return NOERROR;
        }
    }

    switch (messageType) {
        case IPduHeaders::MESSAGE_TYPE_SEND_REQ: {
            if (LOCAL_LOGV) {
                Logger::V(LOGTAG, String("parse: MESSAGE_TYPE_SEND_REQ"));
            }
            AutoPtr<ISendReq> sendReq;
            CSendReq::New(mHeaders, mBody, (ISendReq**)&sendReq);
            *result = IGenericPdu::Probe(sendReq);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case IPduHeaders::MESSAGE_TYPE_SEND_CONF: {
            if (LOCAL_LOGV) {
                Logger::V(LOGTAG, String("parse: MESSAGE_TYPE_SEND_CONF"));
            }
            AutoPtr<ISendConf> sendConf;
            CSendConf::New(mHeaders, (ISendConf**)&sendConf);
            *result = IGenericPdu::Probe(sendConf);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case IPduHeaders::MESSAGE_TYPE_NOTIFICATION_IND: {
            if (LOCAL_LOGV) {
                Logger::V(LOGTAG, String("parse: MESSAGE_TYPE_NOTIFICATION_IND"));
            }
            AutoPtr<INotificationInd> notificationInd;
            CNotificationInd::New(mHeaders, (INotificationInd**)&notificationInd);
            *result = IGenericPdu::Probe(notificationInd);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case IPduHeaders::MESSAGE_TYPE_NOTIFYRESP_IND: {
            if (LOCAL_LOGV) {
                Logger::V(LOGTAG, String("parse: MESSAGE_TYPE_NOTIFYRESP_IND"));
            }
            AutoPtr<INotifyRespInd> notifyRespInd;
            CNotifyRespInd::New(mHeaders, (INotifyRespInd**)&notifyRespInd);
            *result = IGenericPdu::Probe(notifyRespInd);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF: {
            if (LOCAL_LOGV) {
                Logger::V(LOGTAG, String("parse: MESSAGE_TYPE_RETRIEVE_CONF"));
            }
            AutoPtr<IRetrieveConf> retrieveConf;
            CRetrieveConf::New(mHeaders, mBody, (IRetrieveConf**)&retrieveConf);

            AutoPtr<ArrayOf<Byte> > contentType;
            retrieveConf->GetContentType((ArrayOf<Byte>**)&contentType);
            if (NULL == contentType) {
                *result = NULL;
                return NOERROR;
            }
            String ctTypeStr(*contentType);
            if (ctTypeStr.Equals(IContentType::MULTIPART_MIXED)
                    || ctTypeStr.Equals(IContentType::MULTIPART_RELATED)
                    || ctTypeStr.Equals(IContentType::MULTIPART_ALTERNATIVE)) {
                // The MMS content type must be "application/vnd.wap.multipart.mixed"
                // or "application/vnd.wap.multipart.related"
                // or "application/vnd.wap.multipart.alternative"
                *result = IGenericPdu::Probe(retrieveConf);
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
            else if (ctTypeStr.Equals(IContentType::MULTIPART_ALTERNATIVE)) {
                // "application/vnd.wap.multipart.alternative"
                // should take only the first part.
                AutoPtr<IPduPart> firstPart;
                mBody->GetPart(0, (IPduPart**)&firstPart);
                mBody->RemoveAll();
                mBody->AddPart(0, firstPart);
                *result = IGenericPdu::Probe(retrieveConf);
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
            *result = NULL;
            return NOERROR;
        }
        case IPduHeaders::MESSAGE_TYPE_DELIVERY_IND: {
            if (LOCAL_LOGV) {
                Logger::V(LOGTAG, String("parse: MESSAGE_TYPE_DELIVERY_IND"));
            }
            AutoPtr<IDeliveryInd> deliveryInd;
            CDeliveryInd::New(mHeaders, (IDeliveryInd**)&deliveryInd);
            *result = IGenericPdu::Probe(deliveryInd);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case IPduHeaders::MESSAGE_TYPE_ACKNOWLEDGE_IND: {
            if (LOCAL_LOGV) {
                Logger::V(LOGTAG, String("parse: MESSAGE_TYPE_ACKNOWLEDGE_IND"));
            }
            AutoPtr<IAcknowledgeInd> acknowledgeInd;
            CAcknowledgeInd::New(mHeaders, (IAcknowledgeInd**)&acknowledgeInd);
            *result = IGenericPdu::Probe(acknowledgeInd);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case IPduHeaders::MESSAGE_TYPE_READ_ORIG_IND: {
            if (LOCAL_LOGV) {
                Logger::V(LOGTAG, String("parse: MESSAGE_TYPE_READ_ORIG_IND"));
            }
            AutoPtr<IReadOrigInd> readOrigInd;
            CReadOrigInd::New(mHeaders, (IReadOrigInd**)&readOrigInd);
            *result = IGenericPdu::Probe(readOrigInd);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case IPduHeaders::MESSAGE_TYPE_READ_REC_IND: {
            if (LOCAL_LOGV) {
                Logger::V(LOGTAG, String("parse: MESSAGE_TYPE_READ_REC_IND"));
            }
            AutoPtr<IReadRecInd> readRecInd;
            CReadRecInd::New(mHeaders, (IReadRecInd**)&readRecInd);
            *result = IGenericPdu::Probe(readRecInd);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        default: {
            Log(String("Parser doesn't support this message type in this version!"));
            *result = NULL;
            return NOERROR;
        }
    }
    return NOERROR;
}

AutoPtr<IPduHeaders> PduParser::ParseHeaders(
    /* [in] */ IByteArrayInputStream* pduDataStream)
{
    if (pduDataStream == NULL) {
        return NULL;
    }
    Boolean keepParsing = TRUE;
    AutoPtr<IPduHeaders> headers = new PduHeaders();

    Int32 available = 0;
    IInputStream::Probe(pduDataStream)->Available(&available);
    while (keepParsing && (available > 0)) {
        IInputStream::Probe(pduDataStream)->Mark(1);
        Int32 headerField = ExtractByteValue(pduDataStream);
        /* parse custom text header */
        if ((headerField >= TEXT_MIN) && (headerField <= TEXT_MAX)) {
            IInputStream::Probe(pduDataStream)->Reset();
            AutoPtr<ArrayOf<Byte> > bVal = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
            if (LOCAL_LOGV) {
                Logger::V(LOGTAG, String("TextHeader: ") + String(*bVal));
            }
            /* we should ignore it at the moment */
            continue;
        }
        switch (headerField) {
            case IPduHeaders::MESSAGE_TYPE: {
                Int32 messageType = ExtractByteValue(pduDataStream);
                if (LOCAL_LOGV) {
                    Logger::V(LOGTAG, String("parseHeaders: messageType: ") + StringUtils::ToString(messageType));
                }
                switch (messageType) {
                    // We don't support these kind of messages now.
                    case IPduHeaders::MESSAGE_TYPE_FORWARD_REQ:
                    case IPduHeaders::MESSAGE_TYPE_FORWARD_CONF:
                    case IPduHeaders::MESSAGE_TYPE_MBOX_STORE_REQ:
                    case IPduHeaders::MESSAGE_TYPE_MBOX_STORE_CONF:
                    case IPduHeaders::MESSAGE_TYPE_MBOX_VIEW_REQ:
                    case IPduHeaders::MESSAGE_TYPE_MBOX_VIEW_CONF:
                    case IPduHeaders::MESSAGE_TYPE_MBOX_UPLOAD_REQ:
                    case IPduHeaders::MESSAGE_TYPE_MBOX_UPLOAD_CONF:
                    case IPduHeaders::MESSAGE_TYPE_MBOX_DELETE_REQ:
                    case IPduHeaders::MESSAGE_TYPE_MBOX_DELETE_CONF:
                    case IPduHeaders::MESSAGE_TYPE_MBOX_DESCR:
                    case IPduHeaders::MESSAGE_TYPE_DELETE_REQ:
                    case IPduHeaders::MESSAGE_TYPE_DELETE_CONF:
                    case IPduHeaders::MESSAGE_TYPE_CANCEL_REQ:
                    case IPduHeaders::MESSAGE_TYPE_CANCEL_CONF:
                        return NULL;
                }
                // try {
                headers->SetOctet(messageType, headerField);
                // } catch(InvalidHeaderValueException e) {
                //     Log("Set invalid Octet value: " + messageType +
                //             " into the header filed: " + headerField);
                //     return NULL;
                // } catch(RuntimeException e) {
                //     Log(headerField + "is not Octet header field!");
                //     return NULL;
                // }
                break;
            }
            /* Octect value */
            case IPduHeaders::REPORT_ALLOWED:
            case IPduHeaders::ADAPTATION_ALLOWED:
            case IPduHeaders::DELIVERY_REPORT:
            case IPduHeaders::DRM_CONTENT:
            case IPduHeaders::DISTRIBUTION_INDICATOR:
            case IPduHeaders::QUOTAS:
            case IPduHeaders::READ_REPORT:
            case IPduHeaders::STORE:
            case IPduHeaders::STORED:
            case IPduHeaders::TOTALS:
            case IPduHeaders::SENDER_VISIBILITY:
            case IPduHeaders::READ_STATUS:
            case IPduHeaders::CANCEL_STATUS:
            case IPduHeaders::PRIORITY:
            case IPduHeaders::STATUS:
            case IPduHeaders::REPLY_CHARGING:
            case IPduHeaders::MM_STATE:
            case IPduHeaders::RECOMMENDED_RETRIEVAL_MODE:
            case IPduHeaders::CONTENT_CLASS:
            case IPduHeaders::RETRIEVE_STATUS:
            case IPduHeaders::STORE_STATUS:
                /**
                 * The following field has a different value when
                 * used in the M-Mbox-Delete.conf and M-Delete.conf PDU.
                 * For now we ignore this fact, since we do not support these PDUs
                 */
            case IPduHeaders::RESPONSE_STATUS: {
                Int32 value = ExtractByteValue(pduDataStream);
                if (LOCAL_LOGV) {
                    Logger::V(LOGTAG, String("parseHeaders: byte: ") + StringUtils::ToString(headerField) +
                            String(" value: ") + StringUtils::ToString(value));
                }

                // try {
                headers->SetOctet(value, headerField);
                // } catch(InvalidHeaderValueException e) {
                //     Log("Set invalid Octet value: " + value +
                //             " into the header filed: " + headerField);
                //     return NULL;
                // } catch(RuntimeException e) {
                //     Log(headerField + "is not Octet header field!");
                //     return NULL;
                // }
                break;
            }

            /* Long-Integer */
            case IPduHeaders::DATE:
            case IPduHeaders::REPLY_CHARGING_SIZE:
            case IPduHeaders::MESSAGE_SIZE: {
                // try {
                    Int64 value = ParseLongInteger(pduDataStream);
                    if (LOCAL_LOGV) {
                        Logger::V(LOGTAG, String("parseHeaders: longint: ") + StringUtils::ToString(headerField) +
                                String(" value: ") + StringUtils::ToString(value));
                    }
                    headers->SetLongInteger(value, headerField);
                // } catch(RuntimeException e) {
                //     Log(headerField + "is not Long-Integer header field!");
                //     return NULL;
                // }
                break;
            }

            /* Integer-Value */
            case IPduHeaders::MESSAGE_COUNT:
            case IPduHeaders::START:
            case IPduHeaders::LIMIT: {
                // try {
                    Int64 value = ParseIntegerValue(pduDataStream);
                    if (LOCAL_LOGV) {
                        Logger::V(LOGTAG, String("parseHeaders: Int32: ") + StringUtils::ToString(headerField) +
                                String(" value: ") + StringUtils::ToString(value));
                    }
                    headers->SetLongInteger(value, headerField);
                // } catch(RuntimeException e) {
                //     Log(headerField + "is not Long-Integer header field!");
                //     return NULL;
                // }
                break;
            }

            /* Text-String */
            case IPduHeaders::TRANSACTION_ID:
            case IPduHeaders::REPLY_CHARGING_ID:
            case IPduHeaders::AUX_APPLIC_ID:
            case IPduHeaders::APPLIC_ID:
            case IPduHeaders::REPLY_APPLIC_ID:
                /**
                 * The next three header fields are email addresses
                 * as defined in RFC2822,
                 * not including the characters "<" and ">"
                 */
            case IPduHeaders::MESSAGE_ID:
            case IPduHeaders::REPLACE_ID:
            case IPduHeaders::CANCEL_ID:
                /**
                 * The following field has a different value when
                 * used in the M-Mbox-Delete.conf and M-Delete.conf PDU.
                 * For now we ignore this fact, since we do not support these PDUs
                 */
            case IPduHeaders::CONTENT_LOCATION: {
                AutoPtr<ArrayOf<Byte> > value = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                if (NULL != value) {
                    // try {
                        if (LOCAL_LOGV) {
                            Logger::V(LOGTAG, String("parseHeaders: string: ") + StringUtils::ToString(headerField) +
                                    String(" value: ") + String(*value));
                        }
                        headers->SetTextString(value, headerField);
                    // } catch(NullPointerException e) {
                    //     Log("NULL pointer error!");
                    // } catch(RuntimeException e) {
                    //     Log(headerField + "is not Text-String header field!");
                    //     return NULL;
                    // }
                }
                break;
            }

            /* Encoded-string-value */
            case IPduHeaders::SUBJECT:
            case IPduHeaders::RECOMMENDED_RETRIEVAL_MODE_TEXT:
            case IPduHeaders::RETRIEVE_TEXT:
            case IPduHeaders::STATUS_TEXT:
            case IPduHeaders::STORE_STATUS_TEXT:
                /* the next one is not support
                 * M-Mbox-Delete.conf and M-Delete.conf now */
            case IPduHeaders::RESPONSE_TEXT: {
                AutoPtr<IEncodedStringValue> value =
                    ParseEncodedStringValue(pduDataStream);
                if (NULL != value) {
                    // try {
                        if (LOCAL_LOGV) {
                            String str;
                            value->GetString(&str);
                            Logger::V(LOGTAG, String("parseHeaders: encoded string: ") + StringUtils::ToString(headerField)
                                    + String(" value: ") + str);
                        }
                        headers->SetEncodedStringValue(value, headerField);
                    // } catch(NullPointerException e) {
                    //     Log("NULL pointer error!");
                    // } catch (RuntimeException e) {
                    //     Log(headerField + "is not Encoded-String-Value header field!");
                    //     return NULL;
                    // }
                }
                break;
            }

            /* Addressing model */
            case IPduHeaders::BCC:
            case IPduHeaders::CC:
            case IPduHeaders::TO: {
                AutoPtr<IEncodedStringValue> value =
                    ParseEncodedStringValue(pduDataStream);
                if (NULL != value) {
                    AutoPtr<ArrayOf<Byte> > address;
                    value->GetTextString((ArrayOf<Byte>**)&address);
                    if (NULL != address) {
                        String str(*address);
                        if (LOCAL_LOGV) {
                            Logger::V(LOGTAG, String("parseHeaders: (to/cc/bcc) address: ") + StringUtils::ToString(headerField)
                                    + String(" value: ") + str);
                        }
                        Int32 endIndex = str.IndexOf("/");
                        if (endIndex > 0) {
                            str = str.Substring(0, endIndex);
                        }
                        // try {
                            value->SetTextString(str.GetBytes());
                        // } catch(NullPointerException e) {
                        //     Log("NULL pointer error!");
                        //     return NULL;
                        // }
                    }

                    // try {
                        headers->AppendEncodedStringValue(value, headerField);
                    // } catch(NullPointerException e) {
                    //     Log("NULL pointer error!");
                    // } catch(RuntimeException e) {
                    //     Log(headerField + "is not Encoded-String-Value header field!");
                    //     return NULL;
                    // }
                }
                break;
            }

            /* Value-length
             * (Absolute-token Date-value | Relative-token Delta-seconds-value) */
            case IPduHeaders::DELIVERY_TIME:
            case IPduHeaders::EXPIRY:
            case IPduHeaders::REPLY_CHARGING_DEADLINE: {
                /* parse Value-length */
                ParseValueLength(pduDataStream);

                /* Absolute-token or Relative-token */
                Int32 token = ExtractByteValue(pduDataStream);

                /* Date-value or Delta-seconds-value */
                Int64 timeValue = 0;
                // try {
                    timeValue = ParseLongInteger(pduDataStream);
                // } catch(RuntimeException e) {
                //     Log(headerField + "is not Long-Integer header field!");
                //     return NULL;
                // }
                if (IPduHeaders::VALUE_RELATIVE_TOKEN == token) {
                    /* need to convert the Delta-seconds-value
                     * into Date-value */
                    AutoPtr<ISystem> sys;
                    CSystem::AcquireSingleton((ISystem**)&sys);
                    Int64 mills = 0;
                    sys->GetCurrentTimeMillis(&mills);
                    timeValue = mills / 1000 + timeValue;
                }

                // try {
                    if (LOCAL_LOGV) {
                        Logger::V(LOGTAG, String("parseHeaders: time value: ") + StringUtils::ToString(headerField)
                                + String(" value: ") + StringUtils::ToString(timeValue));
                    }
                    headers->SetLongInteger(timeValue, headerField);
                // } catch(RuntimeException e) {
                //     Log(headerField + "is not Long-Integer header field!");
                //     return NULL;
                // }
                break;
            }

            case IPduHeaders::FROM: {
                /* From-value =
                 * Value-length
                 * (Address-present-token Encoded-string-value | Insert-address-token)
                 */
                AutoPtr<IEncodedStringValue> from;
                ParseValueLength(pduDataStream); /* parse value-length */

                /* Address-present-token or Insert-address-token */
                Int32 fromToken = ExtractByteValue(pduDataStream);

                /* Address-present-token or Insert-address-token */
                if (IPduHeaders::FROM_ADDRESS_PRESENT_TOKEN == fromToken) {
                    /* Encoded-string-value */
                    from = ParseEncodedStringValue(pduDataStream);
                    if (NULL != from) {
                        AutoPtr<ArrayOf<Byte> > address;
                        from->GetTextString((ArrayOf<Byte>**)&address);
                        if (NULL != address) {
                            String str(*address);
                            Int32 endIndex = str.IndexOf("/");
                            if (endIndex > 0) {
                                str = str.Substring(0, endIndex);
                            }
                            // try {
                            from->SetTextString(str.GetBytes());
                            // } catch(NullPointerException e) {
                            //     Log("NULL pointer error!");
                            //     return NULL;
                            // }
                        }
                    }
                }
                else {
                    // try {
                        CEncodedStringValue::New(
                                IPduHeaders::FROM_INSERT_ADDRESS_TOKEN_STR.GetBytes(),
                                (IEncodedStringValue**)&from);
                    // } catch(NullPointerException e) {
                    //     Log(headerField + "is not Encoded-String-Value header field!");
                    //     return NULL;
                    // }
                }

                // try {
                    if (LOCAL_LOGV) {
                        String str;
                        from->GetString(&str);
                        Logger::V(LOGTAG, String("parseHeaders: from address: ") + StringUtils::ToString(headerField)
                                + String(" value: ") + str);
                    }
                    headers->SetEncodedStringValue(from, IPduHeaders::FROM);
                // } catch(NullPointerException e) {
                //     Log("NULL pointer error!");
                // } catch(RuntimeException e) {
                //     Log(headerField + "is not Encoded-String-Value header field!");
                //     return NULL;
                // }
                break;
            }

            case IPduHeaders::MESSAGE_CLASS: {
                /* Message-class-value = Class-identifier | Token-text */
                IInputStream::Probe(pduDataStream)->Mark(1);
                Int32 messageClass = ExtractByteValue(pduDataStream);
                if (LOCAL_LOGV) {
                    Logger::V(LOGTAG, String("parseHeaders: MESSAGE_CLASS: ") + StringUtils::ToString(headerField)
                            + String(" value: ") + messageClass);
                }

                if (messageClass >= IPduHeaders::MESSAGE_CLASS_PERSONAL) {
                    /* Class-identifier */
                    // try {
                        if (IPduHeaders::MESSAGE_CLASS_PERSONAL == messageClass) {
                            headers->SetTextString(
                                    IPduHeaders::MESSAGE_CLASS_PERSONAL_STR.GetBytes(),
                                    IPduHeaders::MESSAGE_CLASS);
                        }
                        else if (IPduHeaders::MESSAGE_CLASS_ADVERTISEMENT == messageClass) {
                            headers->SetTextString(
                                    IPduHeaders::MESSAGE_CLASS_ADVERTISEMENT_STR.GetBytes(),
                                    IPduHeaders::MESSAGE_CLASS);
                        }
                        else if (IPduHeaders::MESSAGE_CLASS_INFORMATIONAL == messageClass) {
                            headers->SetTextString(
                                    IPduHeaders::MESSAGE_CLASS_INFORMATIONAL_STR.GetBytes(),
                                    IPduHeaders::MESSAGE_CLASS);
                        }
                        else if (IPduHeaders::MESSAGE_CLASS_AUTO == messageClass) {
                            headers->SetTextString(
                                    IPduHeaders::MESSAGE_CLASS_AUTO_STR.GetBytes(),
                                    IPduHeaders::MESSAGE_CLASS);
                        }
                    // } catch(NullPointerException e) {
                    //     Log("NULL pointer error!");
                    // } catch(RuntimeException e) {
                    //     Log(headerField + "is not Text-String header field!");
                    //     return NULL;
                    // }
                }
                else {
                    /* Token-text */
                    IInputStream::Probe(pduDataStream)->Reset();
                    AutoPtr<ArrayOf<Byte> > messageClassString = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                    if (NULL != messageClassString) {
                        // try {
                            headers->SetTextString(messageClassString, IPduHeaders::MESSAGE_CLASS);
                        // } catch(NullPointerException e) {
                        //     Log("NULL pointer error!");
                        // } catch(RuntimeException e) {
                        //     Log(headerField + "is not Text-String header field!");
                        //     return NULL;
                        // }
                    }
                }
                break;
            }

            case IPduHeaders::MMS_VERSION: {
                Int32 version = ParseShortInteger(pduDataStream);

                // try {
                    if (LOCAL_LOGV) {
                        Logger::V(LOGTAG, String("parseHeaders: MMS_VERSION: ") + StringUtils::ToString(headerField)
                                + String(" value: ") + StringUtils::ToString(version));
                    }
                    headers->SetOctet(version, IPduHeaders::MMS_VERSION);
                // } catch(InvalidHeaderValueException e) {
                //     Log("Set invalid Octet value: " + version +
                //             " into the header filed: " + headerField);
                //     return NULL;
                // } catch(RuntimeException e) {
                //     Log(headerField + "is not Octet header field!");
                //     return NULL;
                // }
                break;
            }

            case IPduHeaders::PREVIOUSLY_SENT_BY: {
                /* Previously-sent-by-value =
                 * Value-length Forwarded-count-value Encoded-string-value */
                /* parse value-length */
                ParseValueLength(pduDataStream);

                /* parse Forwarded-count-value */
                // try {
                ParseIntegerValue(pduDataStream);
                // } catch(RuntimeException e) {
                //     Log(headerField + " is not Integer-Value");
                //     return NULL;
                // }

                /* parse Encoded-string-value */
                AutoPtr<IEncodedStringValue> previouslySentBy =
                    ParseEncodedStringValue(pduDataStream);
                if (NULL != previouslySentBy) {
                    // try {
                        if (LOCAL_LOGV) {
                            String str;
                            previouslySentBy->GetString(&str);
                            Logger::V(LOGTAG, String("parseHeaders: PREVIOUSLY_SENT_BY: ") + StringUtils::ToString(headerField)
                                    + String(" value: ") + str);
                        }
                        headers->SetEncodedStringValue(previouslySentBy,
                                IPduHeaders::PREVIOUSLY_SENT_BY);
                    // } catch(NullPointerException e) {
                    //     Log("NULL pointer error!");
                    // } catch(RuntimeException e) {
                    //     Log(headerField + "is not Encoded-String-Value header field!");
                    //     return NULL;
                    // }
                }
                break;
            }

            case IPduHeaders::PREVIOUSLY_SENT_DATE: {
                /* Previously-sent-date-value =
                 * Value-length Forwarded-count-value Date-value */
                /* parse value-length */
                ParseValueLength(pduDataStream);

                /* parse Forwarded-count-value */
                // try {
                    ParseIntegerValue(pduDataStream);
                // } catch(RuntimeException e) {
                //     Log(headerField + " is not Integer-Value");
                //     return NULL;
                // }

                /* Date-value */
                // try {
                    Int64 perviouslySentDate = ParseLongInteger(pduDataStream);
                    if (LOCAL_LOGV) {
                        Logger::V(LOGTAG, String("parseHeaders: PREVIOUSLY_SENT_DATE: ") + StringUtils::ToString(headerField)
                                + String(" value: ") + StringUtils::ToString(perviouslySentDate));
                    }
                    headers->SetLongInteger(perviouslySentDate,
                            IPduHeaders::PREVIOUSLY_SENT_DATE);
                // } catch(RuntimeException e) {
                //     Log(headerField + "is not Long-Integer header field!");
                //     return NULL;
                // }
                break;
            }

            case IPduHeaders::MM_FLAGS: {
                /* MM-flags-value =
                 * Value-length
                 * ( Add-token | Remove-token | Filter-token )
                 * Encoded-string-value
                 */
                if (LOCAL_LOGV) {
                    Logger::V(LOGTAG, String("parseHeaders: MM_FLAGS: ") + StringUtils::ToString(headerField)
                            + String(" NOT REALLY SUPPORTED"));
                }

                /* parse Value-length */
                ParseValueLength(pduDataStream);

                /* Add-token | Remove-token | Filter-token */
                ExtractByteValue(pduDataStream);

                /* Encoded-string-value */
                ParseEncodedStringValue(pduDataStream);

                /* not store this header filed in "headers",
                 * because now PduHeaders doesn't support it */
                break;
            }

            /* Value-length
             * (Message-total-token | Size-total-token) Integer-Value */
            case IPduHeaders::MBOX_TOTALS:
            case IPduHeaders::MBOX_QUOTAS: {
                if (LOCAL_LOGV) {
                    Logger::V(LOGTAG, String("parseHeaders: MBOX_TOTALS: ") + StringUtils::ToString(headerField));
                }
                /* Value-length */
                ParseValueLength(pduDataStream);

                /* Message-total-token | Size-total-token */
                ExtractByteValue(pduDataStream);

                /*Integer-Value*/
                // try {
                ParseIntegerValue(pduDataStream);
                // } catch(RuntimeException e) {
                //     Log(headerField + " is not Integer-Value");
                //     return NULL;
                // }

                /* not store these headers filed in "headers",
                because now PduHeaders doesn't support them */
                break;
            }

            case IPduHeaders::ELEMENT_DESCRIPTOR: {
                if (LOCAL_LOGV) {
                    Logger::V(LOGTAG, String("parseHeaders: ELEMENT_DESCRIPTOR: ") + StringUtils::ToString(headerField));
                }
                ParseContentType(pduDataStream, NULL);

                /* not store this header filed in "headers",
                because now PduHeaders doesn't support it */
                break;
            }

            case IPduHeaders::CONTENT_TYPE: {
                AutoPtr<IHashMap> map;
                CHashMap::New((IHashMap**)&map);
                AutoPtr<ArrayOf<Byte> > contentType =
                    ParseContentType(pduDataStream, map);

                if (NULL != contentType) {
                    // try {
                        if (LOCAL_LOGV) {
                            Logger::V(LOGTAG, String("parseHeaders: CONTENT_TYPE: ") + StringUtils::ToString(headerField) +
                                    String(*contentType));
                        }
                        headers->SetTextString(contentType, IPduHeaders::CONTENT_TYPE);
                    // } catch(NullPointerException e) {
                    //     Log("NULL pointer error!");
                    // } catch(RuntimeException e) {
                    //     Log(headerField + "is not Text-String header field!");
                    //     return NULL;
                    // }
                }

                /* get start parameter */
                AutoPtr<IInterface> pStart;
                map->Get(CoreUtils::Convert(IPduPart::P_START), (IInterface**)&pStart);
                AutoPtr<IArrayOf> arrayStart = IArrayOf::Probe(pStart);
                Int32 len = 0;
                arrayStart->GetLength(&len);
                mStartParam = ArrayOf<Byte>::Alloc(len);
                for (Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> obj;
                    arrayStart->Get(i, (IInterface**)&obj);
                    IByte::Probe(obj)->GetValue(&(*mStartParam)[i]);
                }
                /* get charset parameter */
                AutoPtr<IInterface> pType;
                map->Get(CoreUtils::Convert(IPduPart::P_TYPE), (IInterface**)&pType);
                AutoPtr<IArrayOf> arrayType = IArrayOf::Probe(pType);
                len = 0;
                arrayType->GetLength(&len);
                mTypeParam = ArrayOf<Byte>::Alloc(len);
                for (Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> obj;
                    arrayType->Get(i, (IInterface**)&obj);
                    IByte::Probe(obj)->GetValue(&(*mTypeParam)[i]);
                }

                keepParsing = FALSE;
                break;
            }

            case IPduHeaders::CONTENT:
            case IPduHeaders::ADDITIONAL_HEADERS:
            case IPduHeaders::ATTRIBUTES:
            default: {
                if (LOCAL_LOGV) {
                    Logger::V(LOGTAG, String("parseHeaders: Unknown header: ") + StringUtils::ToString(headerField));
                }
                Log(String("Unknown header"));
            }
        }
    }

    return headers;
}

AutoPtr<IPduBody> PduParser::ParseParts(
    /* [in] */ IByteArrayInputStream* pduDataStream)
{
    if (pduDataStream == NULL) {
        return NULL;
    }

    Int32 count = ParseUnsignedInt(pduDataStream); // get the number of parts
    AutoPtr<IPduBody> body;
    CPduBody::New((IPduBody**)&body);

    for (Int32 i = 0; i < count; i++) {
        Int32 headerLength = ParseUnsignedInt(pduDataStream);
        Int32 dataLength = ParseUnsignedInt(pduDataStream);
        AutoPtr<IPduPart> part;
        CPduPart::New((IPduPart**)&part);
        Int32 startPos = 0;
        IInputStream::Probe(pduDataStream)->Available(&startPos);
        if (startPos <= 0) {
            // Invalid part.
            return NULL;
        }

        /* parse part's content-type */
        AutoPtr<IHashMap> map;
        CHashMap::New((IHashMap**)&map);
        AutoPtr<ArrayOf<Byte> > contentType = ParseContentType(pduDataStream, map);
        if (NULL != contentType) {
            part->SetContentType(contentType);
        }
        else {
            part->SetContentType(((*(PduContentTypes::sContentTypes))[0]).GetBytes()); //"*/*"
        }

        /* get name parameter */
        AutoPtr<IInterface> pName;
        map->Get(CoreUtils::Convert(IPduPart::P_NAME), (IInterface**)&pName);
        AutoPtr<IArrayOf> array = IArrayOf::Probe(pName);
        Int32 len = 0;
        array->GetLength(&len);
        AutoPtr<ArrayOf<Byte> > name = ArrayOf<Byte>::Alloc(len);
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> obj;
            array->Get(i, (IInterface**)&obj);
            IByte::Probe(obj)->GetValue(&(*name)[i]);
        }
        if (NULL != name) {
            part->SetName(name);
        }

        /* get charset parameter */
        AutoPtr<IInterface> p;
        map->Get(CoreUtils::Convert(IPduPart::P_CHARSET), (IInterface**)&p);
        AutoPtr<IInteger32> charset = IInteger32::Probe(p);
        if (NULL != charset) {
            Int32 value = 0;
            charset->GetValue(&value);
            part->SetCharset(value);
        }

        /* parse part's headers */
        Int32 endPos = 0;
        IInputStream::Probe(pduDataStream)->Available(&endPos);
        Int32 partHeaderLen = headerLength - (startPos - endPos);
        if (partHeaderLen > 0) {
            if (FALSE == ParsePartHeaders(pduDataStream, part, partHeaderLen)) {
                // Parse part header faild.
                return NULL;
            }
        }
        else if (partHeaderLen < 0) {
            // Invalid length of content-type.
            return NULL;
        }

        /* FIXME: check content-id, name, filename and content location,
         * if not set anyone of them, generate a default content-location
         */
        AutoPtr<ArrayOf<Byte> > pLocation;
        part->GetContentLocation((ArrayOf<Byte>**)&pLocation);
        AutoPtr<ArrayOf<Byte> > pArrName;
        part->GetName((ArrayOf<Byte>**)&pArrName);
        AutoPtr<ArrayOf<Byte> > pFilename;
        part->GetFilename((ArrayOf<Byte>**)&pFilename);
        AutoPtr<ArrayOf<Byte> > pContentId;
        part->GetContentId((ArrayOf<Byte>**)&pContentId);
        if ((NULL == pLocation)
                && (NULL == pArrName)
                && (NULL == pFilename)
                && (NULL == pContentId)) {
            AutoPtr<ISystem> sys;
            CSystem::AcquireSingleton((ISystem**)&sys);
            Int64 mills = 0;
            sys->GetCurrentTimeMillis(&mills);
            part->SetContentLocation(StringUtils::ToOctalString(
                    mills).GetBytes());
        }

        /* get part's data */
        if (dataLength > 0) {
            AutoPtr<ArrayOf<Byte> > partData = ArrayOf<Byte>::Alloc(dataLength);
            AutoPtr<ArrayOf<Byte> > pContentType;
            part->GetContentType((ArrayOf<Byte>**)&pContentType);
            String partContentType(*pContentType);
            Int32 res = 0;
            IInputStream::Probe(pduDataStream)->Read(partData, 0, dataLength, &res);
            if (partContentType.EqualsIgnoreCase(IContentType::MULTIPART_ALTERNATIVE)) {
                // parse "multipart/vnd.wap.multipart.alternative".
                AutoPtr<IByteArrayInputStream> p;
                CByteArrayInputStream::New(partData, (IByteArrayInputStream**)&p);
                AutoPtr<IPduBody> childBody = ParseParts(p);
                // take the first part of children.
                childBody->GetPart(0, (IPduPart**)&part);
            }
            else {
                // Check Content-Transfer-Encoding.
                AutoPtr<ArrayOf<Byte> > partDataEncoding;
                part->GetContentTransferEncoding((ArrayOf<Byte>**)&partDataEncoding);
                if (NULL != partDataEncoding) {
                    String encoding(*partDataEncoding);
                    if (encoding.EqualsIgnoreCase(IPduPart::P_BASE64)) {
                        // Decode "base64" into "binary".
                        partData = Base64::DecodeBase64(partData);
                    }
                    else if (encoding.EqualsIgnoreCase(IPduPart::P_QUOTED_PRINTABLE)) {
                        // Decode "quoted-printable" into "binary".
                        partData = QuotedPrintable::DecodeQuotedPrintable(partData);
                    }
                    else {
                        // "binary" is the default encoding.
                    }
                }
                if (NULL == partData) {
                    Log(String("Decode part data error!"));
                    return NULL;
                }
                part->SetData(partData);
            }
        }

        /* add this part to body */
        if (THE_FIRST_PART == CheckPartPosition(part)) {
            /* this is the first part */
            body->AddPart(0, part);
        }
        else {
            /* add the part to the end */
            Boolean b = FALSE;
            body->AddPart(part, &b);
        }
    }

    return body;
}

Int32 PduParser::ParseUnsignedInt(
    /* [in] */ IByteArrayInputStream* pduDataStream)
{
    /**
     * From wap-230-wsp-20010705-a.pdf
     * The maximum size of a uintvar is 32 bits.
     * So it will be encoded in no more than 5 octets.
     */
    assert(NULL != pduDataStream);
    Int32 result = 0;
    Int32 temp = 0;
    IInputStream::Probe(pduDataStream)->Read(&temp);
    if (temp == -1) {
        return temp;
    }

    while((temp & 0x80) != 0) {
        result = result << 7;
        result |= temp & 0x7F;
        IInputStream::Probe(pduDataStream)->Read(&temp);
        if (temp == -1) {
            return temp;
        }
    }

    result = result << 7;
    result |= temp & 0x7F;

    return result;
}

Int32 PduParser::ParseValueLength(
    /* [in] */ IByteArrayInputStream* pduDataStream)
{
    /**
     * From wap-230-wsp-20010705-a.pdf
     * Value-length = Short-length | (Length-quote Length)
     * Short-length = <Any octet 0-30>
     * Length-quote = <Octet 31>
     * Length = Uintvar-integer
     * Uintvar-integer = 1*5 OCTET
     */
    assert(NULL != pduDataStream);
    Int32 temp = 0;
    IInputStream::Probe(pduDataStream)->Read(&temp);
    assert(-1 != temp);
    Int32 first = temp & 0xFF;

    if (first <= SHORT_LENGTH_MAX) {
        return first;
    }
    else if (first == LENGTH_QUOTE) {
        return ParseUnsignedInt(pduDataStream);
    }

    // throw new RuntimeException ("Value length > LENGTH_QUOTE!");
    return -1;
}

AutoPtr<IEncodedStringValue> PduParser::ParseEncodedStringValue(
    /* [in] */ IByteArrayInputStream* pduDataStream)
{
    /**
     * From OMA-TS-MMS-ENC-V1_3-20050927-C.pdf
     * Encoded-string-value = Text-string | Value-length Char-set Text-string
     */
    assert(NULL != pduDataStream);
    IInputStream::Probe(pduDataStream)->Mark(1);
    AutoPtr<IEncodedStringValue> returnValue;
    Int32 charset = 0;
    Int32 temp = 0;
    IInputStream::Probe(pduDataStream)->Read(&temp);
    assert(-1 != temp);
    Int32 first = temp & 0xFF;
    if (first == 0) {
        AutoPtr<IEncodedStringValue> p;
        CEncodedStringValue::New(String(""), (IEncodedStringValue**)&p);
        return p;
    }

    IInputStream::Probe(pduDataStream)->Reset();
    if (first < TEXT_MIN) {
        ParseValueLength(pduDataStream);

        charset = ParseShortInteger(pduDataStream); //get the "Charset"
    }

    AutoPtr<ArrayOf<Byte> > textString = ParseWapString(pduDataStream, TYPE_TEXT_STRING);

    // try {
    if (0 != charset) {
        CEncodedStringValue::New(charset, textString, (IEncodedStringValue**)&returnValue);
    }
    else {
        CEncodedStringValue::New(textString, (IEncodedStringValue**)&returnValue);
    }
    // } catch(Exception e) {
    //     return NULL;
    // }

    return returnValue;
}

AutoPtr<ArrayOf<Byte> > PduParser::ParseWapString(
    /* [in] */ IByteArrayInputStream* pduDataStream,
    /* [in] */ Int32 stringType)
{
    assert(NULL != pduDataStream);
    /**
     * From wap-230-wsp-20010705-a.pdf
     * Text-string = [Quote] *TEXT End-of-string
     * If the first character in the TEXT is in the range of 128-255,
     * a Quote character must precede it.
     * Otherwise the Quote character must be omitted.
     * The Quote is not part of the contents.
     * Quote = <Octet 127>
     * End-of-string = <Octet 0>
     *
     * Quoted-string = <Octet 34> *TEXT End-of-string
     *
     * Token-text = Token End-of-string
     */

    // Mark supposed beginning of Text-string
    // We will have to mark again if first char is QUOTE or QUOTED_STRING_FLAG
    IInputStream::Probe(pduDataStream)->Mark(1);

    // Check first char
    Int32 temp = 0;
    IInputStream::Probe(pduDataStream)->Read(&temp);
    assert(-1 != temp);
    if ((TYPE_QUOTED_STRING == stringType) &&
            (QUOTED_STRING_FLAG == temp)) {
        // Mark again if QUOTED_STRING_FLAG and ignore it
        IInputStream::Probe(pduDataStream)->Mark(1);
    }
    else if ((TYPE_TEXT_STRING == stringType) &&
            (QUOTE == temp)) {
        // Mark again if QUOTE and ignore it
        IInputStream::Probe(pduDataStream)->Mark(1);
    }
    else {
        // Otherwise go back to origin
        IInputStream::Probe(pduDataStream)->Reset();
    }

    // We are now definitely at the beginning of string
    /**
     * Return *TOKEN or *TEXT (Text-String without QUOTE,
     * Quoted-String without QUOTED_STRING_FLAG and without End-of-string)
     */
    return GetWapString(pduDataStream, stringType);
}

Boolean PduParser::IsTokenCharacter(
    /* [in] */ Int32 ch)
{
    /**
     * Token      = 1*<any CHAR except CTLs or separators>
     * separators = "("(40) | ")"(41) | "<"(60) | ">"(62) | "@"(64)
     *            | ","(44) | ";"(59) | ":"(58) | "\"(92) | <">(34)
     *            | "/"(47) | "["(91) | "]"(93) | "?"(63) | "="(61)
     *            | "{"(123) | "}"(125) | SP(32) | HT(9)
     * CHAR       = <any US-ASCII character (octets 0 - 127)>
     * CTL        = <any US-ASCII control character
     *            (octets 0 - 31) and DEL (127)>
     * SP         = <US-ASCII SP, space (32)>
     * HT         = <US-ASCII HT, horizontal-tab (9)>
     */
    if ((ch < 33) || (ch > 126)) {
        return FALSE;
    }

    switch(ch) {
        case '"': /* '"' */
        case '(': /* '(' */
        case ')': /* ')' */
        case ',': /* ',' */
        case '/': /* '/' */
        case ':': /* ':' */
        case ';': /* ';' */
        case '<': /* '<' */
        case '=': /* '=' */
        case '>': /* '>' */
        case '?': /* '?' */
        case '@': /* '@' */
        case '[': /* '[' */
        case '\\': /* '\' */
        case ']': /* ']' */
        case '{': /* '{' */
        case '}': /* '}' */
            return FALSE;
    }

    return TRUE;
}

Boolean PduParser::IsText(
    /* [in] */ Int32 ch)
{
    /**
     * TEXT = <any OCTET except CTLs,
     *      but including LWS>
     * CTL  = <any US-ASCII control character
     *      (octets 0 - 31) and DEL (127)>
     * LWS  = [CRLF] 1*( SP | HT )
     * CRLF = CR LF
     * CR   = <US-ASCII CR, carriage return (13)>
     * LF   = <US-ASCII LF, linefeed (10)>
     */
    if (((ch >= 32) && (ch <= 126)) || ((ch >= 128) && (ch <= 255))) {
        return TRUE;
    }

    switch(ch) {
        case '\t': /* '\t' */
        case '\n': /* '\n' */
        case '\r': /* '\r' */
            return TRUE;
    }

    return FALSE;
}

AutoPtr<ArrayOf<Byte> > PduParser::GetWapString(
    /* [in] */ IByteArrayInputStream* pduDataStream,
    /* [in] */ Int32 stringType)
{
    assert(NULL != pduDataStream);
    AutoPtr<IByteArrayOutputStream> out;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&out);
    Int32 temp = 0;
    IInputStream::Probe(pduDataStream)->Read(&temp);
    assert(-1 != temp);
    while((-1 != temp) && ('\0' != temp)) {
        // check each of the character
        if (stringType == TYPE_TOKEN_STRING) {
            if (IsTokenCharacter(temp)) {
                IOutputStream::Probe(out)->Write(temp);
            }
        }
        else {
            if (IsText(temp)) {
                IOutputStream::Probe(out)->Write(temp);
            }
        }

        IInputStream::Probe(pduDataStream)->Read(&temp);
        assert(-1 != temp);
    }

    Int32 size = 0;
    out->GetSize(&size);
    if (size > 0) {
        AutoPtr<ArrayOf<Byte> > res;
        out->ToByteArray((ArrayOf<Byte>**)&res);
        return res;
    }

    return NULL;
}

Int32 PduParser::ExtractByteValue(
    /* [in] */ IByteArrayInputStream* pduDataStream)
{
    assert(NULL != pduDataStream);
    Int32 temp = 0;
    IInputStream::Probe(pduDataStream)->Read(&temp);
    assert(-1 != temp);
    return temp & 0xFF;
}

Int32 PduParser::ParseShortInteger(
    /* [in] */ IByteArrayInputStream* pduDataStream)
{
    /**
     * From wap-230-wsp-20010705-a.pdf
     * Short-integer = OCTET
     * Integers in range 0-127 shall be encoded as a one
     * octet value with the most significant bit set to one (1xxx xxxx)
     * and with the value in the remaining least significant bits.
     */
    assert(NULL != pduDataStream);
    Int32 temp = 0;
    IInputStream::Probe(pduDataStream)->Read(&temp);
    assert(-1 != temp);
    return temp & 0x7F;
}

Int64 PduParser::ParseLongInteger(
    /* [in] */ IByteArrayInputStream* pduDataStream)
{
    /**
     * From wap-230-wsp-20010705-a.pdf
     * Long-integer = Short-length Multi-octet-integer
     * The Short-length indicates the length of the Multi-octet-integer
     * Multi-octet-integer = 1*30 OCTET
     * The content octets shall be an unsigned integer value
     * with the most significant octet encoded first (big-endian representation).
     * The minimum number of octets must be used to encode the value.
     * Short-length = <Any octet 0-30>
     */
    assert(NULL != pduDataStream);
    Int32 temp = 0;
    IInputStream::Probe(pduDataStream)->Read(&temp);
    assert(-1 != temp);
    Int32 count = temp & 0xFF;

    if (count > LONG_INTEGER_LENGTH_MAX) {
        // throw new RuntimeException("Octet count greater than 8 and I can't represent that!");
        return -1;
    }

    Int64 result = 0;

    for (Int32 i = 0; i < count; i++) {
        IInputStream::Probe(pduDataStream)->Read(&temp);
        assert(-1 != temp);
        result <<= 8;
        result += (temp & 0xFF);
    }

    return result;
}

Int64 PduParser::ParseIntegerValue(
    /* [in] */ IByteArrayInputStream* pduDataStream)
{
    /**
     * From wap-230-wsp-20010705-a.pdf
     * Integer-Value = Short-integer | Long-integer
     */
    assert(NULL != pduDataStream);
    IInputStream::Probe(pduDataStream)->Mark(1);
    Int32 temp = 0;
    IInputStream::Probe(pduDataStream)->Read(&temp);
    assert(-1 != temp);
    IInputStream::Probe(pduDataStream)->Reset();
    if (temp > SHORT_INTEGER_MAX) {
        return ParseShortInteger(pduDataStream);
    }
    else {
        return ParseLongInteger(pduDataStream);
    }
}

Int32 PduParser::SkipWapValue(
    /* [in] */ IByteArrayInputStream* pduDataStream,
    /* [in] */ Int32 length)
{
    assert(NULL != pduDataStream);
    AutoPtr<ArrayOf<Byte> > area = ArrayOf<Byte>::Alloc(length);
    Int32 readLen = 0;
    IInputStream::Probe(pduDataStream)->Read(area, 0, length, &readLen);
    if (readLen < length) { //The actually read length is lower than the length
        return -1;
    }
    else {
        return readLen;
    }
}

void PduParser::ParseContentTypeParams(
    /* [in] */ IByteArrayInputStream* pduDataStream,
    /* [in] */ IHashMap* map, // Integer*, Object
    /* [in] */ IInteger32* length)
{
    /**
     * From wap-230-wsp-20010705-a.pdf
     * Parameter = Typed-parameter | Untyped-parameter
     * Typed-parameter = Well-known-parameter-token Typed-value
     * the actual expected type of the value is implied by the well-known parameter
     * Well-known-parameter-token = Integer-value
     * the code values used for parameters are specified in the Assigned Numbers appendix
     * Typed-value = Compact-value | Text-value
     * In addition to the expected type, there may be no value.
     * If the value cannot be encoded using the expected type, it shall be encoded as text.
     * Compact-value = Integer-value |
     * Date-value | Delta-seconds-value | Q-value | Version-value |
     * Uri-value
     * Untyped-parameter = Token-text Untyped-value
     * the type of the value is unknown, but it shall be encoded as an integer,
     * if that is possible.
     * Untyped-value = Integer-value | Text-value
     */
    assert(pduDataStream != NULL);
    assert(length != NULL);

    Int32 startPos = 0;
    IInputStream::Probe(pduDataStream)->Available(&startPos);
    Int32 tempPos = 0;
    Int32 lastLen = 0;
    length->GetValue(&lastLen);
    while (0 < lastLen) {
        Int32 param = 0;
        IInputStream::Probe(pduDataStream)->Read(&param);
        assert(-1 != param);
        lastLen--;

        switch (param) {
            /**
             * From rfc2387, chapter 3.1
             * The type parameter must be specified and its value is the MIME media
             * type of the "root" body part. It permits a MIME user agent to
             * determine the content-type without reference to the enclosed body
             * part. If the value of the type parameter and the root body part's
             * content-type differ then the User Agent's behavior is undefined.
             *
             * From wap-230-wsp-20010705-a.pdf
             * type = Constrained-encoding
             * Constrained-encoding = Extension-Media | Short-integer
             * Extension-media = *TEXT End-of-string
             */
            case IPduPart::P_TYPE:
            case IPduPart::P_CT_MR_TYPE: {
                IInputStream::Probe(pduDataStream)->Mark(1);
                Int32 first = ExtractByteValue(pduDataStream);
                IInputStream::Probe(pduDataStream)->Reset();
                if (first > TEXT_MAX) {
                    // Short-integer (well-known type)
                    Int32 index = ParseShortInteger(pduDataStream);

                    if (index < PduContentTypes::sContentTypes->GetLength()) {
                        AutoPtr<ArrayOf<Byte> > type = ((*(PduContentTypes::sContentTypes))[index]).GetBytes();
                        map->Put(CoreUtils::Convert(IPduPart::P_TYPE), CoreUtils::Convert(String(*type)));
                    }
                    else {
                        //not support this type, ignore it.
                    }
                }
                else {
                    // Text-String (extension-media)
                    AutoPtr<ArrayOf<Byte> > type = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                    if ((NULL != type) && (NULL != map)) {
                        map->Put(CoreUtils::Convert(IPduPart::P_TYPE), CoreUtils::Convert(String(*type)));
                    }
                }

                IInputStream::Probe(pduDataStream)->Available(&tempPos);
                Int32 ilen = 0;
                length->GetValue(&ilen);
                lastLen = ilen - (startPos - tempPos);
                break;
            }

                /**
                 * From oma-ts-mms-conf-v1_3.pdf, chapter 10.2.3.
                 * Start Parameter Referring to Presentation
                 *
                 * From rfc2387, chapter 3.2
                 * The start parameter, if given, is the content-ID of the compound
                 * object's "root". If not present the "root" is the first body part in
                 * the Multipart/Related entity. The "root" is the element the
                 * applications processes first.
                 *
                 * From wap-230-wsp-20010705-a.pdf
                 * start = Text-String
                 */
            case IPduPart::P_START:
            case IPduPart::P_DEP_START: {
                AutoPtr<ArrayOf<Byte> > start = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                if ((NULL != start) && (NULL != map)) {
                    map->Put(CoreUtils::Convert(IPduPart::P_START), CoreUtils::Convert(String(*start)));
                }

                IInputStream::Probe(pduDataStream)->Available(&tempPos);
                Int32 ilen = 0;
                length->GetValue(&ilen);
                lastLen = ilen - (startPos - tempPos);
                break;
            }

                /**
                 * From oma-ts-mms-conf-v1_3.pdf
                 * In creation, the character set SHALL be either us-ascii
                 * (IANA MIBenum 3) or utf-8 (IANA MIBenum 106)[Unicode].
                 * In retrieval, both us-ascii and utf-8 SHALL be supported.
                 *
                 * From wap-230-wsp-20010705-a.pdf
                 * charset = Well-known-charset|Text-String
                 * Well-known-charset = Any-charset | Integer-value
                 * Both are encoded using values from Character Set
                 * Assignments table in Assigned Numbers
                 * Any-charset = <Octet 128>
                 * Equivalent to the special RFC2616 charset value "*"
                 */
            case IPduPart::P_CHARSET: {
                IInputStream::Probe(pduDataStream)->Mark(1);
                Int32 firstValue = ExtractByteValue(pduDataStream);
                IInputStream::Probe(pduDataStream)->Reset();
                //Check first char
                if (((firstValue > TEXT_MIN) && (firstValue < TEXT_MAX)) ||
                        (END_STRING_FLAG == firstValue)) {
                    //Text-String (extension-charset)
                    AutoPtr<ArrayOf<Byte> > charsetStr = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                    // try {
                        AutoPtr<ICharacterSetsHelper> cshlp;
                        CCharacterSetsHelper::AcquireSingleton((ICharacterSetsHelper**)&cshlp);
                        Int32 charsetInt = 0;
                        cshlp->GetMibEnumValue(String(*charsetStr), &charsetInt);
                        map->Put(CoreUtils::Convert(IPduPart::P_CHARSET), CoreUtils::Convert(charsetInt));
                    // } catch (UnsupportedEncodingException e) {
                    //     // Not a well-known charset, use "*".
                    //     Log.e(LOGTAG, Arrays.toString(charsetStr), e);
                    //     map.put(IPduPart::P_CHARSET, CharacterSets.ANY_CHARSET);
                    // }
                }
                else {
                    //Well-known-charset
                    Int32 charset = (Int32) ParseIntegerValue(pduDataStream);
                    if (map != NULL) {
                        map->Put(CoreUtils::Convert(IPduPart::P_CHARSET), CoreUtils::Convert(charset));
                    }
                }

                IInputStream::Probe(pduDataStream)->Available(&tempPos);
                Int32 ilen = 0;
                length->GetValue(&ilen);
                lastLen = ilen - (startPos - tempPos);
                break;
            }

                /**
                 * From oma-ts-mms-conf-v1_3.pdf
                 * A name for multipart object SHALL be encoded using name-parameter
                 * for Content-Type header in WSP multipart headers.
                 *
                 * From wap-230-wsp-20010705-a.pdf
                 * name = Text-String
                 */
            case IPduPart::P_DEP_NAME:
            case IPduPart::P_NAME: {
                AutoPtr<ArrayOf<Byte> > name = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                if ((NULL != name) && (NULL != map)) {
                    map->Put(CoreUtils::Convert(IPduPart::P_NAME), CoreUtils::Convert(String(*name)));
                }

                IInputStream::Probe(pduDataStream)->Available(&tempPos);
                Int32 ilen = 0;
                length->GetValue(&ilen);
                lastLen = ilen - (startPos - tempPos);
                break;
            }
            default: {
                if (LOCAL_LOGV) {
                    Logger::V(LOGTAG, String("Not supported Content-Type parameter"));
                }
                if (-1 == SkipWapValue(pduDataStream, lastLen)) {
                    Logger::E(LOGTAG, String("Corrupt Content-Type"));
                }
                else {
                    lastLen = 0;
                }
                break;
            }
        }
    }

    if (0 != lastLen) {
        Logger::E(LOGTAG, String("Corrupt Content-Type"));
    }
}

AutoPtr<ArrayOf<Byte> > PduParser::ParseContentType(
    /* [in] */ IByteArrayInputStream* pduDataStream,
    /* [in] */ IHashMap* map) // Integer*, Object>
{
    /**
     * From wap-230-wsp-20010705-a.pdf
     * Content-type-value = Constrained-media | Content-general-form
     * Content-general-form = Value-length Media-type
     * Media-type = (Well-known-media | Extension-Media) *(Parameter)
     */
    assert(NULL != pduDataStream);

    AutoPtr<ArrayOf<Byte> > contentType;
    IInputStream::Probe(pduDataStream)->Mark(1);
    Int32 temp = 0;
    IInputStream::Probe(pduDataStream)->Read(&temp);
    assert(-1 != temp);
    IInputStream::Probe(pduDataStream)->Reset();

    Int32 cur = (temp & 0xFF);

    if (cur < TEXT_MIN) {
        Int32 length = ParseValueLength(pduDataStream);
        Int32 startPos = 0;
        IInputStream::Probe(pduDataStream)->Available(&startPos);
        IInputStream::Probe(pduDataStream)->Mark(1);
        IInputStream::Probe(pduDataStream)->Read(&temp);
        assert(-1 != temp);
        IInputStream::Probe(pduDataStream)->Reset();
        Int32 first = (temp & 0xFF);

        if ((first >= TEXT_MIN) && (first <= TEXT_MAX)) {
            contentType = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
        }
        else if (first > TEXT_MAX) {
            Int32 index = ParseShortInteger(pduDataStream);

            if (index < PduContentTypes::sContentTypes->GetLength()) { //well-known type
                contentType = ((*(PduContentTypes::sContentTypes))[index]).GetBytes();
            }
            else {
                IInputStream::Probe(pduDataStream)->Reset();
                contentType = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
            }
        }
        else {
            Logger::E(LOGTAG, String("Corrupt content-type"));
            return ((*(PduContentTypes::sContentTypes))[0]).GetBytes(); //"*/*"
        }

        Int32 endPos = 0;
        IInputStream::Probe(pduDataStream)->Available(&endPos);
        Int32 parameterLen = length - (startPos - endPos);
        if (parameterLen > 0) {//have parameters
            ParseContentTypeParams(pduDataStream, map, CoreUtils::Convert(parameterLen));
        }

        if (parameterLen < 0) {
            Logger::E(LOGTAG, String("Corrupt MMS message"));
            return ((*(PduContentTypes::sContentTypes))[0]).GetBytes(); //"*/*"
        }
    }
    else if (cur <= TEXT_MAX) {
        contentType = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
    }
    else {
        contentType =
            ((*(PduContentTypes::sContentTypes))[ParseShortInteger(pduDataStream)]).GetBytes();
    }

    return contentType;
}

Boolean PduParser::ParsePartHeaders(
    /* [in] */ IByteArrayInputStream* pduDataStream,
    /* [in] */ IPduPart* part,
    /* [in] */ Int32 length)
{
    assert(NULL != pduDataStream);
    assert(NULL != part);
    assert(length > 0);

    /**
     * From oma-ts-mms-conf-v1_3.pdf, chapter 10.2.
     * A name for multipart object SHALL be encoded using name-parameter
     * for Content-Type header in WSP multipart headers.
     * In decoding, name-parameter of Content-Type SHALL be used if available.
     * If name-parameter of Content-Type is not available,
     * filename parameter of Content-Disposition header SHALL be used if available.
     * If neither name-parameter of Content-Type header nor filename parameter
     * of Content-Disposition header is available,
     * Content-Location header SHALL be used if available.
     *
     * Within SMIL part the reference to the media object parts SHALL use
     * either Content-ID or Content-Location mechanism [RFC2557]
     * and the corresponding WSP part headers in media object parts
     * contain the corresponding definitions.
     */
    Int32 startPos = 0;
    IInputStream::Probe(pduDataStream)->Available(&startPos);
    Int32 tempPos = 0;
    Int32 lastLen = length;
    while (0 < lastLen) {
        Int32 header = 0;
        IInputStream::Probe(pduDataStream)->Read(&header);
        assert(-1 != header);
        lastLen--;

        if (header > TEXT_MAX) {
            // Number assigned headers.
            switch (header) {
                case IPduPart::P_CONTENT_LOCATION: {
                    /**
                     * From wap-230-wsp-20010705-a.pdf, chapter 8.4.2.21
                     * Content-location-value = Uri-value
                     */
                    AutoPtr<ArrayOf<Byte> > contentLocation = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                    if (NULL != contentLocation) {
                        part->SetContentLocation(contentLocation);
                    }

                    IInputStream::Probe(pduDataStream)->Available(&tempPos);
                    lastLen = length - (startPos - tempPos);
                    break;
                }
                case IPduPart::P_CONTENT_ID: {
                    /**
                     * From wap-230-wsp-20010705-a.pdf, chapter 8.4.2.21
                     * Content-ID-value = Quoted-string
                     */
                    AutoPtr<ArrayOf<Byte> > contentId = ParseWapString(pduDataStream, TYPE_QUOTED_STRING);
                    if (NULL != contentId) {
                        part->SetContentId(contentId);
                    }

                    IInputStream::Probe(pduDataStream)->Available(&tempPos);
                    lastLen = length - (startPos - tempPos);
                    break;
                }
                case IPduPart::P_DEP_CONTENT_DISPOSITION:
                case IPduPart::P_CONTENT_DISPOSITION: {
                    /**
                     * From wap-230-wsp-20010705-a.pdf, chapter 8.4.2.21
                     * Content-disposition-value = Value-length Disposition *(Parameter)
                     * Disposition = Form-data | Attachment | Inline | Token-text
                     * Form-data = <Octet 128>
                     * Attachment = <Octet 129>
                     * Inline = <Octet 130>
                     */

                    /*
                     * some carrier mmsc servers do not support content_disposition
                     * field correctly
                     */
                    AutoPtr<IResourcesHelper> hlp;
                    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&hlp);
                    AutoPtr<IResources> resr;
                    hlp->GetSystem((IResources**)&resr);
                    Boolean contentDisposition = FALSE;
                    resr->GetBoolean(
                            R::bool_::config_mms_content_disposition_support, &contentDisposition);

                    if (contentDisposition) {
                        Int32 len = ParseValueLength(pduDataStream);
                        IInputStream::Probe(pduDataStream)->Mark(1);
                        Int32 thisStartPos = 0;
                        IInputStream::Probe(pduDataStream)->Available(&thisStartPos);
                        Int32 thisEndPos = 0;
                        Int32 value = 0;
                        IInputStream::Probe(pduDataStream)->Read(&value);

                        if (value == IPduPart::P_DISPOSITION_FROM_DATA ) {
                            part->SetContentDisposition(PduPart::DISPOSITION_FROM_DATA);
                        }
                        else if (value == IPduPart::P_DISPOSITION_ATTACHMENT) {
                            part->SetContentDisposition(PduPart::DISPOSITION_ATTACHMENT);
                        }
                        else if (value == IPduPart::P_DISPOSITION_INLINE) {
                            part->SetContentDisposition(PduPart::DISPOSITION_INLINE);
                        }
                        else {
                            IInputStream::Probe(pduDataStream)->Reset();
                            /* Token-text */
                            part->SetContentDisposition(ParseWapString(pduDataStream
                                    , TYPE_TEXT_STRING));
                        }

                        /* get filename parameter and skip other parameters */
                        IInputStream::Probe(pduDataStream)->Available(&thisEndPos);
                        if (thisStartPos - thisEndPos < len) {
                            IInputStream::Probe(pduDataStream)->Read(&value);
                            if (value == IPduPart::P_FILENAME) { //filename is text-string
                                part->SetFilename(ParseWapString(pduDataStream
                                        , TYPE_TEXT_STRING));
                            }

                            /* skip other parameters */
                            IInputStream::Probe(pduDataStream)->Available(&thisEndPos);
                            if (thisStartPos - thisEndPos < len) {
                                Int32 last = len - (thisStartPos - thisEndPos);
                                AutoPtr<ArrayOf<Byte> > temp = ArrayOf<Byte>::Alloc(last);
                                Int32 res = 0;
                                IInputStream::Probe(pduDataStream)->Read(temp, 0, last, &res);
                            }
                        }

                        IInputStream::Probe(pduDataStream)->Available(&tempPos);
                        lastLen = length - (startPos - tempPos);
                    }
                    break;
                }
                default: {
                    if (LOCAL_LOGV) {
                        Logger::V(LOGTAG, String("Not supported Part headers: ") + header);
                    }
                    if (-1 == SkipWapValue(pduDataStream, lastLen)) {
                        Logger::E(LOGTAG, String("Corrupt Part headers"));
                        return FALSE;
                    }
                    lastLen = 0;
                    break;
                }
            }
        }
        else if ((header >= TEXT_MIN) && (header <= TEXT_MAX)) {
            // Not assigned header.
            AutoPtr<ArrayOf<Byte> > tempHeader = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
            AutoPtr<ArrayOf<Byte> > tempValue = ParseWapString(pduDataStream, TYPE_TEXT_STRING);

            // Check the header whether it is "Content-Transfer-Encoding".
            if (TRUE ==
                IPduPart::CONTENT_TRANSFER_ENCODING.EqualsIgnoreCase(String(*tempHeader))) {
                part->SetContentTransferEncoding(tempValue);
            }

            IInputStream::Probe(pduDataStream)->Available(&tempPos);
            lastLen = length - (startPos - tempPos);
        }
        else {
            if (LOCAL_LOGV) {
                Logger::V(LOGTAG, String("Not supported Part headers: ") + header);
            }
            // Skip all headers of this part.
            if (-1 == SkipWapValue(pduDataStream, lastLen)) {
                Logger::E(LOGTAG, String("Corrupt Part headers"));
                return FALSE;
            }
            lastLen = 0;
        }
    }

    if (0 != lastLen) {
        Logger::E(LOGTAG, String("Corrupt Part headers"));
        return FALSE;
    }

    return TRUE;
}

Boolean PduParser::CheckMandatoryHeader(
    /* [in] */ IPduHeaders* headers)
{
    if (NULL == headers) {
        return FALSE;
    }

    /* get message type */
    Int32 messageType = 0;
    headers->GetOctet(IPduHeaders::MESSAGE_TYPE, &messageType);

    /* check Mms-Version field */
    Int32 mmsVersion = 0;
    headers->GetOctet(IPduHeaders::MMS_VERSION, &mmsVersion);
    if (0 == mmsVersion) {
        // Every message should have Mms-Version field.
        return FALSE;
    }

    /* check mandatory header fields */
    switch (messageType) {
        case IPduHeaders::MESSAGE_TYPE_SEND_REQ: {
            // Content-Type field.
            AutoPtr<ArrayOf<Byte> > srContentType;
            headers->GetTextString(IPduHeaders::CONTENT_TYPE, (ArrayOf<Byte>**)&srContentType);
            if (NULL == srContentType) {
                return FALSE;
            }

            // From field.
            AutoPtr<IEncodedStringValue> srFrom;
            headers->GetEncodedStringValue(IPduHeaders::FROM, (IEncodedStringValue**)&srFrom);
            if (NULL == srFrom) {
                return FALSE;
            }

            // Transaction-Id field.
            AutoPtr<ArrayOf<Byte> > srTransactionId;
            headers->GetTextString(IPduHeaders::TRANSACTION_ID, (ArrayOf<Byte>**)&srTransactionId);
            if (NULL == srTransactionId) {
                return FALSE;
            }

            break;
        }
        case IPduHeaders::MESSAGE_TYPE_SEND_CONF: {
            // Response-Status field.
            Int32 scResponseStatus = 0;
            headers->GetOctet(IPduHeaders::RESPONSE_STATUS, &scResponseStatus);
            if (0 == scResponseStatus) {
                return FALSE;
            }

            // Transaction-Id field.
            AutoPtr<ArrayOf<Byte> > scTransactionId;
            headers->GetTextString(IPduHeaders::TRANSACTION_ID, (ArrayOf<Byte>**)&scTransactionId);
            if (NULL == scTransactionId) {
                return FALSE;
            }

            break;
        }
        case IPduHeaders::MESSAGE_TYPE_NOTIFICATION_IND: {
            // Content-Location field.
            AutoPtr<ArrayOf<Byte> > niContentLocation;
            headers->GetTextString(IPduHeaders::CONTENT_LOCATION, (ArrayOf<Byte>**)&niContentLocation);
            if (NULL == niContentLocation) {
                return FALSE;
            }

            // Expiry field.
            Int64 niExpiry = 0;
            headers->GetLongInteger(IPduHeaders::EXPIRY, &niExpiry);
            if (-1 == niExpiry) {
                return FALSE;
            }

            // Message-Class field.
            AutoPtr<ArrayOf<Byte> > niMessageClass;
            headers->GetTextString(IPduHeaders::MESSAGE_CLASS, (ArrayOf<Byte>**)&niMessageClass);
            if (NULL == niMessageClass) {
                return FALSE;
            }

            // Message-Size field.
            Int64 niMessageSize = 0;
            headers->GetLongInteger(IPduHeaders::MESSAGE_SIZE, &niMessageSize);
            if (-1 == niMessageSize) {
                return FALSE;
            }

            // Transaction-Id field.
            AutoPtr<ArrayOf<Byte> > niTransactionId;
            headers->GetTextString(IPduHeaders::TRANSACTION_ID, (ArrayOf<Byte>**)&niTransactionId);
            if (NULL == niTransactionId) {
                return FALSE;
            }

            break;
        }
        case IPduHeaders::MESSAGE_TYPE_NOTIFYRESP_IND: {
            // Status field.
            Int32 nriStatus = 0;
            headers->GetOctet(IPduHeaders::STATUS, &nriStatus);
            if (0 == nriStatus) {
                return FALSE;
            }

            // Transaction-Id field.
            AutoPtr<ArrayOf<Byte> > nriTransactionId;
            headers->GetTextString(IPduHeaders::TRANSACTION_ID, (ArrayOf<Byte>**)&nriTransactionId);
            if (NULL == nriTransactionId) {
                return FALSE;
            }

            break;
        }
        case IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF: {
            // Content-Type field.
            AutoPtr<ArrayOf<Byte> > rcContentType;
            headers->GetTextString(IPduHeaders::CONTENT_TYPE, (ArrayOf<Byte>**)&rcContentType);
            if (NULL == rcContentType) {
                return FALSE;
            }

            // Date field.
            Int64 rcDate = 0;
            headers->GetLongInteger(IPduHeaders::DATE, &rcDate);
            if (-1 == rcDate) {
                return FALSE;
            }

            break;
        }
        case IPduHeaders::MESSAGE_TYPE_DELIVERY_IND: {
            // Date field.
            Int64 diDate = 0;
            headers->GetLongInteger(IPduHeaders::DATE, &diDate);
            if (-1 == diDate) {
                return FALSE;
            }

            // Message-Id field.
            AutoPtr<ArrayOf<Byte> > diMessageId;
            headers->GetTextString(IPduHeaders::MESSAGE_ID, (ArrayOf<Byte>**)&diMessageId);
            if (NULL == diMessageId) {
                return FALSE;
            }

            // Status field.
            Int32 diStatus = 0;
            headers->GetOctet(IPduHeaders::STATUS, &diStatus);
            if (0 == diStatus) {
                return FALSE;
            }

            // To field.
            AutoPtr<ArrayOf<IEncodedStringValue*> > diTo;
            headers->GetEncodedStringValues(IPduHeaders::TO, (ArrayOf<IEncodedStringValue*>**)&diTo);
            if (NULL == diTo) {
                return FALSE;
            }

            break;
        }
        case IPduHeaders::MESSAGE_TYPE_ACKNOWLEDGE_IND: {
            // Transaction-Id field.
            AutoPtr<ArrayOf<Byte> > aiTransactionId;
            headers->GetTextString(IPduHeaders::TRANSACTION_ID, (ArrayOf<Byte>**)&aiTransactionId);
            if (NULL == aiTransactionId) {
                return FALSE;
            }

            break;
        }
        case IPduHeaders::MESSAGE_TYPE_READ_ORIG_IND: {
            // Date field.
            Int64 roDate = 0;
            headers->GetLongInteger(IPduHeaders::DATE, &roDate);
            if (-1 == roDate) {
                return FALSE;
            }

            // From field.
            AutoPtr<IEncodedStringValue> roFrom;
            headers->GetEncodedStringValue(IPduHeaders::FROM, (IEncodedStringValue**)&roFrom);
            if (NULL == roFrom) {
                return FALSE;
            }

            // Message-Id field.
            AutoPtr<ArrayOf<Byte> > roMessageId;
            headers->GetTextString(IPduHeaders::MESSAGE_ID, (ArrayOf<Byte>**)&roMessageId);
            if (NULL == roMessageId) {
                return FALSE;
            }

            // Read-Status field.
            Int32 roReadStatus = 0;
            headers->GetOctet(IPduHeaders::READ_STATUS, &roReadStatus);
            if (0 == roReadStatus) {
                return FALSE;
            }

            // To field.
            AutoPtr<ArrayOf<IEncodedStringValue*> > roTo;
            headers->GetEncodedStringValues(IPduHeaders::TO, (ArrayOf<IEncodedStringValue*>**)&roTo);
            if (NULL == roTo) {
                return FALSE;
            }

            break;
        }
        case IPduHeaders::MESSAGE_TYPE_READ_REC_IND: {
            // From field.
            AutoPtr<IEncodedStringValue> rrFrom;
            headers->GetEncodedStringValue(IPduHeaders::FROM, (IEncodedStringValue**)&rrFrom);
            if (NULL == rrFrom) {
                return FALSE;
            }

            // Message-Id field.
            AutoPtr<ArrayOf<Byte> > rrMessageId;
            headers->GetTextString(IPduHeaders::MESSAGE_ID, (ArrayOf<Byte>**)&rrMessageId);
            if (NULL == rrMessageId) {
                return FALSE;
            }

            // Read-Status field.
            Int32 rrReadStatus = 0;
            headers->GetOctet(IPduHeaders::READ_STATUS, &rrReadStatus);
            if (0 == rrReadStatus) {
                return FALSE;
            }

            // To field.
            AutoPtr<ArrayOf<IEncodedStringValue*> > rrTo;
            headers->GetEncodedStringValues(IPduHeaders::TO, (ArrayOf<IEncodedStringValue*>**)&rrTo);
            if (NULL == rrTo) {
                return FALSE;
            }

            break;
        }
        default: {
            // Parser doesn't support this message type in this version.
            return FALSE;
        }
    }

    return TRUE;
}

void PduParser::Log(
    /* [in] */ const String& text)
{
    if (LOCAL_LOGV) {
        Logger::V(LOGTAG, text);
    }
}

Int32 PduParser::CheckPartPosition(
    /* [in] */ IPduPart* part)
{
    assert(NULL != part);
    if ((NULL == mTypeParam) &&
            (NULL == mStartParam)) {
        return THE_LAST_PART;
    }

    /* check part's content-id */
    if (NULL != mStartParam) {
        AutoPtr<ArrayOf<Byte> > contentId;
        part->GetContentId((ArrayOf<Byte>**)&contentId);
        if (NULL != contentId) {
            if (TRUE == Arrays::Equals(mStartParam, contentId)) {
                return THE_FIRST_PART;
            }
        }
    }

    /* check part's content-type */
    if (NULL != mTypeParam) {
        AutoPtr<ArrayOf<Byte> > contentType;
        part->GetContentType((ArrayOf<Byte>**)&contentType);
        if (NULL != contentType) {
            if (TRUE == Arrays::Equals(mTypeParam, contentType)) {
                return THE_FIRST_PART;
            }
        }
    }

    return THE_LAST_PART;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
