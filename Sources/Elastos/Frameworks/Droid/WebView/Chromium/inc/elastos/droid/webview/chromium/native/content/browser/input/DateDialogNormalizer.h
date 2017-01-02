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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_DATEDIALOGNORMALIZER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_DATEDIALOGNORMALIZER_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::IDatePickerOnDateChangedListener;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ITimeZone;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Normalize a date dialog so that it respect min and max.
 */
class DateDialogNormalizer
{
public:
    /**
     * Normalizes an existing DateDialogPicker changing the default date if
     * needed to comply with the {@code min} and {@code max} attributes.
     */
    static CARAPI_(void) Normalize(
        /* [in] */ IDatePicker* picker,
        /* [in] */ IDatePickerOnDateChangedListener* listener,
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int64 minMillis,
        /* [in] */ Int64 maxMillis);

private:
    static CARAPI_(void) SetLimits(
        /* [in] */ IDatePicker* picker,
        /* [in] */ Int64 minMillis,
        /* [in] */ Int64 maxMillis);

    static CARAPI_(AutoPtr<ICalendar>) TrimToDate(
        /* [in] */ Int64 time);
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_DATEDIALOGNORMALIZER_H__
