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

#include "elastos/droid/contacts/common/format/TextHighlighter.h"
#include "elastos/droid/contacts/common/format/FormatUtils.h"
#include <elastos/core/Character.h>

using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::CSpannableString;
using Elastos::Droid::Text::Style::CStyleSpan;
using Elastos::Core::Character;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Format {

const String TextHighlighter::TAG("TextHighlighter");
const Boolean TextHighlighter::DEBUG;

TextHighlighter::TextHighlighter(
    /* [in] */ Int32 textStyle)
    : mTextStyle(textStyle)
{
    mTextStyleSpan = GetStyleSpan();
}

void TextHighlighter::SetPrefixText(
    /* [in] */ ITextView* view,
    /* [in] */ const String& text,
    /* [in] */ const String& prefix)
{
    AutoPtr<ICharSequence> cs;
    CString::New(text, (ICharSequence**)&cs);
    view->SetText(ApplyPrefixHighlight(cs, prefix));
}

AutoPtr<ICharacterStyle> TextHighlighter::GetStyleSpan()
{
    AutoPtr<ICharacterStyle> style;
    CStyleSpan::New(mTextStyle, (ICharacterStyle**)&style);
    return style;
}

void TextHighlighter::ApplyMaskingHighlight(
    /* [in] */ ISpannableString* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    /** Sets text color of the masked locations to be highlighted. */
    ISpannable::Probe(text)->SetSpan(GetStyleSpan(), start, end, 0);
}

AutoPtr<ICharSequence> TextHighlighter::ApplyPrefixHighlight(
    /* [in] */ ICharSequence* text,
    /* [in] */ const String& prefix)
{
    if (prefix == NULL) {
        return text;
    }

    // Skip non-word characters at the beginning of prefix.
    Int32 prefixStart = 0;
    while (prefixStart < prefix.GetLength() &&
            !Character::IsLetterOrDigit(prefix.GetChar(prefixStart))) {
        prefixStart++;
    }
    String trimmedPrefix = prefix.Substring(prefixStart);

    Int32 index = FormatUtils::IndexOfWordPrefix(text, trimmedPrefix);
    if (index != -1) {
        AutoPtr<ISpannable> result;
        CSpannableString::New(text, (ISpannable**)&result);
        result->SetSpan(mTextStyleSpan, index, index + trimmedPrefix.GetLength(), 0 /* flags */);
        return ICharSequence::Probe(result);
    }
    else {
        return text;
    }
}

} // Format
} // Common
} // Contacts
} // Droid
} // Elastos
