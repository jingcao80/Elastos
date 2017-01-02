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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICCSMSCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICCSMSCONTROLLER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Internal::Telephony::IISms;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Core::IArrayOf;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * UiccSmsController to provide an inter-process communication to
 * access Sms in Icc.
 */
class UiccSmsController
    : public Object
    , public IISms
    , public IUiccSmsController
{
private:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("UiccSmsController::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ UiccSmsController* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    public:
        UiccSmsController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    UiccSmsController();

    CARAPI constructor(
        /* [in] */ ArrayOf<IPhone*>* phone,
        /* [in] */ IContext* context);

    CARAPI UpdateMessageOnIccEf(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 index,
        /* [in] */ Int32 status,
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ Boolean* result);

    CARAPI UpdateMessageOnIccEfForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 index,
        /* [in] */ Int32 status,
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ Boolean* result);

    CARAPI CopyMessageToIccEf(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 status,
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ ArrayOf<Byte>* smsc,
        /* [out] */ Boolean* result);

    CARAPI CopyMessageToIccEfForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 status,
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ ArrayOf<Byte>* smsc,
        /* [out] */ Boolean* result);

    CARAPI GetAllMessagesFromIccEf(
        /* [in] */ const String& callingPackage,
        /* [out] */ IList** result);

    CARAPI GetAllMessagesFromIccEfForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPackage,
        /* [out] */ IList** result);

    CARAPI SendData(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    CARAPI SendDataForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    CARAPI SendDataWithOrigPort(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ Int32 origPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    CARAPI SendDataWithOrigPortUsingSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ Int32 origPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    CARAPI SendText(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ const String& text,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    CARAPI SendTextForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ const String& text,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    CARAPI SendTextWithOptionsUsingSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ const String& text,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod);

    CARAPI SendMultipartText(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ IList* parts,
        /* [in] */ IList* sentIntents,
        /* [in] */ IList* deliveryIntents);

    CARAPI SendMultipartTextForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ IList* parts,
        /* [in] */ IList* sentIntents,
        /* [in] */ IList* deliveryIntents);

    CARAPI SendMultipartTextWithOptionsUsingSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ IList* parts,
        /* [in] */ IList* sentIntents,
        /* [in] */ IList* deliveryIntents,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod);

    CARAPI EnableCellBroadcast(
        /* [in] */ Int32 messageIdentifier,
        /* [out] */ Boolean* result);

    CARAPI EnableCellBroadcastForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 messageIdentifier,
        /* [out] */ Boolean* result);

    CARAPI EnableCellBroadcastRange(
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    CARAPI EnableCellBroadcastRangeForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    CARAPI DisableCellBroadcast(
        /* [in] */ Int32 messageIdentifier,
        /* [out] */ Boolean* result);

    CARAPI DisableCellBroadcastForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 messageIdentifier,
        /* [out] */ Boolean* result);

    CARAPI DisableCellBroadcastRange(
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    CARAPI DisableCellBroadcastRangeForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    CARAPI GetPremiumSmsPermission(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetPremiumSmsPermissionForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    CARAPI SetPremiumSmsPermission(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 permission);

    //@Override
    CARAPI SetPremiumSmsPermissionForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 permission);

    CARAPI IsImsSmsSupported(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsImsSmsSupportedForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    CARAPI GetImsSmsFormat(
        /* [out] */ String* result);

    //@Override
    CARAPI GetImsSmsFormatForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI UpdateSmsSendStatus(
        /* [in] */ Int32 messageRef,
        /* [in] */ Boolean success);

    //@Override
    CARAPI InjectSmsPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ const String& format,
        /* [in] */ IPendingIntent* receivedIntent);

    // FIXME: Add injectSmsPduForSubscriber to ISms.aidl
    CARAPI InjectSmsPduForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ const String& format,
        /* [in] */ IPendingIntent* receivedIntent);

    //@Override
    CARAPI SynthesizeMessages(
        /* [in] */ const String& originatingAddress,
        /* [in] */ const String& scAddress,
        /* [in] */ IList* messages,
        /* [in] */ Int64 timestampMillis);

    //@Override
    CARAPI SendStoredText(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* messageUri,
        /* [in] */ const String& scAddress,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    //@Override
    CARAPI SendStoredMultipartText(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* messageUri,
        /* [in] */ const String& scAddress,
        /* [in] */ IList* sentIntents,
        /* [in] */ IList* deliveryIntents);

    /**
     * Get the capacity count of sms on Icc card.
     **/
    CARAPI GetSmsCapacityOnIccForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    CARAPI Log(
        /* [in] */ String msg);

    void DispatchPdus(
        /* [in] */ ArrayOf<ArrayOf<Byte>* >* pdus);

    CARAPI_(void) Dispatch(
        /* [in] */ IIntent* intent,
        /* [in] */ String permission);

    CARAPI_(void) BroadcastOutgoingSms(
        /* [in] */ Int64 subId,
        /* [in] */ String callingPackage,
        /* [in] */ String destAddr,
        /* [in] */ String scAddr,
        /* [in] */ Boolean multipart,
        /* [in] */ IArrayList* parts,
        /* [in] */ IArrayList* sentIntents,
        /* [in] */ IArrayList* deliveryIntents,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod);

    /**
     * get sms interface manager object based on subscription.
     **/
    CARAPI_(AutoPtr<IIccSmsInterfaceManager>) GetIccSmsInterfaceManager(
        /* [in] */ Int64 subId);

    CARAPI_(Int64) GetDefaultSmsSubId();

    CARAPI_(void) CreateWakelock();

public:
    static const String LOGTAG;

    AutoPtr<ArrayOf<IPhone*> > mPhone;

private:
    AutoPtr<IBroadcastReceiver> mReceiver;

    AutoPtr<IContext> mContext;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    static const Int32 WAKE_LOCK_TIMEOUT;
    AutoPtr<IHandler> mHandler;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICCSMSCONTROLLER_H__
