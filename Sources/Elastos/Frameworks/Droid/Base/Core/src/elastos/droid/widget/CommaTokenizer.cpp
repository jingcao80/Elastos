#include "elastos/droid/widget/CommaTokenizer.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CommaTokenizer::CommaTokenizer()
{

}

Int32 CommaTokenizer::FindTokenStart(
        /* [in] */ ICharSequence* text,
        /* [in] */ [in] Int32 cursor)
{
    Int32 i = cursor;
    Char32 c = NULL;
    text->GetCharAt((i - 1), &c);
    while (i > 0 && c != ',') {
        i--;
    }
    while (i < cursor && c == '') {
        i++;
    }
    return i;
}

Int32 CommaTokenizer::FindTokenEnd(
        /* [in] */ ICharSequence* text,
        /* [in] */ [in] Int32 cursor)
{
    Int32 i = cursor;
    Int32 len = 0;
    text->GetLength(&len);

    Char32 c = '';
    text->GetCharAt(i, &c);
    while (i < len) {
        if (c == ',') {
            return i;
        } else {
            i++;
        }
    }
    return len;
}

AutoPtr<ICharSequence> CommaTokenizer::TerminateToken(
        /* [in] */ ICharSequence* text)
{
    Int32 i = 0;
    text->GetLength(&i);

    Char32 c = '';
    text->GetCharAt((i - 1), &c);
    while (i > 0 && c == '') {
        i--;
    }
    if(i > 0 && c == ',') {
        return text;
    } else {
        if(ISpanned::Probe(text)) {
            String str = String();
            text->ToString(str);
            str += ",";
            AutoPtr<ICharSequence> seq;
            FAIL_RETURN(CStringWrapper::New(str, (ICharSequence**)&seq));

            AutoPtr<ISpannableString> sp;
            FAIL_RETURN(CSpannableString::New(seq, (ISpannableString**)&sp));
            return sp;
        } else {
            String str = String();
            text->ToString(str);
            str += String(",");
            AutoPtr<ICharSequence> seq;
            FAIL_RETURN(CStringWrapper::New(str, (ICharSequence**)&seq));

            return seq;
        }
    }
}


}// namespace Widget
}// namespace Droid
}// namespace Elastos