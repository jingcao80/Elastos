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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/method/QwertyKeyListener.h"
#include "elastos/droid/text/method/TextKeyListener.h"
#include "elastos/droid/text/method/CTextKeyListener.h"
#include "elastos/droid/text/method/CQwertyKeyListener.h"
#include "elastos/droid/text/SpannableStringInternal.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/text/CAutoText.h"
#include "elastos/droid/view/KeyEvent.h"
#include <elastos/core/Math.h>
#include <elastos/core/Character.h>
#include <stdio.h>
#include <elastos/core/StringUtils.h>

using namespace Elastos::Core;
using Elastos::Core::StringUtils;
using Elastos::Droid::Text::Selection;
using Elastos::Droid::Text::INoCopySpan;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::KeyEvent;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

const Int32 QwertyKeyListener::CAPITALIZELENGTH = 4;

AutoPtr<ArrayOf<IQwertyKeyListener*> > QwertyKeyListener::sInstance =
    ArrayOf<IQwertyKeyListener*>::Alloc(QwertyKeyListener::CAPITALIZELENGTH * 2);

AutoPtr<IQwertyKeyListener> QwertyKeyListener::sFullKeyboardInstance;

CAR_INTERFACE_IMPL_2(QwertyKeyListener::Replaced, Object, IReplacedSpan, INoCopySpan)

QwertyKeyListener::Replaced::Replaced()
{
}

QwertyKeyListener::Replaced::~Replaced()
{
}

ECode QwertyKeyListener::Replaced::constructor(
    /* [in] */ ArrayOf<Char32>* text)
{
    mText = text;
    return NOERROR;
}


