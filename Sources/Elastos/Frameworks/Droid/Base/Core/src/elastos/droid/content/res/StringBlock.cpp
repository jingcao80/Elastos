
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/content/res/StringBlock.h"
//#include "elastos/droid/graphics/CPaint.h"
//#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/Color.h"
// #include "elastos/droid/text/CStyleSpan.h"
// #include "elastos/droid/text/CSpannableString.h"
// #include "elastos/droid/text/CUnderlineSpan.h"
// #include "elastos/droid/text/CTypefaceSpan.h"
// #include "elastos/droid/text/CRelativeSizeSpan.h"
// #include "elastos/droid/text/CSubscriptSpan.h"
// #include "elastos/droid/text/CSuperscriptSpan.h"
// #include "elastos/droid/text/CStrikethroughSpan.h"
// #include "elastos/droid/text/CBulletSpan.h"
// #include "elastos/droid/text/CAbsoluteSizeSpan.h"
// #include "elastos/droid/text/CForegroundColorSpan.h"
// #include "elastos/droid/text/CBackgroundColorSpan.h"
// #include "elastos/droid/text/CURLSpan.h"
// #include "elastos/droid/text/CAnnotation.h"
// #include "utils/XmlUtils.h"
// #include <string.h>
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <androidfw/ResourceTypes.h>

using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Slogger;
//using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IPaint;
//using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::Color;
using Elastos::Droid::Graphics::IRect;
// using Elastos::Droid::Text::TextUtilsTruncateAt;
// using Elastos::Droid::Text::CAnnotation;
// using Elastos::Droid::Text::IAnnotation;
// using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::Style::EIID_ILineHeightSpanWithDensity;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

StringBlock::StyleIDs::StyleIDs()
  : boldId(-1)
  , italicId(-1)
  , underlineId(-1)
  , ttId(-1)
  , bigId(-1)
  , smallId(-1)
  , subId(-1)
  , supId(-1)
  , strikeId(-1)
  , listItemId(-1)
  , marqueeId(-1)
{}


Float StringBlock::Height::sProportion = 0.0;

CAR_INTERFACE_IMPL(StringBlock::Height, Object, ILineHeightSpanWithDensity)

StringBlock::Height::Height(
    /* [in] */ Int32 size)
    : mSize(size)
{}

ECode StringBlock::Height::ChooseHeight(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 spanstartv,
    /* [in] */ Int32 v,
    /* [in] */ IPaintFontMetricsInt* fm,
    /* [in] */ ITextPaint* paint)
{
    Int32 size = mSize;
    if (paint != NULL) {
        Float density;
        paint->GetDensity(&density);
        size *= density;
    }

    Int32 bottom, top;
    fm->GetBottom(&bottom);
    fm->GetTop(&top);
    if (bottom - top < size) {
        fm->SetTop(bottom - size);
        Int32 ascent;
        fm->GetAscent(&ascent);
        fm->SetAscent(ascent - size);
    }
    else {
        if (sProportion == 0) {
            /*
             * Calculate what fraction of the nominal ascent
             * the height of a capital letter actually is,
             * so that we won't reduce the ascent to less than
             * that unless we absolutely have to.
             */
            AutoPtr<IPaint> p;
            // CPaint::New((IPaint**)&p);
            p->SetTextSize(100);
            AutoPtr<IRect> r;
            // CRect::New((IRect**)&r);
            p->GetTextBounds(String("ABCDEFG"), 0, 7, r);

            Float paintAscent;
            p->Ascent(&paintAscent);
            // sProportion = ((CRect*)r.Get())->mTop / paintAscent;
        }

        fm->GetTop(&top);
        Int32 need = (Int32)Elastos::Core::Math::Ceil(-top * sProportion);

        Int32 descent;
        fm->GetDescent(&descent);
        if (size - descent >= need) {
            /*
             * It is safe to shrink the ascent this much.
             */

            fm->GetBottom(&bottom);
            fm->SetTop(bottom - size);
            fm->SetAscent(descent - size);
        }
        else if (size >= need) {
            /*
             * We can't show all the descent, but we can at least
             * show all the ascent.
             */
            fm->SetAscent(-need);
            fm->SetTop(-need);
            fm->GetTop(&top);
            fm->SetDescent(top + size);
            fm->SetBottom(top + size);
        }
        else {
            /*
             * Show as much of the ascent as we can, and no descent.
             */
            fm->SetAscent(-size);
            fm->SetTop(-size);
            fm->SetAscent(0);
            fm->SetBottom(0);
        }
    }

    return NOERROR;
}

