
#include "elastos/droid/webkit/JsPromptResult.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(JsPromptResult, Object, IJsPromptResult);

JsPromptResult::JsPromptResult()
{
}

JsPromptResult::JsPromptResult(
    /* [in] */ IJsResultReceiver* receiver)
    : JsResult(receiver)
{
}

ECode JsPromptResult::constructor(
    /* [in] */ IJsResultReceiver* receiver)
{
    return JsResult::constructor(receiver);
}


/**
 * Handle a confirmation response from the user.
 */
ECode JsPromptResult::Confirm(
    /* [in] */ const String& result)
{
    mStringResult = result;
    return JsResult::Confirm();
}

/**
 * @hide Only for use by WebViewProvider implementations
 */
ECode JsPromptResult::GetStringResult(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStringResult;
    return NOERROR;
}

ECode JsPromptResult::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "JsPromptResult";
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos