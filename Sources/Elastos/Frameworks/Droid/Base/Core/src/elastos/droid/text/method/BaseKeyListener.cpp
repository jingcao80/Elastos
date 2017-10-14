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

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Math.h>
#include "elastos/droid/view/KeyEvent.h"
#include "elastos/droid/text/method/BaseKeyListener.h"
#include "elastos/droid/text/CNoCopySpanConcrete.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/CSelection.h"

using Elastos::Droid::Text::CSelection;
using Elastos::Droid::Text::CNoCopySpanConcrete;
using Elastos::Droid::Text::Method::IMetaKeyKeyListener;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::KeyEvent;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

static AutoPtr<IInterface> InitNoCopySpanConcrete()
{
    AutoPtr<INoCopySpan> ncs;
    CNoCopySpanConcrete::New((INoCopySpan**)&ncs);
    return (IInterface*)ncs.Get();
}

const AutoPtr<IInterface> BaseKeyListener::OLD_SEL_START = InitNoCopySpanConcrete();

CAR_INTERFACE_IMPL(BaseKeyListener, MetaKeyKeyListener, IKeyListener)

BaseKeyListener::BaseKeyListener()
{}

BaseKeyListener::~BaseKeyListener()
{}

ECode BaseKeyListener::Backspace(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = BackspaceOrForwardDelete(view, content, keyCode, event, FALSE);
    return NOERROR;
}

ECode BaseKeyListener::ForwardDelete(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = BackspaceOrForwardDelete(view, content, keyCode, event, TRUE);
    return NOERROR;
}

Boolean BaseKeyListener::BackspaceOrForwardDelete(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Boolean isForwardDelete)
{
    // Ensure the key event does not have modifiers except ALT or SHIFT.
    Int32 metaState;
    Boolean res = FALSE;
    res = KeyEvent::MetaStateHasNoModifiers((event->GetMetaState(&metaState), metaState) & ~(IKeyEvent::META_SHIFT_MASK | IKeyEvent::META_ALT_MASK));
    if (!res) {
        return FALSE;
    }

    // If there is a current selection, delete it.
    if (DeleteSelection(view, content)) {
        return TRUE;
    }

    // Alt+Backspace or Alt+ForwardDelete deletes the current line, if possible.
    Int32 tmpOut;
    if ((GetMetaState(ICharSequence::Probe(content), IKeyEvent::META_ALT_ON, event, &tmpOut), tmpOut) == 1) {
        if (DeleteLine(view, content)) {
            return TRUE;
        }
    }

    // Delete a character.
    AutoPtr<ISelection> sel;
    CSelection::AcquireSingleton((ISelection**)&sel);
    /*const*/ Int32 start = 0;
    sel->GetSelectionEnd(ICharSequence::Probe(content), &start);
    /*const*/ Int32 end;
    Boolean bIsShiftPressed;
    if (isForwardDelete || (event->IsShiftPressed(&bIsShiftPressed), bIsShiftPressed)
            || ((GetMetaState(ICharSequence::Probe(content), IMetaKeyKeyListener::META_SHIFT_ON, &tmpOut), tmpOut) == 1)) {
        end = TextUtils::GetOffsetAfter(ICharSequence::Probe(content), start);
    } else {
        end = TextUtils::GetOffsetBefore(ICharSequence::Probe(content), start);
    }
    if (start != end) {
        content->Delete(Elastos::Core::Math::Min(start, end), Elastos::Core::Math::Max(start, end));
        return TRUE;
    }
    return FALSE;
}

Boolean BaseKeyListener::DeleteSelection(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content)
{
    AutoPtr<ISelection> sel;
    CSelection::AcquireSingleton((ISelection**)&sel);
    Int32 selectionStart = 0,  selectionEnd = 0;
    sel->GetSelectionStart(ICharSequence::Probe(content), &selectionStart);
    sel->GetSelectionEnd(ICharSequence::Probe(content), &selectionEnd);
    if (selectionEnd < selectionStart) {
        Int32 temp = selectionEnd;
        selectionEnd = selectionStart;
        selectionStart = temp;
    }
    if (selectionStart != selectionEnd) {
        content->Delete(selectionStart, selectionEnd);
        return TRUE;
    }
    return FALSE;
}