const String StringBlock::TAG("StringBlock");
const Boolean StringBlock::sLocalLOGV = FALSE;

StringBlock::StringBlock(
    /* [in] */ const ArrayOf<Byte> & data,
    /* [in] */ Boolean useSparse)
    : mUseSparse(useSparse)
    , mOwnsNative(TRUE)
{
    mNative = NativeCreate(data, 0, data.GetLength());
}

StringBlock::StringBlock(
    /* [in] */ const ArrayOf<Byte> & data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ Boolean useSparse)
    : mUseSparse(useSparse)
    , mOwnsNative(TRUE)
{
    mNative = NativeCreate(data, offset, size);
}

StringBlock::StringBlock(
    /* [in] */ Handle64 obj,
    /* [in] */ Boolean useSparse)
    : mNative(obj)
    , mUseSparse(useSparse)
    , mOwnsNative(FALSE)
{
}

StringBlock::~StringBlock()
{
    mStrings = NULL;
    mSparseStrings = NULL;

    if (mOwnsNative) {
        NativeDestroy(mNative);
    }
}

AutoPtr<ICharSequence> StringBlock::Get(
    /* [in] */ Int32 idx)
{
    AutoLock lock(mSyncLock);

    if (mStrings != NULL) {
        if (idx < mStrings->GetLength()) {
            AutoPtr<ICharSequence> res = (*mStrings)[idx];
            if (res != NULL) {
                return res;
            }
        }
        else {
            Slogger::E(TAG, "Get(Int32 index): index %d out of bound %d.",
                idx, mStrings->GetLength());
            assert(0);
        }
    }
    else if (mSparseStrings != NULL) {
        HashMap<Int32, AutoPtr<ICharSequence> >::Iterator it
                = mSparseStrings->Find(idx);
        if (it != mSparseStrings->End()) {
            return it->mSecond;
        }
    }
    else {
        Int32 num = NativeGetSize(mNative);
        if (mUseSparse && num > 250) {
            mSparseStrings = new HashMap<Int32, AutoPtr<ICharSequence> >();
        }
        else {
            mStrings = ArrayOf<ICharSequence*>::Alloc(num);
        }
    }

    String str = NativeGetString(mNative, idx);
    assert(!str.IsNull());
    AutoPtr<ICharSequence> res;
    CString::New(str, (ICharSequence**)&res);
    AutoPtr< ArrayOf<Int32> > style = NativeGetStyle(mNative, idx);
    if (sLocalLOGV) {
        Slogger::V(TAG, "Got string: %s", (const char*)str);
        if (style) {
            for (Int32 i = 0; i < style->GetLength(); ++i) {
                Slogger::V(TAG, "Got styles: %d", (*style)[i]);
            }
        }
    }

    if (style != NULL) {
        if (mStyleIDs == NULL) {
            mStyleIDs = new StyleIDs();
        }

        // the style array is a flat array of <type, start, end> hence
        // the magic constant 3.
        for (Int32 styleIndex = 0; styleIndex < style->GetLength(); styleIndex += 3) {
            Int32 styleId = (*style)[styleIndex];

            if (styleId == mStyleIDs->boldId || styleId == mStyleIDs->italicId
                    || styleId == mStyleIDs->underlineId || styleId == mStyleIDs->ttId
                    || styleId == mStyleIDs->bigId || styleId == mStyleIDs->smallId
                    || styleId == mStyleIDs->subId || styleId == mStyleIDs->supId
                    || styleId == mStyleIDs->strikeId || styleId == mStyleIDs->listItemId
                    || styleId == mStyleIDs->marqueeId) {
                // id already found skip to next style
                continue;
            }

            String styleTag = NativeGetString(mNative, styleId);

            if (styleTag.Equals("b")) {
                mStyleIDs->boldId = styleId;
            }
            else if (styleTag.Equals("i")) {
                mStyleIDs->italicId = styleId;
            }
            else if (styleTag.Equals("u")) {
                mStyleIDs->underlineId = styleId;
            }
            else if (styleTag.Equals("tt")) {
                mStyleIDs->ttId = styleId;
            }
            else if (styleTag.Equals("big")) {
                mStyleIDs->bigId = styleId;
            }
            else if (styleTag.Equals("small")) {
                mStyleIDs->smallId = styleId;
            }
            else if (styleTag.Equals("sup")) {
                mStyleIDs->supId = styleId;
            }
            else if (styleTag.Equals("sub")) {
                mStyleIDs->subId = styleId;
            }
            else if (styleTag.Equals("strike")) {
                mStyleIDs->strikeId = styleId;
            }
            else if (styleTag.Equals("li")) {
                mStyleIDs->listItemId = styleId;
            }
            else if (styleTag.Equals("marquee")) {
                mStyleIDs->marqueeId = styleId;
            }
        }

        res = ApplyStyles(str, style, mStyleIDs);
    }
    if (mStrings != NULL) mStrings->Set(idx, res);
    else (*mSparseStrings)[idx] = res;
    return res;
}

