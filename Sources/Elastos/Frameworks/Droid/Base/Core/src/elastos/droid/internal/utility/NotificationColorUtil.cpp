
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/internal/utility/NotificationColorUtil.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/style/CTextAppearanceSpan.h"
#include "elastos/droid/utility/CPairHelper.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Color;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::Drawable::IAnimationDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IVectorDrawable;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::Style::CTextAppearanceSpan;
using Elastos::Droid::Utility::CPairHelper;
using Elastos::Droid::Utility::IPairHelper;
using Elastos::Droid::Utility::IPair;
using Elastos::Core::AutoLock;
using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Utility::CWeakHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(NotificationColorUtil, Object, INotificationColorUtil)

const String NotificationColorUtil::TAG("NotificationColorUtil");
Object NotificationColorUtil::sLock;
AutoPtr<INotificationColorUtil> NotificationColorUtil::sInstance;

NotificationColorUtil::NotificationColorUtil(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetDimensionPixelSize(
        R::dimen::notification_large_icon_width, &mGrayscaleIconMaxSize);
    mImageUtils = new ImageUtils();
    CWeakHashMap::New((IWeakHashMap**)&mGrayscaleBitmapCache);
}

AutoPtr<INotificationColorUtil> NotificationColorUtil::GetInstance(
    /* [in] */ IContext* context)
{
    AutoLock lock(sLock);
    if (sInstance == NULL) {
        sInstance = new NotificationColorUtil(context);
    }
    return sInstance;
}

ECode NotificationColorUtil::IsGrayscaleIcon(
    /* [in] */ IBitmap* bitmap,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // quick test: reject large bitmaps
    Int32 w, h;
    bitmap->GetWidth(&w);
    bitmap->GetHeight(&h);
    if (w > mGrayscaleIconMaxSize || h > mGrayscaleIconMaxSize) {
        *result = FALSE;
        return NOERROR;
    }

    {
        AutoLock lock(sLock);
        AutoPtr<IInterface> item;
        mGrayscaleBitmapCache->Get(bitmap, (IInterface**)&item);
        AutoPtr<IPair> cached = IPair::Probe(item);
        if (cached != NULL) {
            AutoPtr<IInterface> second;
            cached->GetSecond((IInterface**)&second);
            Int32 secondValue;
            IInteger32::Probe(second)->GetValue(&secondValue);
            Int32 id;
            bitmap->GetGenerationId(&id);
            if (secondValue == id) {
                AutoPtr<IInterface> first;
                cached->GetFirst((IInterface**)&first);
                return IBoolean::Probe(first)->GetValue(result);
            }
        }
    }
    Boolean res;
    Int32 generationId;
    {
        AutoLock lock(mImageUtils);
        res = mImageUtils->IsGrayscale(bitmap);

        // generationId and the check whether the Bitmap is grayscale can't be read atomically
        // here. However, since the thread is in the process of posting the notification, we can
        // assume that it doesn't modify the bitmap while we are checking the pixels.
        bitmap->GetGenerationId(&generationId);
    }
    {
        AutoPtr<IPairHelper> pairHelper;
        CPairHelper::AcquireSingleton((IPairHelper**)&pairHelper);
        AutoPtr<IBoolean> first;
        CBoolean::New(res, (IBoolean**)&first);
        AutoPtr<IInteger32> second;
        CInteger32::New(generationId, (IInteger32**)&second);
        AutoPtr<IPair> item;
        pairHelper->Create(first, second, (IPair**)&item);
        AutoLock lock(sLock);
        mGrayscaleBitmapCache->Put(bitmap, item);
    }
    *result = res;
    return NOERROR;
}

ECode NotificationColorUtil::IsGrayscaleIcon(
    /* [in] */ IDrawable* d,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (d == NULL) {
        // do nothing
    }
    else if (IBitmapDrawable::Probe(d)) {
        AutoPtr<IBitmapDrawable> bd = IBitmapDrawable::Probe(d);
        AutoPtr<IBitmap> bitmap;
        bd->GetBitmap((IBitmap**)&bitmap);
        if (bitmap != NULL)
            IsGrayscaleIcon(bitmap, result);
    }
    else if (IAnimationDrawable::Probe(d)) {
        AutoPtr<IAnimationDrawable> ad = IAnimationDrawable::Probe(d);
        Int32 count;
        ad->GetNumberOfFrames(&count);
        if (count > 0) {
            AutoPtr<IDrawable> drawable;
            ad->GetFrame(0, (IDrawable**)&drawable);
            IsGrayscaleIcon(drawable, result);
        }
    }
    else if (IVectorDrawable::Probe(d)) {
        // We just assume you're doing the right thing if using vectors
        *result = TRUE;
    }
    return NOERROR;
}

