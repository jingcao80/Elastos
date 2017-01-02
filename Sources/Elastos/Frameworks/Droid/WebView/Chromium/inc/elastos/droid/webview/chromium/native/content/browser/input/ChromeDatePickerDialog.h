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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_CHROMEDATEPICKERDIALOG_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_CHROMEDATEPICKERDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/app/DatePickerDialog.h"

using Elastos::Droid::App::IDatePickerDialog;
using Elastos::Droid::App::DatePickerDialog;
using Elastos::Droid::App::IDatePickerDialogOnDateSetListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Widget::IDatePicker;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

/**
 * The behavior of the DatePickerDialog changed after JellyBean so it now calls
 * OndateSetListener.onDateSet() even when the dialog is dismissed (e.g. back button, tap
 * outside). This class will call the listener instead of the DatePickerDialog only when the
 * BUTTON_POSITIVE has been clicked.
 */
class ChromeDatePickerDialog
    : public DatePickerDialog
{
public:
    ChromeDatePickerDialog(
        /* [in] */ IContext* context,
        /* [in] */ IDatePickerDialogOnDateSetListener* callBack,
        /* [in] */ Int32 year,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ Int32 dayOfMonth);

    /**
     * The superclass DatePickerDialog has null for OnDateSetListener so we need to call the
     * listener manually.
     */
    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

private:
    const AutoPtr<IDatePickerDialogOnDateSetListener> mCallBack;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_CHROMEDATEPICKERDIALOG_H__
