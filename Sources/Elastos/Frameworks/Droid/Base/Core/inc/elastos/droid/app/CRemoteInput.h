#ifndef __ELASTOS_DROID_APP_CREMOTEINPUT_H__
#define __ELASTOS_DROID_APP_CREMOTEINPUT_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CRemoteInput.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IClipDescription;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * A {@code RemoteInput} object specifies input to be collected from a user to be passed along with
 * an intent inside a {@link android.app.PendingIntent} that is sent.
 * Always use {@link RemoteInput.Builder} to create instances of this class.
 * <p class="note"> See
 * <a href="{@docRoot}wear/notifications/remote-input.html">Receiving Voice Input from
 * a Notification</a> for more information on how to use this class.
 *
 * <p>The following example adds a {@code RemoteInput} to a {@link Notification.Action},
 * sets the result key as {@code quick_reply}, and sets the label as {@code Quick reply}.
 * Users are prompted to input a response when they trigger the action. The results are sent along
 * with the intent and can be retrieved with the result key (provided to the {@link Builder}
 * constructor) from the Bundle returned by {@link #getResultsFromIntent}.
 *
 * <pre class="prettyprint">
 * public static final String KEY_QUICK_REPLY_TEXT = "quick_reply";
 * Notification.Action action = new Notification.Action.Builder(
 *         R.drawable.reply, &quot;Reply&quot;, actionIntent)
 *         <b>.addRemoteInput(new RemoteInput.Builder(KEY_QUICK_REPLY_TEXT)
 *                 .setLabel("Quick reply").build()</b>)
 *         .build();</pre>
 *
 * <p>When the {@link android.app.PendingIntent} is fired, the intent inside will contain the
 * input results if collected. To access these results, use the {@link #getResultsFromIntent}
 * function. The result values will present under the result key passed to the {@link Builder}
 * constructor.
 *
 * <pre class="prettyprint">
 * public static final String KEY_QUICK_REPLY_TEXT = "quick_reply";
 * Bundle results = RemoteInput.getResultsFromIntent(intent);
 * if (results != NULL) {
 *     CharSequence quickReplyResult = results.getCharSequence(KEY_QUICK_REPLY_TEXT);
 * }</pre>
 */
CarClass(CRemoteInput)
    , public Object
    , public IRemoteInput
    , public IParcelable
{
public:
    /**
     * Builder class for {@link RemoteInput} objects.
     */
    class Builder
        : public Object
        , public IRemoteInputBuilder
    {
    public:
        CAR_INTERFACE_DECL()

        Builder();

        virtual ~Builder();

        /**
         * Create a builder object for {@link RemoteInput} objects.
         * @param resultKey the Bundle key that refers to this input when collected from the user
         */
        CARAPI constructor(
            /* [in] */ const String& resultKey);

        /**
         * Set a label to be displayed to the user when collecting this input.
         * @param label The label to show to users when they input a response.
         * @return this object for method chaining
         */
        CARAPI SetLabel(
            /* [in] */ ICharSequence* label);

        /**
         * Specifies choices available to the user to satisfy this input.
         * @param choices an array of pre-defined choices for users input.
         *        You must provide a non-NULL and non-empty array if
         *        you disabled free form input using {@link #setAllowFreeFormInput}.
         * @return this object for method chaining
         */
        CARAPI SetChoices(
            /* [in] */ ArrayOf<ICharSequence*>* choices);

        /**
         * Specifies whether the user can provide arbitrary values.
         *
         * @param allowFreeFormInput The default is {@code true}.
         *         If you specify {@code false}, you must provide a non-NULL
         *         and non-empty array to {@link #setChoices} or an
         *         {@link IllegalArgumentException} is thrown.
         * @return this object for method chaining
         */
        CARAPI SetAllowFreeFormInput(
            /* [in] */ Boolean allowFreeFormInput);

        /**
         * Merge additional metadata into this builder.
         *
         * <p>Values within the Bundle will replace existing extras values in this Builder.
         *
         * @see RemoteInput#getExtras
         */
        CARAPI AddExtras(
            /* [in] */ IBundle* extras);

        /**
         * Get the metadata Bundle used by this Builder.
         *
         * <p>The returned Bundle is shared with this Builder.
         */
        CARAPI GetExtras(
            /* [out] */ IBundle** result);

        /**
         * Combine all of the options that have been set and return a new {@link RemoteInput}
         * object.
         */
        CARAPI Build(
            /* [out] */ IRemoteInput** ri);

    private:
        CARAPI SetFlag(
            /* [in] */ Int32 mask,
            /* [in] */ Boolean value);
    private:
        String mResultKey;
        AutoPtr<ICharSequence> mLabel;
        AutoPtr<ArrayOf<ICharSequence*> > mChoices;
        Int32 mFlags;// = DEFAULT_FLAGS;
        AutoPtr<IBundle> mExtras;// = new Bundle();
    };

public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CRemoteInput();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& resultKey,
        /* [in] */ ICharSequence* label,
        /* [in] */ ArrayOf<ICharSequence*>* choices,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle* extras);

    /**
     * Get the key that the result of this input will be set in from the Bundle returned by
     * {@link #getResultsFromIntent} when the {@link android.app.PendingIntent} is sent.
     */
    CARAPI GetResultKey(
        /* [out] */ String* str);

    /**
     * Get the label to display to users when collecting this input.
     */
    CARAPI GetLabel(
        /* [out] */ ICharSequence** seq);

    /**
     * Get possible input choices. This can be {@code NULL} if there are no choices to present.
     */
    CARAPI GetChoices(
        /* [out, callee] */ ArrayOf<ICharSequence*>** choices);

    /**
     * Get whether or not users can provide an arbitrary value for
     * input. If you set this to {@code false}, users must select one of the
     * choices in {@link #getChoices}. An {@link IllegalArgumentException} is thrown
     * if you set this to false and {@link #getChoices} returns {@code NULL} or empty.
     */
    CARAPI GetAllowFreeFormInput(
        /* [out] */ Boolean* result);

    /**
     * Get additional metadata carried around with this remote input.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    /**
     * Get the remote input results bundle from an intent. The returned Bundle will
     * contain a key/value for every result key populated by remote input collector.
     * Use the {@link Bundle#getCharSequence(String)} method to retrieve a value.
     * @param intent The intent object that fired in response to an action or content intent
     *               which also had one or more remote input requested.
     */
    static AutoPtr<IBundle> GetResultsFromIntent(
        /* [in] */ IIntent* intent);

    /**
     * Populate an intent object with the results gathered from remote input. This method
     * should only be called by remote input collection services when sending results to a
     * pending intent.
     * @param remoteInputs The remote inputs for which results are being provided
     * @param intent The intent to add remote inputs to. The {@link ClipData}
     *               field of the intent will be modified to contain the results.
     * @param results A bundle holding the remote input results. This bundle should
     *                be populated with keys matching the result keys specified in
     *                {@code remoteInputs} with values being the result per key.
     */
    static CARAPI AddResultsToIntent(
        /* [in] */ ArrayOf<IRemoteInput*>* remoteInputs,
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* results);

private:
    // Flags bitwise-ored to mFlags
    static const Int32 FLAG_ALLOW_FREE_FORM_INPUT;// = 0x1;

    // Default value for flags integer
    static const Int32 DEFAULT_FLAGS;// = FLAG_ALLOW_FREE_FORM_INPUT;

    String mResultKey;
    AutoPtr<ICharSequence> mLabel;
    AutoPtr<ArrayOf<ICharSequence*> > mChoices;
    Int32 mFlags;
    AutoPtr<IBundle> mExtras;
};



} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CREMOTEINPUT_H__
