#ifndef __ELASTOS_APPS_DIALER_CCALLDETAILACTIVITY_H__
#define __ELASTOS_APPS_DIALER_CCALLDETAILACTIVITY_H__

#include "_Elastos_Apps_Dialer_CCallDetailActivity.h"
#include <AnalyticsActivity.h>
#include "elastos/droid/os/AsyncTask.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;
using Elastos::Apps::Dialer::ICallDetailActivity;
using Elastos::Apps::Dialer::IProximitySensorAware;
using Elastos::Apps::Dialer::IProximitySensorManager;
using Elastos::Apps::Dialer::IProximitySensorManagerListener;
using Elastos::Apps::DialerBinder::AnalyticsActivity;

namespace Elastos{
namespace Apps{
namespace Dialer {

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
        class BlankRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL();

            BlankRunnable(
                /* [in] */ CCallDetailActivity* host);

            CARAPI Run();

        private:
            CCallDetailActivity* mHost;
        };

        class UnblankRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL();

            UnblankRunnable(
                /* [in] */ CCallDetailActivity* host);

            CARAPI Run();

        private:
            CCallDetailActivity* mHost;
        };

    public:
        CAR_INTERFACE_DECL();

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

    class MarkVoiceMailAsReadTask
        : public AsyncTask
    {
    public:
        MarkVoiceMailAsReadTask(
            /* [in] */ CCallDetailActivity* host);

        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        CCallDetailActivity* mHost;
    };


    class UpdateContactDetailsTask
        : public AsyncTask
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
            /* [in] */ IPhoneCallDetails details);

    private:
        CCallDetailActivity* mHost;
    };

    class ViewEntry
        : public Object
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
            /* [in] */ StatusMessage* messge;
            /* [in] */ CCallDetailActivity* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        AutoPtr<StatusMessage> mMessage;
        CCallDetailActivity* mHost;
    };

    class RemoveMenuFromCallLogTask
        : public AsyncTask
    {
    public:
        RemoveMenuFromCallLogTask(
            /* [in] */ IStringBuilder* callIds,
            /* [in] */ CCallDetailActivity* host);

        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        AutoPtr<IStringBuilder> mCallIds;
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
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CCallDetailActivity();

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

    CARAPI OnMenuRemoveFromCallLog(
        /* [in] */ IMenuItem* menuItem);

    CARAPI OnMenuEditNumberBeforeCall(
        /* [in] */ IMenuItem* menuItem);

    CARAPI OnMenuTrashVoicemail(
        /* [in] */ IMenuItem* menuItem);

    CARAPI EnableProximitySensor();

    CARAPI DisableProximitySensor(
        /* [in] */ Boolean waitForFarState);

protected:
    CARAPI_(void) UpdateVoicemailStatusMessage(
        /* [in] */ ICursor* statusCursor);

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
    CARAPI_(AutoPtr<ArrayOf<IUri> >) GetCallLogEntryUris();

    /**
     * Update user interface with details of given call.
     *
     * @param callUris URIs into {@link CallLog.Calls} of the calls to be displayed
     */
    CARAPI_(void) UpdateData(
        /* [in] */ ArrayOf<IUri>* callUris);

    /** Return the phone call details for a given call log URI. */
    CARAPI GetPhoneCallDetailsForUri(
        /* [in] */ IUri* callUri,
        /* [out] */ IPhoneCallDetails ** details);

    /** Load the contact photos and places them in the corresponding views. */
    CARAPI_(void) LoadContactPhotos(
        /* [in] */ IUri* contactUri,
        /* [in] */ IUri* photoUri,
        /* [in] */ const String& displayName,
        /* [in] */ const String& lookupKey,
        /* [in] */ Int32 contactType);

    CARAPI_(AutoPtr<IStatusMessage>) GetStatusMessage(
        /* [in] */ ICursor* statusCursor);

    CARAPI_(void) CloseSystemDialogs();

    /** Returns the given text, forced to be left-to-right. */
    static CARAPI_(AutoPtr<ICharSequence>) ForceLeftToRight(
        /* [in] */ ICharSequence* text);

private:
    static const String TAG; // = "CallDetail";

    static const Int32 LOADER_ID; // = 0;
    static const String BUNDLE_CONTACT_URI_EXTRA; // = "contact_uri_extra";

    static const Char32 LEFT_TO_RIGHT_EMBEDDING; // = '\u202A';
    static const Char32 POP_DIRECTIONAL_FORMATTING; // = '\u202C';

    /** The time to wait before enabling the blank the screen due to the proximity sensor. */
    static const Int64 PROXIMITY_BLANK_DELAY_MILLIS; // = 100;
    /** The time to wait before disabling the blank the screen due to the proximity sensor. */
    static const Int64 PROXIMITY_UNBLANK_DELAY_MILLIS; // = 500;

public:
    /** A long array extra containing ids of call log entries to display. */
    static const String EXTRA_CALL_LOG_IDS; // = "EXTRA_CALL_LOG_IDS";
    /** If we are started with a voicemail, we'll find the uri to play with this extra. */
    static const String EXTRA_VOICEMAIL_URI; // = "EXTRA_VOICEMAIL_URI";
    /** If we should immediately start playback of the voicemail, this extra will be set to true. */
    static const String EXTRA_VOICEMAIL_START_PLAYBACK; // = "EXTRA_VOICEMAIL_START_PLAYBACK";
    /** If the activity was triggered from a notification. */
    static const String EXTRA_FROM_NOTIFICATION; // = "EXTRA_FROM_NOTIFICATION";

    static const String VOICEMAIL_FRAGMENT_TAG; // = "voicemail_fragment";

    static const String CALL_LOG_PROJECTION[];

    static Int32 DATE_COLUMN_INDEX; // = 0;
    static Int32 DURATION_COLUMN_INDEX; // = 1;
    static Int32 NUMBER_COLUMN_INDEX; // = 2;
    static Int32 CALL_TYPE_COLUMN_INDEX; // = 3;
    static Int32 COUNTRY_ISO_COLUMN_INDEX; // = 4;
    static Int32 GEOCODED_LOCATION_COLUMN_INDEX; // = 5;
    static Int32 NUMBER_PRESENTATION_COLUMN_INDEX; // = 6;
    static Int32 ACCOUNT_COMPONENT_NAME; // = 7;
    static Int32 ACCOUNT_ID; // = 8;
    static Int32 FEATURES; // = 9;
    static Int32 DATA_USAGE; // = 10;
    static Int32 TRANSCRIPTION_COLUMN_INDEX; // = 11;
private:
    AutoPtr<ICallTypeHelper> CallTypeHelper;
    AutoPtr<IPhoneNumberDisplayHelper> mPhoneNumberHelper;
    AutoPtr<IQuickContactBadge> mQuickContactBadge;
    AutoPtr<ITextView> mCallerName;
    AutoPtr<ITextView> mCallerNumber;
    AutoPtr<ITextView> mAccountLabel;
    AutoPtr<IAsyncTaskExecutor> mAsyncTaskExecutor;
    AutoPtr<IContactInfoHelper> mContactInfoHelper;

    String mNumber;
    String mDefaultCountryIso;

    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<IResources> mResources;
    /** Helper to load contact photos. */
    AutoPtr<IContactPhotoManager> mContactPhotoManager;
    /** Helper to make async queries to content resolver. */
    AutoPtr<ICallDetailActivityQueryHandler> mAsyncQueryHandler;
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

    AutoPtr<IProximitySensorManager> mProximitySensorManager;
    AutoPtr<ProximitySensorListener> mProximitySensorListener;
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CCALLDETAILACTIVITY_H__
