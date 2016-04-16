
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateTimeSuggestion.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // because this class is not CAR class, only use IObject to check
    // if (object.Probe(EIID_DateTimeSuggestion) == NULL) {
    //     *result = FALSE;
    //     return NOERROR;
    // }
    if (IObject::Probe(object) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<DateTimeSuggestion> other = (DateTimeSuggestion*)(IObject::Probe(object));
     *result = mValue == other->mValue &&
            mLocalizedValue == other->mLocalizedValue &&
            mLabel == other->mLabel;

    return NOERROR;
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
