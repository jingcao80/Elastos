
#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/CAndroidCharacter.h"
#include "elastos/droid/text/MeasuredText.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/CSpannedString.h"
#include "elastos/droid/text/CSpannableString.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/CAnnotation.h"
#include "elastos/droid/text/style/CAlignmentSpanStandard.h"
#include "elastos/droid/text/style/CForegroundColorSpan.h"
#include "elastos/droid/text/style/CRelativeSizeSpan.h"
#include "elastos/droid/text/style/CScaleXSpan.h"
#include "elastos/droid/text/style/CStrikethroughSpan.h"
#include "elastos/droid/text/style/CUnderlineSpan.h"
#include "elastos/droid/text/style/CStyleSpan.h"
#include "elastos/droid/text/style/CBulletSpan.h"
#include "elastos/droid/text/style/CQuoteSpan.h"
#include "elastos/droid/text/style/CLeadingMarginSpanStandard.h"
#include "elastos/droid/text/style/CURLSpan.h"
#include "elastos/droid/text/style/CBackgroundColorSpan.h"
#include "elastos/droid/text/style/CTypefaceSpan.h"
#include "elastos/droid/text/style/CSuperscriptSpan.h"
#include "elastos/droid/text/style/CSubscriptSpan.h"
#include "elastos/droid/text/style/CAbsoluteSizeSpan.h"
#include "elastos/droid/text/style/CTextAppearanceSpan.h"
#include "elastos/droid/text/style/CSuggestionSpan.h"
#include "elastos/droid/text/style/CSpellCheckSpan.h"
#include "elastos/droid/text/style/CSuggestionRangeSpan.h"
#include "elastos/droid/text/style/CEasyEditSpan.h"
#include "elastos/droid/text/style/CLocaleSpan.h"
#include "elastos/droid/text/style/CTtsSpan.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/R.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include <elastos/core/Character.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using namespace Elastos::Droid::Text::Style;
using Elastos::Droid::Text::TextUtilsTruncateAt_START;
using Elastos::Droid::Text::TextUtilsTruncateAt_END;
using Elastos::Droid::Text::TextUtilsTruncateAt_END_SMALL;
using Elastos::Droid::Text::TextUtilsTruncateAt_MIDDLE;
using Elastos::Droid::R;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::View::IView;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Libcore::ICU::IICUUtil;
using Libcore::ICU::CICUUtil;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::Character;
using Elastos::Core::CString;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::IStringBuffer;
using Elastos::Core::IStringBuilder;
using Elastos::Core::EIID_ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

const Char32 TextUtils::FIRST_RIGHT_TO_LEFT;
const Char32 TextUtils::ZWNBS_CHAR;

Object TextUtils::sLock;
AutoPtr< ArrayOf<Char32> > TextUtils::sTemp;
AutoPtr< ArrayOf<String> > TextUtils::EMPTY_STRING_ARRAY = ArrayOf<String>::Alloc(0);
String TextUtils::ARAB_SCRIPT_SUBTAG("Arab");
String TextUtils::HEBR_SCRIPT_SUBTAG("Hebr");
const String TextUtils::TAG("TextUtils");


//===========================================================================
// TextUtils::Reverser
//===========================================================================

CAR_INTERFACE_IMPL_2(TextUtils::Reverser, Object, ICharSequence, IGetChars)

TextUtils::Reverser::Reverser(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
    : mSource(source)
    , mStart(start)
    , mEnd(end)
{
}

ECode TextUtils::Reverser::GetLength(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len)
    *len = mEnd - mStart;
    return NOERROR;
}

ECode TextUtils::Reverser::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq)
    AutoPtr<ArrayOf<Char32> > buf = ArrayOf<Char32>::Alloc(end - start);
    FAIL_RETURN(GetChars(start, end, buf, 0))

    String str(*buf);
    return CString::New(str, csq);
}

ECode TextUtils::Reverser::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    AutoPtr<ICharSequence> sub;
    Int32 length;
    GetLength(&length);
    SubSequence(0, length, (ICharSequence**)&sub);
    return sub->ToString(info);
}

ECode TextUtils::Reverser::GetCharAt(
    /* [in] */ Int32 off,
    /* [out] */ Char32* ch)
{
    VALIDATE_NOT_NULL(ch)

    Char32 orig;
    mSource->GetCharAt(mEnd - 1 - off, &orig);
    AutoPtr<IAndroidCharacter> ac;
    CAndroidCharacter::AcquireSingleton((IAndroidCharacter**)&ac);
    return ac->GetMirror(orig, ch);
}

ECode TextUtils::Reverser::GetChars(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Char32>* dest,
    /* [in] */ Int32 destoff)
{
    TextUtils::GetChars(mSource, start + mStart, end + mStart, dest, destoff);
    AutoPtr<IAndroidCharacter> ac;
    CAndroidCharacter::AcquireSingleton((IAndroidCharacter**)&ac);
    Boolean bval;
    ac->Mirror(dest, 0, end - start, &bval);

    Int32 len = end - start;
    Int32 n = (end - start) / 2;
    for (Int32 i = 0; i < n; i++) {
        Char32 tmp = (*dest)[destoff + i];

        (*dest)[destoff + i] = (*dest)[destoff + len - i - 1];
        (*dest)[destoff + len - i - 1] = tmp;
    }
    return NOERROR;
}

//===========================================================================
// TextUtils
//===========================================================================
ECode TextUtils::GetChars(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Char32>* dest,
    /* [in] */ Int32 destoff)
{
    VALIDATE_NOT_NULL(s)

    IGetChars* gc;
    IStringBuilder* sbld;
    IStringBuffer* sbuf = IStringBuffer::Probe(s);
    if (sbuf) {
        sbuf->GetChars(start, end, dest, destoff);
    }
    else if (sbld = IStringBuilder::Probe(s), sbld) {
        sbld->GetChars(start, end, dest, destoff);
    }
    else if (gc = IGetChars::Probe(s), gc) {
        gc->GetChars(start, end, dest, destoff);
    }
    else {
        String str;
        s->ToString(&str);
        AutoPtr< ArrayOf<Char32> > chars = str.GetChars(start, end);
        for (Int32 i = 0; i < chars->GetLength(); i++) {
            (*dest)[destoff++] = (*chars)[i];
        }
    }
    return NOERROR;
}

Int32 TextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch)
{
    return IndexOf(s, ch, 0);
}

Int32 TextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch,
    /* [in] */ Int32 start)
{
    assert(s != NULL);
    Int32 len;
    s->GetLength(&len);
    return IndexOf(s, ch, start, len);
}

Int32 TextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    assert(s != NULL);

    if (IGetChars::Probe(s) || IStringBuffer::Probe(s) ||
        IStringBuilder::Probe(s) /* TODO || c == String.class */) {
        const Int32 INDEX_INCREMENT = 500;
        AutoPtr< ArrayOf<Char32> > temp = Obtain(INDEX_INCREMENT);

        while (start < end) {
            Int32 segend = start + INDEX_INCREMENT;
            if (segend > end)
                segend = end;

            GetChars(s, start, segend, temp, 0);

            Int32 count = segend - start;
            for (Int32 i = 0; i < count; i++) {
                if ((*temp)[i] == ch) {
                    Recycle(temp);
                    return i + start;
                }
            }

            start = segend;
        }

        Recycle(temp);
        return -1;
    }

    for (Int32 i = start; i < end; i++) {
        Char32 ch_;
        s->GetCharAt(i, &ch_);
        if (ch_ == ch) {
            return i;
        }
    }

    return -1;
}

