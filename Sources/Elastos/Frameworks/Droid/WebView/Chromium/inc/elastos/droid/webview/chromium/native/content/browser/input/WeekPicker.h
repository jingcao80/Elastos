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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_WEEKPICKER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_WEEKPICKER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/TwoFieldDatePicker.h"

using Elastos::Droid::Content::IContext;

// import org.chromium.content.R;

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

// This class is heavily based on android.widget.DatePicker.
class WeekPicker : public TwoFieldDatePicker
{
public:
    WeekPicker(
        /* [in] */ IContext* context,
        /* [in] */ Double minValue,
        /* [in] */ Double maxValue);

    /**
     * Creates a date object from the |year| and |week|.
     */
    static CARAPI_(AutoPtr<ICalendar>) CreateDateFromWeek(
        /* [in] */ Int32 year,
        /* [in] */ Int32 week);

    /**
     * Creates a date object from the |value| which is milliseconds since epoch.
     */
    static CARAPI_(AutoPtr<ICalendar>) CreateDateFromValue(
        /* [in] */ Double value);

    static CARAPI_(Int32) GetISOWeekYearForDate(
        /* [in] */ ICalendar* date);

    static CARAPI_(Int32) GetWeekForDate(
        /* [in] */ ICalendar* date);

    /**
     * @return The selected year.
     */
    //@Override
    CARAPI_(Int32) GetYear();

    /**
     * @return The selected week.
     */
    CARAPI_(Int32) GetWeek();

    //@Override
    CARAPI_(Int32) GetPositionInYear();

protected:
    //@Override
    CARAPI_(AutoPtr<ICalendar>) GetDateForValue(
        /* [in] */ Double value);

    //@Override
    CARAPI SetCurrentDate(
        /* [in] */ Int32 year,
        /* [in] */ Int32 week);

    //@Override
    CARAPI_(Int32) GetMaxYear();

    //@Override
    CARAPI_(Int32) GetMinYear();

    //@Override
    CARAPI_(Int32) GetMaxPositionInYear(
        /* [in] */ Int32 year);

    //@Override
    CARAPI_(Int32) GetMinPositionInYear(
        /* [in] */ Int32 year);

private:
    CARAPI_(Int32) GetNumberOfWeeks(
        /* [in] */ Int32 year);
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_WEEKPICKER_H__
