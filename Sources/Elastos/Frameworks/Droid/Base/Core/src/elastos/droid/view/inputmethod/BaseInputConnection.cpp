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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/inputmethod/BaseInputConnection.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/ViewRootImpl.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/text/CEditableFactory.h"
#include "elastos/droid/text/method/MetaKeyKeyListener.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
//#include "utils/ArrayUtils.h"
#include "elastos/droid/view/CKeyCharacterMap.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::CEditableFactory;
using Elastos::Droid::Text::IEditableFactory;
using Elastos::Droid::Text::Selection;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::EIID_INoCopySpan;
using Elastos::Droid::Text::IGetChars;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::Method::MetaKeyKeyListener;
using Elastos::Droid::Text::Method::IMetaKeyKeyListener;
using Elastos::Droid::View::InputMethod::CInputMethodManager;
using Elastos::Droid::View::CKeyCharacterMap;

using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              BaseInputConnection::ComposingText::
//========================================================================================
CAR_INTERFACE_IMPL(BaseInputConnection::ComposingText, Object, INoCopySpan)

//========================================================================================
//              BaseInputConnection::
//========================================================================================
const Boolean BaseInputConnection::DEBUG = FALSE;
const String BaseInputConnection::TAG("BaseInputConnection");
const AutoPtr<IInterface> BaseInputConnection::COMPOSING = (IInterface*)(INoCopySpan*)(new BaseInputConnection::ComposingText());

CAR_INTERFACE_IMPL_2(BaseInputConnection, Object, IBaseInputConnection, IInputConnection)

BaseInputConnection::BaseInputConnection()
    : mDummyMode(FALSE)
{}

ECode BaseInputConnection::constructor(
    /* [in] */ IInputMethodManager* mgr,
    /* [in] */ Boolean fullEditor)
{
    mIMM = mgr;
    mDummyMode = !fullEditor;
    return NOERROR;
}

ECode BaseInputConnection::constructor(
    /* [in] */ IView* targetView,
    /* [in] */ Boolean fullEditor)
{
    AutoPtr<IContext> context;
    targetView->GetContext((IContext**)&context);
    assert(context != NULL);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    mIMM = IInputMethodManager::Probe(obj);
    assert(mIMM != NULL);
    mTargetView = targetView;
    mDummyMode = !fullEditor;
    return NOERROR;
}

BaseInputConnection::~BaseInputConnection()
{
    mDefaultComposingSpans = NULL;
}

ECode BaseInputConnection::RemoveComposingSpans(
    /* [in] */ ISpannable* text)
{
    assert(text != NULL);
    text->RemoveSpan(COMPOSING);
    Int32 len = 0;
    ICharSequence::Probe(text)->GetLength(&len);
    AutoPtr<ArrayOf< AutoPtr<IInterface> > > sps;
    ISpanned::Probe(text)->GetSpans(0, len, EIID_IInterface, (ArrayOf<IInterface*>**)&sps);
    if (sps != NULL) {
        for (Int32 i = sps->GetLength() - 1; i >= 0; i--) {
            IInterface* o = (*sps)[i];
            Int32 flag = 0;
            ISpanned::Probe(text)->GetSpanFlags(o, &flag);
            if ((flag & ISpanned::SPAN_COMPOSING) != 0) {
                text->RemoveSpan(o);
            }
        }

        sps = NULL;
    }
    return NOERROR;
}

ECode BaseInputConnection::SetComposingSpans(
    /* [in] */ ISpannable* text)
{
    assert(text != NULL);
    Int32 len = 0;
    ICharSequence::Probe(text)->GetLength(&len);
    return SetComposingSpans(text, 0, len);
}

