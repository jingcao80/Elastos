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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECALL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECALL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/Call.h"

//TODO using Elastos::Droid::Ims::IImsCall;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::Call;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

/**
  * {@hide}
  */
class ImsPhoneCall
    : public Call
    , public IImsPhoneCall
{
public:
    CAR_INTERFACE_DECL();

    /****************************** Constructors *****************************/
    /*package*/
    ImsPhoneCall();

    CARAPI constructor();
    /*package*/
    CARAPI constructor(
        /* [in] */ IImsPhoneCallTracker* owner);

    virtual CARAPI Dispose();

    /************************** Overridden from Call *************************/
    // @Override
    CARAPI GetConnections(
        /* [out] */ IList/*<Connection>*/** result);

    // @Override
    CARAPI GetPhone(
        /* [out] */ IPhone** result);

    // @Override
    CARAPI IsMultiparty(
        /* [out] */ Boolean* result);

    /** Please note: if this is the foreground call and a
      *  background call exists, the background call will be resumed.
      */
    // @Override
    CARAPI Hangup();

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI GetExtras(
        /* [out] */ IBundle** result);

    //***** Called from ImsPhoneConnection
    /*package*/
    virtual CARAPI Attach(
        /* [in] */ IConnection* conn);

    /*package*/
    virtual CARAPI Attach(
        /* [in] */ IConnection* conn,
        /* [in] */ ICallState state);

    /*package*/
    virtual CARAPI AttachFake(
        /* [in] */ IConnection* conn,
        /* [in] */ ICallState state);

    /**
      * Called by ImsPhoneConnection when it has disconnected
      */
    virtual CARAPI ConnectionDisconnected(
        /* [in] */ IImsPhoneConnection* conn,
        /* [out] */ Boolean* result);

    /*package*/
    virtual CARAPI Detach(
        /* [in] */ IImsPhoneConnection* conn);

    /**
      * @return true if there's no space in this call for additional
      * connections to be added via "conference"
      */
    /*package*/
    virtual CARAPI IsFull(
        /* [out] */ Boolean* result);

    //***** Called from ImsPhoneCallTracker
    /**
      * Called when this Call is being hung up locally (eg, user pressed "end")
      */
    virtual CARAPI OnHangupLocal();

    /**
      * Called when it's time to clean up disconnected Connection objects
      */
    virtual CARAPI ClearDisconnected();

    /*package*/
    virtual CARAPI GetFirstConnection(
        /* [out] */ IImsPhoneConnection** result);

    /*package*/
    virtual CARAPI SetMute(
        /* [in] */ Boolean mute);

    /* package */
    virtual CARAPI Merge(
        /* [in] */ IImsPhoneCall* that,
        /* [in] */ ICallState state);

    /**
      * Retrieves the {@link ImsCall} for the current {@link ImsPhoneCall}.
      * <p>
      * Marked as {@code VisibleForTesting} so that the
      * {@link com.android.internal.telephony.TelephonyTester} class can inject a test conference
      * event package into a regular ongoing IMS call.
      *
      * @return The {@link ImsCall}.
      */
    // @VisibleForTesting
    virtual CARAPI GetImsCall(
            /* [out] */ /*TODO IImsCall*/IInterface** result);

    /*package*/
    static CARAPI_(Boolean) IsLocalTone(
        /* [in] */ /*TODO IImsCall*/IInterface* imsCall);

    /*package*/
    virtual CARAPI Update(
        /* [in] */ IImsPhoneConnection* conn,
        /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
        /* [in] */ ICallState state,
        /* [out] */ Boolean* result);

    /* package */
    virtual CARAPI GetHandoverConnection(
        /* [out] */ IImsPhoneConnection** result);

    virtual CARAPI SwitchWith(
        /* [in] */ IImsPhoneCall* that);

private:
    CARAPI_(void) TakeOver(
        /* [in] */ IImsPhoneCall* that);

public:
    /*package*/
    AutoPtr<IImsPhoneCallTracker> mOwner;

private:
    /*************************** Instance Variables **************************/
    static const String LOGTAG;
    static const Boolean DBG;
    Boolean mRingbackTonePlayed;
};

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECALL_H__

