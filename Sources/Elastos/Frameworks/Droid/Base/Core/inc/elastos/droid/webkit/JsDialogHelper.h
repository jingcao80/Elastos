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

#ifndef __ELASTOS_DROID_WEBKIT_JSDIALOGHELPER_H__
#define __ELASTOS_DROID_WEBKIT_JSDIALOGHELPER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Widget::IEditText;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Helper class to create JavaScript dialogs. It is used by
 * different WebView implementations.
 *
 * @hide Helper class for internal use
 */
class JsDialogHelper
    : public Object
    , public IJsDialogHelper
{
private:
    class CancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        CancelListener(
            /* [in] */ JsDialogHelper* owner);

        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        JsDialogHelper* mOwner;
    };

    class PositiveListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        PositiveListener(
            /* [in] */ JsDialogHelper* owner,
            /* [in] */ IEditText* edit);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        JsDialogHelper* mOwner;
        AutoPtr<IEditText> mEdit;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IJsPromptResult* result,
        /* [in] */ Int32 type,
        /* [in] */ const String& defaultValue,
        /* [in] */ const String& message,
        /* [in] */ const String& url);

    CARAPI constructor(
        /* [in] */ IJsPromptResult* result,
        /* [in] */ IMessage* msg);

    CARAPI InvokeCallback(
        /* [in] */ IWebChromeClient* client,
        /* [in] */ IWebView* webView,
        /* [out] */ Boolean* result);

    CARAPI ShowDialog(
        /* [in] */ IContext* context);

private:
    CARAPI_(String) GetJsDialogTitle(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) CanShowAlertDialog(
        /* [in] */ IContext* context);

private:
    static const String TAG;

    String mDefaultValue;
    AutoPtr<IJsPromptResult> mResult;
    String mMessage;
    Int32 mType;
    String mUrl;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_JSDIALOGHELPER_H__
