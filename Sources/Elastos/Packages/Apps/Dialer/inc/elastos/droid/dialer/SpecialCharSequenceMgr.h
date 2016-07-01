#ifndef __ELASTOS_DROID_DIALER_SPECIALCHARSEQUENCEMGR_H__
#define __ELASTOS_DROID_DIALER_SPECIALCHARSEQUENCEMGR_H__

#include <elastos/core/Object.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Widget::IEditText;

namespace Elastos {
namespace Droid {
namespace Dialer {

/**
 * Helper class to listen for some magic character sequences
 * that are handled specially by the dialer.
 *
 * Note the Phone app also handles these sequences too (in a couple of
 * relatively obscure places in the UI), so there's a separate version of
 * this class under apps/Phone.
 *
 * TODO: there's lots of duplicated code between this class and the
 * corresponding class under apps/Phone.  Let's figure out a way to
 * unify these two classes (in the framework? in a common shared library?)
 */
class SpecialCharSequenceMgr
    : public Object
{
private:
    /**
     * Cookie object that contains everything we need to communicate to the
     * handler's onQuery Complete, as well as what we need in order to cancel
     * the query (if requested).
     *
     * Note, access to the textField field is going to be synchronized, because
     * the user can request a cancel at any time through the UI.
     */
    class SimContactQueryCookie
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        CAR_INTERFACE_DECL();

        SimContactQueryCookie(
            /* [in] */ Int32 number,
            // TODO:
            // /* [in] */ QueryHandler* handler,
            /* [in] */ Int32 token);

        /**
         * Synchronized getter for the EditText.
         */
        /*synchronized*/ CARAPI_(AutoPtr<IEditText>) GetTextField();

        /**
         * Synchronized setter for the EditText.
         */
        /*synchronized*/ CARAPI_(void) SetTextField(
            /* [in] */ IEditText* text);

        /**
         * Cancel the ADN query by stopping the operation and signaling
         * the cookie that a cancel request is made.
         */
        /*synchronized*/ CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    public:
        AutoPtr<IProgressDialog> mProgressDialog;
        Int32 mContactNum;

    private:
        // Used to identify the query request.
        Int32 mToken;
        // TODO:
        // AutoPtr<QueryHandler> mHandler;

        // The text field we're going to update
        AutoPtr<IEditText> mTextField;
    };

    /**
     * Asynchronous query handler that services requests to look up ADNs
     *
     * Queries originate from {@link #handleAdnEntry}.
     */
    // class QueryHandler
    //     : public NoNullCursorAsyncQueryHandler
    // {
    // public:
    //     CARAPI constructor(
    //     /* [in] */ IContentResolver* cr);

    //     CARAPI_(void) Cancel();

    // protected:
    //     /*
    //      * Override basic onQueryComplete to fill in the textfield when
    //      * we're handed the ADN cursor.

    //     // @Override*/
    //     CARAPI OnNotNullableQueryComplete(
    //         /* [in] */ Int32 token,
    //         /* [in] */ IInterface* cookie,
    //         /* [in] */ ICursor* cursor);

    // private:
    //     Boolean mCanceled;
    // };

public:
    static CARAPI_(Boolean) HandleChars(
        /* [in] */ IContext* context,
        /* [in] */ const String& input,
        /* [in] */ IEditText* textField);

    static CARAPI_(Boolean) HandleChars(
        /* [in] */ IContext* context,
        /* [in] */ const String& input);

    static CARAPI_(Boolean) HandleChars(
        /* [in] */ IContext* context,
        /* [in] */ const String& input,
        /* [in] */ Boolean useSystemWindow,
        /* [in] */ IEditText* textField);

    /**
     * Cleanup everything around this class. Must be run inside the main thread.
     *
     * This should be called when the screen becomes background.
     */
    static CARAPI_(void) Cleanup();

    /**
     * Handles secret codes to launch arbitrary activities in the form of *#*#<code>#*#*.
     * If a secret code is encountered an Intent is started with the android_secret_code://<code>
     * URI.
     *
     * @param context the context to use
     * @param input the text to check for a secret code in
     * @return true if a secret code was encountered
     */
    static CARAPI_(Boolean) HandleSecretCode(
        /* [in] */ IContext* context,
        /* [in] */ const String& input);

    /**
     * Handle ADN requests by filling in the SIM contact number into the requested
     * EditText.
     *
     * This code works alongside the Asynchronous query handler {@link QueryHandler}
     * and query cancel handler implemented in {@link SimContactQueryCookie}.
     */
    static CARAPI_(Boolean) HandleAdnEntry(
        /* [in] */ IContext* context,
        /* [in] */ const String& input,
        /* [in] */ IEditText* textField);

    static CARAPI_(Boolean) HandlePinEntry(
        /* [in] */ IContext* context,
        /* [in] */ const String& input);

    static CARAPI_(Boolean) HandleIMEIDisplay(
        /* [in] */ IContext* context,
        /* [in] */ const String& input,
        /* [in] */ Boolean useSystemWindow);


private:
    /** This class is never instantiated. */
    SpecialCharSequenceMgr();

    static CARAPI_(Boolean) HandleRegulatoryInfoDisplay(
        /* [in] */ IContext* context,
        /* [in] */ const String& input);

    // TODO: Combine showIMEIPanel() and showMEIDPanel() into a single
    // generic "showDeviceIdPanel()" method, like in the apps/Phone
    // version of SpecialCharSequenceMgr.java.  (This will require moving
    // the phone app's TelephonyCapabilities.getDeviceIdLabel() method
    // into the telephony framework, though.)

    static CARAPI_(void) ShowIMEIPanel(
        /* [in] */ IContext* context,
        /* [in] */ Boolean useSystemWindow,
        /* [in] */ ITelephonyManager* telephonyManager);

    static CARAPI_(void) ShowMEIDPanel(
        /* [in] */ IContext* context,
        /* [in] */ Boolean useSystemWindow,
        /* [in] */ ITelephonyManager* telephonyManager);

private:
    static const String TAG; // = "SpecialCharSequenceMgr";

    static const String SECRET_CODE_ACTION; // = "android.provider.Telephony.SECRET_CODE";
    static const String MMI_IMEI_DISPLAY; // = "*#06#";
    static const String MMI_REGULATORY_INFO_DISPLAY; // = "*#07#";

    /**
     * Remembers the previous {@link QueryHandler} and cancel the operation when needed, to
     * prevent possible crash.
     *
     * QueryHandler may call {@link ProgressDialog#dismiss()} when the screen is already gone,
     * which will cause the app crash. This variable enables the class to prevent the crash
     * on {@link #cleanup()}.
     *
     * TODO: Remove this and replace it (and {@link #cleanup()}) with better implementation.
     * One complication is that we have SpecialCharSequenceMgr in Phone package too, which has
     * *slightly* different implementation. Note that Phone package doesn't have this problem,
     * so the class on Phone side doesn't have this functionality.
     * Fundamental fix would be to have one shared implementation and resolve this corner case more
     * gracefully.
     */
    // TODO:
    // static AutoPtr<QueryHandler> sPreviousAdnQueryHandler;

    /*******
     * This code is used to handle SIM Contact queries
     *******/
    static const String ADN_PHONE_NUMBER_COLUMN_NAME; // = "number";
    static const String ADN_NAME_COLUMN_NAME; // = "name";
    static const Int32 ADN_QUERY_TOKEN; // = -1;
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_SPECIALCHARSEQUENCEMGR_H__