ECode BaseInputConnection::SetComposingSpans(
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    assert(text != NULL);
    AutoPtr<ArrayOf< AutoPtr<IInterface> > > sps;
    ISpanned::Probe(text)->GetSpans(0, end, EIID_IInterface, (ArrayOf<IInterface*>**)&sps);
    if (sps != NULL) {
        for (int i = sps->GetLength() - 1; i >= 0; i--) {
            IInterface* o = (*sps)[i];
            if (o == COMPOSING.Get()) {
                text->RemoveSpan(o);
                continue;
            }

            Int32 flag = 0;
            ISpanned::Probe(text)->GetSpanFlags(o, &flag);
            if ((flag & (ISpanned::SPAN_COMPOSING | ISpanned::SPAN_POINT_MARK_MASK))
                    != (ISpanned::SPAN_COMPOSING | ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE)) {
                Int32 s = 0, e = 0;
                ISpanned::Probe(text)->GetSpanStart(o, &s);
                ISpanned::Probe(text)->GetSpanEnd(o, &e);
                text->SetSpan(o, s, e,
                        (flag & ~ISpanned::SPAN_POINT_MARK_MASK)
                                | ISpanned::SPAN_COMPOSING
                                | ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
            }
        }

        sps = NULL;
    }

    return text->SetSpan(COMPOSING, start, end,
            ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE | ISpanned::SPAN_COMPOSING);
}

Int32 BaseInputConnection::GetComposingSpanStart(
    /* [in] */ ISpannable* text)
{
    assert(text != NULL);
    Int32 start;
    ASSERT_SUCCEEDED(ISpanned::Probe(text)->GetSpanStart(COMPOSING, &start));
    return start;
}

Int32 BaseInputConnection::GetComposingSpanEnd(
    /* [in] */ ISpannable* text)
{
    assert(text != NULL);
    Int32 end;
    ASSERT_SUCCEEDED(ISpanned::Probe(text)->GetSpanEnd(COMPOSING, &end));
    return end;
}

ECode BaseInputConnection::GetEditable(
    /* [out] */ IEditable** result)
{
    VALIDATE_NOT_NULL(result)

    if (mEditable == NULL) {
        AutoPtr<IEditableFactory> editableFactory;
        ASSERT_SUCCEEDED(CEditableFactory::AcquireSingleton(
            (IEditableFactory**)&editableFactory));
        AutoPtr<ICharSequence> tmpStr;
        CString::New(String(""), (ICharSequence**)&tmpStr);
        editableFactory->NewEditable(tmpStr, (IEditable**)&mEditable);
        Selection::SetSelection(ISpannable::Probe(mEditable), 0);
    }
    *result = mEditable;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode BaseInputConnection::BeginBatchEdit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode BaseInputConnection::EndBatchEdit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

void BaseInputConnection::ReportFinish()
{
    // Intentionaly empty
}

ECode BaseInputConnection::ClearMetaKeyStates(
    /* [in] */ Int32 states,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    MetaKeyKeyListener::ClearMetaKeyState(content, states);
    *result = TRUE;
    return NOERROR;
}

ECode BaseInputConnection::CommitCompletion(
    /* [in] */ ICompletionInfo* text,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode BaseInputConnection::CommitCorrection(
    /* [in] */ ICorrectionInfo* correctionInfo,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode BaseInputConnection::CommitText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DEBUG) {
        String str;
        text->ToString(&str);
        Logger::V(TAG, "commitText %s", str.string());
    }
    ReplaceText(text, newCursorPosition, FALSE);
    mIMM->NotifyUserAction();
    SendCurrentText();
    *result = TRUE;
    return NOERROR;
}

ECode BaseInputConnection::DeleteSurroundingText(
    /* [in] */ Int32 beforeLength,
    /* [in] */ Int32 afterLength,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DEBUG)
        Logger::V(TAG, "deleteSurroundingText %d / %d", beforeLength, afterLength);
    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean bEdit = FALSE;
    BeginBatchEdit(&bEdit);

    Int32 a = Selection::GetSelectionStart(ICharSequence::Probe(content));
    Int32 b = Selection::GetSelectionEnd(ICharSequence::Probe(content));

    if (a > b) {
        Int32 tmp = a;
        a = b;
        b = tmp;
    }

    // ignore the composing text.
    Int32 ca = GetComposingSpanStart(ISpannable::Probe(content));
    Int32 cb = GetComposingSpanEnd(ISpannable::Probe(content));
    if (cb < ca) {
        Int32 tmp = ca;
        ca = cb;
        cb = tmp;
    }
    if (ca != -1 && cb != -1) {
        if (ca < a) a = ca;
        if (cb > b) b = cb;
    }

    Int32 deleted = 0;

    if (beforeLength > 0) {
        Int32 start = a - beforeLength;
        if (start < 0) start = 0;
        content->Delete(start, a);
        deleted = a - start;
    }

    if (afterLength > 0) {
        b = b - deleted;

        Int32 end = b + afterLength;
        Int32 len = 0;
        ICharSequence::Probe(content)->GetLength(&len);
        if (end > len) {
            end = len;
        }

        content->Delete(b, end);
    }

    EndBatchEdit(&bEdit);

    *result = TRUE;
    return NOERROR;
}

ECode BaseInputConnection::FinishComposingText(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DEBUG) Logger::V(TAG, "finishComposingText");
    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content != NULL) {
        Boolean bEdit = FALSE;
        BeginBatchEdit(&bEdit);
        RemoveComposingSpans(ISpannable::Probe(content));
        // Note: sendCurrentText does nothing unless mDummyMode is set
        SendCurrentText();
        EndBatchEdit(&bEdit);
    }
    *result = TRUE;
    return NOERROR;
}

