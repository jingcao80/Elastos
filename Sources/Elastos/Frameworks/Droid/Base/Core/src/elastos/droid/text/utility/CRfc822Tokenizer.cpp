
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/text/utility/CRfc822Tokenizer.h"
#include "elastos/droid/text/utility/CRfc822Token.h"


using Elastos::Droid::Text::Utility::CRfc822Token;
using Elastos::Core::CString;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CAR_INTERFACE_IMPL(CRfc822Tokenizer, Object, IRfc822Tokenizer)

CAR_OBJECT_IMPL(CRfc822Tokenizer)

ECode CRfc822Tokenizer::Tokenize(
    /* [in] */ ICharSequence* text,
    /* [in] */ ICollection* out)
{
    VALIDATE_NOT_NULL(text);
    VALIDATE_NOT_NULL(out);

    StringBuilder name;
    StringBuilder address;
    StringBuilder comment;

    Int32 i = 0;
    Int32 cursor;
    text->GetLength(&cursor);

    while (i < cursor) {
        Char32 c;
        text->GetCharAt(i, &c);

        if (c == ',' || c == ';') {
            i++;

            Char32 cc;
            while(i < cursor && (text->GetCharAt(i, &cc), cc) == ' ') {
                i++;
            }
            Crunch(name);
            if (address.GetLength() > 0) {
                AutoPtr<IRfc822Token> rfc822Token;
                CRfc822Token::New(name.ToString(), address.ToString(), comment.ToString(), (IRfc822Token**)&rfc822Token);
                out->Add(rfc822Token.Get());
            }
            else if (name.GetLength() > 0) {
                AutoPtr<IRfc822Token> rfc822Token;
                CRfc822Token::New(String(NULL), name.ToString(), comment.ToString(), (IRfc822Token**)&rfc822Token);
                out->Add(rfc822Token.Get());
            }

            name.Reset();
            address.Reset();
            comment.Reset();
        }
        else if (c == '"') {
            i++;

            while (i < cursor) {
                text->GetCharAt(i, &c);

                if (c == '"') {
                    i++;
                    break;
                }
                else if (c == '\\') {
                    if (i + 1 < cursor) {
                        Char32 cc;
                        name.AppendChar((text->GetCharAt(i + 1, &cc), cc));
                    }
                    i += 2;
                }
                else {
                    name.AppendChar(c);
                    i++;
                }
            }
        }
        else if (c == '(') {
            Int32 level = 1;
            i++;

            while (i < cursor && level > 0) {
                text->GetCharAt(i, &c);

                if (c == ')') {
                    if (level > 1) {
                        comment.AppendChar(c);
                    }

                    level--;
                    i++;
                }
                else if (c == '(') {
                    comment.AppendChar(c);
                    level++;
                    i++;
                }
                else if (c == '\\') {
                    if (i + 1 < cursor) {
                        Char32 cc;
                        comment.AppendChar((text->GetCharAt(i + 1, &cc), cc));
                    }
                    i += 2;
                }
                else {
                    comment.AppendChar(c);
                    i++;
                }
            }
        }
        else if (c == '<') {
            i++;

            while (i < cursor) {
                text->GetCharAt(i, &c);

                if (c == '>') {
                    i++;
                    break;
                }
                else {
                    address.AppendChar(c);
                    i++;
                }
            }
        }
        else if (c == ' ') {
            name.AppendChar('\0');
            i++;
        }
        else {
            name.AppendChar(c);
            i++;
        }
    }

    Crunch(name);

    if (address.GetLength() > 0) {
        AutoPtr<IRfc822Token> rfc822Token;
        CRfc822Token::New(name.ToString(), address.ToString(), comment.ToString(), (IRfc822Token**)&rfc822Token);
        out->Add(rfc822Token.Get());
    }
    else if (name.GetLength() > 0) {
        AutoPtr<IRfc822Token> rfc822Token;
        CRfc822Token::New(String(NULL), name.ToString(), comment.ToString(), (IRfc822Token**)&rfc822Token);
        out->Add(rfc822Token.Get());
    }
    return NOERROR;
}

