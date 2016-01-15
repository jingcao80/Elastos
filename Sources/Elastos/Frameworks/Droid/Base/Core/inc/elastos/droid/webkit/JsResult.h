
#ifndef __ELASTOS_DROID_WEBKIT_JSRESULT_H__
#define __ELASTOS_DROID_WEBKIT_JSRESULT_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * An instance of this class is passed as a parameter in various {@link WebChromeClient} action
 * notifications. The object is used as a handle onto the underlying JavaScript-originated request,
 * and provides a means for the client to indicate whether this action should proceed.
 */
class JsResult
    : public Object
    , public IJsResult
{
public:
    CAR_INTERFACE_DECL();

    JsResult();

    /**
     * @hide Only for use by WebViewProvider implementations
     */
    JsResult(
        /* [in] */ IJsResultReceiver* receiver);

    CARAPI constructor(
        /* [in] */ IJsResultReceiver* receiver);

    /**
     * Handle the result if the user cancelled the dialog.
     */
    CARAPI Cancel();

    /**
     * Handle a confirmation response from the user.
     */
    CARAPI Confirm();

    /**
     * @hide Only for use by WebViewProvider implementations
     */
    CARAPI GetResult(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* info);

protected:
    /* Notify the caller that the JsResult has completed */
    virtual CARAPI_(void) WakeUp();

protected:
    // This is the caller of the prompt and is the object that is waiting.
    AutoPtr<IJsResultReceiver> mReceiver;

private:
    // This is a basic result of a confirm or prompt dialog.
    Boolean mResult;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_JSRESULT_H__