ECode BaseInputConnection::GetCursorCapsMode(
    /* [in] */ Int32 reqModes,
    /* [out] */ Int32* afterLength)
{
    VALIDATE_NOT_NULL(afterLength)

    if (mDummyMode) {
        *afterLength = 0;
        return NOERROR;
    }

    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content == NULL) {
        *afterLength = 0;
        return NOERROR;
    }

    Int32 a = Selection::GetSelectionStart(ICharSequence::Probe(content));
    Int32 b = Selection::GetSelectionEnd(ICharSequence::Probe(content));

    if (a > b) {
        Int32 tmp = a;
        a = b;
        b = tmp;
    }

    *afterLength = TextUtils::GetCapsMode(ICharSequence::Probe(content), a, reqModes);
    return NOERROR;
}

ECode BaseInputConnection::GetExtractedText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ Int32 flags,
    /* [out] */ IExtractedText** text)
{
    VALIDATE_NOT_NULL(text)

    *text = NULL;
    return NOERROR;
}

ECode BaseInputConnection::GetTextBeforeCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq)

    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content == NULL) {
        *seq = NULL;
        return NOERROR;
    }

    Int32 a = Selection::GetSelectionStart(ICharSequence::Probe(content));
    Int32 b = Selection::GetSelectionEnd(ICharSequence::Probe(content));

    if (a > b) {
        Int32 tmp = a;
        a = b;
        b = tmp;
    }

    if (a <= 0) {
        AutoPtr<ICharSequence> text;
        CString::New(String(""), (ICharSequence**)&text);
        *seq = text;
        REFCOUNT_ADD(*seq)
        return NOERROR;
    }

    if (length > a) {
        length = a;
    }

    if ((flags & IInputConnection::GET_TEXT_WITH_STYLES) != 0) {
        AutoPtr<ICharSequence> text;
        ICharSequence::Probe(content)->SubSequence(a - length, a, (ICharSequence**)&text);
        *seq = text;
        REFCOUNT_ADD(*seq)
        return NOERROR;
    }

    String str = TextUtils::Substring(ICharSequence::Probe(content), a - length, a);
    AutoPtr<ICharSequence> text;
    CString::New(str, (ICharSequence**)&text);
    *seq = text;
    REFCOUNT_ADD(*seq)
    return NOERROR;
}

ECode BaseInputConnection::GetSelectedText(
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq)

    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content == NULL) {
        *seq = NULL;
        return NOERROR;
    }

    Int32 a = Selection::GetSelectionStart(ICharSequence::Probe(content));
    Int32 b = Selection::GetSelectionEnd(ICharSequence::Probe(content));

    if (a > b) {
        Int32 tmp = a;
        a = b;
        b = tmp;
    }

    if (a == b) {
        *seq = NULL;
        return NOERROR;
    }

    if ((flags & IInputConnection::GET_TEXT_WITH_STYLES) != 0) {
        AutoPtr<ICharSequence> text;
        ICharSequence::Probe(content)->SubSequence(a, b, (ICharSequence**)&text);
        *seq = text;
        REFCOUNT_ADD(*seq)
        return NOERROR;
    }

    String str = TextUtils::Substring(ICharSequence::Probe(content), a, b);
    AutoPtr<ICharSequence> text;
    CString::New(str, (ICharSequence**)&text);
    *seq = text;
    REFCOUNT_ADD(*seq)
    return NOERROR;
}

ECode BaseInputConnection::GetTextAfterCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq)

    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content == NULL) {
        *seq = NULL;
        return NOERROR;
    }

    Int32 a = Selection::GetSelectionStart(ICharSequence::Probe(content));
    Int32 b = Selection::GetSelectionEnd(ICharSequence::Probe(content));

    if (a > b) {
        Int32 tmp = a;
        a = b;
        b = tmp;
    }

    // Guard against the case where the cursor has not been positioned yet.
    if (b < 0) {
        b = 0;
    }

    Int32 len = 0;
    ICharSequence::Probe(content)->GetLength(&len);
    if (b + length > len) {
        length = len - b;
    }

    if ((flags & IInputConnection::GET_TEXT_WITH_STYLES) != 0) {
        AutoPtr<ICharSequence> text;
        ICharSequence::Probe(content)->SubSequence(b, b + length, (ICharSequence**)&text);
        *seq = text;
        REFCOUNT_ADD(*seq)
        return NOERROR;
    }

    String str = TextUtils::Substring(ICharSequence::Probe(content), b, b + length);
    AutoPtr<ICharSequence> text;
    CString::New(str, (ICharSequence**)&text);
    *seq = text;
    REFCOUNT_ADD(*seq)
    return NOERROR;
}

ECode BaseInputConnection::PerformEditorAction(
    /* [in] */ Int32 actionCode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 eventTime = SystemClock::GetUptimeMillis();

    AutoPtr<IKeyEvent> event;
    ASSERT_SUCCEEDED(CKeyEvent::New(eventTime, eventTime,
            IKeyEvent::ACTION_DOWN, IKeyEvent::KEYCODE_ENTER, 0, 0,
            /*KeyCharacterMap.*/IKeyCharacterMap::VIRTUAL_KEYBOARD, 0,
            IKeyEvent::FLAG_SOFT_KEYBOARD | IKeyEvent::FLAG_KEEP_TOUCH_MODE
            | IKeyEvent::FLAG_EDITOR_ACTION, (IKeyEvent**)&event));
    Boolean bSKE = FALSE;
    SendKeyEvent(event, &bSKE);

    event = NULL;
    ASSERT_SUCCEEDED(CKeyEvent::New(SystemClock::GetUptimeMillis(), eventTime,
            IKeyEvent::ACTION_UP, IKeyEvent::KEYCODE_ENTER, 0, 0,
            /*KeyCharacterMap.*/IKeyCharacterMap::VIRTUAL_KEYBOARD, 0,
            IKeyEvent::FLAG_SOFT_KEYBOARD | IKeyEvent::FLAG_KEEP_TOUCH_MODE
            | IKeyEvent::FLAG_EDITOR_ACTION, (IKeyEvent**)&event));
    SendKeyEvent(event, &bSKE);
    *result = TRUE;
    return NOERROR;
}

ECode BaseInputConnection::PerformContextMenuAction(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode BaseInputConnection::PerformPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode BaseInputConnection::RequestCursorUpdates(
    /* [in] */ Int32 cursorUpdateMode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode BaseInputConnection::SetComposingText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String str;
    text->ToString(&str);
    if (DEBUG) Logger::V(TAG, "setComposingText %s", str.string());
    ReplaceText(text, newCursorPosition, TRUE);
    mIMM->NotifyUserAction();
    *result = TRUE;
    return NOERROR;
}

ECode BaseInputConnection::SetComposingRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content != NULL) {
        Boolean bEdit = FALSE;
        BeginBatchEdit(&bEdit);
        RemoveComposingSpans(ISpannable::Probe(content));
        Int32 a = start;
        Int32 b = end;
        if (a > b) {
            Int32 tmp = a;
            a = b;
            b = tmp;
        }
        // Clip the end points to be within the content bounds.
        Int32 length = 0;
        ICharSequence::Probe(content)->GetLength(&length);
        if (a < 0) a = 0;
        if (b < 0) b = 0;
        if (a > length) a = length;
        if (b > length) b = length;

        EnsureDefaultComposingSpans();
        if (mDefaultComposingSpans != NULL) {
            for (Int32 i = 0; i < mDefaultComposingSpans->GetLength(); ++i) {
                ISpannable::Probe(content)->SetSpan((*mDefaultComposingSpans)[i], a, b,
                        ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE | ISpanned::SPAN_COMPOSING);
            }
        }

        ISpannable::Probe(content)->SetSpan(COMPOSING, a, b,
                ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE | ISpanned::SPAN_COMPOSING);

        // Note: sendCurrentText does nothing unless mDummyMode is set
        SendCurrentText();
        EndBatchEdit(&bEdit);
    }
    *result = TRUE;
    return NOERROR;
}

