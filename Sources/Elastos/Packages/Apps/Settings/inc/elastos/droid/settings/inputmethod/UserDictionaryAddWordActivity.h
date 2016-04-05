

package com.android.settings.inputmethod;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::View::IView;

public class UserDictionaryAddWordActivity extends Activity {

    public static const String MODE_EDIT_ACTION = "com.android.settings.USER_DICTIONARY_EDIT";
    public static const String MODE_INSERT_ACTION = "com.android.settings.USER_DICTIONARY_INSERT";

    /* package */ static const Int32 CODE_WORD_ADDED = 0;
    /* package */ static const Int32 CODE_CANCEL = 1;
    /* package */ static const Int32 CODE_ALREADY_PRESENT = 2;

    private UserDictionaryAddWordContents mContents;

    //@Override
    CARAPI OnCreate(final Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        SetContentView(R.layout.user_dictionary_add_word);
        final Intent intent = GetIntent();
        final String action = intent->GetAction();
        final Int32 mode;
        if (MODE_EDIT_ACTION->Equals(action)) {
            mode = UserDictionaryAddWordContents.MODE_EDIT;
        } else if (MODE_INSERT_ACTION->Equals(action)) {
            mode = UserDictionaryAddWordContents.MODE_INSERT;
        } else {
            // Can never come here because we only support these two actions in the manifest
            throw new RuntimeException("Unsupported action: " + action);
        }

        // The following will get the EXTRA_WORD and EXTRA_LOCALE fields that are in the intent.
        // We do need to add the action by hand, because UserDictionaryAddWordContents expects
        // it to be in the bundle, in the EXTRA_MODE key.
        final Bundle args = intent->GetExtras();
        args->PutInt(UserDictionaryAddWordContents.EXTRA_MODE, mode);

        if (NULL != savedInstanceState) {
            // Override options if we have a saved state.
            args->PutAll(savedInstanceState);
        }

        mContents = new UserDictionaryAddWordContents(GetWindow()->GetDecorView(), args);
    }

    //@Override
    CARAPI OnSaveInstanceState(final Bundle outState) {
        mContents->SaveStateIntoBundle(outState);
    }

    private void ReportBackToCaller(final Int32 resultCode, final Bundle result) {
        final Intent senderIntent = GetIntent();
        final Object listener = senderIntent->GetExtras()->Get("listener");
        if (!(listener instanceof Messenger)) return; // This will work if listener is NULL too.
        final Messenger messenger = (Messenger)listener;

        final Message m = Message->Obtain();
        m.obj = result;
        m.what = resultCode;
        try {
            messenger->Send(m);
        } catch (RemoteException e) {
            // Couldn't report back, but there is nothing we can do to fix it
        }
    }

    CARAPI OnClickCancel(final View v) {
        ReportBackToCaller(CODE_CANCEL, NULL);
        Finish();
    }

    CARAPI OnClickConfirm(final View v) {
        final Bundle parameters = new Bundle();
        final Int32 resultCode = mContents->Apply(this, parameters);
        ReportBackToCaller(resultCode, parameters);
        Finish();
    }
}