AutoPtr<ICharSequence> StringBlock::ApplyStyles(
    /* [in] */ const String& str,
    /* [in] */ ArrayOf<Int32>* style,
    /* [in] */ StyleIDs* ids)
{
    if (style->GetLength() == 0) {
        assert(!str.IsNull());
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        return cs;
    }

    assert(0 && "TODO");
    AutoPtr<ISpannable> buffer;
    // CSpannableString(str, (ISpannable**)&buffer);
    Int32 i=0;
    while (i < style->GetLength()) {
        Int32 type = (*style)[i];
        if (sLocalLOGV) {
            Slogger::V(TAG, "Applying style span id = %d, start = %d, end = %d"
                , type, (*style)[i+1], (*style)[i+2]);
        }

        if (type == ids->boldId) {
            assert(0);
            // AutoPtr<IStyleSpan> span;
            // CStyleSpan::New((ITypeface::BOLD), (IStyleSpan**)&span);
            // buffer->SetSpan(span, (*style)[i + 1], (*style)[i + 2] + 1,
            //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
        }
        else if (type == ids->italicId) {
            assert(0);
            // AutoPtr<IStyleSpan> span;
            // CStyleSpan::New((ITypeface::ITALIC), (IStyleSpan**)&span);
            // buffer->SetSpan(span, (*style)[i + 1], (*style)[i + 2] + 1,
            //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
        }
        else if (type == ids->underlineId) {
            assert(0);
            // AutoPtr<IUnderlineSpan> span;
            // CUnderlineSpan::New((IUnderlineSpan**)&span);
            // buffer->SetSpan(span, (*style)[i + 1], (*style)[i + 2] + 1,
            //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
        }
        else if (type == ids->ttId) {
            assert(0);
            // AutoPtr<ITypefaceSpan> span;
            // CTypefaceSpan::New(String("monospace"), (ITypefaceSpan**)&span);
            // buffer->SetSpan(span, (*style)[i + 1], (*style)[i + 2] + 1,
            //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
        }
        else if (type == ids->bigId) {
            assert(0);
            // AutoPtr<IRelativeSizeSpan> span
            // CRelativeSizeSpan::New(1.25, (IRelativeSizeSpan**)&span);
            // buffer->SetSpan(span, (*style)[i + 1], (*style)[i + 2] + 1,
            //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
        }
        else if (type == ids->smallId) {
            assert(0);
            // AutoPtr<IRelativeSizeSpan> span;
            // CRelativeSizeSpan::New(0.8, (IRelativeSizeSpan**)&span);
            // buffer->SetSpan(span, (*style)[i + 1], (*style)[i + 2] + 1,
            //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
        }
        else if (type == ids->subId) {
            assert(0);
            // AutoPtr<ISubscriptSpan> span;
            // CSubscriptSpan::New((ISubscriptSpan**)&span);
            // buffer->SetSpan(span, (*style)[i + 1], (*style)[i + 2] + 1,
            //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
        }
        else if (type == ids->supId) {
            assert(0);
            // AutoPtr<ISuperscriptSpan> span;
            // CSuperscriptSpan::New((ISuperscriptSpan**)&span);
            // buffer->SetSpan(span, (*style)[i + 1], (*style)[i + 2] + 1,
            //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
        }
        else if (type == ids->strikeId) {
            assert(0);
            // AutoPtr<IStrikethroughSpan> span;
            // CStrikethroughSpan::New((IStrikethroughSpan**)&span);
            // buffer->SetSpan(span, (*style)[i + 1], (*style)[i + 2] + 1,
            //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
        }
        else if (type == ids->listItemId) {
            assert(0);
            // AutoPtr<IBulletSpan> span;
            // CBulletSpan::New(10, (IStrikethroughSpan**)&span);
            // AddParagraphSpan(buffer, span, (*style)[i + 1], (*style)[i + 2] + 1);
        }
        else if (type == ids->marqueeId) {
            assert(0);
            // buffer->SetSpan(TextUtilsTruncateAt_MARQUEE,
            //         (*style)[i + 1], (*style)[i + 2] + 1,
            //         ISpanned::SPAN_INCLUSIVE_INCLUSIVE);
        }
        else {
            String tag = NativeGetString(mNative, type);

            if (tag.StartWith("font;")) {
                String sub = Subtag(tag, String(";height="));
                if (!sub.IsNull()) {
                    Int32 size = StringUtils::ParseInt32(sub);
                    AutoPtr<Height> h = new Height(size);
                    AddParagraphSpan(buffer, TO_IINTERFACE(h), (*style)[i + 1], (*style)[i + 2] + 1);
                }

                sub = Subtag(tag, String(";size="));
                if (!sub.IsNull()) {
                    assert(0);
                    // Int32 size = StringUtils::ParseInt32(sub);
                    // AutoPtr<IAbsoluteSizeSpan> span;
                    // CAbsoluteSizeSpan::New(size, TRUE, (IAbsoluteSizeSpan**)&span);
                    // buffer->SetSpan(span, (*style)[i + 1], (*style)[i + 2] + 1,
                    //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
                }

                sub = Subtag(tag, String(";fgcolor="));
                if (!sub.IsNull()) {
                    assert(0);
                    // AutoPtr<ICharacterStyle> color = GetColor(sub, TRUE);
                    // buffer->SetSpan(color,
                    //    (*style)[i + 1], (*style)[i + 2] + 1,
                    //    ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
                }

                sub = Subtag(tag, String(";color="));
                if (!sub.IsNull()) {
                    assert(0);
                    // AutoPtr<ICharacterStyle> color = GetColor(sub, TRUE);
                    // buffer->SetSpan(color,
                    //    (*style)[i + 1], (*style)[i + 2] + 1,
                    //    ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
                }

                sub = Subtag(tag, String(";bgcolor="));
                if (!sub.IsNull()) {
                    assert(0);
                    // AutoPtr<ICharacterStyle> color = GetColor(sub, FALSE);
                    // buffer->SetSpan(color,
                    //    (*style)[i + 1], (*style)[i + 2] + 1,
                    //    ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
                }
                sub = Subtag(tag, String(";face="));
                if (!sub.IsNull()) {
                    assert(0);
                    // AutoPtr<ITypefaceSpan> span;
                    // CTypefaceSpan::New(sub, (ITypefaceSpan**)&span);
                    // buffer->SetSpan(span,
                    //    (*style)[i + 1], (*style)[i + 2] + 1,
                    //    ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
                }
            }
            else if (tag.StartWith("a;")) {
                String sub = Subtag(tag, String(";href="));
                if (!sub.IsNull()) {
                    assert(0);
                    // AutoPtr<IURLSpan> span;
                    // CURLSpan::New(color, (IURLSpan**)&span);
                    // buffer->SetSpan(span, (*style)[i + 1], (*style)[i + 2] + 1,
                    //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
                }
            }
            else if (tag.StartWith("annotation;")) {
                Int32 len = tag.GetLength();
                Int32 next;

                for (Int32 t = tag.IndexOf(';'); t < len; t = next) {
                    Int32 eq = tag.IndexOf('=', t);
                    if (eq < 0) {
                        break;
                    }

                    next = tag.IndexOf(';', eq);
                    if (next < 0) {
                        next = len;
                    }

                    String key = tag.Substring(t + 1, eq);
                    String value = tag.Substring(eq + 1, next);

                    assert(0);
                    // AutoPtr<IAnnotation> annotation;
                    // CAnnotation::New(key, value, (IAnnotation**)&annotation);
                    // buffer->SetSpan(annotation, (*style)[i + 1], (*style)[i + 2] + 1,
                    //         ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
                }
            }
        }

        i += 3;
    }

    // Auto<ISpannable> result;
    // CSpannableString(buffer, (ISpannable**)&result);
    assert(0);
    return NULL;
}

