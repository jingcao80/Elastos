
#ifndef __ELASTOS_DROID_CONTENT_RESTRICTIONENTRY_H__
#define __ELASTOS_DROID_CONTENT_RESTRICTIONENTRY_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * Applications can expose restrictions for a restricted user on a
 * multiuser device. The administrator can configure these restrictions that will then be
 * applied to the restricted user. Each RestrictionsEntry is one configurable restriction.
 * <p/>
 * Any application that chooses to expose such restrictions does so by implementing a
 * receiver that handles the {@link Intent#ACTION_GET_RESTRICTION_ENTRIES} action.
 * The receiver then returns a result bundle that contains an entry called "restrictions", whose
 * value is an ArrayList<RestrictionsEntry>.
 */
class RestrictionEntry
    : public Object
    , public IRestrictionEntry
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    RestrictionEntry();

    virtual ~RestrictionEntry();

    CARAPI constructor();

    /**
     * Constructor for specifying the type and key, with no initial value;
     *
     * @param type the restriction type.
     * @param key the unique key for this restriction
     */
    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ const String& key);

    /**
     * Constructor for {@link #TYPE_CHOICE} type.
     * @param key the unique key for this restriction
     * @param selectedString the current value
     */
    CARAPI constructor(
        /* [in] */ const String& key,
        /* [in] */ const String& selectedString);

    /**
     * Constructor for {@link #TYPE_BOOLEAN} type.
     * @param key the unique key for this restriction
     * @param selectedState whether this restriction is selected or not
     */
    CARAPI constructor(
        /* [in] */ const String& key,
        /* [in] */ Boolean selectedState);

    /**
     * Constructor for {@link #TYPE_MULTI_SELECT} type.
     * @param key the unique key for this restriction
     * @param selectedStrings the list of values that are currently selected
     */
    CARAPI constructor(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<String>* selectedStrings);

    /**
     * Constructor for {@link #TYPE_INTEGER} type.
     * @param key the unique key for this restriction
     * @param selectedInt the integer value of the restriction
     */
    CARAPI constructor(
        /* [in] */ const String& key,
        /* [in] */ Int32 selectedInt);

    /**
     * Sets the type for this restriction.
     * @param type the type for this restriction.
     */
    CARAPI SetType(
        /* [in] */ Int32 type);

    /**
     * Returns the type for this restriction.
     * @return the type for this restriction
     */
    CARAPI GetType(
        /* [out] */ Int32* type);

    /**
     * Returns the currently selected string value.
     * @return the currently selected value, which can be null for types that aren't for holding
     * single string values.
     */
    CARAPI GetSelectedString(
        /* [out] */ String* str);

    /**
     * Returns the list of currently selected values.
     * @return the list of current selections, if type is {@link #TYPE_MULTI_SELECT},
     *  null otherwise.
     */
    CARAPI GetAllSelectedStrings(
        /* [out, callee] */ ArrayOf<String>** strs);

    /**
     * Returns the current selected state for an entry of type {@link #TYPE_BOOLEAN}.
     * @return the current selected state of the entry.
     */
    CARAPI GetSelectedState(
        /* [out] */ Boolean* state);

    /**
     * Returns the value of the entry as an integer when the type is {@link #TYPE_INTEGER}.
     * @return the integer value of the entry.
     */
    CARAPI GetInt32Value(
        /* [out] */ Int32* value);

    /**
     * Sets the integer value of the entry when the type is {@link #TYPE_INTEGER}.
     * @param value the integer value to set.
     */
    CARAPI SetInt32Value(
        /* [in] */ Int32 value);

    /**
     * Sets the string value to use as the selected value for this restriction. This value will
     * be persisted by the system for later use by the application.
     * @param selectedString the string value to select.
     */
    CARAPI SetSelectedString(
        /* [in] */ const String& selectedString);

    /**
     * Sets the current selected state for an entry of type {@link #TYPE_BOOLEAN}. This value will
     * be persisted by the system for later use by the application.
     * @param state the current selected state
     */
    CARAPI SetSelectedState(
        /* [in] */ Boolean state);

    /**
     * Sets the current list of selected values for an entry of type {@link #TYPE_MULTI_SELECT}.
     * These values will be persisted by the system for later use by the application.
     * @param allSelectedStrings the current list of selected values.
     */
    CARAPI SetAllSelectedStrings(
        /* [in] */ ArrayOf<String>* allSelectedStrings);

    /**
     * Sets a list of string values that can be selected by the user. If no user-visible entries
     * are set by a call to {@link #setChoiceEntries(String[])}, these values will be the ones
     * shown to the user. Values will be chosen from this list as the user's selection and the
     * selected values can be retrieved by a call to {@link #getAllSelectedStrings()}, or
     * {@link #getSelectedString()}, depending on whether it is a multi-select type or choice type.
     * This method is not relevant for types other than
     * {@link #TYPE_CHOICE}, and {@link #TYPE_MULTI_SELECT}.
     * @param choiceValues an array of Strings which will be the selected values for the user's
     * selections.
     * @see #getChoiceValues()
     * @see #getAllSelectedStrings()
     */
    CARAPI SetChoiceValues(
        /* [in] */ ArrayOf<String>* choiceValues);

    /**
     * Sets a list of string values that can be selected by the user, similar to
     * {@link #setChoiceValues(String[])}.
     * @param context the application context for retrieving the resources.
     * @param stringArrayResId the resource id for a string array containing the possible values.
     * @see #setChoiceValues(String[])
     */
    CARAPI SetChoiceValues(
        /* [in] */ IContext* context,
        /* [in] */ Int32 stringArrayResId);

    /**
     * Returns the list of possible string values set earlier.
     * @return the list of possible values.
     */
    CARAPI GetChoiceValues(
        /* [out, callee] */ ArrayOf<String>** choiceValues);

    /**
     * Sets a list of strings that will be presented as choices to the user. When the
     * user selects one or more of these choices, the corresponding value from the possible values
     * are stored as the selected strings. The size of this array must match the size of the array
     * set in {@link #setChoiceValues(String[])}. This method is not relevant for types other
     * than {@link #TYPE_CHOICE}, and {@link #TYPE_MULTI_SELECT}.
     * @param choiceEntries the list of user-visible choices.
     * @see #setChoiceValues(String[])
     */
    CARAPI SetChoiceEntries(
        /* [in] */ ArrayOf<String>* choiceEntries);

    /** Sets a list of strings that will be presented as choices to the user. This is similar to
     * {@link #setChoiceEntries(String[])}.
     * @param context the application context, used for retrieving the resources.
     * @param stringArrayResId the resource id of a string array containing the possible entries.
     */
    CARAPI SetChoiceEntries(
        /* [in] */ IContext* context,
        /* [in] */ Int32 stringArrayResId);

    /**
     * Returns the list of strings, set earlier, that will be presented as choices to the user.
     * @return the list of choices presented to the user.
     */
    CARAPI GetChoiceEntries(
        /* [out, callee] */ ArrayOf<String>** choiceValues);

    /**
     * Returns the provided user-visible description of the entry, if any.
     * @return the user-visible description, null if none was set earlier.
     */
    CARAPI GetDescription(
        /* [out] */ String* description);

    /**
     * Sets the user-visible description of the entry, as a possible sub-text for the title.
     * You can use this to describe the entry in more detail or to display the current state of
     * the restriction.
     * @param description the user-visible description string.
     */
    CARAPI SetDescription(
        /* [in] */ const String& description);

    /**
     * This is the unique key for the restriction entry.
     * @return the key for the restriction.
     */
    CARAPI GetKey(
        /* [out] */ String* key);

    /**
     * Returns the user-visible title for the entry, if any.
     * @return the user-visible title for the entry, null if none was set earlier.
     */
    CARAPI GetTitle(
        /* [out] */ String* title);

    /**
     * Sets the user-visible title for the entry.
     * @param title the user-visible title for the entry.
     */
    CARAPI SetTitle(
        /* [in] */ const String& title);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

