
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
        CAR_INTERFACE_DECL();

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
        CAR_INTERFACE_DECL();

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
    CAR_INTERFACE_DECL();

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
