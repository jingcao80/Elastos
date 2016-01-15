
#include "elastos/droid/webkit/native/content/browser/input/DateTimeSuggestion.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Constructs a color suggestion container.
 * @param value The suggested date/time value.
 * @param localizedValue The suggested value localized.
 * @param label The label for the suggestion.
 */
DateTimeSuggestion::DateTimeSuggestion(
    /* [in] */ Double value,
    /* [in] */ const String& localizedValue,
    /* [in] */ const String& label)
    : mValue(value)
    , mLocalizedValue(localizedValue)
    , mLabel(label)
{
}

Double DateTimeSuggestion::Value()
{
    return mValue;
}

String DateTimeSuggestion::LocalizedValue()
{
    return mLocalizedValue;
}

String DateTimeSuggestion::Label()
{
    return mLabel;
}

//@Override
ECode DateTimeSuggestion::Equals(
    /* [in] */ Object& object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    assert(0);
    // TODO
    // if (object.Probe(EIID_DateTimeSuggestion) == NULL) {
    //     *result = FALSE;
    //     return NOERROR;
    // }

    assert(0);
    // TODO
    // const AutoPtr<DateTimeSuggestion> other = (DateTimeSuggestion*)object.Probe(EIID_DateTimeSuggestion);
    // *result = mValue == other->mValue &&
    //        mLocalizedValue == other->mLocalizedValue &&
    //        mLabel == other->mLabel;

    return E_NOT_IMPLEMENTED;
}

//@Override
ECode DateTimeSuggestion::HashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 hash = 31;
    hash = 37 * hash + (Int32) mValue;
    Int32 hash1 = mLocalizedValue.GetHashCode();
    hash = 37 * hash + hash1;
    Int32 hash2 = mLabel.GetHashCode();
    hash = 37 * hash + hash2;
    *hashCode = hash;

    return NOERROR;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