Int32 TextUtils::LastIndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch)
{
    assert(s != NULL);
    Int32 len;
    s->GetLength(&len);
    return LastIndexOf(s, ch, len - 1);
}

Int32 TextUtils::LastIndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch,
    /* [in] */ Int32 last)
{
    // assert(0);
//    Class<? extends CharSequence> c = s.getClass();
//
   // if (c == String.class)
   //     return ((String) s).lastIndexOf(ch, last);
    String str;
    s->ToString(&str);
    Int32 index = str.LastIndexOf(ch, last);
    if (index != -1) {
        return index;
    }

    return LastIndexOf(s, ch, 0, last);
}

Int32 TextUtils::LastIndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 last)
{
    assert(s != NULL);

    if (last < 0) {
        return -1;
    }
    Int32 len;
    s->GetLength(&len);
    if (last >= len) {
        last = len - 1;
    }

    Int32 end = last + 1;

//    assert(0);
//    Class<? extends CharSequence> c = s.getClass();
//
   if (IGetChars::Probe(s) != NULL || IStringBuffer::Probe(s) != NULL ||
       IStringBuilder::Probe(s) != NULL/* || c == String.class*/) {
       const Int32 INDEX_INCREMENT = 500;
       AutoPtr<ArrayOf<Char32> > temp = Obtain(INDEX_INCREMENT);

       while (start < end) {
           Int32 segstart = end - INDEX_INCREMENT;
           if (segstart < start)
               segstart = start;

           GetChars(s, segstart, end, temp, 0);

           Int32 count = end - segstart;
           for (Int32 i = count - 1; i >= 0; i--) {
               if ((*temp)[i] == ch) {
                   Recycle(temp);
                   return i + segstart;
               }
           }

           end = segstart;
       }

       Recycle(temp);
       return -1;
   }

    for (Int32 i = end - 1; i >= start; i--) {
        Char32 ch_;
        s->GetCharAt(i, &ch_);
        if (ch_ == ch) {
            return i;
        }
    }

    return -1;
}

Int32 TextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ ICharSequence* needle)
{
    assert(s != NULL);
    Int32 len;
    s->GetLength(&len);
    return IndexOf(s, needle, 0, len);
}

Int32 TextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ ICharSequence* needle,
    /* [in] */ Int32 start)
{
    assert(s != NULL);
    Int32 len;
    s->GetLength(&len);
    return IndexOf(s, needle, start, len);
}

Int32 TextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ ICharSequence* needle,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    assert(s != NULL);
    assert(needle != NULL);

    Int32 nlen;
    needle->GetLength(&nlen);
    if (nlen == 0) {
        return start;
    }

    Char32 ch;
    needle->GetCharAt(0, &ch);

    for (;;) {
        start = IndexOf(s, ch, start);
        if (start > end - nlen) {
            break;
        }

        if (start < 0) {
            return -1;
        }

        if (RegionMatches(s, start, needle, 0, nlen)) {
            return start;
        }

        start++;
    }
    return -1;
}

Boolean TextUtils::RegionMatches(
    /* [in] */ ICharSequence* one,
    /* [in] */ Int32 toffset,
    /* [in] */ ICharSequence* two,
    /* [in] */ Int32 ooffset,
    /* [in] */ Int32 len)
{
    Int32 tempLen = 2 * len;
    if (tempLen < len) {
        // Integer overflow; len is unreasonably large
        // throw new IndexOutOfBoundsException();
        return FALSE;
    }

    AutoPtr< ArrayOf<Char32> > temp = Obtain(tempLen);

    GetChars(one, toffset, toffset + len, temp, 0);
    GetChars(two, ooffset, ooffset + len, temp, len);

    Boolean match = TRUE;
    for (Int32 i = 0; i < len; i++) {
        if ((*temp)[i] != (*temp)[i + len]) {
            match = FALSE;
            break;
        }
    }

    Recycle(temp);
    return match;
}

String TextUtils::Substring(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (IStringBuilder::Probe(source) != NULL) {
        IStringBuilder* builder = IStringBuilder::Probe(source);
        String str;
        builder->Substring(start, end, &str);
        return str;
    }
    else if (IStringBuffer::Probe(source) != NULL) {
        IStringBuffer* builder = IStringBuffer::Probe(source);
        String str;
        builder->Substring(start, end, &str);
        return str;
    }

    AutoPtr<ArrayOf<Char32> > temp = Obtain(end - start);
    GetChars(source, start, end, temp, 0);
    String ret = String(*temp, 0, end - start);
    Recycle(temp);

    return ret;
}

AutoPtr<ICharSequence> TextUtils::Join(
    /* [in] */ IIterable* list)
{
    AutoPtr<IResources> res= CResources::GetSystem();
    AutoPtr<ICharSequence> delimiter;
    res->GetText(R::string::list_delimeter, (ICharSequence**)&delimiter);
    String str = Join(delimiter.Get(), list);
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(str);
    return csq;
}

String TextUtils::Join(
    /* [in] */ ICharSequence* delimiter,
    /* [in] */ ArrayOf<IInterface*>* tokens)
{
    StringBuilder sb;
    Boolean firstTime = TRUE;

    for (Int32 i = 0; i < tokens->GetLength(); ++i) {
        if (firstTime) {
            firstTime = FALSE;
        }
        else {
            sb.Append(delimiter);
        }

        sb.Append((*tokens)[i]);
    }

    return sb.ToString();
}

String TextUtils::Join(
    /* [in] */ const String& delimiter,
    /* [in] */ ArrayOf<String>* tokens)
{
    StringBuilder sb;
    Boolean firstTime = TRUE;

    for (Int32 i = 0; i < tokens->GetLength(); ++i) {
        if (firstTime) {
            firstTime = FALSE;
        }
        else {
            sb.Append(delimiter);
        }

        sb.Append((*tokens)[i]);
    }

    return sb.ToString();
}

String TextUtils::Join(
    /* [in] */ const String& delimiter,
    /* [in] */ List<String>* tokens)
{
    StringBuilder sb;
    Boolean firstTime = TRUE;
    if (tokens != NULL) {
        List<String>::Iterator it;
        for (it = tokens->Begin(); it != tokens->End(); ++it) {
            if (firstTime) {
                firstTime = FALSE;
            }
            else {
                sb.Append(delimiter);
            }

            sb.Append(*it);
        }
    }
    return sb.ToString();
}