Boolean BaseKeyListener::DeleteLine(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content)
{
    AutoPtr<ITextView> textView = ITextView::Probe(view);
    if(textView!=NULL){
        AutoPtr<ILayout> layout;
        textView->GetLayout((ILayout**)&layout);
        if (layout != NULL) {
            Int32 line, selStart;
            AutoPtr<ISelection> sel;
            CSelection::AcquireSingleton((ISelection**)&sel);
            sel->GetSelectionStart(ICharSequence::Probe(content), &selStart);
            layout->GetLineForOffset(selStart, &line);
            Int32 start;
            layout->GetLineStart(line, &start);
            Int32 end;
            layout->GetLineEnd(line, &end);
            if (end != start) {
                content->Delete(start, end);
                return TRUE;
            }
        }
    }
    return FALSE;
}

ECode BaseKeyListener::MakeTextContentType(
    /* [in] */ Capitalize caps,
    /* [in] */ Boolean autoText,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 contentType = IInputType::TYPE_CLASS_TEXT;
    switch (caps) {
        case Capitalize_CHARACTERS:
            contentType |= IInputType::TYPE_TEXT_FLAG_CAP_CHARACTERS;
            break;
        case Capitalize_WORDS:
            contentType |= IInputType::TYPE_TEXT_FLAG_CAP_WORDS;
            break;
        case Capitalize_SENTENCES:
            contentType |= IInputType::TYPE_TEXT_FLAG_CAP_SENTENCES;
            break;
    }
    if (autoText) {
        contentType |= IInputType::TYPE_TEXT_FLAG_AUTO_CORRECT;
    }

    *ret = contentType;
    return NOERROR;
}

ECode BaseKeyListener::OnKeyDown(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    Boolean handled;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DEL:
            Backspace(view, content, keyCode, event, &handled);
            break;
        case IKeyEvent::KEYCODE_FORWARD_DEL:
            ForwardDelete(view, content, keyCode, event, &handled);
            break;
        default:
            handled = FALSE;
            break;
    }

    if (handled) {
        AdjustMetaAfterKeypress(ISpannable::Probe(content));
    }

    return MetaKeyKeyListener::OnKeyDown(view, content, keyCode, event, ret);
}

ECode BaseKeyListener::OnKeyOther(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 action, keyCode;
    if ((event->GetAction(&action), action) != IKeyEvent::ACTION_MULTIPLE
            || (event->GetKeyCode(&keyCode), keyCode) != IKeyEvent::KEYCODE_UNKNOWN) {
        // Not something we are interested in.
        *ret = FALSE;
        return NOERROR;
    }
    AutoPtr<ISelection> sel;
    CSelection::AcquireSingleton((ISelection**)&sel);
    Int32 selectionStart,  selectionEnd;
    sel->GetSelectionStart(ICharSequence::Probe(content), &selectionStart);
    sel->GetSelectionEnd(ICharSequence::Probe(content), &selectionEnd);
    if (selectionEnd < selectionStart) {
        Int32 temp = selectionEnd;
        selectionEnd = selectionStart;
        selectionStart = temp;
    }

    String text;
    event->GetCharacters(&text);
    if (text.IsNullOrEmpty()) {
        *ret = FALSE;
        return NOERROR;
    }

    AutoPtr<ICharSequence> cs;
    CString::New(text, (ICharSequence**)&cs);
    content->Replace(selectionStart, selectionEnd, cs);
    *ret = TRUE;
    return NOERROR;
}

ECode BaseKeyListener::ClearMetaKeyState(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 states)
{
    return MetaKeyKeyListener::ClearMetaKeyState(view, content, states);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
