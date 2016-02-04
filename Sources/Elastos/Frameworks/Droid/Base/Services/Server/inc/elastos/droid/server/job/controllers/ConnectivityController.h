/*
 * Copyright (C) 2014 The Android Open Source Project
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
 * limitations under the License
 */

package com.android.server.job.controllers;


using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::IConnectivityService;
using Elastos::Droid::Server::Ijob.JobSchedulerService;
using Elastos::Droid::Server::Ijob.StateChangedListener;

using Elastos::IO::IPrintWriter;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IList;

/**
 * Handles changes in connectivity.
 * We are only interested in metered vs. unmetered networks, and we're interested in them on a
 * per-user basis.
 */
public class ConnectivityController extends StateController implements
        ConnectivityManager.OnNetworkActiveListener {
    private static const String TAG = "JobScheduler.Conn";

    private final List<JobStatus> mTrackedJobs = new LinkedList<JobStatus>();
    private final BroadcastReceiver mConnectivityChangedReceiver =
            new ConnectivityChangedReceiver();
    /** Singleton. */
    private static ConnectivityController mSingleton;
    private static Object sCreationLock = new Object();
    /** Track whether the latest active network is metered. */
    private Boolean mNetworkUnmetered;
    /** Track whether the latest active network is connected. */
    private Boolean mNetworkConnected;

    public static ConnectivityController Get(JobSchedulerService jms) {
        synchronized(sCreationLock) {
            if (mSingleton == NULL) {
                mSingleton = new ConnectivityController(jms, jms->GetContext());
            }
            return mSingleton;
        }
    }

    private ConnectivityController(StateChangedListener stateChangedListener, Context context) {
        Super(stateChangedListener, context);
        // Register connectivity changed BR.
        IntentFilter intentFilter = new IntentFilter();
        intentFilter->AddAction(ConnectivityManager.CONNECTIVITY_ACTION);
        mContext->RegisterReceiverAsUser(
                mConnectivityChangedReceiver, UserHandle.ALL, intentFilter, NULL, NULL);
        ConnectivityService cs =
                (ConnectivityService)ServiceManager->GetService(Context.CONNECTIVITY_SERVICE);
        if (cs != NULL) {
            if (cs->GetActiveNetworkInfo() != NULL) {
                mNetworkConnected = cs->GetActiveNetworkInfo()->IsConnected();
            }
            mNetworkUnmetered = mNetworkConnected && !cs->IsActiveNetworkMetered();
        }
    }

    //@Override
    CARAPI MaybeStartTrackingJob(JobStatus jobStatus) {
        if (jobStatus->HasConnectivityConstraint() || jobStatus->HasUnmeteredConstraint()) {
            synchronized(mTrackedJobs) {
                jobStatus.connectivityConstraintSatisfied->Set(mNetworkConnected);
                jobStatus.unmeteredConstraintSatisfied->Set(mNetworkUnmetered);
                mTrackedJobs->Add(jobStatus);
            }
        }
    }

    //@Override
    CARAPI MaybeStopTrackingJob(JobStatus jobStatus) {
        if (jobStatus->HasConnectivityConstraint() || jobStatus->HasUnmeteredConstraint()) {
            synchronized(mTrackedJobs) {
                mTrackedJobs->Remove(jobStatus);
            }
        }
    }

    /**
     * @param userId Id of the user for whom we are updating the connectivity state.
     */
    private void UpdateTrackedJobs(Int32 userId) {
        synchronized(mTrackedJobs) {
            Boolean changed = FALSE;
            for (JobStatus js : mTrackedJobs) {
                if (js->GetUserId() != userId) {
                    continue;
                }
                Boolean prevIsConnected =
                        js.connectivityConstraintSatisfied->GetAndSet(mNetworkConnected);
                Boolean prevIsMetered = js.unmeteredConstraintSatisfied->GetAndSet(mNetworkUnmetered);
                if (prevIsConnected != mNetworkConnected || prevIsMetered != mNetworkUnmetered) {
                    changed = TRUE;
                }
            }
            if (changed) {
                mStateChangedListener->OnControllerStateChanged();
            }
        }
    }

    /**
     * We know the network has just come up. We want to run any jobs that are ready.
     */
    public synchronized void OnNetworkActive() {
        synchronized(mTrackedJobs) {
            for (JobStatus js : mTrackedJobs) {
                if (js->IsReady()) {
                    if (DEBUG) {
                        Slogger::D(TAG, "Running " + js + " due to network activity.");
                    }
                    mStateChangedListener->OnRunJobNow(js);
                }
            }
        }
    }

    class ConnectivityChangedReceiver extends BroadcastReceiver {
        /**
         * We'll receive connectivity changes for each user here, which we process independently.
         * We are only interested in the active network here. We're only interested in the active
         * network, b/c the end result of this will be for apps to try to hit the network.
         * @param context The Context in which the receiver is running.
         * @param intent The Intent being received.
         */
        // TODO: Test whether this will be called twice for each user.
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (DEBUG) {
                Slogger::D(TAG, "Received connectivity event: " + intent->GetAction() + " u"
                        + context->GetUserId());
            }
            final String action = intent->GetAction();
            if (action->Equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
                final Int32 networkType =
                        intent->GetIntExtra(ConnectivityManager.EXTRA_NETWORK_TYPE,
                                ConnectivityManager.TYPE_NONE);
                // Connectivity manager for THIS context - important!
                final ConnectivityManager connManager = (ConnectivityManager)
                        context->GetSystemService(Context.CONNECTIVITY_SERVICE);
                final NetworkInfo activeNetwork = connManager->GetActiveNetworkInfo();
                final Int32 userid = context->GetUserId();
                // This broadcast gets sent a lot, only update if the active network has changed.
                if (activeNetwork == NULL) {
                    mNetworkUnmetered = FALSE;
                    mNetworkConnected = FALSE;
                    UpdateTrackedJobs(userid);
                } else if (activeNetwork->GetType() == networkType) {
                    mNetworkUnmetered = FALSE;
                    mNetworkConnected = !intent->GetBooleanExtra(
                            ConnectivityManager.EXTRA_NO_CONNECTIVITY, FALSE);
                    if (mNetworkConnected) {  // No point making the call if we know there's no conn.
                        mNetworkUnmetered = !connManager->IsActiveNetworkMetered();
                    }
                    UpdateTrackedJobs(userid);
                }
            } else {
                if (DEBUG) {
                    Slogger::D(TAG, "Unrecognised action in intent: " + action);
                }
            }
        }
    };

    //@Override
    CARAPI DumpControllerState(PrintWriter pw) {
        pw->Println("Conn.");
        pw->Println("connected: " + mNetworkConnected + " unmetered: " + mNetworkUnmetered);
        for (JobStatus js: mTrackedJobs) {
            pw->Println(String->ValueOf(js->HashCode()).Substring(0, 3) + ".."
                    + ": C=" + js->HasConnectivityConstraint()
                    + ", UM=" + js->HasUnmeteredConstraint());
        }
    }
}