String TextUtils::Join(
    /* [in] */ ICharSequence* delimiter,
    /* [in] */ IIterable* tokens)
{
    StringBuilder sb;
    Boolean firstTime = TRUE;

    AutoPtr<IIterator> it;
    tokens->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        if (firstTime) {
            firstTime = FALSE;
        }
        else {
            sb.Append(delimiter);
        }

        AutoPtr<IInterface> token;
        it->GetNext((IInterface**)&token);
        sb.Append(token);
    }

    return sb.ToString();
}

AutoPtr<ArrayOf<String> > TextUtils::Split(
    /* [in] */ const String& text,
    /* [in] */ const String& expression)
{
    if (text.GetLength() == 0) {
        return EMPTY_STRING_ARRAY;
    }

    AutoPtr<ArrayOf<String> > result;
    StringUtils::Split(text, expression, (ArrayOf<String> **)&result);
    return result;
}

AutoPtr<ArrayOf<String> > TextUtils::Split(
    /* [in] */ const String& text,
    /* [in] */ IPattern* pattern)
{
    assert(pattern != NULL);

    if (text.GetLength() == 0) {
        return EMPTY_STRING_ARRAY;
    }

    AutoPtr<ICharSequence> seq = CoreUtils::Convert(text);
    AutoPtr<ArrayOf<String> > result;
    pattern->Split(seq, -1, (ArrayOf<String>**)&result);
    return result;
}

AutoPtr<ICharSequence> TextUtils::StringOrSpannedString(
    /* [in] */ ICharSequence* source)
{
    if (source == NULL)
        return NULL;

    if (ISpannedString::Probe(source)) {
        return source;
    }
    else if (ISpanned::Probe(source)) {
        AutoPtr<ICharSequence> ss;
        CSpannedString::New(source, (ICharSequence**)&ss);
        return ss;
    }

    String str;
    source->ToString(&str);
    return CoreUtils::Convert(str);
}

Boolean TextUtils::IsEmpty(
    /* [in] */ ICharSequence* str)
{
    if (str == NULL) return TRUE;

    Int32 len;
    str->GetLength(&len);
    if (len == 0) return TRUE;

    return FALSE;
}

Boolean TextUtils::IsEmpty(
    /* [in] */ const String& str)
{
    return str.IsNullOrEmpty();
}

Int32 TextUtils::GetTrimmedLength(
    /* [in] */ ICharSequence* s)
{
    String str = Object::ToString(s);
    return GetTrimmedLength(str);
}

Int32 TextUtils::GetTrimmedLength(
    /* [in] */ const String& str)
{
    if (str.IsNullOrEmpty()) {
        return 0;
    }

    AutoPtr<ArrayOf<Char32> > chars = str.GetChars();
    Int32 start = 0;
    for (; start < chars->GetLength(); ++start) {
        if ((*chars)[start] > ' ')
            break;
    }
    Int32 end = chars->GetLength();
    for (; end > start; --end) {
        if ((*chars)[end] > ' ')
            break;
    }
    return end -start;
}

Boolean TextUtils::Equals(
    /* [in] */ ICharSequence* a,
    /* [in] */ ICharSequence* b)
{
    if (a == b) return TRUE;

    Int32 len1, len2;
    if (a != NULL && b != NULL && (a->GetLength(&len1), b->GetLength(&len2), len1 == len2)) {
//        if (a instanceof String && b instanceof String) {
//            return a.equals(b);
//        } else {
        Char32 ch1, ch2;
        for (Int32 i = 0; i < len1; i++) {

            a->GetCharAt(i, &ch1);
            b->GetCharAt(i, &ch2);
            if (ch1 != ch2) return FALSE;
        }
        return TRUE;
//        }
    }
    return FALSE;
}

Boolean TextUtils::Equals(
    /* [in] */ const String& a,
    /* [in] */ const String& b)
{
    if (a.IsNull() && b.IsNull())
        return TRUE;

    if (a.IsNull() || b.IsNull())
        return FALSE;

    return a.Equals(b);
}

// XXX currently this only reverses chars, not spans
AutoPtr<ICharSequence> TextUtils::GetReverse(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<ICharSequence> csq = new Reverser(source, start, end);
    return csq;
}

ECode TextUtils::WriteToParcel(
    /* [in] */ ICharSequence* cs,
    /* [in] */ IParcel* p)
{
    ISpanned* sp = ISpanned::Probe(cs);
    if (sp) {
        p->WriteInt32(0);
        String str;
        cs->ToString(&str);
        p->WriteString(str);

        Int32 len;
        cs->GetLength(&len);
        AutoPtr<ArrayOf<IInterface*> > os;
        sp->GetSpans(0, len, EIID_IInterface, (ArrayOf<IInterface*>**)&os);

        // note to people adding to this: check more specific types
        // before more generic types.  also notice that it uses
        // "if" instead of "else if" where there are interfaces
        // so one object can be several.
        len = os->GetLength();
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> o = (*os)[i];
            AutoPtr<IInterface> prop = (*os)[i];

            AutoPtr<ICharacterStyle> temp = ICharacterStyle::Probe(prop);
            if (temp != NULL) {
                AutoPtr<ICharacterStyle> style;
                temp->GetUnderlying((ICharacterStyle**)&style);
                prop = IInterface::Probe(style);
            }

            AutoPtr<IParcelableSpan> ps = IParcelableSpan::Probe(prop);
            if (ps != NULL) {
                Int32 typeId;
                ps->GetSpanTypeId(&typeId);

                if (typeId < ITextUtils::FIRST_SPAN || typeId > ITextUtils::LAST_SPAN) {
                    String str = Object::ToString(ps);
                    Logger::E(TAG, "external class \"%s\" is attempting to"
                        " use the frameworks-only ParcelableSpan interface", str.string());
                }
                else {
                    p->WriteInt32(typeId);
                    IParcelable::Probe(ps)->WriteToParcel(p);
                    WriteWhere(p, sp, o);
                }
            }
        }

        p->WriteInt32(0);
   }
   else {
       p->WriteInt32(1);
       if (cs != NULL) {
            String str;
            cs->ToString(&str);
            p->WriteString(str);
       }
       else {
            p->WriteString(String(NULL));
       }
   }

   return NOERROR;
}

ECode TextUtils::WriteWhere(
    /* [in] */ IParcel* p,
    /* [in] */ ISpanned* sp,
    /* [in] */ IInterface* o)
{
    Int32 start, end, flags;
    sp->GetSpanStart(o, &start);
    p->WriteInt32(start);
    sp->GetSpanEnd(o, &end);
    p->WriteInt32(end);
    sp->GetSpanFlags(o, &flags);
    p->WriteInt32(flags);
    return NOERROR;
}

