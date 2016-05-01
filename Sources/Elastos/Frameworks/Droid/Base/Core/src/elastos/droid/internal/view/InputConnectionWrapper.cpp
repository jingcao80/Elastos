
#include "elastos/droid/internal/view/InputConnectionWrapper.h"
#include "elastos/droid/internal/view/CInputContextCallback.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::InputMethod::EIID_IInputConnection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

static AutoPtr<InputConnectionWrapper::InputContextCallback> InitCallback()
{
    AutoPtr<CInputContextCallback> callback;
    CInputContextCallback::NewByFriend((CInputContextCallback**)&callback);
    return callback;
}

const String InputConnectionWrapper::InputContextCallback::TAG("InputConnectionWrapper.ICC");
AutoPtr<InputConnectionWrapper::InputContextCallback> InputConnectionWrapper::InputContextCallback::sInstance = InitCallback();
Int32 InputConnectionWrapper::InputContextCallback::sSequenceNumber = 1;
Object InputConnectionWrapper::InputContextCallback::sLock;

CAR_INTERFACE_IMPL_2(InputConnectionWrapper::InputContextCallback, Object, IIInputContextCallback, IBinder)

InputConnectionWrapper::InputContextCallback::InputContextCallback()
    : mSeq(0)
    , mHaveValue(FALSE)
    , mCursorCapsMode(0)
    , mRequestUpdateCursorAnchorInfoResult(FALSE)
{
}

ECode InputConnectionWrapper::InputContextCallback::constructor()
{
    return NOERROR;
}

AutoPtr<InputConnectionWrapper::InputContextCallback>
InputConnectionWrapper::InputContextCallback::GetInstance()
{
    AutoLock lock(sLock);

    // Return sInstance if it's non-NULL, otherwise construct a new callback
    AutoPtr<InputContextCallback> callback;
    if (sInstance != NULL) {
        callback = sInstance;
        sInstance = NULL;

        // Reset the callback
        callback->mHaveValue = FALSE;
    }
    else {
        callback = InitCallback();
    }

    // Set the sequence number
    callback->mSeq = sSequenceNumber++;
    return callback;
}

/**
 * Makes the given InputContextCallback available for use in the future.
 */
void InputConnectionWrapper::InputContextCallback::Dispose()
{
    AutoLock lock(sLock);

    // If sInstance is non-NULL, just let this object be garbage-collected
    if (sInstance == NULL) {
        // Allow any objects being held to be gc'ed
        mTextAfterCursor = NULL;
        mTextBeforeCursor = NULL;
        mExtractedText = NULL;
        sInstance = this;
    }
}

ECode InputConnectionWrapper::InputContextCallback::SetTextBeforeCursor(
    /* [in] */ ICharSequence* textBeforeCursor,
    /* [in] */ Int32 seq)
{
    AutoLock lock(this);

    if (seq == mSeq) {
        mTextBeforeCursor = textBeforeCursor;
        mHaveValue = TRUE;
        NotifyAll();
    }
    else {
        Logger::I(TAG, "Got out-of-sequence callback %d (expected %d)"
            " in setTextBeforeCursor, ignoring.", seq, mSeq);
    }

    return NOERROR;
}

ECode InputConnectionWrapper::InputContextCallback::SetTextAfterCursor(
    /* [in] */ ICharSequence* textAfterCursor,
    /* [in] */ Int32 seq)
{
    AutoLock lock(this);

    if (seq == mSeq) {
        mTextAfterCursor = textAfterCursor;
        mHaveValue = TRUE;
        NotifyAll();
    }
    else {
        Logger::I(TAG, "Got out-of-sequence callback %d (expected %d)"
            " in setTextAfterCursor, ignoring.", seq, mSeq);
    }

    return NOERROR;
}

ECode InputConnectionWrapper::InputContextCallback::SetSelectedText(
    /* [in] */ ICharSequence* selectedText,
    /* [in] */ Int32 seq)
{
    AutoLock lock(this);

    if (seq == mSeq) {
        mSelectedText = selectedText;
        mHaveValue = TRUE;
        NotifyAll();
    }
    else {
        Logger::I(TAG, "Got out-of-sequence callback %d (expected %d)"
            " in SetSelectedText, ignoring.", seq, mSeq);
    }

    return NOERROR;
}

