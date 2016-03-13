
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_DATETIMESUGGESTION_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_DATETIMESUGGESTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Date/time suggestion container used to store information for each suggestion that will be shown
 * in the suggestion list dialog. Keep in sync with date_time_suggestion.h.
 */
class DateTimeSuggestion
    : public Object
{
public:
    /**
     * Constructs a color suggestion container.
     * @param value The suggested date/time value.
     * @param localizedValue The suggested value localized.
     * @param label The label for the suggestion.
     */
    DateTimeSuggestion(
        /* [in] */ Double value,
        /* [in] */ const String& localizedValue,
        /* [in] */ const String& label);

    CARAPI_(Double) Value();

    CARAPI_(String) LocalizedValue();

    CARAPI_(String) Label();

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI HashCode(
        /* [out] */ Int32* code);

private:
    /*const*/ Double mValue;
    const String mLocalizedValue;
    const String mLabel;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_DATETIMESUGGESTION_H__