ECode CRfc822Tokenizer::Tokenize(
    /* [in] */ ICharSequence* text,
    /* [out, callee] */ ArrayOf<IRfc822Token*>** ret)
{
    VALIDATE_NOT_NULL(ret);

    AutoPtr<ICollection> list;
    CArrayList::New((ICollection**)&list);

    FAIL_RETURN(Tokenize(text, list))

    Int32 size;
    list->GetSize(&size);

    AutoPtr<ArrayOf<IRfc822Token*> > array = ArrayOf<IRfc822Token*>::Alloc(size);

    AutoPtr<IIterator> it;
    list->GetIterator((IIterator**)&it);
    Boolean hasNext;
    Int32 i = 0;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        array->Set(i++, IRfc822Token::Probe(obj));
    }

    *ret = array.Get();
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CRfc822Tokenizer::Crunch(
    /* [in] */ StringBuilder& sb)
{
    Int32 i = 0;
    Int32 len = sb.GetLength();
    while (i < len) {
        Char32 c;
        sb.GetCharAt(i, &c);

        if (c == '\0') {
            if (i == 0 || i == len - 1 ||
                    (sb.GetCharAt(i-1)) == ' ' ||
                    (sb.GetCharAt(i-1)) == '\0' ||
                    (sb.GetCharAt(i+1)) == ' ' ||
                    (sb.GetCharAt(i+1)) == '\0') {
                sb.DeleteCharAt(i);
                len--;
            }
            else {
                i++;
            }
        }
        else {
            i++;
        }
    }

    for (i = 0; i < len; i++) {
        if ((sb.GetCharAt(i)) == '\0') {
            sb.SetCharAt(i, ' ');
        }
    }
    return NOERROR;
}

ECode CRfc822Tokenizer::FindTokenStart(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 cursor,
    /* [out] */ Int32* ret)
{
    /*
     * It's hard to search backward, so search forward until
     * we reach the cursor.
     */
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    VALIDATE_NOT_NULL(text);
    Int32 best = 0;
    Int32 i = 0;

    while (i < cursor) {
        FAIL_RETURN(FindTokenEnd(text, i, &i));

        if (i < cursor) {
            i++; // Skip terminating punctuation

            Char32 cc;
            while(i < cursor && (text->GetCharAt(i, &cc), cc) == ' '){
                i++;
            }

            if (i < cursor) {
                best = i;
            }
        }
    }

    *ret = best;
    return NOERROR;
}

ECode CRfc822Tokenizer::FindTokenEnd(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 cursor,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    VALIDATE_NOT_NULL(text);
    Int32 len;
    text->GetLength(&len);
    Int32 i = cursor;

    while (i < len) {
        Char32 c;
        text->GetCharAt(i, &c);

        if (c == ',' || c == ';') {
            *ret = i;
            return NOERROR;
        }
        else if (c == '"') {
            i++;

            while (i < len) {
                text->GetCharAt(i, &c);

                if (c == '"') {
                    i++;
                    break;
                }
                else if (c == '\\' && i + 1 < len) {
                    i += 2;
                }
                else {
                    i++;
                }
            }
        }
        else if (c == '(') {
            Int32 level = 1;
            i++;

            while (i < len && level > 0) {
                text->GetCharAt(i, &c);

                if (c == ')') {
                    level--;
                    i++;
                }
                else if (c == '(') {
                    level++;
                    i++;
                }
                else if (c == '\\' && i + 1 < len) {
                    i += 2;
                }
                else {
                    i++;
                }
            }
        }
        else if (c == '<') {
            i++;
            while (i < len) {
                text->GetCharAt(i, &c);

                if (c == '>') {
                    i++;
                    break;
                }
                else {
                    i++;
                }
            }
        }
        else {
            i++;
        }
    }

    *ret = i;
    return NOERROR;
}

ECode CRfc822Tokenizer::TerminateToken(
    /* [in] */ ICharSequence* text,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = NULL;
    VALIDATE_NOT_NULL(text);
    String strText;
    text->ToString(&strText);
    return CString::New(strText + String(", "), ret);
}

} // namespace Utility
} // namespace Text
} // namepsace Droid
} // namespace Elastos