ECode NotificationColorUtil::IsGrayscaleIcon(
    /* [in] */ IContext* context,
    /* [in] */ Int32 drawableResId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (drawableResId != 0) {
        AutoPtr<IDrawable> drawable;
        context->GetDrawable(drawableResId, (IDrawable**)&drawable);
        if (IsGrayscaleIcon(drawable, result)
            == (ECode)E_RESOURCES_NOT_FOUND_EXCEPTION) {
            Logger::E(TAG, "Drawable not found: %d", drawableResId);
            *result = FALSE;
        }
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode NotificationColorUtil::InvertCharSequenceColors(
    /* [in] */ ICharSequence* charSequence,
    /* [out] */ ICharSequence** outCs)
{
    VALIDATE_NOT_NULL(outCs)
    if (ISpanned::Probe(charSequence)) {
        AutoPtr<ISpanned> ss = ISpanned::Probe(charSequence);
        Int32 length;
        charSequence->GetLength(&length);
        AutoPtr<ArrayOf<IInterface*> > spans;
        ss->GetSpans(0, length, EIID_IInterface, (ArrayOf<IInterface*>**)&spans);
        AutoPtr<ISpannableStringBuilder> builder;
        CSpannableStringBuilder::New(charSequence, (ISpannableStringBuilder**)&builder);
        for (Int32 i = 0; i < spans->GetLength(); i++) {
            AutoPtr<IInterface> span = (*spans)[i];
            AutoPtr<IInterface> resultSpan = span;
            if (ITextAppearanceSpan::Probe(span)) {
                resultSpan = ProcessTextAppearanceSpan(ITextAppearanceSpan::Probe(span));
            }
            Int32 start, end, flags;
            ss->GetSpanStart(span, &start);
            ss->GetSpanEnd(span, &end);
            ss->GetSpanFlags(span, &flags);
            ISpannable::Probe(builder)->SetSpan(resultSpan, start, end, flags);
        }
        *outCs = ICharSequence::Probe(builder);
    }
    else
        *outCs = charSequence;
    REFCOUNT_ADD(*outCs)
    return NOERROR;
}

AutoPtr<ITextAppearanceSpan> NotificationColorUtil::ProcessTextAppearanceSpan(
    /* [in] */ ITextAppearanceSpan* span)
{
    AutoPtr<IColorStateList> colorStateList;
    span->GetTextColor((IColorStateList**)&colorStateList);
    if (colorStateList != NULL) {
        AutoPtr<ArrayOf<Int32> > colors = ((CColorStateList*)colorStateList.Get())->GetColors();
        Boolean changed = FALSE;
        for (Int32 i = 0; i < colors->GetLength(); i++) {
            if (ImageUtils::IsGrayscale((*colors)[i])) {

                // Allocate a new array so we don't change the colors in the old color state
                // list.
                if (!changed) {
                    colors = colors->Clone();
                }
                (*colors)[i] = ProcessColor((*colors)[i]);
                changed = TRUE;
            }
        }
        if (changed) {
            String family;
            span->GetFamily(&family);
            Int32 style, size;
            span->GetTextStyle(&style);
            span->GetTextSize(&size);
            AutoPtr<IColorStateList> newColorStateList;
            AutoPtr<ArrayOf<Int32Array > > states = ((CColorStateList*)colorStateList.Get())->GetStates();
            AutoPtr<ArrayOf<Handle32> >  states2 = ArrayOf<Handle32>::Alloc(states->GetLength());
            for (Int32 i = 0; i < states->GetLength(); i++)
                (*states2)[i] = (Handle32)(*states)[i].Get();
            CColorStateList::New(states2, colors, (IColorStateList**)&newColorStateList);
            AutoPtr<IColorStateList> textColor;
            span->GetLinkTextColor((IColorStateList**)&textColor);
            AutoPtr<ITextAppearanceSpan> newSpan;
            CTextAppearanceSpan::New(family, style, size,
                newColorStateList, textColor, (ITextAppearanceSpan**)&newSpan);
            return newSpan;
        }
    }
    return span;
}

Int32 NotificationColorUtil::ProcessColor(
    /* [in] */ Int32 color)
{
    return Color::Argb(Color::Alpha(color),
            255 - Color::Red(color),
            255 - Color::Green(color),
            255 - Color::Blue(color));
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