ECode BaseInputConnection::SetSelection(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DEBUG) Logger::V(TAG, "setSelection %d, %d", start, end);
    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    int len = 0;
    ICharSequence::Probe(content)->GetLength(&len);
    if (start > len || end > len || start < 0 || end < 0) {
        // If the given selection is out of bounds, just ignore it.
        // Most likely the text was changed out from under the IME,
        // and the IME is going to have to update all of its state
        // anyway.
        *result = TRUE;
        return NOERROR;
    }
    Int32 ms = 0;
    MetaKeyKeyListener::GetMetaState(ICharSequence::Probe(content),
            IMetaKeyKeyListener::META_SELECTING, &ms);
    if (start == end && ms != 0) {
        // If we are in selection mode, then we want to extend the
        // selection instead of replacing it.
        Selection::ExtendSelection(ISpannable::Probe(content), start);
    }
    else {
        Selection::SetSelection(ISpannable::Probe(content), start, end);
    }
    *result = TRUE;
    return NOERROR;
}

ECode BaseInputConnection::SendKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock lock(((CInputMethodManager*)mIMM.Get())->mHLock);

        AutoPtr<IViewRootImpl> viewRootImpl;
        if (mTargetView != NULL) {
            VIEW_PROBE(mTargetView)->GetViewRootImpl((IViewRootImpl**)&viewRootImpl);
        }

        if (viewRootImpl == NULL) {
            if (((CInputMethodManager*)mIMM.Get())->mServedView != NULL) {
                View* v = VIEW_PROBE(((CInputMethodManager*)mIMM.Get())->mServedView);
                v->GetViewRootImpl((IViewRootImpl**)&viewRootImpl);
            }
        }

        if (viewRootImpl != NULL) {
            viewRootImpl->DispatchKeyFromIme(event);
        }
    }
    mIMM->NotifyUserAction();
    *result = FALSE;
    return NOERROR;
}

ECode BaseInputConnection::ReportFullscreenMode(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mIMM->SetFullscreenMode(enabled);
    *result = TRUE;
    return NOERROR;
}

void BaseInputConnection::SendCurrentText()
{
    if (!mDummyMode) {
        return;
    }

    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content != NULL) {
        Int32 N = 0;
        ICharSequence::Probe(content)->GetLength(&N);
        if (N == 0) {
            return;
        }
        if (N == 1) {
            // If it's 1 character, we have a chance of being
            // able to generate normal key events...
            if (mKeyCharacterMap == NULL) {
                CKeyCharacterMap::Load(IKeyCharacterMap::VIRTUAL_KEYBOARD, (IKeyCharacterMap**)&mKeyCharacterMap);
            }
            AutoPtr<ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(1);
            assert(IGetChars::Probe(content) != NULL);
            IGetChars::Probe(content.Get())->GetChars(0, 1, (ArrayOf<Char32>*)chars.Get(), 0);
            AutoPtr<ArrayOf<IKeyEvent*> > events;
            mKeyCharacterMap->GetEvents((ArrayOf<Char32>*)chars.Get(), (ArrayOf<IKeyEvent*>**)&events);
            if (events != NULL) {
                for (Int32 i = 0; i < events->GetLength(); ++i) {
                    if (DEBUG) Logger::V(TAG, "Sending: %p", (*events)[i]);
                    Boolean bSend = FALSE;
                    SendKeyEvent((*events)[i], &bSend);
                }

                content->Clear();
                return;
            }
        }

        // Otherwise, revert to the special key event containing
        // the actual characters.
        String contentStr;
        ICharSequence::Probe(content)->ToString(&contentStr);
        AutoPtr<IKeyEvent> event;
        CKeyEvent::New(SystemClock::GetUptimeMillis(), contentStr,
            IKeyCharacterMap::VIRTUAL_KEYBOARD, 0, (IKeyEvent**)&event);
        Boolean bSKE = FALSE;
        SendKeyEvent(event, &bSKE);
        content->Clear();
    }
}