static Boolean InitStaticPICKER_SETS()
{
    QwertyKeyListener::PICKER_SETS['A'] = String("\u00C0\u00C1\u00C2\u00C4\u00C6\u00C3\u00C5\u0104\u0100");
    QwertyKeyListener::PICKER_SETS['C'] = String("\u00C7\u0106\u010C");
    QwertyKeyListener::PICKER_SETS['D'] = String("\u010E");
    QwertyKeyListener::PICKER_SETS['E'] = String("\u00C8\u00C9\u00CA\u00CB\u0118\u011A\u0112");
    QwertyKeyListener::PICKER_SETS['G'] = String("\u011E");
    QwertyKeyListener::PICKER_SETS['L'] = String("\u0141");
    QwertyKeyListener::PICKER_SETS['I'] = String("\u00CC\u00CD\u00CE\u00CF\u012A\u0130");
    QwertyKeyListener::PICKER_SETS['N'] = String("\u00D1\u0143\u0147");
    QwertyKeyListener::PICKER_SETS['O'] = String("\u00D8\u0152\u00D5\u00D2\u00D3\u00D4\u00D6\u014C");
    QwertyKeyListener::PICKER_SETS['R'] = String("\u0158");
    QwertyKeyListener::PICKER_SETS['S'] = String("\u015A\u0160\u015E");
    QwertyKeyListener::PICKER_SETS['T'] = String("\u0164");
    QwertyKeyListener::PICKER_SETS['U'] = String("\u00D9\u00DA\u00DB\u00DC\u016E\u016A");
    QwertyKeyListener::PICKER_SETS['Y'] = String("\u00DD\u0178");
    QwertyKeyListener::PICKER_SETS['Z'] = String("\u0179\u017B\u017D");
    QwertyKeyListener::PICKER_SETS['a'] = String("\u00E0\u00E1\u00E2\u00E4\u00E6\u00E3\u00E5\u0105\u0101");
    QwertyKeyListener::PICKER_SETS['c'] = String("\u00E7\u0107\u010D");
    QwertyKeyListener::PICKER_SETS['d'] = String("\u010F");
    QwertyKeyListener::PICKER_SETS['e'] = String("\u00E8\u00E9\u00EA\u00EB\u0119\u011B\u0113");
    QwertyKeyListener::PICKER_SETS['g'] = String("\u011F");
    QwertyKeyListener::PICKER_SETS['i'] = String("\u00EC\u00ED\u00EE\u00EF\u012B\u0131");
    QwertyKeyListener::PICKER_SETS['l'] = String("\u0142");
    QwertyKeyListener::PICKER_SETS['n'] = String("\u00F1\u0144\u0148");
    QwertyKeyListener::PICKER_SETS['o'] = String("\u00F8\u0153\u00F5\u00F2\u00F3\u00F4\u00F6\u014D");
    QwertyKeyListener::PICKER_SETS['r'] = String("\u0159");
    QwertyKeyListener::PICKER_SETS['s'] = String("\u00A7\u00DF\u015B\u0161\u015F");
    QwertyKeyListener::PICKER_SETS['t'] = String("\u0165");
    QwertyKeyListener::PICKER_SETS['u'] = String("\u00F9\u00FA\u00FB\u00FC\u016F\u016B");
    QwertyKeyListener::PICKER_SETS['y'] = String("\u00FD\u00FF");
    QwertyKeyListener::PICKER_SETS['z'] = String("\u017A\u017C\u017E");
    // QwertyKeyListener::PICKER_SETS[CKeyCharacterMap::PICKER_DIALOG_INPUT] = String("\u2026\u00A5\u2022\u00AE\u00A9\u00B1[]{}\\|");
    QwertyKeyListener::PICKER_SETS['/'] = String("\\");

    // From packages/inputmethods/LatinIME/res/xml/kbd_symbols.xml

    QwertyKeyListener::PICKER_SETS['1'] = String("\u00b9\u00bd\u2153\u00bc\u215b");
    QwertyKeyListener::PICKER_SETS['2'] = String("\u00b2\u2154");
    QwertyKeyListener::PICKER_SETS['3'] = String("\u00b3\u00be\u215c");
    QwertyKeyListener::PICKER_SETS['4'] = String("\u2074");
    QwertyKeyListener::PICKER_SETS['5'] = String("\u215d");
    QwertyKeyListener::PICKER_SETS['7'] = String("\u215e");
    QwertyKeyListener::PICKER_SETS['0'] = String("\u207f\u2205");
    QwertyKeyListener::PICKER_SETS['$'] = String("\u00a2\u00a3\u20ac\u00a5\u20a3\u20a4\u20b1");
    QwertyKeyListener::PICKER_SETS['%'] = String("\u2030");
    QwertyKeyListener::PICKER_SETS['*'] = String("\u2020\u2021");
    QwertyKeyListener::PICKER_SETS['-'] = String("\u2013\u2014");
    QwertyKeyListener::PICKER_SETS['+'] = String("\u00b1");
    QwertyKeyListener::PICKER_SETS['('] = String("[{<");
    QwertyKeyListener::PICKER_SETS[')'] = String("]}>");
    QwertyKeyListener::PICKER_SETS['!'] = String("\u00a1");
    QwertyKeyListener::PICKER_SETS['"'] = String("\u201c\u201d\u00ab\u00bb\u02dd");
    QwertyKeyListener::PICKER_SETS['?'] = String("\u00bf");
    QwertyKeyListener::PICKER_SETS[','] = String("\u201a\u201e");

    // From packages/inputmethods/LatinIME/res/xml/kbd_symbols_shift.xml

    QwertyKeyListener::PICKER_SETS['='] = String("\u2260\u2248\u221e");
    QwertyKeyListener::PICKER_SETS['<'] = String("\u2264\u00ab\u2039");
    QwertyKeyListener::PICKER_SETS['>'] = String("\u2265\u00bb\u203a");
    return TRUE;
};

HashMap<Char32, String> QwertyKeyListener::PICKER_SETS;
Boolean QwertyKeyListener::sInitPickerSet = InitStaticPICKER_SETS();

QwertyKeyListener::QwertyKeyListener()
{}

QwertyKeyListener::~QwertyKeyListener()
{}

CAR_INTERFACE_IMPL(QwertyKeyListener, BaseKeyListener, IQwertyKeyListener)

ECode QwertyKeyListener::constructor(
    /* [in] */ Capitalize cap,
    /* [in] */ Boolean autotext)
{
    return constructor(cap, autotext, FALSE);
}

ECode QwertyKeyListener::constructor(
    /* [in] */ Capitalize cap,
    /* [in] */ Boolean autotext,
    /* [in] */ Boolean fullKeyboard)
{
    mAutoCap = cap;
    mAutoText = autotext;
    mFullKeyboard = fullKeyboard;
    return NOERROR;
}

