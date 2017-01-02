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

#ifndef __ELASTOS_DROID_DIALER_CCALLDETAILACTIVITY_H__
#define __ELASTOS_DROID_DIALER_CCALLDETAILACTIVITY_H__

#include "_Elastos_Droid_Dialer_CCallDetailActivity.h"
#include "elastos/droid/dialer/ProximitySensorManager.h"
#include "elastos/droid/dialer/CallDetailActivityQueryHandler.h"
#include "elastos/droid/dialer/PhoneCallDetails.h"
#include "elastos/droid/dialer/calllog/CallTypeHelper.h"
#include "elastos/droid/dialer/calllog/PhoneNumberDisplayHelper.h"
#include "elastos/droid/dialer/calllog/ContactInfoHelper.h"
#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperStatusMessage.h"
#include "elastos/droid/dialerbind/analytics/AnalyticsActivity.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/core/Runnable.h"
#include "elastos/core/StringBuilder.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Text.h"

using Elastos::Droid::Contacts::Common::IContactPhotoManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Dialer::CallLog::CallTypeHelper;
using Elastos::Droid::Dialer::CallLog::ContactInfoHelper;
using Elastos::Droid::Dialer::CallLog::PhoneNumberDisplayHelper;
using Elastos::Droid::Dialer::Util::IAsyncTaskExecutor;
using Elastos::Droid::Dialer::Voicemail::IVoicemailStatusHelper;
using Elastos::Droid::Dialer::Voicemail::VoicemailStatusHelperStatusMessage;
using Elastos::Droid::DialerBind::Analytics::AnalyticsActivity;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IQuickContactBadge;
using Elastos::Core::Runnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Dialer {

/**
 * Displays the details of a specific call log entry.
 * <p>
 * This activity can be either started with the URI of a single call log entry, or with the
 * {@link #EXTRA_CALL_LOG_IDS} extra to specify a group of call log entries.
 */
CarClass(CCallDetailActivity)
    , public AnalyticsActivity
    , public IProximitySensorAware
    , public ICallDetailActivity
{
private:
    /** Listener to changes in the proximity sensor state. */
    class ProximitySensorListener
        : public Object
        , public IProximitySensorManagerListener
    {
    private:
        /** Used to show a blank view and hide the action bar. */
        class BlankRunnable : public Runnable
        {
        public:
            BlankRunnable(
                /* [in] */ CCallDetailActivity* host)
                : mHost(host)
            {}

            CARAPI Run();

        private:
            CCallDetailActivity* mHost;
        };

        class UnblankRunnable : public Runnable
        {
        public:
            UnblankRunnable(
                /* [in] */ CCallDetailActivity* host);

            CARAPI Run();

        private:
            CCallDetailActivity* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        ProximitySensorListener(
            /* [in] */ CCallDetailActivity* host);

        CARAPI OnNear();

        CARAPI OnFar();

        /** Removed any delayed requests that may be pending. */
        CARAPI_(void) ClearPendingRequests();

    private:
        /** Post a {@link Runnable} with a delay on the main thread. */
        CARAPI_(void) PostDelayed(
            /* [in] */ IRunnable* runnable,
            /* [in] */ Int64 delayMillis);

    private:
        CCallDetailActivity* mHost;
        AutoPtr<IRunnable> mBlankRunnable;
        AutoPtr<IRunnable> mUnblankRunnable;
    };

    class MarkVoiceMailAsReadTask : public AsyncTask
    {
    public:
        MarkVoiceMailAsReadTask(
            /* [in] */ IUri* voicemailUri,
            /* [in] */ CCallDetailActivity* host);

        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        AutoPtr<IUri> mVoicemailUri;
        CCallDetailActivity* mHost;
    };


    class UpdateContactDetailsTask : public AsyncTask
    {
    public:
        UpdateContactDetailsTask(
            /* [in] */ CCallDetailActivity* host);

        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* callUris,
            /* [out] */ IInterface** result);

        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        /**
         * Determines the location geocode text for a call, or the phone number type
         * (if available).
         *
         * @param details The call details.
         * @return The phone number type or location.
         */
        CARAPI_(AutoPtr<ICharSequence>) GetNumberTypeOrLocation(
            /* [in] */ PhoneCallDetails* details);

    private:
        CCallDetailActivity* mHost;
    };

    class ViewEntry : public Object
    {
    public:
        ViewEntry(
            /* [in] */ const String& text,
            /* [in] */ IIntent* intent,
            /* [in] */ const String& description,
            /* [in] */ CCallDetailActivity* host);

        CARAPI_(void) SetSecondaryAction(
            /* [in] */ Int32 icon,
            /* [in] */ IIntent* intent,
            /* [in] */ const String& description);

    public:
        String mText;
        AutoPtr<IIntent> mPrimaryIntent;
        /** The description for accessibility of the primary action. */
        String mPrimaryDescription;

        AutoPtr<ICharSequence> mLabel;
        /** Icon for the secondary action. */
        Int32 mSecondaryIcon;
        /** Intent for the secondary action. If not null, an icon must be defined. */
        AutoPtr<IIntent> mSecondaryIntent;
        /** The description for accessibility of the secondary action. */
        String mSecondaryDescription;

    private:
        CCallDetailActivity* mHost;
    };

    class StatusMessageViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        StatusMessageViewOnClickListener(
            /* [in] */ VoicemailStatusHelperStatusMessage* messge,
            /* [in] */ CCallDetailActivity* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        AutoPtr<VoicemailStatusHelperStatusMessage> mMessage;
        CCallDetailActivity* mHost;
    };

    class RemoveMenuFromCallLogTask
        : public AsyncTask
    {
    public:
        RemoveMenuFromCallLogTask(
            /* [in] */ StringBuilder* callIds,
            /* [in] */ CCallDetailActivity* host);

        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        AutoPtr<StringBuilder> mCallIds;
        CCallDetailActivity* mHost;
    };

    class MenuTrashVoicemailTask
        : public AsyncTask
    {
    public:
        MenuTrashVoicemailTask(
            /* [in] */ CCallDetailActivity* host);

        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        CCallDetailActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCallDetailActivity();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    // @Override
    CARAPI OnResume();

    // @Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* allowToShow);

    // @Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* res);

    CARAPI_(void) OnMenuRemoveFromCallLog(
        /* [in] */ IMenuItem* menuItem);

    CARAPI_(void) OnMenuEditNumberBeforeCall(
        /* [in] */ IMenuItem* menuItem);

    CARAPI_(void) OnMenuTrashVoicemail(
        /* [in] */ IMenuItem* menuItem);

    // @Override
    CARAPI EnableProximitySensor();

    // @Override
    CARAPI DisableProximitySensor(
        /* [in] */ Boolean waitForFarState);

protected:
    CARAPI_(void) UpdateVoicemailStatusMessage(
        /* [in] */ ICursor* statusCursor);

    // @Override
    CARAPI OnPause();

private:
    /**
     * Handle voicemail playback or hide voicemail ui.
     * <p>
     * If the Intent used to start this Activity contains the suitable extras, then start voicemail
     * playback.  If it doesn't, then don't inflate the voicemail ui.
     */
    CARAPI_(void) OptionallyHandleVoicemail();

    CARAPI_(Boolean) HasVoicemail();

    CARAPI_(void) MarkVoicemailAsRead(
        /* [in] */ IUri* voicemailUri);

    /**
     * Returns the list of URIs to show.
     * <p>
     * There are two ways the URIs can be provided to the activity: as the data on the intent, or as
     * a list of ids in the call log added as an extra on the URI.
     * <p>
     * If both are available, the data on the intent takes precedence.
     */
    CARAPI_(AutoPtr<ArrayOf<IUri*> >) GetCallLogEntryUris();

    /**
     * Update user interface with details of given call.
     *
     * @param callUris URIs into {@link CallLog.Calls} of the calls to be displayed
     */
    CARAPI_(void) UpdateData(
        /* [in] */ ArrayOf<IUri*>* callUris);

    /** Return the phone call details for a given call log URI. */
    CARAPI GetPhoneCallDetailsForUri(
        /* [in] */ IUri* callUri,
        /* [out] */ PhoneCallDetails** details);

    /** Load the contact photos and places them in the corresponding views. */
    CARAPI_(void) LoadContactPhotos(
        /* [in] */ IUri* contactUri,
        /* [in] */ IUri* photoUri,
        /* [in] */ const String& displayName,
        /* [in] */ const String& lookupKey,
        /* [in] */ Int32 contactType);

    CARAPI_(AutoPtr<VoicemailStatusHelperStatusMessage>) GetStatusMessage(
        /* [in] */ ICursor* statusCursor);

    CARAPI_(void) CloseSystemDialogs();

    /** Returns the given text, forced to be left-to-right. */
    static CARAPI_(AutoPtr<ICharSequence>) ForceLeftToRight(
        /* [in] */ ICharSequence* text);

public:
    /** The enumeration of {@link AsyncTask} objects used in this class. */
    enum Tasks {
        MARK_VOICEMAIL_READ,
        DELETE_VOICEMAIL_AND_FINISH,
        REMOVE_FROM_CALL_LOG_AND_FINISH,
        UPDATE_PHONE_CALL_DETAILS,
    };

    static const AutoPtr<ArrayOf<String> > CALL_LOG_PROJECTION;

    static const Int32 DATE_COLUMN_INDEX = 0;
    static const Int32 DURATION_COLUMN_INDEX = 1;
    static const Int32 NUMBER_COLUMN_INDEX = 2;
    static const Int32 CALL_TYPE_COLUMN_INDEX = 3;
    static const Int32 COUNTRY_ISO_COLUMN_INDEX = 4;
    static const Int32 GEOCODED_LOCATION_COLUMN_INDEX = 5;
    static const Int32 NUMBER_PRESENTATION_COLUMN_INDEX = 6;
    static const Int32 ACCOUNT_COMPONENT_NAME = 7;
    static const Int32 ACCOUNT_ID = 8;
    static const Int32 FEATURES = 9;
    static const Int32 DATA_USAGE = 10;
    static const Int32 TRANSCRIPTION_COLUMN_INDEX = 11;

private:
    static const String TAG;

    static const Int32 LOADER_ID = 0;
    static const String BUNDLE_CONTACT_URI_EXTRA;

    static const Char32 LEFT_TO_RIGHT_EMBEDDING = 0x202A;
    static const Char32 POP_DIRECTIONAL_FORMATTING = 0x202C;

    /** The time to wait before enabling the blank the screen due to the proximity sensor. */
    static const Int64 PROXIMITY_BLANK_DELAY_MILLIS = 100;
    /** The time to wait before disabling the blank the screen due to the proximity sensor. */
    static const Int64 PROXIMITY_UNBLANK_DELAY_MILLIS = 500;

    AutoPtr<CallTypeHelper> mCallTypeHelper;
    AutoPtr<PhoneNumberDisplayHelper> mPhoneNumberHelper;
    AutoPtr<IQuickContactBadge> mQuickContactBadge;
    AutoPtr<ITextView> mCallerName;
    AutoPtr<ITextView> mCallerNumber;
    AutoPtr<ITextView> mAccountLabel;
    AutoPtr<IAsyncTaskExecutor> mAsyncTaskExecutor;
    AutoPtr<ContactInfoHelper> mContactInfoHelper;

    String mNumber;
    String mDefaultCountryIso;

    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<IResources> mResources;
    /** Helper to load contact photos. */
    AutoPtr<IContactPhotoManager> mContactPhotoManager;
    /** Helper to make async queries to content resolver. */
    AutoPtr<CallDetailActivityQueryHandler> mAsyncQueryHandler;
    /** Helper to get voicemail status messages. */
    AutoPtr<IVoicemailStatusHelper> mVoicemailStatusHelper;
    // Views related to voicemail status message.
    AutoPtr<IView> mStatusMessageView;
    AutoPtr<ITextView> mStatusMessageText;
    AutoPtr<ITextView> mStatusMessageAction;
    AutoPtr<ITextView> mVoicemailTranscription;
    AutoPtr<ILinearLayout> mVoicemailHeader;

    AutoPtr<IUri> mVoicemailUri;
    AutoPtr<IBidiFormatter> mBidiFormatter; // = BidiFormatter.getInstance();

    /** Whether we should show "edit number before call" in the options menu. */
    Boolean mHasEditNumberBeforeCallOption;
    /** Whether we should show "trash" in the options menu. */
    Boolean mHasTrashOption;
    /** Whether we should show "remove from call log" in the options menu. */
    Boolean mHasRemoveFromCallLogOption;

    AutoPtr<ProximitySensorManager> mProximitySensorManager;
    AutoPtr<ProximitySensorListener> mProximitySensorListener;

    friend class UpdateContactDetailsTask;
    friend class CallDetailActivityQueryHandler;
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CCALLDETAILACTIVITY_H__
