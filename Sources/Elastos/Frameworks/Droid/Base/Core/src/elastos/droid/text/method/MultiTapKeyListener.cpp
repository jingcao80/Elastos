
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/text/method/MultiTapKeyListener.h"
#include "elastos/droid/text/method/CMultiTapKeyListener.h"
#include "elastos/droid/text/method/CTextKeyListener.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Math.h>
#include <elastos/core/Character.h>

using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::Method::CTextKeyListener;
using Elastos::Droid::Text::Selection;
using Elastos::Core::Character;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {
/*****************************MultiTapKeyListener::Timeout*****************************/

MultiTapKeyListener::Timeout::Timeout(
    /* [in] */ IEditable* buffer)
{
    mBuffer = buffer;
    Int32 bufLen;
    ICharSequence::Probe(mBuffer)->GetLength(&bufLen);
    ISpannable::Probe(mBuffer)->SetSpan((IInterface*)(IRunnable*)this, 0, bufLen, ISpanned::SPAN_INCLUSIVE_INCLUSIVE);
    Boolean result;
    PostAtTime(this, SystemClock::GetUptimeMillis() + 2000, &result);
}

CAR_INTERFACE_IMPL_2(MultiTapKeyListener::Timeout, Object, IRunnable, IHandler)

ECode MultiTapKeyListener::Timeout::Run()
{
    AutoPtr<ISpannable> buf = ISpannable::Probe(mBuffer);

    if (buf != NULL) {

        Int32 st;
        st = Selection::GetSelectionStart(ICharSequence::Probe(buf));
        Int32 en;
        en = Selection::GetSelectionEnd(ICharSequence::Probe(buf));

        Int32 start;
        ISpanned::Probe(buf)->GetSpanStart((CTextKeyListener::ACTIVE).Get(), &start);
        Int32 end;
        ISpanned::Probe(buf)->GetSpanEnd((CTextKeyListener::ACTIVE).Get(), &end);

        if (st == start && en == end) {
            en = Selection::GetSelectionEnd(ICharSequence::Probe(buf));
            Selection::SetSelection(ISpannable::Probe(buf), en);
        }

        ISpannable::Probe(buf)->RemoveSpan((IInterface*)(IRunnable*)this);
    }
    return NOERROR;
}

/*****************************MultiTapKeyListener*****************************/
static AutoPtr<HashMap<Int32, String> > InitStaticRecs()
{
    AutoPtr<HashMap<Int32, String> > ret = new HashMap<Int32, String>();
    (*ret)[IKeyEvent::KEYCODE_1] = ".,1!@#$%^&*:/?'=()";
    (*ret)[IKeyEvent::KEYCODE_2] = "abc2ABC";
    (*ret)[IKeyEvent::KEYCODE_3] = "def3DEF";
    (*ret)[IKeyEvent::KEYCODE_4] = "ghi4GHI";
    (*ret)[IKeyEvent::KEYCODE_5] = "jkl5JKL";
    (*ret)[IKeyEvent::KEYCODE_6] = "mno6MNO";
    (*ret)[IKeyEvent::KEYCODE_7] = "pqrs7PQRS";
    (*ret)[IKeyEvent::KEYCODE_8] = "tuv8TUV";
    (*ret)[IKeyEvent::KEYCODE_9] = "wxyz9WXYZ";
    (*ret)[IKeyEvent::KEYCODE_0] = "0+";
    (*ret)[IKeyEvent::KEYCODE_POUND] = " ";
    return ret;
}

AutoPtr<HashMap<Int32, String> > MultiTapKeyListener::sRecs = InitStaticRecs();

const Int32 MultiTapKeyListener::CAPITALIZELENGTH = 4;

AutoPtr<ArrayOf<IMultiTapKeyListener*> > MultiTapKeyListener::sInstance = ArrayOf<IMultiTapKeyListener*>::Alloc(MultiTapKeyListener::CAPITALIZELENGTH * 2);

MultiTapKeyListener::MultiTapKeyListener()
{}

MultiTapKeyListener::~MultiTapKeyListener()
{}

CAR_INTERFACE_IMPL(MultiTapKeyListener, Object, IMultiTapKeyListener)

ECode MultiTapKeyListener::constructor(
    /* [in] */ Capitalize cap,
    /* [in] */ Boolean autotext)
{
    mCapitalize = cap;
    mAutoText = autotext;
    return NOERROR;
}

ECode MultiTapKeyListener::GetInstance(
    /* [in] */ Boolean autotext,
    /* [in] */ Capitalize cap,
    /* [out] */ IMultiTapKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 off = cap * 2 + (autotext ? 1 : 0);

    if ((*sInstance)[off] == NULL) {
        AutoPtr<IMultiTapKeyListener> listener;
        CMultiTapKeyListener::New(cap, autotext, (IMultiTapKeyListener**)&listener);
        sInstance->Set(off, listener);
    }

    *ret = (*sInstance)[off];
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

ECode MultiTapKeyListener::GetInputType(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    return MakeTextContentType(mCapitalize, mAutoText, ret);
}

ECode MultiTapKeyListener::OnKeyDown(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    Int32 selStart, selEnd;
    Int32 pref = 0;

    if (view != NULL) {
        AutoPtr<IContext> contextT;
        view->GetContext((IContext**)&contextT);
        AutoPtr<ITextKeyListener> tkl;
        TextKeyListener::GetInstance((ITextKeyListener**)&tkl);
        ((TextKeyListener*)tkl.Get())->GetPrefs(contextT.Get(), &pref);
    }

    {
        Int32 a;
        a = Selection::GetSelectionStart(ICharSequence::Probe(content));
        Int32 b;
        b = Selection::GetSelectionEnd(ICharSequence::Probe(content));

        selStart = Elastos::Core::Math::Min(a, b);
        selEnd = Elastos::Core::Math::Max(a, b);
    }

    Int32 activeStart;
    ISpanned::Probe(content)->GetSpanStart(CTextKeyListener::ACTIVE, &activeStart);
    Int32 activeEnd;
    ISpanned::Probe(content)->GetSpanEnd(CTextKeyListener::ACTIVE, &activeEnd);

    // now for the multitap cases...

    // Try to increment the character we were working on before
    // if we have one and it's still the same key.

    Int32 flagsT;
    ISpanned::Probe(content)->GetSpanFlags(CTextKeyListener::ACTIVE, &flagsT);

    //Java:    int rec = (content.getSpanFlags(TextKeyListener.ACTIVE) & Spannable.SPAN_USER) >>> Spannable.SPAN_USER_SHIFT;
    Int32 rec = (flagsT & ISpanned::SPAN_USER)/ ((Int32)(Elastos::Core::Math::Pow(2, ISpanned::SPAN_USER_SHIFT)));

    if (activeStart == selStart && activeEnd == selEnd &&
            selEnd - selStart == 1 &&
            rec >= 0 && rec < sRecs->GetSize()) {
        if (keyCode == IKeyEvent::KEYCODE_STAR) {
            Char32 current;
            ICharSequence::Probe(content)->GetCharAt(selStart, &current);

            if (Character::IsLowerCase(current)) {
                String strT = String((char*)&current,1);
                AutoPtr<ICharSequence> cs;
                CString::New(strT.ToUpperCase(), (ICharSequence**)&cs);
                content->Replace(selStart, selEnd , cs.Get());
                RemoveTimeouts(ISpannable::Probe(content));
                AutoPtr<Timeout> t = new Timeout(content); // for its side effects
                *ret = TRUE;
                return NOERROR;
            }
            if (Character::IsUpperCase(current)) {
                String strT = String((char*)&current,1);
                AutoPtr<ICharSequence> cs;
                CString::New(strT.ToLowerCase(), (ICharSequence**)&cs);
                content->Replace(selStart, selEnd, cs.Get());
                RemoveTimeouts(ISpannable::Probe(content));
                AutoPtr<Timeout> t = new Timeout(content); // for its side effects
                *ret = TRUE;
                return NOERROR;
            }
        }
        //Java:    if (sRecs.indexOfKey(keyCode) == rec)
        if (((keyCode < 8 ? keyCode + 10 : keyCode) - 8) == rec)
        {
            //Java:    String val = sRecs.valueAt(rec);
            HashMap<Int32, String>::Iterator iterRecs = sRecs->Find(keyCode);
            String val = iterRecs->mSecond;
            Char32 ch;
            ICharSequence::Probe(content)->GetCharAt(selStart, &ch);
            Int32 ix = val.IndexOf(ch);

            if (ix >= 0) {
                ix = (ix + 1) % (val.GetLength());
                AutoPtr<ICharSequence> cs;
                CString::New(val, (ICharSequence**)&cs);
                content->Replace(selStart, selEnd, cs, ix, ix + 1);
                RemoveTimeouts(ISpannable::Probe(content));
                AutoPtr<Timeout> t = new Timeout(content); // for its side effects
                *ret = TRUE;
                return NOERROR;
            }
        }

        // Is this key one we know about at all?  If so, acknowledge
        // that the selection is our fault but the key has changed
        // or the text no longer matches, so move the selection over
        // so that it inserts instead of replaces.

        //Java:    rec = sRecs.indexOfKey(keyCode);
        rec = ((keyCode < 8 ? keyCode + 10 : keyCode) - 8);

        if (rec >= 0) {
            Selection::SetSelection(ISpannable::Probe(content), selEnd, selEnd);
            selStart = selEnd;
        }
    }
    else {
        //Java:    rec = sRecs.indexOfKey(keyCode);
        rec = ((keyCode < 8 ? keyCode + 10 : keyCode) - 8);
    }

    if (rec >= 0) {
        // We have a valid key.  Replace the selection or insertion point
        // with the first character for that key, and remember what
        // record it came from for next time.
        //Java:    String val = sRecs.valueAt(rec);
        HashMap<Int32, String>::Iterator iterRecs = (*sRecs).Find(keyCode);
        String val = (*iterRecs).mSecond;

        Int32 off = 0;
        Boolean flag = FALSE;
        if ((pref & CTextKeyListener::AUTO_CAP) != 0 &&
                (TextKeyListener::ShouldCap(mCapitalize, ICharSequence::Probe(content), selStart, &flag), flag)) {
            AutoPtr<ArrayOf<Char32> > chars = val.GetChars();
            for (Int32 i = 0; i < chars->GetLength(); i++) {
                if (Character::IsUpperCase((*chars)[i])) {
                    off = i;
                    break;
                }
            }
        }

        if (selStart != selEnd) {
            Selection::SetSelection(ISpannable::Probe(content), selEnd);
        }

        ISpannable::Probe(content)->SetSpan(OLD_SEL_START, selStart, selStart, ISpanned::SPAN_MARK_MARK);

        AutoPtr<ICharSequence> cs;
        CString::New(val, (ICharSequence**)&cs);
        content->Replace(selStart, selEnd, cs.Get(), off, off + 1);

        Int32 oldStart;
        ISpanned::Probe(content)->GetSpanStart(OLD_SEL_START, &oldStart);
        selEnd = Selection::GetSelectionEnd(ICharSequence::Probe(content));

        if (selEnd != oldStart) {
            Selection::SetSelection(ISpannable::Probe(content), oldStart, selEnd);

            ISpannable::Probe(content)->SetSpan(CTextKeyListener::LAST_TYPED,
                                oldStart, selEnd,
                                ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);

            ISpannable::Probe(content)->SetSpan(CTextKeyListener::ACTIVE,
                            oldStart, selEnd,
                            ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE |
                            (rec << ISpanned::SPAN_USER_SHIFT));

        }

        RemoveTimeouts(ISpannable::Probe(content));
        AutoPtr<Timeout> t = new Timeout(content); // for its side effects
        *ret = TRUE;
        return NOERROR;

        // Set up the callback so we can remove the timeout if the
        // cursor moves.

        Int32 spanStart;
        if ((ISpanned::Probe(content)->GetSpanStart((IInterface*)(IMultiTapKeyListener*)this, &spanStart), spanStart) < 0) {
            AutoPtr< ArrayOf<IKeyListener*> > methods;
            Int32 contentLen;
            ISpanned::Probe(content)->GetSpans(0, (ICharSequence::Probe(content)->GetLength(&contentLen), contentLen), EIID_IKeyListener, (ArrayOf<IInterface*>**)&methods );

            Int32 aryLen = methods->GetLength();
            for(Int32 i =0; i < aryLen; i++) {
                AutoPtr<IKeyListener> method = (*methods)[i];
                ISpannable::Probe(content)->RemoveSpan((IInterface*)(method.Get()));
            }
            ISpannable::Probe(content)->SetSpan((IInterface*)(IMultiTapKeyListener*)this, 0, (ICharSequence::Probe(content)->GetLength(&contentLen), contentLen), ISpanned::SPAN_INCLUSIVE_INCLUSIVE);
        }

        *ret = TRUE;
        return NOERROR;
    }

    return BaseKeyListener::OnKeyDown(view, content, keyCode, event, ret);
}

ECode MultiTapKeyListener::OnSpanChanged(
    /* [in] */ ISpannable* buf,
    /* [in] */ IInterface* what,
    /* [in] */ Int32 s,
    /* [in] */ Int32 e,
    /* [in] */ Int32 start,
    /* [in] */ Int32 stop)
{
    AutoPtr<IInterface> end;
    end = Selection::SELECTION_END;

    if (what == end) {
        ISpannable::Probe(buf)->RemoveSpan(CTextKeyListener::ACTIVE);
        RemoveTimeouts(ISpannable::Probe(buf));
    }
    return NOERROR;
}

void MultiTapKeyListener::RemoveTimeouts(
    /* [in] */ ISpannable* buf)
{
    Int32 bufLen;
    ICharSequence::Probe(buf)->GetLength(&bufLen);
    AutoPtr< ArrayOf<IRunnable*> > timeout;
    ISpanned::Probe(buf)->GetSpans(0, bufLen, EIID_IRunnable, (ArrayOf<IInterface*>**)&timeout);

    for (Int32 i = 0; i < timeout->GetLength(); i++) {
        AutoPtr<Timeout> t = (Timeout*)((*timeout)[i]);
        t->RemoveCallbacks(t);

        t->mBuffer = NULL;
        ISpannable::Probe(buf)->RemoveSpan((IInterface*)(IRunnable*)(t.Get()));
    }
}

ECode MultiTapKeyListener::OnSpanAdded(
    /* [in] */ ISpannable* s,
    /* [in] */ IInterface* what,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{return NOERROR;}

ECode MultiTapKeyListener::OnSpanRemoved(
    /* [in] */ ISpannable* s,
    /* [in] */ IInterface* what,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{return NOERROR;}

//override
ECode MultiTapKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    return MetaKeyKeyListener::OnKeyUp(view, content, keyCode, event, ret);
}

CARAPI MultiTapKeyListener::ClearMetaKeyState(
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