ECode InputConnectionWrapper::InputContextCallback::SetCursorCapsMode(
    /* [in] */ Int32 capsMode,
    /* [in] */ Int32 seq)
{
    AutoLock lock(this);

    if (seq == mSeq) {
        mCursorCapsMode = capsMode;
        mHaveValue = TRUE;
        NotifyAll();
    }
    else {
        Logger::I(TAG, "Got out-of-sequence callback %d (expected %d)"
            " in SetCursorCapsMode, ignoring.", seq, mSeq);
    }

    return NOERROR;
}

ECode InputConnectionWrapper::InputContextCallback::SetExtractedText(
    /* [in] */ IExtractedText* extractedText,
    /* [in] */ Int32 seq)
{
    AutoLock lock(this);

    if (seq == mSeq) {
        mExtractedText = extractedText;
        mHaveValue = TRUE;
        NotifyAll();
    }
    else {
        Logger::I(TAG, "Got out-of-sequence callback %d (expected %d)"
            " in SetExtractedText, ignoring.", seq, mSeq);
    }

    return NOERROR;
}

ECode InputConnectionWrapper::InputContextCallback::SetRequestUpdateCursorAnchorInfoResult(
    /* [in] */ Boolean result,
    /* [in] */ Int32 seq)
{
    AutoLock lock(this);

    if (seq == mSeq) {
        mRequestUpdateCursorAnchorInfoResult = result;
        mHaveValue = TRUE;
        NotifyAll();
    }
    else {
        Logger::I(TAG, "Got out-of-sequence callback %d (expected %d)"
            " in SetRequestUpdateCursorAnchorInfoResult, ignoring.", seq, mSeq);
    }

    return NOERROR;
}

void InputConnectionWrapper::InputContextCallback::WaitForResultLocked()
{
    Int64 startTime = SystemClock::GetUptimeMillis();
    Int64 endTime = startTime + InputConnectionWrapper::MAX_WAIT_TIME_MILLIS;

    while (!mHaveValue) {
        Int64 remainingTime = endTime - SystemClock::GetUptimeMillis();
        if (remainingTime <= 0) {
            Logger::W(TAG, "Timed out waiting on IIInputContextCallback");
            return;
        }
        // try {
        Wait(remainingTime);
        // } catch (InterruptedException e) {
        // }
    }
}

ECode InputConnectionWrapper::InputContextCallback::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

const Int32 InputConnectionWrapper::MAX_WAIT_TIME_MILLIS = 2000;

CAR_INTERFACE_IMPL(InputConnectionWrapper, Object, IInputConnection)

ECode InputConnectionWrapper::constructor(
    /* [in] */ IIInputContext* inputContext)
{
    assert(inputContext != NULL);
    mIInputContext = inputContext;
    return NOERROR;
}

ECode InputConnectionWrapper::GetTextAfterCursor(
    /* [in] */ Int32 n,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = NULL;
    AutoPtr<InputContextCallback> callback = InputContextCallback::GetInstance();
    if (FAILED(mIInputContext->GetTextAfterCursor(n, flags, callback->mSeq, callback.Get())))
        return NOERROR;

    {
        AutoLock lock(callback);
        callback->WaitForResultLocked();
        if (callback->mHaveValue) {
            *text = callback->mTextAfterCursor;
        }
    }

    callback->Dispose();
    // catch (RemoteException e) {
    //     return NULL;
    // }
    REFCOUNT_ADD(*text);

    return NOERROR;
}

ECode InputConnectionWrapper::GetTextBeforeCursor(
    /* [in] */ Int32 n,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = NULL;
    AutoPtr<InputContextCallback> callback = InputContextCallback::GetInstance();
    if (FAILED(mIInputContext->GetTextBeforeCursor(n, flags, callback->mSeq, callback.Get())))
        return NOERROR;

    {
        AutoLock lock(callback);
        callback->WaitForResultLocked();
        if (callback->mHaveValue) {
            *text = callback->mTextBeforeCursor;
            REFCOUNT_ADD(*text);
        }
    }

    callback->Dispose();
    // } catch (RemoteException e) {
    //     return NULL;
    // }

    return NOERROR;
}

