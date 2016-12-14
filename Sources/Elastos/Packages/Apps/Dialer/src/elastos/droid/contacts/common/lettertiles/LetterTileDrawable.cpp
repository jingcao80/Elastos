
#include "elastos/droid/contacts/common/lettertiles/LetterTileDrawable.h"
#include <elastos/droid/text/TextUtils.h>
// #include "eunit/framework/Assert.h"
#include <elastos/core/Math.h>
#include <elastos/core/Character.h>
#include "R.h"

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::ITypefaceHelper;
using Elastos::Droid::Graphics::CTypefaceHelper;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Graphics::PaintAlign_CENTER;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::Character;
// using Eunit::Framework::Assert;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Lettertiles {

const Int32 LetterTileDrawable::TYPE_PERSON = 1;
const Int32 LetterTileDrawable::TYPE_BUSINESS = 2;
const Int32 LetterTileDrawable::TYPE_VOICEMAIL = 3;
const Int32 LetterTileDrawable::TYPE_DEFAULT = LetterTileDrawable::TYPE_PERSON;
const String LetterTileDrawable::TAG("LetterTileDrawable");
AutoPtr<ITypedArray> LetterTileDrawable::sColors;
Int32 LetterTileDrawable::sDefaultColor;
Int32 LetterTileDrawable::sTileFontColor;
Float LetterTileDrawable::sLetterToTileRatio;
AutoPtr<IBitmap> LetterTileDrawable::DEFAULT_PERSON_AVATAR;
AutoPtr<IBitmap> LetterTileDrawable::DEFAULT_BUSINESS_AVATAR;
AutoPtr<IBitmap> LetterTileDrawable::DEFAULT_VOICEMAIL_AVATAR;

static AutoPtr<IPaint> InitPaint()
{
    AutoPtr<IPaint> paint;
    CPaint::New((IPaint**)&paint);
    return paint;
}
AutoPtr<IPaint> LetterTileDrawable::sPaint = InitPaint();

static AutoPtr<IRect> InitRect()
{
    AutoPtr<IRect> paint;
    CRect::New((IRect**)&paint);
    return paint;
}
AutoPtr<IRect> LetterTileDrawable::sRect = InitRect();
AutoPtr<ArrayOf<Char32> > LetterTileDrawable::sFirstChar = ArrayOf<Char32>::Alloc(1);

LetterTileDrawable::LetterTileDrawable(
    /* [in] */ IResources* res)
    : mContactType(TYPE_DEFAULT)
    , mScale(1.0f)
    , mOffset(0.0f)
    , mIsCircle(FALSE)
{
    CPaint::New((IPaint**)&mPaint);
    mPaint->SetFilterBitmap(TRUE);
    mPaint->SetDither(TRUE);

    if (sColors == NULL) {
        res->ObtainTypedArray(Elastos::Droid::Dialer::R::array::letter_tile_colors, (ITypedArray**)&sColors);
        res->GetColor(Elastos::Droid::Dialer::R::color::letter_tile_default_color, &sDefaultColor);
        res->GetColor(Elastos::Droid::Dialer::R::color::letter_tile_font_color, &sTileFontColor);
        res->GetFraction(Elastos::Droid::Dialer::R::dimen::letter_to_tile_ratio, 1, 1, &sLetterToTileRatio);

        AutoPtr<IBitmapFactory> factory;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
        factory->DecodeResource(res, Elastos::Droid::Dialer::R::drawable::ic_person_white_120dp,
                (IBitmap**)&DEFAULT_PERSON_AVATAR);
        factory->DecodeResource(res, Elastos::Droid::Dialer::R::drawable::ic_business_white_120dp,
                (IBitmap**)&DEFAULT_BUSINESS_AVATAR);
        factory->DecodeResource(res, Elastos::Droid::Dialer::R::drawable::ic_voicemail_avatar,
                (IBitmap**)&DEFAULT_VOICEMAIL_AVATAR);
        String family;
        res->GetString(Elastos::Droid::Dialer::R::string::letter_tile_letter_font_family, &family);
        AutoPtr<ITypefaceHelper> helper;
        CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&helper);
        AutoPtr<ITypeface> typeface;
        helper->Create(family, ITypeface::NORMAL, (ITypeface**)&typeface);
        sPaint->SetTypeface(typeface);
        sPaint->SetTextAlign(PaintAlign_CENTER);
        sPaint->SetAntiAlias(TRUE);
    }
}

ECode LetterTileDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    Boolean isVisible, isEmpty;
    if ((IsVisible(&isVisible), !isVisible) || (bounds->IsEmpty(&isEmpty), isEmpty)) {
        return NOERROR;
    }
    // Draw letter tile.
    DrawLetterTile(canvas);
    return NOERROR;
}

