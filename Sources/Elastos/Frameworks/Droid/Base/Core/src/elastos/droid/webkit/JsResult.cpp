
#include "elastos/droid/webkit/JsResult.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(JsResult, Object, IJsResult);

JsResult::JsResult()
    : mResult(FALSE)
{
}

JsResult::JsResult(
    /* [in] */ IJsResultReceiver* receiver)
    : mReceiver(receiver)
    , mResult(FALSE)
{
}

ECode JsResult::constructor(
    /* [in] */ IJsResultReceiver* receiver)
{
    mReceiver = receiver;
    mResult = FALSE;
    return NOERROR;
}

/**
 * Handle the result if the user cancelled the dialog.
 */
ECode JsResult::Cancel()
{
    mResult = FALSE;
    WakeUp();
    return NOERROR;
}

/**
 * Handle a confirmation response from the user.
 */
ECode JsResult::Confirm()
{
    mResult = TRUE;
    WakeUp();
    return NOERROR;
}

/**
 * @hide Only for use by WebViewProvider implementations
 */
ECode JsResult::GetResult(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResult;
    return NOERROR;
}

/* Notify the caller that the JsResult has completed */
void JsResult::WakeUp()
{
    assert(0);
//    mReceiver->OnJsResultComplete((IJsResult*)this->Probe(EIID_IJsResult));
}

ECode JsResult::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "JsResult";
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
