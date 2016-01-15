
#ifndef __ELASTOS_DROID_PREFERENCE_MULTICHECKPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_MULTICHECKPREFERENCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/DialogPreference.h"

using Elastos::Droid::Content::IDialogInterfaceOnMultiChoiceClickListener;

namespace Elastos {
namespace Droid {
namespace Preference {

/**
 * @hide
* A {@link Preference} that displays a list of entries as
* a dialog which allow the user to toggle each individually on and off.
 *
* @attr ref android.R.styleable#ListPreference_entries
* @attr ref android.R.styleable#ListPreference_entryValues
 */
class MultiCheckPreference
    : public DialogPreference
    , public IMultiCheckPreference
{
private:
    class MultiChoiceClickListener
        : public Object
        , public IDialogInterfaceOnMultiChoiceClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MultiChoiceClickListener(
            /* [in] */ MultiCheckPreference* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which,
            /* [in] */ Boolean isChecked);

    private:
        MultiCheckPreference* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    MultiCheckPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Sets the human-readable entries to be shown in the list. This will be
     * shown in subsequent dialogs.
     * <p>
     * Each entry must have a corresponding index in
     * {@link #setEntryValues(CharSequence[])}.
     *
     * @param entries The entries.
     * @see #setEntryValues(CharSequence[])
     */
    CARAPI SetEntries(
        /* [in] */ ArrayOf<ICharSequence*>* entries);

    /**
     * @see #setEntries(CharSequence[])
     * @param entriesResId The entries array as a resource.
     */
    CARAPI SetEntries(
        /* [in] */ Int32 entriesResId);

    /**
     * The list of entries to be shown in the list in subsequent dialogs.
     *
     * @return The list as an array.
     */
    CARAPI GetEntries(
        /* [out,callee] */ ArrayOf<ICharSequence*>** result);

    /**
     * The array to find the value to save for a preference when an entry from
     * entries is selected. If a user clicks on the second item in entries, the
     * second item in this array will be saved to the preference.
     *
     * @param entryValues The array to be used as values to save for the preference.
     */
    CARAPI SetEntryValues(
        /* [in] */ ArrayOf<String>* entryValues);

    /**
     * @see #setEntryValues(CharSequence[])
     * @param entryValuesResId The entry values array as a resource.
     */
    CARAPI SetEntryValues(
        /* [in] */ Int32 entryValuesResId);

    /**
     * Returns the array of values to be saved for the preference.
     *
     * @return The array of values.
     */
    CARAPI GetEntryValues(
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * Get the boolean state of a given value.
     */
    CARAPI GetValue(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* value);

    /**
     * Set the boolean state of a given value.
     */
    CARAPI SetValue(
        /* [in] */ Int32 index,
        /* [in] */ Boolean state);

    /**
     * Sets the current values.
     */
    CARAPI SetValues(
        /* [in] */ ArrayOf<Boolean>* values);

    /**
     * Returns the summary of this ListPreference. If the summary
     * has a {@linkplain java.lang.String#format String formatting}
     * marker in it (i.e. "%s" or "%1$s"), then the current entry
     * value will be substituted in its place.
     *
     * @return the summary with appropriate string substitution
     */
    CARAPI GetSummary(
        /* [out] */ ICharSequence** summary);

    /**
     * Sets the summary for this Preference with a CharSequence.
     * If the summary has a
     * {@linkplain java.lang.String#format String formatting}
     * marker in it (i.e. "%s" or "%1$s"), then the current entry
     * value will be substituted in its place when it's retrieved.
     *
     * @param summary The summary for the preference.
     */
    CARAPI SetSummary(
        /* [in] */ ICharSequence* summary);

    /**
     * Returns the currently selected values.
     */
   CARAPI GetValues(
        /* [out,callee] */ ArrayOf<Boolean>** result);

    /**
     * Returns the index of the given value (in the entry values array).
     *
     * @param value The value whose index should be returned.
     * @return The index of the value, or -1 if not found.
     */
    CARAPI FindIndexOfValue(
        /* [in] */ const String& value,
        /* [out] */ Int32* result);

protected:
    CARAPI OnPrepareDialogBuilder(
        /* [in] */ IAlertDialogBuilder* builder);

    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

    CARAPI OnGetDefaultValue(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 index,
        /* [out] */ IInterface** value);

    CARAPI OnSetInitialValue(
        /* [in] */ Boolean restorePersistedValue,
        /* [in] */ IInterface* defaultValue);

    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** state);

    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    CARAPI_(void) SetEntryValuesCS(
        /* [in] */ ArrayOf<ICharSequence*>* values);

private:
    AutoPtr< ArrayOf<ICharSequence*> > mEntries;
    AutoPtr< ArrayOf<String> > mEntryValues;
    AutoPtr< ArrayOf<Boolean> > mSetValues;
    AutoPtr< ArrayOf<Boolean> > mOrigValues;
    String mSummary;
};

}
}
}
#endif // __ELASTOS_DROID_PREFERENCE_MULTICHECKPREFERENCE_H__