void LetterTileDrawable::DrawBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ ICanvas* canvas)
{
    // The bitmap should be drawn in the middle of the canvas without changing its width to
    // height ratio.
    AutoPtr<IRect> destRect;
    CopyBounds((IRect**)&destRect);

    // Crop the destination bounds into a square, scaled and offset as appropriate
    Int32 w;
    destRect->GetWidth(&w);
    Int32 h;
    destRect->GetHeight(&h);
    Int32 halfLength = (Int32)(mScale * Elastos::Core::Math::Min(w, h) / 2);

    Int32 centerX;
    destRect->GetCenterX(&centerX);
    Int32 centerY;
    destRect->GetCenterY(&centerY);
    destRect->GetHeight(&h);
    destRect->Set(centerX - halfLength,
            (Int32)(centerY - halfLength + mOffset * h),
            centerX + halfLength,
            (Int32)(centerY + halfLength + mOffset * h));

    // Source rectangle remains the entire bounds of the source bitmap.
    sRect->Set(0, 0, width, height);

    canvas->DrawBitmap(bitmap, sRect, destRect, mPaint);
}

void LetterTileDrawable::DrawLetterTile(
    /* [in] */ ICanvas* canvas)
{
    // Draw background color.
    sPaint->SetColor(PickColor(mIdentifier));

    Int32 alpha;
    mPaint->GetAlpha(&alpha);
    sPaint->SetAlpha(alpha);
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    Int32 width, height;
    bounds->GetWidth(&width);
    bounds->GetHeight(&height);
    Int32 minDimension = Elastos::Core::Math::Min(width, height);

    if (mIsCircle) {
        Int32 centerX, centerY;
        bounds->GetCenterX(&centerX);
        bounds->GetCenterY(&centerY);
        canvas->DrawCircle(centerX, centerY, minDimension / 2, sPaint);
    }
    else {
        canvas->DrawRect(bounds, sPaint);
    }

    // Draw letter/digit only if the first character is an english letter
    if (!mDisplayName.IsNull() && IsEnglishLetter(mDisplayName.GetChar(0))) {
        // Draw letter or digit.
        (*sFirstChar)[0] = Character::ToUpperCase(mDisplayName.GetChar(0));

        // Scale text by canvas bounds and user selected scaling factor
        sPaint->SetTextSize(mScale * sLetterToTileRatio * minDimension);
        //sPaint.setTextSize(sTileLetterFontSize);
        sPaint->GetTextBounds(sFirstChar, 0, 1, sRect);
        sPaint->SetColor(sTileFontColor);

        // Draw the letter in the canvas, vertically shifted up or down by the user-defined
        // offset
        Int32 centerX, centerY;
        bounds->GetCenterX(&centerX);
        bounds->GetCenterY(&centerY);
        bounds->GetHeight(&height);
        Int32 h;
        sRect->GetHeight(&h);
        canvas->DrawText(sFirstChar, 0, 1, centerX,
                centerY + mOffset * height + h / 2,
                sPaint);
    }
    else {
        // Draw the default image if there is no letter/digit to be drawn
        AutoPtr<IBitmap> bitmap = GetBitmapForContactType(mContactType);
        Int32 width, height;
        bitmap->GetWidth(&width);
        bitmap->GetHeight(&height);
        DrawBitmap(bitmap, width, height, canvas);
    }
}

Int32 LetterTileDrawable::GetColor()
{
    return PickColor(mIdentifier);
}

Int32 LetterTileDrawable::PickColor(
    /* [in] */ const String& identifier)
{
    if (TextUtils::IsEmpty(identifier) || mContactType == TYPE_VOICEMAIL) {
        return sDefaultColor;
    }
    // String.hashCode() implementation is not supposed to change across java versions, so
    // this should guarantee the same email address always maps to the same color.
    // The email should already have been normalized by the ContactRequest.
    Int32 len;
    sColors->GetLength(&len);
    Int32 color = Elastos::Core::Math::Abs(identifier.GetHashCode()) % len;
    Int32 c;
    sColors->GetColor(color, sDefaultColor, &c);
    return c;
}

AutoPtr<IBitmap> LetterTileDrawable::GetBitmapForContactType(
    /* [in] */ Int32 contactType)
{
    switch (contactType) {
        case TYPE_PERSON:
            return DEFAULT_PERSON_AVATAR;
        case TYPE_BUSINESS:
            return DEFAULT_BUSINESS_AVATAR;
        case TYPE_VOICEMAIL:
            return DEFAULT_VOICEMAIL_AVATAR;
        default:
            return DEFAULT_PERSON_AVATAR;
    }
}

Boolean LetterTileDrawable::IsEnglishLetter(
    /* [in] */ Char32 c)
{
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

ECode LetterTileDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return mPaint->SetAlpha(alpha);
}

ECode LetterTileDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return mPaint->SetColorFilter(cf);
}

ECode LetterTileDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity)
    *opacity = IPixelFormat::OPAQUE;
    return NOERROR;
}

void LetterTileDrawable::SetScale(
    /* [in] */ Float scale)
{
    mScale = scale;
}

void LetterTileDrawable::SetOffset(
    /* [in] */ Float offset)
{
    assert(offset >= -0.5f && offset <= 0.5f);
    mOffset = offset;
}

void LetterTileDrawable::SetContactDetails(
    /* [in] */ const String& displayName,
    /* [in] */ const String& identifier)
{
    mDisplayName = displayName;
    mIdentifier = identifier;
}

void LetterTileDrawable::SetContactType(
    /* [in] */ Int32 contactType)
{
    mContactType = contactType;
}

void LetterTileDrawable::SetIsCircular(
    /* [in] */ Boolean isCircle)
{
    mIsCircle = isCircle;
}

} // Lettertiles
} // Common
} // Contacts
} // Droid
} // Elastos