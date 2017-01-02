//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    CARAPI GetHashCode(
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