AutoPtr<ICharacterStyle> StringBlock::GetColor(
    /* [in] */ const String& color,
    /* [in] */ Boolean foreground)
{
    Int32 c = 0xff000000;

    assert(0 && "TODO");
    // if (!TextUtils::IsEmpty(color)) {
    //     if (color.StartWith("@")) {
    //         AutoPtr<IResources> res = CResources::GetSystem();
    //         String name = color.Substring(1);
    //         Int32 colorRes;
    //         res->GetIdentifier(name, String("color"), String("android"), &colorRes);
    //         if (colorRes != 0) {
    //             AutoPtr<IColorStateList> colors;
    //             res->GetColorStateList(colorRes, (IColorStateList**)&colors);
    //             if (foreground) {
    //                 AutoPtr<ICharacterStyle> style;
    //                 CTextAppearanceSpan::New(String(NULL), 0, 0, colors, NULL, (ICharacterStyle**)&style);
    //                 return style;
    //             }
    //             else {
    //                 colors->GetDefaultColor(&c);
    //             }
    //         }
    //     }
    //     else {
    //         //c = Color::GetHtmlColor(color);
    //     }
    // }

    AutoPtr<ICharacterStyle> style;
    // if (foreground) {
    //     CForegroundColorSpan::New(c, (ICharacterStyle**)&style);
    //     return NOERROR;
    // }
    // else {
    //     CBackgroundColorSpan::New(c, (ICharacterStyle**)&style);
    // }
    return style;
}

