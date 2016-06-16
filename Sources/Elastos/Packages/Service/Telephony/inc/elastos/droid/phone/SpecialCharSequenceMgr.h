#ifndef  __ELASTOS_DROID_PHONE_SPECIALCHARSEQUENCEMGR_H__
#define  __ELASTOS_DROID_PHONE_SPECIALCHARSEQUENCEMGR_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Helper class to listen for some magic dialpad character sequences
 * that are handled specially by the Phone app.
 *
 * Note the Contacts app also handles these sequences too, so there's a
 * separate version of this class under apps/Contacts.
 *
 * In fact, the most common use case for these special sequences is typing
 * them from the regular "Dialer" used for outgoing calls, which is part
 * of the contacts app; see DialtactsActivity and DialpadFragment.
 * *This* version of SpecialCharSequenceMgr is used for only a few
 * relatively obscure places in the UI:
 * - The "SIM network unlock" PIN entry screen (see
 *   IccNetworkDepersonalizationPanel.java)
 * - The emergency dialer (see EmergencyDialer.java).
 *
 * TODO: there's lots of duplicated code between this class and the
 * corresponding class under apps/Contacts.  Let's figure out a way to
 * unify these two classes (in the framework? in a common shared library?)
 */
class SpecialCharSequenceMgr
{
public:
    TO_STRING_IMPL("SpecialCharSequenceMgr");

    /**
     * Check for special strings of digits from an input
     * string.
     * @param context input Context for the events we handle.
     * @param input the dial string to be examined.
     */
    static CARAPI_(Boolean) HandleChars(
        /* [in] */ IContext* context,
        /* [in] */ const String& input);

    /**
     * Generally used for the Personal Unblocking Key (PUK) unlocking
     * case, where we want to be able to maintain a handle to the
     * calling activity so that we can close it or otherwise display
     * indication if the PUK code is recognized.
     *
     * NOTE: The counterpart to this file in Contacts does
     * NOT contain the special PUK handling code, since it
     * does NOT need it.  When the device gets into PUK-
     * locked state, the keyguard comes up and the only way
     * to unlock the device is through the Emergency dialer,
     * which is still in the Phone App.
     *
     * @param context input Context for the events we handle.
     * @param input the dial string to be examined.
     * @param pukInputActivity activity that originated this
     * PUK call, tracked so that we can close it or otherwise
     * indicate that special character sequence is
     * successfully processed. Can be null.
     * @return true if the input was a special string which has been
     * handled.
     */
    static CARAPI_(Boolean) HandleChars(
        /* [in] */ IContext* context,
        /* [in] */ const String& input,
        /* [in] */ IActivity* pukInputActivity);

    /**
     * Variant of handleChars() that looks for the subset of "special
     * sequences" that are available even if the device is locked.
     *
     * (Specifically, these are the sequences that you're allowed to type
     * in the Emergency Dialer, which is accessible *without* unlocking
     * the device.)
     */
    static CARAPI_(Boolean) HandleCharsForLockedDevice(
        /* [in] */ IContext* context,
        /* [in] */ const String& input,
        /* [in] */ IActivity* pukInputActivity);

private:
    /*
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

    static CARAPI_(Boolean) HandleAdnEntry(
        /* [in] */ IContext* context,
        /* [in] */ const String& input);

    static CARAPI_(Boolean) HandlePinEntry(
        /* [in] */ IContext* context,
        /* [in] */ const String& input,
        /* [in] */ IActivity* pukInputActivity);

    static CARAPI_(Boolean) HandleIMEIDisplay(
        /* [in] */ IContext* context,
        /* [in] */ const String& input);

    static CARAPI_(void) ShowDeviceIdPanel(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) HandleRegulatoryInfoDisplay(
        /* [in] */ IContext* context,
        /* [in] */ const String& input);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    /** This class is never instantiated. */
    SpecialCharSequenceMgr() {}

private:
    static const String TAG;
    static const Boolean DBG;

    static const String MMI_IMEI_DISPLAY;
    static const String MMI_REGULATORY_INFO_DISPLAY;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_SPECIALCHARSEQUENCEMGR_H__