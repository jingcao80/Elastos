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

#ifndef  __ELASTOS_DROID_PHONE_CNETWORKSETTING_H__
#define  __ELASTOS_DROID_PHONE_CNETWORKSETTING_H__

#include "_Elastos_Droid_TeleService_Phone_CNetworkSetting.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IOperatorInfo;
using Elastos::Core::IThrowable;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * "Networks" settings UI for the Phone app.
 */
CarClass(CNetworkSetting)
    , public PreferenceActivity
    , public INetworkSetting
    , public IDialogInterfaceOnCancelListener
{
public:
    class MyNetworkQueryServiceCallback
        : public Object
        , public IINetworkQueryServiceCallback
        , public IBinder
    {
    public:
        TO_STRING_IMPL("CNetworkSetting::MyNetworkQueryServiceCallback")

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ INetworkSetting* host);

        /** place the message on the looper queue upon query completion. */
        CARAPI OnQueryComplete(
            /* [in] */ IList* networkInfoArray,
            /* [in] */ Int32 status);

    private:
        CNetworkSetting* mHost;
    };

private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CNetworkSetting::MyHandler")

        MyHandler(
            /* [in] */ CNetworkSetting* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CNetworkSetting* mHost;
    };

    class MyServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        TO_STRING_IMPL("CNetworkSetting::MyServiceConnection")

        CAR_INTERFACE_DECL()

        MyServiceConnection(
            /* [in] */ CNetworkSetting* host)
            : mHost(host)
        {}

        /** Handle the task of binding the local object to the service */
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* className,
            /* [in] */ IBinder* service);

        /** Handle the task of cleaning up the local binding */
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* className);

    private:
        CNetworkSetting* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CNetworkSetting::MyRunnable")

        MyRunnable(
            /* [in] */ CNetworkSetting* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CNetworkSetting* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CNetworkSetting();

    CARAPI constructor();

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    //implemented for DialogInterface.OnCancelListener
    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    CARAPI GetNormalizedCarrierName(
        /* [in] */ IOperatorInfo* ni,
        /* [out] */ String* name);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    /**
     * Override onDestroy() to unbind the query service, avoiding service
     * leak exceptions.
     */
    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id);

    //@Override
    CARAPI_(void) OnPrepareDialog(
        /* [in] */ Int32 id,
        /* [in] */ IDialog* dialog);

private:
    CARAPI_(void) DisplayEmptyNetworkList(
        /* [in] */ Boolean flag);

    CARAPI_(void) DisplayNetworkSeletionInProgress(
        /* [in] */ const String& networkStr);

    CARAPI_(void) DisplayNetworkQueryFailed(
        /* [in] */ Int32 error);

    CARAPI_(void) DisplayNetworkSelectionFailed(
        /* [in] */ IThrowable* ex);

    CARAPI_(void) DisplayNetworkSelectionSucceeded();

    CARAPI_(void) LoadNetworksList();

    /**
     * networksListLoaded has been rewritten to take an array of
     * OperatorInfo objects and a status field, instead of an
     * AsyncResult.  Otherwise, the functionality which takes the
     * OperatorInfo array and creates a list of preferences from it,
     * remains unchanged.
     */
    CARAPI_(void) NetworksListLoaded(
        /* [in] */ IList* result,
        /* [in] */ Int32 status);

    /**
     * Returns the title of the network obtained in the manual search.
     *
     * @param OperatorInfo contains the information of the network.
     *
     * @return Long Name if not null/empty, otherwise Short Name if not null/empty,
     * else MCCMNC string.
     */

    CARAPI_(String) GetNetworkTitle(
        /* [in] */ IOperatorInfo* ni);

    CARAPI_(void) ClearList();

    CARAPI_(void) SelectNetworkAutomatic();

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

protected:
    Boolean mIsForeground;

private:
    static const String TAG;
    static const Boolean DBG;

    static const Int32 EVENT_NETWORK_SCAN_COMPLETED = 100;
    static const Int32 EVENT_NETWORK_SELECTION_DONE = 200;
    static const Int32 EVENT_AUTO_SELECT_DONE = 300;

    //dialog ids
    static const Int32 DIALOG_NETWORK_SELECTION = 100;
    static const Int32 DIALOG_NETWORK_LIST_LOAD = 200;
    static const Int32 DIALOG_NETWORK_AUTO_SELECT = 300;

    //String keys for preference lookup
    static const String LIST_NETWORKS_KEY;
    static const String BUTTON_SRCH_NETWRKS_KEY;
    static const String BUTTON_AUTO_SELECT_KEY;

    //map of network controls to the network data.
    AutoPtr<IHashMap> mNetworkMap;

    AutoPtr<IPhone> mPhone;

    AutoPtr<IUserManager> mUm;
    Boolean mUnavailable;

    /** message for network selection */
    String mNetworkSelectMsg;

    //preference objects
    AutoPtr<IPreferenceGroup> mNetworkList;
    AutoPtr<IPreference> mSearchButton;
    AutoPtr<IPreference> mAutoSelect;

    AutoPtr<IHandler> mHandler;

    /**
     * Service connection code for the NetworkQueryService.
     * Handles the work of binding to a local object so that we can make
     * the appropriate service calls.
     */

    /** Local service interface */
    AutoPtr<IINetworkQueryService> mNetworkQueryService;

    /** Service connection */
    AutoPtr<IServiceConnection> mNetworkQueryServiceConnection;

    /**
     * This implementation of INetworkQueryServiceCallback is used to receive
     * callback notifications from the network query service.
     */
    AutoPtr<IINetworkQueryServiceCallback> mCallback;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CNETWORKSETTING_H__