void BaseInputConnection::EnsureDefaultComposingSpans()
{
    if (mDefaultComposingSpans == NULL) {
        AutoPtr<IContext> context;
        if (mTargetView != NULL) {
            mTargetView->GetContext((IContext**)&context);
        }
        else if (((CInputMethodManager*)mIMM.Get())->mServedView != NULL) {
            ((CInputMethodManager*)mIMM.Get())->mServedView->GetContext((IContext**)&context);
        }

        if (context != NULL) {
            AutoPtr<IResourcesTheme> theme;
            context->GetTheme((IResourcesTheme**)&theme);
            AutoPtr< ArrayOf<Int32> > attrs = ArrayOf<Int32>::Alloc(1);
            attrs->Set(0, R::attr::candidatesTextStyleSpans);
            AutoPtr<ITypedArray> ta;
            theme->ObtainStyledAttributes(attrs, (ITypedArray**)&ta);

            AutoPtr<ICharSequence> style;
            ta->GetText(0, (ICharSequence**)&style);
            ta->Recycle();
            if (style != NULL && ISpanned::Probe(style) != NULL) {
                Int32 len = 0;
                style->GetLength(&len);
                ISpanned::Probe(style)->GetSpans(0, len, EIID_IInterface, (ArrayOf<IInterface*>**)&mDefaultComposingSpans);
            }
        }
    }
}

void BaseInputConnection::ReplaceText(
    /* [in] */ ICharSequence* _text,
    /* [in] */ Int32 newCursorPosition,
    /* [in] */ Boolean composing)
{
    AutoPtr<ICharSequence> text = _text;
    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content == NULL) {
        return;
    }

    Boolean bEdit = FALSE;
    BeginBatchEdit(&bEdit);

    // delete composing text set previously.
    Int32 a = GetComposingSpanStart(ISpannable::Probe(content));
    Int32 b = GetComposingSpanEnd(ISpannable::Probe(content));

    if (DEBUG) Logger::V(TAG, "Composing span: %d to %d", a, b);

    if (b < a) {
        Int32 tmp = a;
        a = b;
        b = tmp;
    }

    if (a != -1 && b != -1) {
        RemoveComposingSpans(ISpannable::Probe(content));
    }
    else {
        a = Selection::GetSelectionStart(ICharSequence::Probe(content));
        b = Selection::GetSelectionEnd(ICharSequence::Probe(content));
        if (a < 0) a = 0;
        if (b < 0) b = 0;
        if (b < a) {
            Int32 tmp = a;
            a = b;
            b = tmp;
        }
    }

    if (composing) {
        assert(text != NULL);
        AutoPtr<ISpannable> sp;
        if (ISpannable::Probe(text) == NULL) {
            CSpannableStringBuilder::New(text, (ISpannable**)&sp);
            text = ICharSequence::Probe(sp);
            EnsureDefaultComposingSpans();
            if (mDefaultComposingSpans != NULL) {
                Int32 len2 = 0;
                text->GetLength(&len2);
                for (Int32 i = 0; i < mDefaultComposingSpans->GetLength(); ++i) {
                    sp->SetSpan((*mDefaultComposingSpans)[i], 0, len2,
                            ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE | ISpanned::SPAN_COMPOSING);
                }
            }
        }
        else {
            sp = ISpannable::Probe(text);
        }
        SetComposingSpans(sp);
    }

    if (DEBUG) {
        Logger::V(TAG, "Replacing from %d to %d with \"%s\", composing=%d",
            a, b, TO_CSTR(text), composing);
    }

    // if (DEBUG) {
    //     LogPrinter lp = new LogPrinter(Log.VERBOSE, TAG);
    //     lp.println("Current text:");
    //     TextUtils.dumpSpans(content, lp, "  ");
    //     lp.println("Composing text:");
    //     TextUtils.dumpSpans(text, lp, "  ");
    // }

    // Position the cursor appropriately, so that after replacing the
    // desired range of text it will be located in the correct spot.
    // This allows us to deal with filters performing edits on the text
    // we are providing here.
    if (newCursorPosition > 0) {
        newCursorPosition += b - 1;
    }
    else {
        newCursorPosition += a;
    }

    if (newCursorPosition < 0) {
        newCursorPosition = 0;
    }

    Int32 len = 0;
    ICharSequence::Probe(content)->GetLength(&len);
    if (newCursorPosition > len) {
        newCursorPosition = len;
    }
    Selection::SetSelection(ISpannable::Probe(content), newCursorPosition);

    content->Replace(a, b, text);

    // if (DEBUG) {
    //     LogPrinter lp = new LogPrinter(Log.VERBOSE, TAG);
    //     lp.println("Final text:");
    //     TextUtils.dumpSpans(content, lp, "  ");
    // }

    EndBatchEdit(&bEdit);
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