ECode QwertyKeyListener::GetInstance(
    /* [in] */ Boolean autoText,
    /* [in] */ Capitalize cap,
    /* [out] */ IQwertyKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 off = cap * 2 + (autoText ? 1 : 0);

    if ((*sInstance)[off] == NULL) {
        AutoPtr<IQwertyKeyListener> listener;
        CQwertyKeyListener::New(cap, autoText, (IQwertyKeyListener**)&listener);
        sInstance->Set(off, listener);
    }

    *ret = (*sInstance)[off];
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

ECode QwertyKeyListener::GetInstanceForFullKeyboard(
    /* [out] */ IQwertyKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret)
    if (sFullKeyboardInstance == NULL) {
        CQwertyKeyListener::New(Capitalize_NONE, FALSE, TRUE, (IQwertyKeyListener**)&sFullKeyboardInstance);
    }

    *ret = sFullKeyboardInstance;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode QwertyKeyListener::GetInputType(
    /* [out] */ Int32* ret)
{
    return MakeTextContentType(mAutoCap, mAutoText, ret);
}

ECode QwertyKeyListener::OnKeyDown(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 selStart, selEnd;
    Int32 pref = 0;

    if (view != NULL) {
        AutoPtr<IContext> context;
        view->GetContext((IContext**)&context);
        AutoPtr<ITextKeyListener> listener;
        TextKeyListener::GetInstance((ITextKeyListener**)&listener);
        ((TextKeyListener*)listener.Get())->GetPrefs(context, &pref);
    }

    {
        Int32 a = Selection::GetSelectionStart(ICharSequence::Probe(content));
        Int32 b = Selection::GetSelectionEnd(ICharSequence::Probe(content));

        selStart = Elastos::Core::Math::Min(a, b);
        selEnd = Elastos::Core::Math::Max(a, b);

        if (selStart < 0 || selEnd < 0) {
            selStart = selEnd = 0;
            Selection::SetSelection(ISpannable::Probe(content), 0, 0);
        }
    }
    Int32 activeStart;
    ISpanned::Probe(content)->GetSpanStart(
        TextKeyListener::ACTIVE, &activeStart);
    Int32 activeEnd;
    ISpanned::Probe(content)->GetSpanEnd(
        TextKeyListener::ACTIVE, &activeEnd);

    // QWERTY keyboard normal case

    Int32 i;
    Int32 eMetaState;
    MetaKeyKeyListener::GetMetaState(ICharSequence::Probe(content), event, &eMetaState);
    event->GetUnicodeChar(eMetaState, &i);

    if (!mFullKeyboard) {
        Int32 count;
        event->GetRepeatCount(&count);
        if (count > 0 && selStart == selEnd && selStart > 0) {
            Char32 c;
            ICharSequence::Probe(content)->GetCharAt(selStart - 1, &c);

            if ((c == i || c == Character::ToUpperCase(i)) && view != NULL) {
                if (ShowCharacterPicker(view, content, c, FALSE, count)) {
                    ResetMetaState(ISpannable::Probe(content));
                    *ret = TRUE;
                    return NOERROR;
                }
            }
        }
    }

    if (i == IKeyCharacterMap::PICKER_DIALOG_INPUT) {
        if (view != NULL) {
            ShowCharacterPicker(view, content,
                IKeyCharacterMap::PICKER_DIALOG_INPUT, TRUE, 1);
        }
        ResetMetaState(ISpannable::Probe(content));
        *ret = TRUE;
        return NOERROR;
    }
    if (i == IKeyCharacterMap::HEX_INPUT) {
        Int32 start;

        if (selStart == selEnd) {
            start = selEnd;

            Char32 ch;
            while (start > 0 && selEnd - start < 4 &&
                Character::ToDigit((ICharSequence::Probe(content)->GetCharAt(start - 1, &ch), ch), 16) >= 0) {
                start--;
            }
        }
        else {
            start = selStart;
        }

        Int32 ch = -1;

        String hex;
        hex = TextUtils::Substring(ICharSequence::Probe(content), start, selEnd);
        ch = StringUtils::ParseInt32(hex, 16);

        if (ch >= 0) {
            selStart = start;
            Selection::SetSelection(ISpannable::Probe(content), selStart, selEnd);
            i = ch;
        }
        else {
            i = 0;
        }
    }
    Boolean bHasNoModifiers, bHasModifiers;
    if (i != 0) {
        Boolean dead = FALSE;

        if ((i & IKeyCharacterMap::COMBINING_ACCENT) != 0) {
            dead = TRUE;
            i = i & IKeyCharacterMap::COMBINING_ACCENT_MASK;
        }

        if (activeStart == selStart && activeEnd == selEnd) {
            Boolean replace = FALSE;

            if (selEnd - selStart - 1 == 0) {
                Char32 accent;
                ICharSequence::Probe(content)->GetCharAt(selStart, &accent);
                Int32 composed = KeyEvent::GetDeadChar(accent, i);

                if (composed != 0) {
                    i = composed;
                    replace = TRUE;
                    dead = FALSE;
                }
            }

            if (!replace) {
                Selection::SetSelection(ISpannable::Probe(content), selEnd);
                ISpannable::Probe(content)->RemoveSpan(TextKeyListener::ACTIVE);
                selStart = selEnd;
            }
        }
        Boolean bTmp = FALSE;
        if ((pref & TextKeyListener::AUTO_CAP) != 0 &&
            Character::IsLowerCase(i) &&
            (TextKeyListener::ShouldCap(mAutoCap, ICharSequence::Probe(content), selStart, &bTmp), bTmp)) {
            Int32 where;
            ISpanned::Probe(content)->GetSpanEnd(TextKeyListener::CAPPED, &where);
            Int32 flags;
            ISpanned::Probe(content)->GetSpanFlags(TextKeyListener::CAPPED, &flags);

            if (where == selStart && (((flags >> 16) & 0xFFFF) == i)) {
                ISpannable::Probe(content)->RemoveSpan(TextKeyListener::CAPPED);
            }
            else {
                flags = i << 16;
                i = Character::ToUpperCase(i);

                if (selStart == 0)
                    ISpannable::Probe(content)->SetSpan(TextKeyListener::CAPPED, 0, 0,
                                    ISpanned::SPAN_MARK_MARK | flags);
                else
                    ISpannable::Probe(content)->SetSpan(TextKeyListener::CAPPED,
                                    selStart - 1, selStart,
                                    ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE |
                                    flags);
            }
        }
        if (selStart != selEnd) {
            Selection::SetSelection(ISpannable::Probe(content), selEnd);
        }
        ISpannable::Probe(content)->SetSpan(OLD_SEL_START, selStart, selStart,
                        ISpanned::SPAN_MARK_MARK);

        String str("");
        str.Append(i);
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        content->Replace(selStart, selEnd, cs);

        Int32 oldStart;
        ISpanned::Probe(content)->GetSpanStart(OLD_SEL_START, &oldStart);
        selEnd = Selection::GetSelectionEnd(ICharSequence::Probe(content));
        if (oldStart < selEnd) {
            ISpannable::Probe(content)->SetSpan(TextKeyListener::LAST_TYPED,
                            oldStart, selEnd,
                            ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
            if (dead) {
                Selection::SetSelection(ISpannable::Probe(content), oldStart, selEnd);
                ISpannable::Probe(content)->SetSpan(TextKeyListener::ACTIVE, oldStart, selEnd,
                    ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
            }
        }

        AdjustMetaAfterKeypress(ISpannable::Probe(content));

        // potentially do autotext replacement if the character
        // that was typed was an autotext terminator

        Int32 end;
        if ((pref & TextKeyListener::AUTO_TEXT) != 0 && mAutoText &&
            (i == ' ' || i == '\t' || i == '\n' ||
             i == ',' || i == '.' || i == '!' || i == '?' ||
             i == '"' || Character::GetType(i) == Character::END_PUNCTUATION) &&
             (ISpanned::Probe(content)->GetSpanEnd(TextKeyListener::INHIBIT_REPLACEMENT, &end), end)
             != oldStart) {
            Int32 x;

            for (x = oldStart; x > 0; x--) {
                Char32 c;
                ICharSequence::Probe(content)->GetCharAt(x - 1, &c);
                if (c != '\'' && !Character::IsLetter(c)) {
                    break;
                }
            }

            String rep = GetReplacement(ICharSequence::Probe(content), x, oldStart, view);

            if (!rep.IsNull()) {
                AutoPtr<ArrayOf<IInterface*> > repl;
                Int32 len;
                ICharSequence::Probe(content)->GetLength(&len);
                ISpanned::Probe(content)->GetSpans(0, len, EIID_IReplacedSpan, (ArrayOf<IInterface*>**)&repl);
                for (Int32 a = 0; a < repl->GetLength(); a++)
                    ISpannable::Probe(content)->RemoveSpan((*repl)[a]);

                AutoPtr<ArrayOf<Char32> > orig = ArrayOf<Char32>::Alloc((oldStart - x) * 4);
                //TODO
                TextUtils::GetChars(ICharSequence::Probe(content), x, oldStart, (ArrayOf<Char32>*)orig.Get(), 0);

                AutoPtr<Replaced> r = new Replaced();
                r->constructor(orig.Get());
                ISpannable::Probe(content)->SetSpan((INoCopySpan*)r, x, oldStart,
                                ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
                AutoPtr<ICharSequence> cs;
                CString::New(rep, (ICharSequence**)&cs);
                content->Replace(x, oldStart, cs);
            }
        }

        // Replace two spaces by a period and a space.

        if ((pref & TextKeyListener::AUTO_PERIOD) != 0 && mAutoText) {
            selEnd = Selection::GetSelectionEnd(ICharSequence::Probe(content));
            if (selEnd - 3 >= 0) {
                Char32 ch;
                if ((ICharSequence::Probe(content)->GetCharAt(selEnd - 1, &ch), ch) == ' ' &&
                    (ICharSequence::Probe(content)->GetCharAt(selEnd - 2, &ch), ch) == ' ') {
                    Char32 c;
                    ICharSequence::Probe(content)->GetCharAt(selEnd - 3, &c);

                    for (Int32 j = selEnd - 3; j > 0; j--) {
                        if (c == '"' ||
                            Character::GetType(c) == Character::END_PUNCTUATION) {
                            ICharSequence::Probe(content)->GetCharAt(j - 1, &c);
                        }
                        else {
                            break;
                        }
                    }

                    if (Character::IsLetter(c) || Character::IsDigit(c)) {
                        AutoPtr<ICharSequence> cs;
                        CString::New(String("."), (ICharSequence**)&cs);
                        content->Replace(selEnd - 2, selEnd - 1, cs);
                    }
                }
            }
        }

        *ret = TRUE;
        return NOERROR;
    }
    else if ((keyCode == IKeyEvent::KEYCODE_DEL && (event->HasNoModifiers(&bHasNoModifiers), bHasNoModifiers)) ||
        ((event->HasModifiers(IKeyEvent::META_ALT_ON, &bHasModifiers), bHasModifiers) && selStart == selEnd)) {
        // special backspace case for undoing autotext

        Int32 consider = 1;

        // if backspacing over the last typed character,
        // it undoes the autotext prior to that character
        // (unless the character typed was newline, in which
        // case this behavior would be confusing)

        Int32 end;
        ISpanned::Probe(content)->GetSpanEnd(TextKeyListener::LAST_TYPED, &end);
        if (end == selStart) {
            Char32 c;
            if ((ICharSequence::Probe(content)->GetCharAt(selStart - 1, &c), c) != '\n')
                consider = 2;
        }

        AutoPtr<ArrayOf<IInterface*> > repl = NULL;
        ISpanned::Probe(content)->GetSpans(
            selStart - consider, selStart, EIID_IReplacedSpan, (ArrayOf<IInterface*>**)&repl);

        if (repl->GetLength() > 0) {
            Int32 st;
            ISpanned::Probe(content)->GetSpanStart((*repl)[0], &st);
            Int32 en;
            ISpanned::Probe(content)->GetSpanEnd((*repl)[0], &en);
            Replaced* replaced = (Replaced*)IObject::Probe((*repl)[0]);
            String old(*(replaced->mText));

            ISpannable::Probe(content)->RemoveSpan((*repl)[0]);

            // only cancel the autocomplete if the cursor is at the end of
            // the replaced span (or after it, because the user is
            // backspacing over the space after the word, not the word
            // itself).
            if (selStart >= en) {
                ISpannable::Probe(content)->SetSpan(TextKeyListener::INHIBIT_REPLACEMENT,
                                en, en, ISpanned::SPAN_POINT_POINT);
                AutoPtr<ICharSequence> oldCs;
                CString::New(old, (ICharSequence**)&oldCs);
                content->Replace(st, en, oldCs);

                ISpanned::Probe(content)->GetSpanStart(TextKeyListener::INHIBIT_REPLACEMENT, &en);
                if (en - 1 >= 0) {
                    ISpannable::Probe(content)->SetSpan(
                        TextKeyListener::INHIBIT_REPLACEMENT,
                        en - 1, en, ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
                }
                else {
                    ISpannable::Probe(content)->RemoveSpan(TextKeyListener::INHIBIT_REPLACEMENT);
                }
                AdjustMetaAfterKeypress(ISpannable::Probe(content));
            }
            else {
                AdjustMetaAfterKeypress(ISpannable::Probe(content));
                return BaseKeyListener::OnKeyDown(view, content, keyCode, event, ret);
            }

            *ret = TRUE;
            return NOERROR;
        }
    }
    return BaseKeyListener::OnKeyDown(view, content, keyCode, event, ret);
}

String QwertyKeyListener::GetReplacement(
    /* [in] */ ICharSequence* src,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IView* view)
{
    Int32 len = end - start;
    Boolean changecase = FALSE;

    String replacement = CAutoText::Get(src, start, end, view);

    if (replacement.IsNull()) {
        String key;
        key = TextUtils::Substring(src, start, end);
        key = key.ToLowerCase();
        AutoPtr<ICharSequence> csKey;
        CString::New(key, (ICharSequence**)&csKey);
        replacement = CAutoText::Get(csKey, 0, end - start, view);
        changecase = TRUE;

        if (replacement.IsNull())
            return String(NULL);
    }

    Int32 caps = 0;

    if (changecase) {
        for (Int32 j = start; j < end; j++) {
            Char32 c;
            src->GetCharAt(j, &c);
            if (Character::IsUpperCase(c))
                caps++;
        }
    }

    String out;

    if (caps == 0)
        out = replacement;
    else if (caps == 1)
        out = ToTitleCase(replacement);
    else if (caps == len)
        out = replacement.ToUpperCase();
    else
        out = ToTitleCase(replacement);

    AutoPtr<ICharSequence> cs;
    CString::New(out, (ICharSequence**)&cs);
    if (out.GetLength() == len &&
        TextUtils::RegionMatches(src, start, cs, 0, len))
        return String(NULL);

    return out;
}

/**
 * Marks the specified region of <code>content</code> as having
 * contained <code>original</code> prior to AutoText replacement.
 * Call this method when you have done or are about to do an
 * AutoText-style replacement on a region of text and want to let
 * the same mechanism (the user pressing DEL immediately after the
 * change) undo the replacement.
 *
 * @param content the Editable text where the replacement was made
 * @param start the start of the replaced region
 * @param end the end of the replaced region; the location of the cursor
 * @param original the text to be restored if the user presses DEL
 */
ECode QwertyKeyListener::MarkAsReplaced(
    /* [in] */ ISpannable* content,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const String& original)
{
    AutoPtr<ArrayOf<IInterface*> > repl;
    Int32 len;
    ICharSequence::Probe(content)->GetLength(&len);
    ISpanned::Probe(content)->GetSpans(0, len, EIID_IReplacedSpan, (ArrayOf<IInterface*>**)&repl);
    for (Int32 a = 0; a < repl->GetLength(); a++) {
        content->RemoveSpan((*repl)[a]);
    }

    len = original.GetByteLength();
    AutoPtr<ArrayOf<Char32> > orig = ArrayOf<Char32>::Alloc(len);
    memcpy(orig->GetPayload(), original.string(), len);

    AutoPtr<Replaced> r = new Replaced();
    r->constructor(orig.Get());
    content->SetSpan(INoCopySpan::Probe(r), start, end, ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
    return NOERROR;
}

Boolean QwertyKeyListener::ShowCharacterPicker(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Char32 c,
    /* [in] */ Boolean insert,
    /* [in] */ Int32 count)
{
    HashMap<Char32, String>::Iterator iter = PICKER_SETS.Find(c);
    HashMap<Char32, String>::ValueType value = *iter;
    String set = value.mSecond;
    if (set == NULL) {
       return FALSE;
    }

    if (count == 1) {
//       new CharacterPickerDialog(view.getContext(),
//                                 view, content, set, insert).show();
    }

    return TRUE;
}

String QwertyKeyListener::ToTitleCase(
    /* [in] */ const String& src)
{
    return src.ToUpperCase(0, 1);
}

ECode QwertyKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    return MetaKeyKeyListener::OnKeyUp(view, content, keyCode, event, ret);
}

ECode QwertyKeyListener::ClearMetaKeyState(
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
