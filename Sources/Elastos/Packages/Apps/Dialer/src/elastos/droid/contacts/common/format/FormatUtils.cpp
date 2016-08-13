
#include "Elastos.Droid.Text.h"
#include "elastos/droid/contacts/common/format/FormatUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/Character.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::CSpannableString;
using Elastos::Droid::Text::Style::IStyleSpan;
using Elastos::Droid::Text::Style::CStyleSpan;
using Elastos::Core::Character;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Format {

Int32 FormatUtils::OverlapPoint(
    /* [in] */ ICharArrayBuffer* buffer1,
    /* [in] */ ICharArrayBuffer* buffer2)
{
    if (buffer1 == NULL || buffer2 == NULL) {
        return -1;
    }
    AutoPtr<ArrayOf<Char32> > data1;
    buffer1->GetData((ArrayOf<Char32>**)&data1);
    Int32 sizeCopied1;
    buffer1->GetSizeCopied(&sizeCopied1);
    AutoPtr<ArrayOf<Char32> > charCopy1;
    Arrays::CopyOfRange(data1, 0, sizeCopied1, (ArrayOf<Char32>**)&charCopy1);
    AutoPtr<ArrayOf<Char32> > data2;
    buffer1->GetData((ArrayOf<Char32>**)&data2);
    Int32 sizeCopied2;
    buffer1->GetSizeCopied(&sizeCopied2);
    AutoPtr<ArrayOf<Char32> > charCopy2;
    Arrays::CopyOfRange(data2, 0, sizeCopied2, (ArrayOf<Char32>**)&charCopy2);
    return OverlapPoint(charCopy1, charCopy2);
}

Int32 FormatUtils::OverlapPoint(
    /* [in] */ const String& string1,
    /* [in] */ const String& string2)
{
    if (string1.IsNull() || string2.IsNull()) {
        return -1;
    }
    return OverlapPoint(string1.GetChars(), string2.GetChars());
}

Int32 FormatUtils::OverlapPoint(
    /* [in] */ ArrayOf<Char32>* array1,
    /* [in] */ ArrayOf<Char32>* array2)
{
    if (array1 == NULL || array2 == NULL) {
        return -1;
    }
    Int32 count1 = array1->GetLength();
    Int32 count2 = array2->GetLength();

    // Ignore matching tails of the two arrays.
    while (count1 > 0 && count2 > 0 && (*array1)[count1 - 1] == (*array2)[count2 - 1]) {
        count1--;
        count2--;
    }

    Int32 size = count2;
    for (Int32 i = 0; i < count1; i++) {
        if (i + size > count1) {
            size = count1 - i;
        }
        Int32 j;
        for (j = 0; j < size; j++) {
            if ((*array1)[i+j] != (*array2)[j]) {
                break;
            }
        }
        if (j == size) {
            return i;
        }
    }

    return -1;
}

AutoPtr<ICharSequence> FormatUtils::ApplyStyleToSpan(
    /* [in] */ Int32 style,
    /* [in] */ ICharSequence* input,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 flags)
{
    // Enforce bounds of the char sequence.
    start = Elastos::Core::Math::Max(0, start);
    Int32 len;
    input->GetLength(&len);
    end = Elastos::Core::Math::Min(len, end);
    AutoPtr<ISpannable> text;
    CSpannableString::New(input, (ISpannable**)&text);
    AutoPtr<IStyleSpan> styleSpan;
    CStyleSpan::New(style, (IStyleSpan**)&styleSpan);
    text->SetSpan(styleSpan, start, end, flags);
    return ICharSequence::Probe(text);
}

void FormatUtils::CopyToCharArrayBuffer(
    /* [in] */ const String& text,
    /* [in] */ ICharArrayBuffer* buffer)
{
    if (text != NULL) {
        AutoPtr<ArrayOf<Char32> > data;
        buffer->GetData((ArrayOf<Char32>**)&data);
        if (data == NULL || data->GetLength() < text.GetLength()) {
            buffer->SetData(text.GetChars());
        }
        else {
            data->Copy(0, text.GetChars(), 0, text.GetLength());
        }
        buffer->SetSizeCopied(text.GetLength());
    }
    else {
        buffer->SetSizeCopied(0);
    }
}

String FormatUtils::CharArrayBufferToString(
    /* [in] */ ICharArrayBuffer* buffer)
{
    AutoPtr<ArrayOf<Char32> > data;
    buffer->GetData((ArrayOf<Char32>**)&data);
    Int32 sizeCopied;
    buffer->GetSizeCopied(&sizeCopied);
    return String(*data, 0, sizeCopied);
}

Int32 FormatUtils::IndexOfWordPrefix(
    /* [in] */ ICharSequence* text,
    /* [in] */ const String& prefix)
{
    if (prefix.IsNull() || text == NULL) {
        return -1;
    }

    Int32 textLength;
    text->GetLength(&textLength);
    Int32 prefixLength = prefix.GetLength();

    if (prefixLength == 0 || textLength < prefixLength) {
        return -1;
    }

    Int32 i = 0;
    while (i < textLength) {
        // Skip non-word characters
        Char32 c;
        while (i < textLength && (text->GetCharAt(i, &c), !Character::IsLetterOrDigit(c))) {
            i++;
        }

        if (i + prefixLength > textLength) {
            return -1;
        }

        // Compare the prefixes
        Int32 j;
        for (j = 0; j < prefixLength; j++) {
            text->GetCharAt(i + j, &c);
            if (Character::ToUpperCase(c) != prefix.GetChar(j)) {
                break;
            }
        }
        if (j == prefixLength) {
            return i;
        }

        // Skip this word
        while (i < textLength && (text->GetCharAt(i, &c), Character::IsLetterOrDigit(c))) {
            i++;
        }
    }

    return -1;
}

} // Format
} // Common
} // Contacts
} // Droid
} // Elastos
