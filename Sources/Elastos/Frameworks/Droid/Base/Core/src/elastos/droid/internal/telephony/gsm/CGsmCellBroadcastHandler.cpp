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

#include "elastos/droid/internal/telephony/gsm/CGsmCellBroadcastHandler.h"
#include "elastos/droid/internal/telephony/gsm/CGsmSmsCbMessage.h"
#include "elastos/droid/internal/telephony/gsm/CSmsCbHeader.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/telephony/CSmsCbLocation.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Internal::Telephony::PhoneBase;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Telephony::CSmsCbLocation;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::Gsm::IGsmCellLocation;
using Elastos::Core::CArrayOf;
using Elastos::Core::EIID_IArrayOf;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const Boolean CGsmCellBroadcastHandler::VDBG;

CAR_INTERFACE_IMPL(CGsmCellBroadcastHandler, CellBroadcastHandler, IGsmCellBroadcastHandler)

CAR_OBJECT_IMPL(CGsmCellBroadcastHandler)

CGsmCellBroadcastHandler::SmsCbConcatInfo::SmsCbConcatInfo(
    /* [in] */ ISmsCbHeader* header,
    /* [in] */ ISmsCbLocation* location)
    : mHeader(header)
    , mLocation(location)
{
}

ECode CGsmCellBroadcastHandler::SmsCbConcatInfo::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 num;
    mHeader->GetSerialNumber(&num);
    Int32 hc;
    IObject::Probe(mLocation)->GetHashCode(&hc);
    *result = (num * 31) + hc;
    return NOERROR;
}

ECode CGsmCellBroadcastHandler::SmsCbConcatInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<SmsCbConcatInfo> other = (SmsCbConcatInfo*)IObject::Probe(obj);
    if (other != NULL) {

        // Two pages match if they have the same serial number (which includes the
        // geographical scope and update number), and both pages belong to the same
        // location (PLMN, plus LAC and CID if these are part of the geographical scope).
        Int32 num1, num2;
        mHeader->GetSerialNumber(&num1);
        other->mHeader->GetSerialNumber(&num2);
        Boolean b;
        *result = num1 == num2
                && (IObject::Probe(mLocation)->Equals(other->mLocation, &b), b);
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode CGsmCellBroadcastHandler::SmsCbConcatInfo::MatchesLocation(
    /* [in] */ const String& plmn,
    /* [in] */ Int32 lac,
    /* [in] */ Int32 cid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mLocation->IsInLocationArea(plmn, lac, cid, result);
}

CGsmCellBroadcastHandler::CGsmCellBroadcastHandler()
{
    CHashMap::New(4, (IHashMap**)&mSmsCbPageMap);
}

ECode CGsmCellBroadcastHandler::constructor()
{
    return NOERROR;
}

ECode CGsmCellBroadcastHandler::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneBase* phone)
{
    CellBroadcastHandler::constructor(String("GsmCellBroadcastHandler"), context, phone);
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    ((PhoneBase*)mPhone.Get())->mCi->SetOnNewGsmBroadcastSms(handler, EVENT_NEW_SMS_MESSAGE, NULL);
    return NOERROR;
}

AutoPtr<IGsmCellBroadcastHandler> CGsmCellBroadcastHandler::MakeGsmCellBroadcastHandler(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneBase* phone)
{
    AutoPtr<IGsmCellBroadcastHandler> handler;
    CGsmCellBroadcastHandler::New(context, phone, (IGsmCellBroadcastHandler**)&handler);
    IStateMachine::Probe(handler)->Start();
    return handler;
}

void CGsmCellBroadcastHandler::OnQuitting()
{
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    ((PhoneBase*)mPhone.Get())->mCi->UnSetOnNewGsmBroadcastSms(handler);
    CellBroadcastHandler::OnQuitting();     // release wakelock
}

ECode CGsmCellBroadcastHandler::HandleSmsMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    // if (obj instanceof AsyncResult) {
    if (IAsyncResult::Probe(obj) != NULL) {
        AutoPtr<ISmsCbMessage> cbMessage = HandleGsmBroadcastSms((AsyncResult*)IAsyncResult::Probe(obj));
        if (cbMessage != NULL) {
            HandleBroadcastSms(cbMessage);
            *result = TRUE;
            return NOERROR;
        }
    }

    return CellBroadcastHandler::HandleSmsMessage(message, result);
}