ECode InputConnectionWrapper::GetSelectedText(
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = NULL;
    AutoPtr<InputContextCallback> callback = InputContextCallback::GetInstance();
    if (FAILED(mIInputContext->GetSelectedText(flags, callback->mSeq, callback.Get())))
        return NOERROR;

    {
        AutoLock lock(callback);
        callback->WaitForResultLocked();
        if (callback->mHaveValue) {
            *text = callback->mSelectedText;
        }
    }

    callback->Dispose();
    // } catch (RemoteException e) {
    //     return NULL;
    // }
    REFCOUNT_ADD(*text);

    return NOERROR;
}

ECode InputConnectionWrapper::GetCursorCapsMode(
    /* [in] */ Int32 reqModes,
    /* [out] */ Int32* capsMode)
{
    VALIDATE_NOT_NULL(capsMode);
    *capsMode = 0;

    AutoPtr<InputContextCallback> callback = InputContextCallback::GetInstance();
    if (FAILED(mIInputContext->GetCursorCapsMode(reqModes, callback->mSeq, callback.Get())))
        return NOERROR;

    {
        AutoLock lock(callback);
        callback->WaitForResultLocked();
        if (callback->mHaveValue) {
            *capsMode = callback->mCursorCapsMode;
        }
    }

    callback->Dispose();
    // } catch (RemoteException e) {
    //     return 0;
    // }
    return NOERROR;
}

ECode InputConnectionWrapper::GetExtractedText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ Int32 flags,
    /* [out] */ IExtractedText** extractedText)
{
    VALIDATE_NOT_NULL(extractedText);
    *extractedText = NULL;

    AutoPtr<InputContextCallback> callback = InputContextCallback::GetInstance();
    if (FAILED(mIInputContext->GetExtractedText(request, flags, callback->mSeq, callback.Get())))
        return NOERROR;

    {
        AutoLock lock(callback);
        callback->WaitForResultLocked();
        if (callback->mHaveValue) {
            *extractedText = callback->mExtractedText;
        }
    }

    callback->Dispose();
    // } catch (RemoteException e) {
    //     return NULL;
    // }

    REFCOUNT_ADD(*extractedText);

    return NOERROR;
}

ECode InputConnectionWrapper::CommitText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->CommitText(text, newCursorPosition);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::CommitCompletion(
    /* [in] */ ICompletionInfo* text,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->CommitCompletion(text);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::CommitCorrection(
    /* [in] */ ICorrectionInfo* correctionInfo,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->CommitCorrection(correctionInfo);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::SetSelection(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->SetSelection(start, end);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::PerformEditorAction(
    /* [in] */ Int32 editorAction,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->PerformEditorAction(editorAction);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::PerformContextMenuAction(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->PerformContextMenuAction(id);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::SetComposingRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->SetComposingRegion(start, end);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::SetComposingText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->SetComposingText(text, newCursorPosition);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::FinishComposingText(
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->FinishComposingText();
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::BeginBatchEdit(
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->BeginBatchEdit();
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::EndBatchEdit(
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->EndBatchEdit();
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::SendKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->SendKeyEvent(event);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::ClearMetaKeyStates(
    /* [in] */ Int32 states,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->ClearMetaKeyStates(states);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::DeleteSurroundingText(
    /* [in] */ Int32 leftLength,
    /* [in] */ Int32 rightLength,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->DeleteSurroundingText(leftLength, rightLength);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::ReportFullscreenMode(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->ReportFullscreenMode(enabled);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::PerformPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    // try {
    ECode ec = mIInputContext->PerformPrivateCommand(action, data);
    *flag = SUCCEEDED(ec) ? TRUE : FALSE;
    return ec;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode InputConnectionWrapper::RequestCursorUpdates(
    /* [in] */ Int32 cursorUpdateMode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<InputContextCallback> callback = InputContextCallback::GetInstance();
    if (FAILED(mIInputContext->RequestUpdateCursorAnchorInfo(
        cursorUpdateMode, callback->mSeq, callback)))
        return NOERROR;

    {
        AutoLock lock(callback);
        callback->WaitForResultLocked();
        if (callback->mHaveValue) {
            *result = callback->mRequestUpdateCursorAnchorInfoResult;
        }
    }

    callback->Dispose();

    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