private:
    Boolean EqualArrays(
        /* [in] */ ArrayOf<String>* one,
        /* [in] */ ArrayOf<String>* other);

    AutoPtr<ArrayOf<String> > ReadArray(
        /* [in] */ IParcel* in);

    CARAPI WriteArray(
        /* [in] */ IParcel* dest,
        /* [in] */ ArrayOf<String>* values);

private:
    /** The type of restriction. */
    Int32 mType;

    /** The unique key that identifies the restriction. */
    String mKey;

    /** The user-visible title of the restriction. */
    String mTitle;

    /** The user-visible secondary description of the restriction. */
    String mDescription;

    /** The user-visible set of choices used for single-select and multi-select lists. */
    AutoPtr<ArrayOf<String> > mChoiceEntries;

    /** The values corresponding to the user-visible choices. The value(s) of this entry will
     * one or more of these, returned by {@link #getAllSelectedStrings()} and
     * {@link #getSelectedString()}.
     */
    AutoPtr<ArrayOf<String> > mChoiceValues;

    /* The chosen value, whose content depends on the type of the restriction. */
    String mCurrentValue;

    /* List of selected choices in the multi-select case. */
    AutoPtr<ArrayOf<String> > mCurrentValues;
};

} // Content
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTENT_RESTRICTIONENTRY_H__