AutoPtr<ISmsCbMessage> CGsmCellBroadcastHandler::HandleGsmBroadcastSms(
    /* [in] */ AsyncResult* ar)
{
    // try {
    AutoPtr<IArrayOf> receivedPdu = IArrayOf::Probe(ar->mResult);  // byte[]

    Int32 pduLength;
    receivedPdu->GetLength(&pduLength);
    if (VDBG) {
        for (Int32 i = 0; i < pduLength; i += 8) {
            StringBuilder sb("SMS CB pdu data: ");
            for (Int32 j = i; j < i + 8 && j < pduLength; j++) {
                AutoPtr<IInterface> obj;
                receivedPdu->Get(j, (IInterface**)&obj);
                Byte byte;
                IByte::Probe(obj)->GetValue(&byte);
                Int32 b = byte & 0xff;
                if (b < 0x10) {
                    sb.Append('0');
                }
                sb.Append(StringUtils::ToHexString(b));
                sb.Append(' ');
            }
            String str;
            sb.ToString(&str);
            Log(str);
        }
    }

    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(pduLength);
    for (Int32 i = 0; i < pduLength; i++) {
        AutoPtr<IInterface> obj;
        receivedPdu->Get(i, (IInterface**)&obj);
        Byte byte;
        IByte::Probe(obj)->GetValue(&byte);
        (*array)[i] = byte;
    }
    AutoPtr<ISmsCbHeader> header;
    CSmsCbHeader::New(array, (ISmsCbHeader**)&header);
    String plmn;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_OPERATOR_NUMERIC, &plmn);
    Int32 lac = -1;
    Int32 cid = -1;
    AutoPtr<ICellLocation> cl;
    IPhone::Probe(mPhone)->GetCellLocation((ICellLocation**)&cl);
    // Check if cell location is GsmCellLocation.  This is required to support
    // dual-mode devices such as CDMA/LTE devices that require support for
    // both 3GPP and 3GPP2 format messages
    AutoPtr<IGsmCellLocation> cellLocation = IGsmCellLocation::Probe(cl);
    if (cellLocation != NULL) {
        cellLocation->GetLac(&lac);
        cellLocation->GetCid(&cid);
    }

    Int32 val;
    header->GetGeographicalScope(&val);
    AutoPtr<ISmsCbLocation> location;
    switch (val) {
        case ISmsCbMessage::GEOGRAPHICAL_SCOPE_LA_WIDE:
            CSmsCbLocation::New(plmn, lac, -1, (ISmsCbLocation**)&location);
            break;

        case ISmsCbMessage::GEOGRAPHICAL_SCOPE_CELL_WIDE:
        case ISmsCbMessage::GEOGRAPHICAL_SCOPE_CELL_WIDE_IMMEDIATE:
            CSmsCbLocation::New(plmn, lac, cid, (ISmsCbLocation**)&location);
            break;

        case ISmsCbMessage::GEOGRAPHICAL_SCOPE_PLMN_WIDE:
        default:
            CSmsCbLocation::New(plmn, (ISmsCbLocation**)&location);
            break;
    }

    AutoPtr<IArrayOf> pdus; // Byte[][] pdus;
    Int32 pageCount;
    header->GetNumberOfPages(&pageCount);
    if (pageCount > 1) {
        // Multi-page message
        AutoPtr<SmsCbConcatInfo> concatInfo = new SmsCbConcatInfo(header, location);

        // Try to find other pages of the same message
        AutoPtr<IInterface> obj;
        mSmsCbPageMap->Get((IInterface*)(IObject*)concatInfo, (IInterface**)&obj);
        pdus = IArrayOf::Probe(obj);

        if (pdus == NULL) {
            // This is the first page of this message, make room for all
            // pages and keep until complete
            CArrayOf::New(EIID_IArrayOf, pageCount, (IArrayOf**)&pdus); // pdus = new byte[pageCount][];

            mSmsCbPageMap->Put((IInterface*)(IObject*)concatInfo, pdus);
        }

        // Page parameter is one-based
        Int32 index;
        header->GetPageIndex(&index);
        pdus->Set(index - 1, receivedPdu);

        Int32 size;
        pdus->GetLength(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            pdus->Get(i, (IInterface**)&obj);
            AutoPtr<IArrayOf> pdu = IArrayOf::Probe(obj);
            if (pdu == NULL) {
                // Still missing pages, exit
                return NULL;
            }
        }

        // Message complete, remove and dispatch
        mSmsCbPageMap->Remove((IObject*)concatInfo);
    }
    else {
        // Single page message
        CArrayOf::New(EIID_IArrayOf, 1, (IArrayOf**)&pdus);   // pdus = new byte[1][];
        pdus->Set(0, receivedPdu);
    }

    // Remove messages that are out of scope to prevent the map from
    // growing indefinitely, containing incomplete messages that were
    // never assembled
    AutoPtr<ISet> keySet;
    mSmsCbPageMap->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> iter;
    keySet->GetIterator((IIterator**)&iter);

    Boolean b;
    while (iter->HasNext(&b), b) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<SmsCbConcatInfo> info = (SmsCbConcatInfo*)(IObject*)obj.Get();

        Boolean b1;
        if (info->MatchesLocation(plmn, lac, cid, &b1), b1) {
            iter->Remove();
        }
    }

    AutoPtr<ISmsCbMessage> cbMessage;
    CGsmSmsCbMessage::CreateSmsCbMessage(header, location, pdus, (ISmsCbMessage**)&cbMessage);
    return cbMessage;

    // } catch (RuntimeException e) {
    //     loge("Error in decoding SMS CB pdu", e);
    //     return null;
    // }
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos