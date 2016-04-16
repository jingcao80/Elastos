
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/webkit/webview/chromium/native/content/browser/accessibility/JellyBeanAccessibilityInjector.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/core/AutoLock.h"
#include "elastos/core/StringUtils.h"
#include "elastos/core/CoreUtils.h"

using Elastos::Droid::Os::SystemClock;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Org::Json::IJSONObject;
using Org::Json::CJSONObject;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Accessibility {

//==================================================================
//        JellyBeanAccessibilityInjector::CallbackHandler
//==================================================================

static String JAVASCRIPT_ACTION_TEMPLATE_Init()
{
    String str("(function() {");
    str += "  retVal = false;";
    str += "  try {";
    str += "    retVal = %s;";
    str += "  } catch (e) {";
    str += "    retVal = false;";
    str += "  }";
    str += "  %s.onResult(%d, retVal);";
    str += "})()";

    return str;
}

const String JellyBeanAccessibilityInjector::CallbackHandler::JAVASCRIPT_ACTION_TEMPLATE = JAVASCRIPT_ACTION_TEMPLATE_Init();

// Time in milliseconds to wait for a result before failing.
const Int64 JellyBeanAccessibilityInjector::CallbackHandler::RESULT_TIMEOUT;

static AutoPtr<IAtomicInteger32> mResultIdCounter_init()
{
    AutoPtr<IAtomicInteger32> ai;
    CAtomicInteger32::New((IAtomicInteger32**)&ai);
    return ai;
}

JellyBeanAccessibilityInjector::CallbackHandler::CallbackHandler(
    /* [in] */ const String& interfaceName)
    : mResultIdCounter(mResultIdCounter_init())
    , mInterfaceName(interfaceName)
    , mResult(FALSE)
    , mResultId(-1)
{
}

/**
 * Performs an action and attempts to wait for a result.
 *
 * @param contentView The ContentViewCore to perform the action on.
 * @param code Javascript code that evaluates to a result.
 * @return The result of the action.
 */
Boolean JellyBeanAccessibilityInjector::CallbackHandler::PerformAction(
    /* [in] */ ContentViewCore* contentView,
    /* [in] */ const String& code)
{
    Int32 resultId;
    mResultIdCounter->GetAndIncrement(&resultId);
    AutoPtr<ILocaleHelper> localHelper;
    AutoPtr<ILocale> us;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localHelper);
    localHelper->GetUS((ILocale**)&us);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);

    AutoPtr<ICharSequence> icode = CoreUtils::Convert(code);
    AutoPtr<ICharSequence> iinterfaceName = CoreUtils::Convert(mInterfaceName);
    AutoPtr<IInteger32> iResultId = CoreUtils::Convert(resultId);
    args->Set(0, TO_IINTERFACE(icode));
    args->Set(1, TO_IINTERFACE(iinterfaceName));
    args->Set(2, TO_IINTERFACE(iResultId));
    String js = StringUtils::Format(us, JAVASCRIPT_ACTION_TEMPLATE, args);
    contentView->EvaluateJavaScript(js, NULL);

    return GetResultAndClear(resultId);
}

/**
 * Gets the result of a request to perform an accessibility action.
 *
 * @param resultId The result id to match the result with the request.
 * @return The result of the request.
 */
Boolean JellyBeanAccessibilityInjector::CallbackHandler::GetResultAndClear(
    /* [in] */ Int32 resultId)
{
    AutoLock lock(mResultLock);
    const Boolean success = WaitForResultTimedLocked(resultId);
    const Boolean result = success ? mResult : FALSE;
    ClearResultLocked();

    return result;
}

/**
 * Clears the result state.
 */
void JellyBeanAccessibilityInjector::CallbackHandler::ClearResultLocked()
{
    mResultId = -1;
    mResult = FALSE;
}

/**
 * Waits up to a given bound for a result of a request and returns it.
 *
 * @param resultId The result id to match the result with the request.
 * @return Whether the result was received.
 */
Boolean JellyBeanAccessibilityInjector::CallbackHandler::WaitForResultTimedLocked(
    /* [in] */ Int32 resultId)
{
    Int64 waitTimeMillis = RESULT_TIMEOUT;
    const Int64 startTimeMillis = SystemClock::GetUptimeMillis();
    while (TRUE) {
        // try {
            if (mResultId == resultId) return TRUE;
            if (mResultId > resultId) return FALSE;
            const Int64 elapsedTimeMillis = SystemClock::GetUptimeMillis() - startTimeMillis;
            waitTimeMillis = RESULT_TIMEOUT - elapsedTimeMillis;
            if (waitTimeMillis <= 0) return FALSE;
            mResultLock.Wait(waitTimeMillis);
        // } catch (InterruptedException ie) {
        //     /* ignore */
        // }
    }
}

/**
 * Callback exposed to JavaScript.  Handles returning the result of a
 * request to a waiting (or potentially timed out) thread.
 *
 * @param id The result id of the request as a {@link String}.
 * @param result The result of a request as a {@link String}.
 */
//@JavascriptInterface
//@SuppressWarnings("unused")
void JellyBeanAccessibilityInjector::CallbackHandler::OnResult(
    /* [in] */ const String& id,
    /* [in] */ const String& result)
{
    Int64 resultId;
    // try {
        resultId = StringUtils::ParseInt64(id);
    // } catch (NumberFormatException e) {
    //     return;
    // }

    {
        AutoLock lock(mResultLock);
        if (resultId > mResultId) {
            mResult = StringUtils::ParseBoolean(result);
            mResultId = resultId;
        }
        mResultLock.NotifyAll();
    }
}

//==================================================================
//                  JellyBeanAccessibilityInjector
//==================================================================

const String JellyBeanAccessibilityInjector::ALIAS_TRAVERSAL_JS_INTERFACE("accessibilityTraversal");

// Template for JavaScript that performs AndroidVox actions.
const String JellyBeanAccessibilityInjector::ACCESSIBILITY_ANDROIDVOX_TEMPLATE("cvox.AndroidVox.performAction('%1s')");

/**
 * Constructs an instance of the JellyBeanAccessibilityInjector.
 * @param view The ContentViewCore that this AccessibilityInjector manages.
 */
JellyBeanAccessibilityInjector::JellyBeanAccessibilityInjector(
    /* [in] */ ContentViewCore* view)
    : AccessibilityInjector(view)
{
}

//@Override
void JellyBeanAccessibilityInjector::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    info->SetMovementGranularities(IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_CHARACTER |
            IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_WORD |
            IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_LINE |
            IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_PARAGRAPH |
            IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_PAGE);
    info->AddAction(IAccessibilityNodeInfo::ACTION_NEXT_AT_MOVEMENT_GRANULARITY);
    info->AddAction(IAccessibilityNodeInfo::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY);
    info->AddAction(IAccessibilityNodeInfo::ACTION_NEXT_HTML_ELEMENT);
    info->AddAction(IAccessibilityNodeInfo::ACTION_PREVIOUS_HTML_ELEMENT);
    info->AddAction(IAccessibilityNodeInfo::ACTION_CLICK);
    info->SetClickable(TRUE);
}

//@Override
Boolean JellyBeanAccessibilityInjector::SupportsAccessibilityAction(
    /* [in] */ Int32 action)
{
    if (action == IAccessibilityNodeInfo::ACTION_NEXT_AT_MOVEMENT_GRANULARITY ||
            action == IAccessibilityNodeInfo::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY ||
            action == IAccessibilityNodeInfo::ACTION_NEXT_HTML_ELEMENT ||
            action == IAccessibilityNodeInfo::ACTION_PREVIOUS_HTML_ELEMENT ||
            action == IAccessibilityNodeInfo::ACTION_CLICK) {
        return TRUE;
    }

    return FALSE;
}

//@Override
Boolean JellyBeanAccessibilityInjector::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    if (!AccessibilityIsAvailable() || !mContentViewCore->IsAlive() ||
            !mInjectedScriptEnabled || !mScriptInjected) {
        return FALSE;
    }

    Boolean actionSuccessful = SendActionToAndroidVox(action, arguments);

    if (actionSuccessful) mContentViewCore->ShowImeIfNeeded();

    return actionSuccessful;
}

//@Override
void JellyBeanAccessibilityInjector::AddAccessibilityApis()
{
    AccessibilityInjector::AddAccessibilityApis();

    AutoPtr<IContext> context = mContentViewCore->GetContext();
    if (context != NULL && mCallback == NULL) {
        mCallback = new CallbackHandler(ALIAS_TRAVERSAL_JS_INTERFACE);
        mContentViewCore->AddJavascriptInterface(TO_IINTERFACE(mCallback), ALIAS_TRAVERSAL_JS_INTERFACE);
    }
}

//@Override
void JellyBeanAccessibilityInjector::RemoveAccessibilityApis()
{
    AccessibilityInjector::RemoveAccessibilityApis();

    if (mCallback != NULL) {
        mContentViewCore->RemoveJavascriptInterface(ALIAS_TRAVERSAL_JS_INTERFACE);
        mCallback = NULL;
    }
}

/**
 * Packs an accessibility action into a JSON object and sends it to AndroidVox.
 *
 * @param action The action identifier.
 * @param arguments The action arguments, if applicable.
 * @return The result of the action.
 */
Boolean JellyBeanAccessibilityInjector::SendActionToAndroidVox(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    if (mCallback == NULL) return FALSE;

    if (mAccessibilityJSONObject == NULL) {
        CJSONObject::New((IJSONObject**)&mAccessibilityJSONObject);
    }
    else {
        // Remove all keys from the object.
        //final Iterator<?> keys = mAccessibilityJSONObject.keys();
        AutoPtr<IIterator> keyIt;
        mAccessibilityJSONObject->GetKeys((IIterator**)&keyIt);

        Boolean hasNext;
        while (keyIt->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            keyIt->GetNext((IInterface**)&obj);
            keyIt->Remove();
        }
    }

    // try {
        AutoPtr<IInteger32> iAction = CoreUtils::Convert(action);
        mAccessibilityJSONObject->Accumulate(String("action"), TO_IINTERFACE(iAction));
        if (arguments != NULL) {
            if (action == IAccessibilityNodeInfo::ACTION_NEXT_AT_MOVEMENT_GRANULARITY ||
                    action == IAccessibilityNodeInfo::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY) {
                Int32 granularity;
                arguments->GetInt32(IAccessibilityNodeInfo::ACTION_ARGUMENT_MOVEMENT_GRANULARITY_INT,
                    &granularity);
                AutoPtr<IInteger32> igranularity = CoreUtils::Convert(granularity);
                mAccessibilityJSONObject->Accumulate(String("granularity"), TO_IINTERFACE(igranularity));
            }
            else if (action == IAccessibilityNodeInfo::ACTION_NEXT_HTML_ELEMENT ||
                    action == IAccessibilityNodeInfo::ACTION_PREVIOUS_HTML_ELEMENT) {
                String element;
                arguments->GetString(
                        IAccessibilityNodeInfo::ACTION_ARGUMENT_HTML_ELEMENT_STRING,
                        &element);
                AutoPtr<ICharSequence> iElement = CoreUtils::Convert(element);
                mAccessibilityJSONObject->Accumulate(String("element"), TO_IINTERFACE(iElement));
            }
        }
    // } catch (JSONException ex) {
    //     return false;
    // }

    String jsonString;
    mAccessibilityJSONObject->ToString(0, &jsonString);
    String jsCode;
    AutoPtr<ILocaleHelper> localHelper;
    AutoPtr<ILocale> us;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localHelper);
    localHelper->GetUS((ILocale**)&us);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> istr = CoreUtils::Convert(jsonString);
    args->Set(0, TO_IINTERFACE(istr));

    jsCode = StringUtils::Format(us, ACCESSIBILITY_ANDROIDVOX_TEMPLATE, args);
    Boolean result = mCallback->PerformAction(mContentViewCore, jsCode);
    return result;
}

} // namespace Accessibility
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
