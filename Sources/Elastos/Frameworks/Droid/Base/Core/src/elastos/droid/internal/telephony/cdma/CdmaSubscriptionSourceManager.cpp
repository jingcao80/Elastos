/*
 * Copyright (c) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.cdma;

using Elastos::Utility::Concurrent::Atomic::IAtomicInteger;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telephony::IRlog;

/**
 * Class that handles the CDMA subscription source changed events from RIL
 */
public class CdmaSubscriptionSourceManager extends Handler {
    static const String LOG_TAG = "CdmaSSM";
    private static const Int32 EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED = 1;
    private static const Int32 EVENT_GET_CDMA_SUBSCRIPTION_SOURCE     = 2;
    private static const Int32 EVENT_RADIO_ON                         = 3;
    private static const Int32 EVENT_SUBSCRIPTION_STATUS_CHANGED      = 4;

    // To know subscription is activated
    private static const Int32 SUBSCRIPTION_ACTIVATED                 = 1;

    public static const Int32 SUBSCRIPTION_SOURCE_UNKNOWN = -1;
    public static const Int32 SUBSCRIPTION_FROM_RUIM      = 0; /* CDMA subscription from RUIM */
    public static const Int32 SUBSCRIPTION_FROM_NV        = 1; /* CDMA subscription from NV */
    public static const Int32 PREFERRED_CDMA_SUBSCRIPTION = SUBSCRIPTION_FROM_NV;

    private static CdmaSubscriptionSourceManager sInstance;
    private static const Object sReferenceCountMonitor = new Object();
    private static Int32 sReferenceCount = 0;

    // ***** Instance Variables
    private CommandsInterface mCi;
    private Context mContext;
    private RegistrantList mCdmaSubscriptionSourceChangedRegistrants = new RegistrantList();

    // Type of CDMA subscription source
    private AtomicInteger mCdmaSubscriptionSource = new AtomicInteger(SUBSCRIPTION_FROM_NV);

    // Constructor
    private CdmaSubscriptionSourceManager(Context context, CommandsInterface ci) {
        mContext = context;
        mCi = ci;
        mCi->RegisterForCdmaSubscriptionChanged(this, EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL);
        mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
        Int32 subscriptionSource = GetDefault(context);
        mCdmaSubscriptionSource->Set(subscriptionSource);
        mCi->RegisterForSubscriptionStatusChanged(this, EVENT_SUBSCRIPTION_STATUS_CHANGED, NULL);
    }

    /**
     * This function creates a single instance of this class
     *
     * @return object of type CdmaSubscriptionSourceManager
     */
    public static CdmaSubscriptionSourceManager GetInstance(Context context,
            CommandsInterface ci, Handler h, Int32 what, Object obj) {
        Synchronized (sReferenceCountMonitor) {
            If (NULL == sInstance) {
                sInstance = new CdmaSubscriptionSourceManager(context, ci);
            }
            CdmaSubscriptionSourceManager.sReferenceCount++;
        }
        sInstance->RegisterForCdmaSubscriptionSourceChanged(h, what, obj);
        return sInstance;
    }

    /**
     * Unregisters for the registered event with RIL
     */
    CARAPI Dispose(Handler h) {
        mCdmaSubscriptionSourceChangedRegistrants->Remove(h);
        Synchronized (sReferenceCountMonitor) {
            sReferenceCount--;
            If (sReferenceCount <= 0) {
                mCi->UnregisterForCdmaSubscriptionChanged(this);
                mCi->UnregisterForOn(this);
                mCi->UnregisterForSubscriptionStatusChanged(this);
                sInstance = NULL;
            }
        }
    }

    /*
     * (non-Javadoc)
     * @see android.os.Handler#HandleMessage(android.os.Message)
     */
    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;
        Switch (msg.what) {
            case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED:
            case EVENT_GET_CDMA_SUBSCRIPTION_SOURCE:
            {
                Log("CDMA_SUBSCRIPTION_SOURCE event = " + msg.what);
                ar = (AsyncResult) msg.obj;
                HandleGetCdmaSubscriptionSource(ar);
            }
            break;
            case EVENT_RADIO_ON: {
                mCi->GetCdmaSubscriptionSource(ObtainMessage(EVENT_GET_CDMA_SUBSCRIPTION_SOURCE));
            }
            break;
            case EVENT_SUBSCRIPTION_STATUS_CHANGED: {
                Log("EVENT_SUBSCRIPTION_STATUS_CHANGED");
                ar = (AsyncResult)msg.obj;
                If (ar.exception == NULL) {
                    Int32 actStatus = ((Int32[])ar.result)[0];
                    Log("actStatus = " + actStatus);
                    If (actStatus == SUBSCRIPTION_ACTIVATED) { // Subscription Activated
                        // In case of multi-SIM, framework should wait for the subscription ready
                        // to send any request to RIL.  Otherwise it will return failure.
                        Rlog->V(LOG_TAG,"get Cdma Subscription Source");
                        mCi->GetCdmaSubscriptionSource(
                                ObtainMessage(EVENT_GET_CDMA_SUBSCRIPTION_SOURCE));
                    }
                } else {
                    Logw("EVENT_SUBSCRIPTION_STATUS_CHANGED, Exception:" + ar.exception);
                }
            }
            break;
            default:
                super->HandleMessage(msg);
        }
    }

    /**
     * Returns the current CDMA subscription source value
     * @return CDMA subscription source value
     */
    public Int32 GetCdmaSubscriptionSource() {
        return mCdmaSubscriptionSource->Get();
    }

    /**
     * Gets the default CDMA subscription source
     *
     * @return Default CDMA subscription source from Settings DB if present.
     */
    public static Int32 GetDefault(Context context) {
        // Get the default value from the Settings
        Int32 subscriptionSource = Settings::Global::>GetInt(context->GetContentResolver(),
                Settings::Global::CDMA_SUBSCRIPTION_MODE, PREFERRED_CDMA_SUBSCRIPTION);
        return subscriptionSource;
    }

    /**
     * Clients automatically register for CDMA subscription source changed event
     * when they get an instance of this object.
     */
    private void RegisterForCdmaSubscriptionSourceChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mCdmaSubscriptionSourceChangedRegistrants->Add(r);
    }

    /**
     * Handles the call to get the subscription source
     *
     * @param ar AsyncResult object that contains the result of get CDMA
     *            subscription source call
     */
    private void HandleGetCdmaSubscriptionSource(AsyncResult ar) {
        If ((ar.exception == NULL) && (ar.result != NULL)) {
            Int32 newSubscriptionSource = ((Int32[]) ar.result)[0];

            If (newSubscriptionSource != mCdmaSubscriptionSource->Get()) {
                Log("Subscription Source Changed : " + mCdmaSubscriptionSource + " >> "
                        + newSubscriptionSource);
                mCdmaSubscriptionSource->Set(newSubscriptionSource);

                // Notify registrants of the new CDMA subscription source
                mCdmaSubscriptionSourceChangedRegistrants->NotifyRegistrants(new AsyncResult(NULL,
                        NULL, NULL));
            }
        } else {
            // GET_CDMA_SUBSCRIPTION is returning Failure. Probably
            // because modem created GSM Phone. If modem created
            // GSMPhone, then PhoneProxy will trigger a change in
            // Phone objects and this object will be destroyed.
            Logw("Unable to get CDMA Subscription Source, Exception: " + ar.exception
                    + ", result: " + ar.result);
        }
    }

    private void Log(String s) {
        Rlog->D(LOG_TAG, s);
    }

    private void Logw(String s) {
        Rlog->W(LOG_TAG, s);
    }

}