ECode TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(
    /* [in] */ IParcel* p,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);
    *csq = NULL;

    Int32 kind;
    p->ReadInt32(&kind);

    String string;
    p->ReadString(&string);
    if (string.IsNull()) {
        return NOERROR;
    }

    AutoPtr<ICharSequence> cs;
    CString::New(string, (ICharSequence**)&cs);
    if (kind == 1) {
        *csq = cs;
        REFCOUNT_ADD(*csq);
        return NOERROR;
    }

    AutoPtr<ISpannableString> sp;
    CSpannableString::New(cs, (ISpannableString**)&sp);

    while (TRUE) {
        p->ReadInt32(&kind);

        if (kind == 0) {
            break;
        }

        switch (kind) {
        case ITextUtils::ALIGNMENT_SPAN: {
            AutoPtr<IAlignmentSpan> object;
            CAlignmentSpanStandard::New((IAlignmentSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::FOREGROUND_COLOR_SPAN: {
            AutoPtr<IForegroundColorSpan> object;
            CForegroundColorSpan::New((IForegroundColorSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::RELATIVE_SIZE_SPAN: {
            AutoPtr<IRelativeSizeSpan> object;
            CRelativeSizeSpan::New((IRelativeSizeSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::SCALE_X_SPAN: {
            AutoPtr<IScaleXSpan> object;
            CScaleXSpan::New((IScaleXSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::STRIKETHROUGH_SPAN: {
            AutoPtr<IStrikethroughSpan> object;
            CStrikethroughSpan::New((IStrikethroughSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::UNDERLINE_SPAN: {
            AutoPtr<IUnderlineSpan> object;
            CUnderlineSpan::New((IUnderlineSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::STYLE_SPAN: {
            AutoPtr<IStyleSpan> object;
            CStyleSpan::New((IStyleSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::BULLET_SPAN: {
            AutoPtr<IBulletSpan> object;
            CBulletSpan::New((IBulletSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::QUOTE_SPAN: {
            AutoPtr<IQuoteSpan> object;
            CQuoteSpan::New((IQuoteSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::LEADING_MARGIN_SPAN: {
            AutoPtr<ILeadingMarginSpan> object;
            CLeadingMarginSpanStandard::New((ILeadingMarginSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::URL_SPAN: {
            AutoPtr<IURLSpan> object;
            CURLSpan::New((IURLSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::BACKGROUND_COLOR_SPAN: {
            AutoPtr<IBackgroundColorSpan> object;
            CBackgroundColorSpan::New((IBackgroundColorSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::TYPEFACE_SPAN: {
            AutoPtr<ITypefaceSpan> object;
            CTypefaceSpan::New((ITypefaceSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::SUPERSCRIPT_SPAN: {
            AutoPtr<ISuperscriptSpan> object;
            CSuperscriptSpan::New((ISuperscriptSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::SUBSCRIPT_SPAN: {
            AutoPtr<ISubscriptSpan> object;
            CSubscriptSpan::New((ISubscriptSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::ABSOLUTE_SIZE_SPAN: {
            AutoPtr<IAbsoluteSizeSpan> object;
            CAbsoluteSizeSpan::New((IAbsoluteSizeSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::TEXT_APPEARANCE_SPAN: {
            AutoPtr<ITextAppearanceSpan> object;
            CTextAppearanceSpan::New((ITextAppearanceSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::ANNOTATION: {
            AutoPtr<IAnnotation> object;
            CAnnotation::New((IAnnotation**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::SUGGESTION_SPAN: {
            AutoPtr<ISuggestionSpan> object;
            CSuggestionSpan::New((ISuggestionSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::SPELL_CHECK_SPAN: {
            AutoPtr<ISpellCheckSpan> object;
            CSpellCheckSpan::New((ISpellCheckSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::SUGGESTION_RANGE_SPAN: {
            AutoPtr<ISuggestionRangeSpan> object;
            CSuggestionRangeSpan::New((ISuggestionRangeSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::EASY_EDIT_SPAN: {
            AutoPtr<IEasyEditSpan> object;
            CEasyEditSpan::New((IEasyEditSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::LOCALE_SPAN: {
            AutoPtr<ILocaleSpan> object;
            CLocaleSpan::New((ILocaleSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        case ITextUtils::TTS_SPAN: {
            AutoPtr<ITtsSpan> object;
            CTtsSpan::New((ITtsSpan**)&object);
            IParcelable::Probe(object)->ReadFromParcel(p);
            ReadSpan(p, ISpannable::Probe(sp), (IInterface*)object.Get());
            break;
        }

        default:
            Logger::E(TAG, "bogus span encoding %d", kind);
            return E_RUNTIME_EXCEPTION;
        }
    }

    *csq = ICharSequence::Probe(sp);
    REFCOUNT_ADD(*csq);
    return NOERROR;
}

AutoPtr<ArrayOf<ICharSequence*> > TextUtils::CHAR_SEQUENCE_CREATOR::NewArray(
    /* [in] */ Int32 size)
{
    AutoPtr<ArrayOf<ICharSequence*> > array = ArrayOf<ICharSequence*>::Alloc(size);
    return array;
}

/**
 * Debugging tool to print the spans in a CharSequence.  The output will
 * be printed one span per line.  If the CharSequence is not a Spanned,
 * then the entire string will be printed on a single line.
 */
//    public static void dumpSpans(CharSequence cs, Printer printer, String prefix);


AutoPtr<ICharSequence> TextUtils::Replace(
    /* [in] */ ICharSequence* temp,
    /* [in] */ ArrayOf<String>* sources,
    /* [in] */ ArrayOf<ICharSequence*>* destinations)
{
    AutoPtr<ISpannableStringBuilder> tb;
    CSpannableStringBuilder::New(temp, (ISpannableStringBuilder**)&tb);
    ICharSequence* csq = ICharSequence::Probe(tb);
    ISpannable* spannable = ISpannable::Probe(tb);
    ISpanned* spanned = ISpanned::Probe(tb);
    IEditable* enditable = IEditable::Probe(tb);

    for (Int32 i = 0; i < sources->GetLength(); i++) {
        ICharSequence* src = CoreUtils::Convert((*sources)[i]);
        Int32 where = IndexOf(csq, src);

        if (where >= 0)
            spannable->SetSpan(src, where, where + (*sources)[i].GetLength(),
                ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
    }

    Int32 start, end;
    for (Int32 i = 0; i < sources->GetLength(); i++) {
        ICharSequence* src = CoreUtils::Convert((*sources)[i]);
        spanned->GetSpanStart(src, &start);
        spanned->GetSpanEnd(src, &end);

        if (start >= 0) {
            enditable->Replace(start, end, (*destinations)[i]);
        }
    }

    return csq;
}

AutoPtr<ICharSequence> TextUtils::ExpandTemplate(
    /* [in] */ ICharSequence* temp,
    /* [in] */ ArrayOf<ICharSequence*>* values)
{
    if (values->GetLength() > 9) {
        Logger::E(TAG, "TextUtils::ExpandTemplate : max of 9 values are supported");
        return NULL;
    }

    AutoPtr<ISpannableStringBuilder> ssb;
    CSpannableStringBuilder::New(temp, (ISpannableStringBuilder**)&ssb);
    ICharSequence* csq = ICharSequence::Probe(ssb);
    IEditable* enditable = IEditable::Probe(ssb);

    // try {
    Int32 i = 0, length;
    Char32 ch;
    while ((csq->GetLength(&length), length) > i) {
        csq->GetCharAt(i, &ch);
        if (ch == '^') {
            Char32 next;
            csq->GetCharAt(i+1, &next);
            if (next == '^') {
                enditable->Delete(i+1, i+2);
                ++i;
                continue;
            }
            else if (Character::IsDigit(next)) {
                Int32 which = Character::GetNumericValue(next) - 1;
                if (which < 0) {
                    // throw new IllegalArgumentException(
                    //     "template requests value ^" + (which+1));
                    Logger::E(TAG, "IllegalArgumentException template requests value ^%d", (which + 1));
                    return NULL;
                }
                if (which >= values->GetLength()) {
                    // throw new IllegalArgumentException(
                    //     "template requests value ^" + (which+1) +
                    //     "; only " + values.length + " provided");
                    Logger::E(TAG, "IllegalArgumentException template requests value ^%d; only %d provided.",
                        (which + 1), values->GetLength());
                    return NULL;
                }

                enditable->Replace(i, i + 2, (*values)[which]);
                (*values)[which]->GetLength(&length);
                i += length;
                continue;
            }
        }
        ++i;
    }
    // } catch (IndexOutOfBoundsException ignore) {
    //     // happens when ^ is the last character in the string.
    // }
    return csq;
}

Int32 TextUtils::GetOffsetBefore(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 offset)
{
    if (offset == 0) {
        return 0;
    }
    if (offset == 1) {
        return 0;
    }

    Char32 c;
    text->GetCharAt(offset - 1, &c);

    if (c >= 0xDC00/*'\uDC00'*/ && c <= 0xDFFF/*'\uDFFF'*/) {
        Char32 c1;
        text->GetCharAt(offset - 2, &c1);

        if (c1 >= 0xD800/*'\uD800'*/ && c1 <= 0xDBFF/*'\uDBFF'*/) {
            offset -= 2;
        }
        else {
            offset -= 1;
        }
    }
    else {
        offset -= 1;
    }

    if (ISpanned::Probe(text) != NULL) {
        AutoPtr< ArrayOf<IInterface*> > spans;
        AutoPtr<ISpanned> spanned = ISpanned::Probe(text);
        spanned->GetSpans(offset, offset, EIID_IReplacementSpan, (ArrayOf<IInterface*>**)&spans);

        for (Int32 i = 0; i < spans->GetLength(); i++) {
            Int32 start, end;
            spanned->GetSpanStart((*spans)[i], &start);
            spanned->GetSpanEnd((*spans)[i], &end);

            if (start < offset && end > offset) {
                offset = start;
            }
        }
    }

    return offset;
}

Int32 TextUtils::GetOffsetAfter(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 offset)
{
    Int32 len;
    text->GetLength(&len);

    if (offset == len) {
        return len;
    }
    if (offset == len - 1) {
        return len;
    }

    Char32 c;
    text->GetCharAt(offset, &c);

    if (c >= 0xD800/*'\uD800'*/ && c <= 0xDBFF/*'\uDBFF'*/) {
        Char32 c1;
        text->GetCharAt(offset + 1, &c1);

        if (c1 >= 0xDC00/*'\uDC00'*/ && c1 <= 0xDFFF/*'\uDFFF'*/) {
            offset += 2;
        }
        else {
            offset += 1;
        }
    }
    else {
        offset += 1;
    }

    if (ISpanned::Probe(text) != NULL) {
        AutoPtr< ArrayOf<IInterface*> > spans;
        AutoPtr<ISpanned> spanned = ISpanned::Probe(text);
        spanned->GetSpans(offset, offset, EIID_IReplacementSpan, (ArrayOf<IInterface*>**)&spans);

        for (Int32 i = 0; i < spans->GetLength(); i++) {
            Int32 start, end;
            spanned->GetSpanStart((*spans)[i], &start);
            spanned->GetSpanEnd((*spans)[i], &end);

            if (start < offset && end > offset) {
                offset = end;
            }
        }
    }

    return offset;
}

ECode TextUtils::ReadSpan(
    /* [in] */ IParcel* p,
    /* [in] */ ISpannable* sp,
    /* [in] */ IInterface* o)
{
    Int32 start, end, flags;
    FAIL_RETURN(p->ReadInt32(&start));
    FAIL_RETURN(p->ReadInt32(&end));
    FAIL_RETURN(p->ReadInt32(&flags));
    return sp->SetSpan(o, start, end, flags);
}

ECode TextUtils::CopySpansFrom(
    /* [in] */ ISpanned* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const InterfaceID& kind,
    /* [in] */ ISpannable* dest,
    /* [in] */ Int32 destoff)
{
//    if (kind == NULL) {
//        kind = Object.class;
//    }
    AutoPtr< ArrayOf<IInterface*> > spans;
    source->GetSpans(start, end, kind, (ArrayOf<IInterface*>**)&spans);

    for (Int32 i = 0; i < spans->GetLength(); i++) {
        Int32 st, en, fl;
        source->GetSpanStart((*spans)[i], &st);
        source->GetSpanEnd((*spans)[i], &en);
        source->GetSpanFlags((*spans)[i], &fl);

        if (st < start) {
            st = start;
        }
        if (en > end) {
            en = end;
        }

        dest->SetSpan((*spans)[i], st - start + destoff,
                en - start + destoff, fl);
    }
    return NOERROR;
}

AutoPtr<ICharSequence> TextUtils::Ellipsize(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* p,
    /* [in] */ Float avail,
    /* [in] */ TextUtilsTruncateAt where)
{
    return Ellipsize(text, p, avail, where, FALSE, NULL);
}

AutoPtr<ICharSequence> TextUtils::Ellipsize(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Float avail,
    /* [in] */ TextUtilsTruncateAt where,
    /* [in] */ Boolean preserveLength,
    /* [in] */ ITextUtilsEllipsizeCallback* callback)
{
    AutoPtr<IResourcesHelper> helper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&helper);
    AutoPtr<IResources> resources;
    helper->GetSystem((IResources**)&resources);

    String ellipsis;
    if (where == TextUtilsTruncateAt_END_SMALL)  {
        resources->GetString(R::string::ellipsis_two_dots, &ellipsis);
    }
    else {
        resources->GetString(R::string::ellipsis, &ellipsis);
    }

    AutoPtr<ICharSequence> rst;
    rst = Ellipsize(text, paint, avail, where, preserveLength, callback,
        TextDirectionHeuristics::FIRSTSTRONG_LTR, ellipsis);
    if(rst == NULL) {
        String sNull("");
        CString::New(sNull, (ICharSequence**)&rst);
    }
    return rst;
}

AutoPtr<ICharSequence> TextUtils::Ellipsize(
   /* [in] */ ICharSequence* text,
   /* [in] */ ITextPaint* paint,
   /* [in] */ Float avail,
   /* [in] */ TextUtilsTruncateAt where,
   /* [in] */ Boolean preserveLength,
   /* [in] */ ITextUtilsEllipsizeCallback* callback,
   /* [in] */ ITextDirectionHeuristic* textDir,
   /* [in] */ const String& ellipsis)
{
    assert(text != NULL);

    Int32 len;
    text->GetLength(&len);

    AutoPtr<MeasuredText> mt = MeasuredText::Obtain();
    //try {
    Float width = SetPara(mt, paint, text, 0, len, textDir);

    if (width <= avail) {
        if (callback != NULL) {
            callback->Ellipsized(0, 0);
        }
        MeasuredText::Recycle(mt);
        return text;
    }

    // XXX assumes ellipsis string does not require shaping and
    // is unaffected by style
    Float ellipsiswid;
    IPaint::Probe(paint)->MeasureText(ellipsis, &ellipsiswid);
    avail -= ellipsiswid;

    Int32 left = 0;
    Int32 right = len;
    if (avail < 0) {
        // it all goes
    }
    else if (where == TextUtilsTruncateAt_START) {
        right = len - mt->BreakText(len, FALSE, avail);
    }
    else if (where == TextUtilsTruncateAt_END
        || where == TextUtilsTruncateAt_END_SMALL) {
        left = mt->BreakText(len, TRUE, avail);
    }
    else {
        right = len - mt->BreakText(len, FALSE, avail / 2);
        avail -= mt->Measure(right, len);
        left = mt->BreakText(right, TRUE, avail);
    }

    if (callback != NULL) {
        callback->Ellipsized(left, right);
    }

    AutoPtr<ArrayOf<Char32> > buf = mt->mChars;
    AutoPtr<ISpanned> sp = ISpanned::Probe(text);
    Int32 remaining = len - (right - left);
    if (preserveLength) {
        if (remaining > 0) { // else eliminate the ellipsis too
           (*buf)[left++] = ellipsis.GetChar(0);
        }
        for (Int32 i = left; i < right; i++) {
           (*buf)[i] = ZWNBS_CHAR;
        }

        String str(*buf, 0, len);
        AutoPtr<ICharSequence> s;
        CString::New(str, (ICharSequence**)&s);
        if (sp == NULL) {
            MeasuredText::Recycle(mt);
            return s;
        }

        AutoPtr<ISpannableString> ss;
        CSpannableString::New(s, (ISpannableString**)&ss);
        CopySpansFrom(sp, 0, len, EIID_IInterface, ISpannable::Probe(ss), 0);
        MeasuredText::Recycle(mt);
        return ICharSequence::Probe(ss);
    }

    if (remaining == 0) {
        AutoPtr<ICharSequence> seq;
        CString::New(String(""), (ICharSequence**)&seq);
        MeasuredText::Recycle(mt);
        return seq;
    }

    if (sp == NULL) {
        Int32 length = remaining + ellipsis.GetByteLength();
        StringBuilder sb(length);
        String tmp(*buf, 0, left);
        sb += tmp;
        sb += ellipsis;
        String tmp2(*buf, right, len - right);
        sb += tmp2;
        String str;
        sb.ToString(&str);
        AutoPtr<ICharSequence> seq;
        CString::New(str, (ICharSequence**)&seq);
        MeasuredText::Recycle(mt);
        return seq;
    }

    AutoPtr<ISpannableStringBuilder> ssb;
    CSpannableStringBuilder::New((ISpannableStringBuilder**)&ssb);
    ssb->Append(text, 0, left);
    AutoPtr<ICharSequence> elpSeq;
    CString::New(ellipsis, (ICharSequence**)&elpSeq);
    assert(0 && "TODO");
    //ssb->Append(elpSeq);
    //ssb->Append(text, right, len);
    MeasuredText::Recycle(mt);
    return ICharSequence::Probe(ssb);
   //} finally {
      // MeasuredText.recycle(mt);
   //}
}

AutoPtr<ICharSequence> TextUtils::CommaEllipsize(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* p,
    /* [in] */ Float avail,
    /* [in] */ const String& oneMore,
    /* [in] */ const String& more)
{
    return CommaEllipsize(text, p, avail, oneMore, more,
            TextDirectionHeuristics::FIRSTSTRONG_LTR);
}

AutoPtr<ICharSequence> TextUtils::CommaEllipsize(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* p,
    /* [in] */ Float avail,
    /* [in] */ const String& oneMore,
    /* [in] */ const String& more,
    /* [in] */ ITextDirectionHeuristic* textDir)
{
    assert(0 && "TODO");
    // AutoPtr<MeasuredText> mt = MeasuredText::Obtain();
    // // try {
    // Int32 len;
    // text->GetLength(&len);
    // Float width = SetPara(mt, p, text, 0, len, textDir);
    // if (width <= avail) {
    //     return text;
    // }

    // AutoPtr<ArrayOf<Char32> > buf = mt->mChars;

    // Int32 commaCount = 0;
    // for (Int32 i = 0; i < len; i++) {
    //     if ((&buf)[i] == ',') {
    //         commaCount++;
    //     }
    // }

    // Int32 remaining = commaCount + 1;

    // Int32 ok = 0;
    // String okFormat("");

    // Int32 w = 0;
    // Int32 count = 0;
    // AutoPtr<ArrayOf<Float> > widths = mt->mWidths;

    // AutoPtr<MeasuredText> tempMt = MeasuredText::Obtain();
    // for (Int32 i = 0; i < len; i++) {
    //     w += (*widths)[i];

    //     if ((*buf)[i] == ',') {
    //         count++;

    //         String format;
    //         // XXX should not insert spaces, should be part of string
    //         // XXX should use plural rules and not assume English plurals
    //         if (--remaining == 1) {
    //             format += " ";
    //             format += oneMore;
    //         } else {
    //             format += " ";
    //             AutoPtr<IInteger32> iobj = CoreUtils::Convert(remaining);
    //             format += StringUtils::Format(more, TO_IINTERFACE(iobj));
    //         }

    //         // XXX this is probably ok, but need to look at it more
    //         tempMt->SetPara(format, 0, format->GetLength(), textDir);
    //         Float moreWid = tempMt->AddStyleRun(p, tempMt.mLen, NULL);

    //         if (w + moreWid <= avail) {
    //             ok = i + 1;
    //             okFormat = format;
    //         }
    //     }
    // }
    // MeasuredText::Recycle(tempMt);

    AutoPtr<ISpannableStringBuilder> out;
    // CSpannableStringBuilder(okFormat, (ISpannableStringBuilder**)&out);
    // out->Insert(0, text, 0, ok);

    // // } finally {
    // MeasuredText::Recycle(mt);
    // }

    return ICharSequence::Probe(out);
}

Float TextUtils::SetPara(
   /* [in] */ MeasuredText* mt,
   /* [in] */ ITextPaint* paint,
   /* [in] */ ICharSequence* text,
   /* [in] */ Int32 start,
   /* [in] */ Int32 end,
   /* [in] */ ITextDirectionHeuristic* textDir)
{
    assert(mt != NULL && text != NULL);
    mt->SetPara(text, start, end, textDir);

    Float width;
    AutoPtr<ISpanned> sp = ISpanned::Probe(text);

    Int32 len = end - start;
    if (sp == NULL) {
        width = mt->AddStyleRun(paint, len, NULL);
    }
    else {
        width = 0;
        Int32 spanEnd;
        for (Int32 spanStart = 0; spanStart < len; spanStart = spanEnd) {
            sp->NextSpanTransition(spanStart, len, EIID_IMetricAffectingSpan, &spanEnd);
            AutoPtr<ArrayOf<IInterface*> > spans, spansResult;//IMetricAffectingSpan
            sp->GetSpans(spanStart, spanEnd, EIID_IMetricAffectingSpan, (ArrayOf<IInterface*>**)&spans);
            spansResult = RemoveEmptySpans(spans, sp, EIID_IMetricAffectingSpan);
            width += mt->AddStyleRun(paint, spansResult, spanEnd - spanStart, NULL);
        }
    }

    return width;
}

/* package */
Boolean TextUtils::DoesNotNeedBidi(
    /* [ini] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    assert(s != NULL);
    for (Int32 i = start; i < end; i++) {
        Char32 c;
        s->GetCharAt(i, &c);
        if (c >= FIRST_RIGHT_TO_LEFT) {
            return FALSE;
        }
    }
    return TRUE;
}

/* package */
Boolean TextUtils::DoesNotNeedBidi(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 len)
{
    assert(text != NULL);
    for (Int32 i = start, e = i + len; i < e; i++) {
        if ((*text)[i] >= FIRST_RIGHT_TO_LEFT) {
            return FALSE;
        }
    }
    return TRUE;
}

/* package */
AutoPtr< ArrayOf<Char32> > TextUtils::Obtain(
    /* [in] */ Int32 len)
{
    assert(len >= 0);
    AutoPtr< ArrayOf<Char32> > buf;
    {
        AutoLock lock(sLock);
        buf = sTemp;
        sTemp = NULL;
    }

    if (buf == NULL || buf->GetLength() < len) {
        buf = ArrayUtils::NewUnpaddedChar32Array(len);
    }

    return buf;
}

/* package */
void TextUtils::Recycle(
    /* [in] */ ArrayOf<Char32>* temp)
{
    if (temp && temp->GetLength() > 1000) {
        return;
    }

    AutoLock lock(sLock);
    sTemp = temp;
}

/**
 * Html-encode the string.
 * @param s the string to be encoded
 * @return the encoded string
 */
String TextUtils::HtmlEncode(
    /* [in] */ const String& s)
{
    AutoPtr<ArrayOf<Char32> > chars = s.GetChars();
    StringBuilder sb;
    Char32 c;
    for (Int32 i = 0; i < chars->GetLength(); i++) {
        c = (*chars)[i];
        switch (c) {
        case '<':
            sb.Append("&lt;"); //$NON-NLS-1$
            break;
        case '>':
            sb.Append("&gt;"); //$NON-NLS-1$
            break;
        case '&':
            sb.Append("&amp;"); //$NON-NLS-1$
            break;
        case '\'':
            //http://www.w3.org/TR/xhtml1
            // The named character reference &apos; (the apostrophe, U+0027) was introduced in
            // XML 1.0 but does not appear in HTML. Authors should therefore use &#39; instead
            // of &apos; to work as expected in HTML 4 user agents.
            sb.Append("&#39;"); //$NON-NLS-1$
            break;
        case '"':
            sb.Append("&quot;"); //$NON-NLS-1$
            break;
        default:
            sb.AppendChar(c);
        }
    }
    return sb.ToString();
}

/**
 * Returns a CharSequence concatenating the specified CharSequences,
 * retaining their spans if any.
 */
AutoPtr<ICharSequence> TextUtils::Concat(
    /* [in] */ ArrayOf<ICharSequence*>* text)
{
    AutoPtr<ICharSequence> result;
    Int32 len = text->GetLength();
    if (len == 0) {
        CString::New(String(""), (ICharSequence**)&result);
        return result;
    }
    if (len == 1) {
        return (*text)[0];
    }

    Boolean spanned = FALSE;
    for (Int32 i = 0; i < len; ++i) {
        if (ISpanned::Probe((*text)[i])) {
            spanned = TRUE;
            break;
        }
    }

    StringBuilder sb;
    for (Int32 i = 0; i < len; ++i) {
        sb.Append((*text)[i]);
    }

    if (!spanned) {
        CString::New(sb.ToString(), (ICharSequence**)&result);
        return result;
    }
    AutoPtr<ISpannableString> ss;
    AutoPtr<ICharSequence> s = sb.ToCharSequence();
    CSpannableString::New(s, (ISpannableString**)&ss);
    Int32 off = 0;
    Int32 iLen;
    for (Int32 i = 0; i < len; ++i) {
        (*text)[i]->GetLength(&iLen);
        if (ISpanned::Probe((*text)[i])) {
            CopySpansFrom(ISpanned::Probe((*text)[i]), 0, iLen, EIID_IInterface , ISpannable::Probe(ss), off);
        }

        off += iLen;
    }
    AutoPtr<ISpannedString> span;
    CSpannedString::New(ICharSequence::Probe(ss),(ISpannedString**)&span);
    result = ICharSequence::Probe(span.Get());
    return result;
}

Boolean TextUtils::IsGraphic(
    /* [in] */ ICharSequence* str)
{
    Int32 len;
    str->GetLength(&len);
    Char32 ch;
    for (Int32 i = 0; i < len; i++) {
        str->GetCharAt(i, &ch);
        Int32 gc = Character::GetType(ch);
        if (gc != Character::CONTROL
                && gc != Character::FORMAT
                && gc != Character::SURROGATE
                && gc != Character::UNASSIGNED
                && gc != Character::LINE_SEPARATOR
                && gc != Character::PARAGRAPH_SEPARATOR
                && gc != Character::SPACE_SEPARATOR) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean TextUtils::IsGraphic(
    /* [in] */ Char32 c)
{
    Int32 gc = Character::GetType(c);
    return     gc != Character::CONTROL
            && gc != Character::FORMAT
            && gc != Character::SURROGATE
            && gc != Character::UNASSIGNED
            && gc != Character::LINE_SEPARATOR
            && gc != Character::PARAGRAPH_SEPARATOR
            && gc != Character::SPACE_SEPARATOR;
}

Boolean TextUtils::IsDigitsOnly(
    /* [in] */ ICharSequence* str)
{
    Int32 len;
    str->GetLength(&len);
    Char32 ch;
    for (Int32 i = 0; i < len; i++) {
        str->GetCharAt(i, &ch);
        if (!Character::IsDigit(ch)) {
            return FALSE;
        }
    }
    return TRUE;
}

Boolean TextUtils::IsPrintableAscii(
    /* [in] */ Char32 c)
{
    Int32 asciiFirst = 0x20;
    Int32 asciiLast = 0x7E;  // included
    return (asciiFirst <= (Int32)c && (Int32)c <= asciiLast) || c == '\r' || c == '\n';
}

Boolean TextUtils::IsPrintableAsciiOnly(
    /* [in] */ ICharSequence* str)
{
    Int32 len;
    str->GetLength(&len);
    Char32 ch;
    for (Int32 i = 0; i < len; i++) {
        str->GetCharAt(i, &ch);
        if (!IsPrintableAscii(ch)) {
            return FALSE;
        }
    }
    return TRUE;
}

Int32 TextUtils::GetCapsMode(
    /* [in] */ ICharSequence* cs,
    /* [in] */ Int32 off,
    /* [in] */ Int32 reqModes)
{
    if (off < 0) {
        return 0;
    }

    Int32 i;
    Char32 c;
    Int32 mode = 0;

    if ((reqModes & ITextUtils::CAP_MODE_CHARACTERS) != 0) {
        mode |= ITextUtils::CAP_MODE_CHARACTERS;
    }
    if ((reqModes & (ITextUtils::CAP_MODE_WORDS | ITextUtils::CAP_MODE_SENTENCES)) == 0) {
        return mode;
    }

    // Back over allowed opening punctuation.

    for (i = off; i > 0; i--) {
        cs->GetCharAt(i - 1, &c);

        if (c != '"' && c != '\'' &&
            Character::GetType(c) != Character::START_PUNCTUATION) {
            break;
        }
    }

    // Start of paragraph, with optional whitespace.

    Int32 j = i;
    while (j > 0 && ((cs->GetCharAt(j - 1, &c), c) == ' ' || c == '\t')) {
        j--;
    }
    if (j == 0 || (cs->GetCharAt(j - 1, &c), c) == '\n') {
        return mode | ITextUtils::CAP_MODE_WORDS;
    }

    // Or start of word if we are that style.

    if ((reqModes & ITextUtils::CAP_MODE_SENTENCES) == 0) {
        if (i != j) mode |= ITextUtils::CAP_MODE_WORDS;
        return mode;
    }

    // There must be a space if not the start of paragraph.

    if (i == j) {
        return mode;
    }

    // Back over allowed closing punctuation.

    for (; j > 0; j--) {
        cs->GetCharAt(j - 1, &c);

        if (c != '"' && c != '\'' &&
            Character::GetType(c) != Character::END_PUNCTUATION) {
            break;
        }
    }

    if (j > 0) {
        cs->GetCharAt(j - 1, &c);

        if (c == '.' || c == '?' || c == '!') {
            // Do not capitalize if the word ends with a period but
            // also contains a period, in which case it is an abbreviation.

            if (c == '.') {
                for (Int32 k = j - 2; k >= 0; k--) {
                    cs->GetCharAt(k, &c);

                    if (c == '.') {
                        return mode;
                    }

                    if (!Character::IsLetter(c)) {
                        break;
                    }
                }
            }

            return mode | ITextUtils::CAP_MODE_SENTENCES;
        }
    }

    return mode;
}

Boolean TextUtils::DelimitedStringContains(
    /* [in] */ const String& delimitedString,
    /* [in] */ Char32 delimiter,
    /* [in] */ const String& item)
{
    if (IsEmpty(delimitedString) || IsEmpty(item)) {
        return FALSE;
    }
    Int32 pos = -1;
    Int32 length = delimitedString.GetLength();
    while ((pos = delimitedString.IndexOf(item, pos + 1)) != -1) {
        if (pos > 0 && delimitedString.GetChar(pos - 1) != delimiter) {
            continue;
        }
        Int32 expectedDelimiterPos = pos + item.GetLength();
        if (expectedDelimiterPos == length) {
            // Match at end of string.
            return TRUE;
        }
        if (delimitedString.GetChar(expectedDelimiterPos) == delimiter) {
            return TRUE;
        }
    }
    return FALSE;
}

AutoPtr< ArrayOf<IInterface*> > TextUtils::RemoveEmptySpans(
    /* [in] */ ArrayOf<IInterface*>* spans,
    /* [in] */ ISpanned* spanned,
    /* [in] */ const InterfaceID& klass)
{
    assert(spans != NULL);
    assert(spanned != NULL);

    AutoPtr< ArrayOf<IInterface*> > copy;
    Int32 count = 0;

    for (Int32 i = 0; i < spans->GetLength(); i++) {
        IInterface* span = (*spans)[i];
        Int32 start, end;
        spanned->GetSpanStart(span, &start);
        spanned->GetSpanEnd(span, &end);

        if (start == end) {
            if (copy == NULL) {
                assert(0);
//                copy = (T[]) Array.newInstance(klass, spans.length - 1);
//                System.arraycopy(spans, 0, copy, 0, i);
                count = i;
            }
        }
        else {
            if (copy != NULL) {
                copy->Set(count, span);
                count++;
            }
        }
    }

    if (copy != NULL) {
        assert(0);
//        T[] result = (T[]) Array.newInstance(klass, count);
//        System.arraycopy(copy, 0, result, 0, count);
        return NULL;
    }
    else {
        return spans;
    }
}

Int64 TextUtils::PackRangeInInt64(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return (((Int64) start) << 32) | end;
}

Int32 TextUtils::UnpackRangeStartFromInt64(
    /* [in] */ Int64 range)
{
    return (Int32)(((UInt64)range) >> 32);
}

Int32 TextUtils::UnpackRangeEndFromInt64(
    /* [in] */ Int64 range)
{
    return (Int32) (range & 0x00000000FFFFFFFFll);
}

Int32 TextUtils::GetLayoutDirectionFromLocale(
    /* [in] */ ILocale* locale)
{
    Boolean mirror = FALSE;
    SystemProperties::GetBoolean(ISettingsGlobal::DEVELOPMENT_FORCE_RTL, FALSE, &mirror);

    AutoPtr<ILocale> root;
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    helper->GetROOT((ILocale**)&root);
    Boolean result;
    if (locale != NULL && !(locale->Equals(root, &result), result)) {
        String scriptSubtag;
        AutoPtr<IICUUtil> icuHelper;
        CICUUtil::AcquireSingleton((IICUUtil**)&icuHelper);
        AutoPtr<ILocale> target;
        icuHelper->AddLikelySubtags(locale, (ILocale**)&target);
        target->GetScript(&scriptSubtag);
        if (scriptSubtag.IsNull()) {
            return GetLayoutDirectionFromFirstChar(locale);
        }

        if (scriptSubtag.EqualsIgnoreCase(ARAB_SCRIPT_SUBTAG) ||
            scriptSubtag.EqualsIgnoreCase(HEBR_SCRIPT_SUBTAG)) {
            // If forcing into RTL layout mode and language is RTL
            // return LTR as default, else RTL
            return mirror ? IView::LAYOUT_DIRECTION_LTR : IView::LAYOUT_DIRECTION_RTL;
        }
    }

    // If forcing into RTL layout mode and language is LTR
    // return RTL as default, else LTR
    return mirror ? IView::LAYOUT_DIRECTION_RTL : IView::LAYOUT_DIRECTION_LTR;
}

Int32 TextUtils::GetLayoutDirectionFromFirstChar(
    /* [in] */ ILocale* locale)
{
    String displayName;
    locale->GetDisplayName(locale, &displayName);
    const Byte directionality = Character::GetDirectionality(displayName.GetChar(0));
    if (directionality == Character::DIRECTIONALITY_RIGHT_TO_LEFT ||
        directionality == Character::DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC) {
        return IView::LAYOUT_DIRECTION_RTL;
    }
    return IView::LAYOUT_DIRECTION_LTR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