void StringBlock::AddParagraphSpan(
    /* [in] */ ISpannable* buffer,
    /* [in] */ IInterface* what,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    // Int32 len;
    // buffer->GetLength(&len);
    // Char32 c;
    // buffer->GetCharAt(start - 1, &c);
    // if (start != 0 && start != len && c != '\n') {
    //     for (start--; start > 0; start--) {
    //         buffer->GetCharAt(start - 1, &c);
    //         if (c == '\n') {
    //             break;
    //         }
    //     }
    // }

    // buffer->GetCharAt(end - 1, &c);
    // if (end != 0 && end != len && c != '\n') {
    //     for (end++; end < len; end++) {
    //         buffer->GetCharAt(end - 1, &c);
    //         if (c == '\n') {
    //             break;
    //         }
    //     }
    // }

    // buffer->SetSpan(what, start, end, ISpanned::SPAN_PARAGRAPH);
    assert(0);
}

String StringBlock::Subtag(
    /* [in] */ const String& full,
    /* [in] */ const String& attribute)
{
    Int32 start = full.IndexOf(attribute);
    if (start < 0) {
        return String(NULL);
    }

    start += attribute.GetLength();
    Int32 end = full.IndexOf(';', start);

    if (end < 0) {
        return full.Substring(start);
    }
    else {
        return full.Substring(start, end);
    }
}

Handle64 StringBlock::NativeCreate(
    /* [in] */ const ArrayOf<Byte>& data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size)
{
    Int32 len = data.GetLength();
    if (offset < 0 || offset >= len || size < 0 || size > len || (offset + size) > len) {
        assert(0);
        // jniThrowException(env, "java/lang/IndexOutOfBoundsException", NULL);
        return 0;
    }

    android::ResStringPool* osb = new android::ResStringPool(
            data.GetPayload() + offset, size, TRUE);

    if (osb == NULL || osb->getError() != android::NO_ERROR) {
        assert(0);
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return 0;
    }

    return (Handle64)osb;
}

Int32 StringBlock::NativeGetSize(
    /* [in] */ Handle64 pool)
{
    assert(pool);
    return ((android::ResStringPool*)pool)->size();
}

String StringBlock::NativeGetString(
    /* [in] */ Handle64 pool,
    /* [in] */ Int32 idx)
{
    assert(pool);

    size_t len;
    const char* str8 = ((android::ResStringPool*)pool)->string8At(idx, &len);
    if (str8 != NULL) {
        return String(str8);
    }

    const char16_t* str16 = ((android::ResStringPool*)pool)->stringAt(idx, &len);
    if (str16 == NULL) {
//          doThrow(env, "java/lang/IndexOutOfBoundsException");
        assert(0);
        return String(NULL);
    }

    android::String8 s8(str16);
    return String(s8.string());
}

AutoPtr<ArrayOf<Int32> > StringBlock::NativeGetStyle(
    /* [in] */ Handle64 pool,
    /* [in] */ Int32 idx)
{
    assert(pool);

    android::ResStringPool* osb = (android::ResStringPool*)pool;
    const android::ResStringPool_span* spans = osb->styleAt(idx);
    if (spans == NULL) {
        return NULL;
    }

    const android::ResStringPool_span* pos = spans;
    Int32 num = 0;
    while (pos->name.index != android::ResStringPool_span::END) {
        num++;
        pos++;
    }

    if (num == 0) {
        return NULL;
    }

    static const Int32 numInts = sizeof(android::ResStringPool_span) / sizeof(Int32);
    AutoPtr< ArrayOf<Int32> > newArray = ArrayOf<Int32>::Alloc(num * numInts);
    if (newArray == NULL) { // NewIntArray already threw OutOfMemoryError.
        return NULL;
    }

    num = 0;
    while (spans->name.index != android::ResStringPool_span::END) {
        memcpy(newArray->GetPayload() + num * numInts, spans, numInts * sizeof(Int32));
        spans++;
        num++;
    }

    return newArray;
}

void StringBlock::NativeDestroy(
    /* [in] */ Handle64 pool)
{
    assert(pool);
    delete (android::ResStringPool*)pool;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

