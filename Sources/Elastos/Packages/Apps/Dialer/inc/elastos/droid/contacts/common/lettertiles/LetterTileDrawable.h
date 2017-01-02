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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LETTERTILES_LETTERTILEDRAWABLE_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LETTERTILES_LETTERTILEDRAWABLE_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/graphics/drawable/Drawable.h"

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::Drawable::Drawable;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Lettertiles {

/**
 * A drawable that encapsulates all the functionality needed to display a letter tile to
 * represent a contact image.
 */
class LetterTileDrawable
    : public Drawable
{
public:
    LetterTileDrawable(
        /* [in] */ IResources* res);

    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    CARAPI_(Int32) GetColor();

    // @Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    // @Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    /**
     * Scale the drawn letter tile to a ratio of its default size
     *
     * @param scale The ratio the letter tile should be scaled to as a percentage of its default
     * size, from a scale of 0 to 2.0f. The default is 1.0f.
     */
    CARAPI_(void) SetScale(
        /* [in] */ Float scale);

    /**
     * Assigns the vertical offset of the position of the letter tile to the ContactDrawable
     *
     * @param offset The provided offset must be within the range of -0.5f to 0.5f.
     * If set to -0.5f, the letter will be shifted upwards by 0.5 times the height of the canvas
     * it is being drawn on, which means it will be drawn with the center of the letter starting
     * at the top edge of the canvas.
     * If set to 0.5f, the letter will be shifted downwards by 0.5 times the height of the canvas
     * it is being drawn on, which means it will be drawn with the center of the letter starting
     * at the bottom edge of the canvas.
     * The default is 0.0f.
     */
    CARAPI_(void) SetOffset(
        /* [in] */ Float offset);

    CARAPI_(void) SetContactDetails(
        /* [in] */ const String& displayName,
        /* [in] */ const String& identifier);

    CARAPI_(void) SetContactType(
        /* [in] */ Int32 contactType);

    CARAPI_(void) SetIsCircular(
        /* [in] */ Boolean isCircle);

private:
    /**
     * Draw the bitmap onto the canvas at the current bounds taking into account the current scale.
     */
    CARAPI_(void) DrawBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) DrawLetterTile(
        /* [in] */ ICanvas* canvas);

    /**
     * Returns a deterministic color based on the provided contact identifier string.
     */
    CARAPI_(Int32) PickColor(
        /* [in] */ const String& identifier);

    CARAPI_(AutoPtr<IBitmap>) GetBitmapForContactType(
        /* [in] */ Int32 contactType);

    static CARAPI_(Boolean) IsEnglishLetter(
        /* [in] */ Char32 c);

public:
    /** Contact type constants */
    static const Int32 TYPE_PERSON;
    static const Int32 TYPE_BUSINESS;
    static const Int32 TYPE_VOICEMAIL;
    static const Int32 TYPE_DEFAULT;

private:
    static const String TAG;

    /** Letter tile */
    static AutoPtr<ITypedArray> sColors;
    static Int32 sDefaultColor;
    static Int32 sTileFontColor;
    static Float sLetterToTileRatio;
    static AutoPtr<IBitmap> DEFAULT_PERSON_AVATAR;
    static AutoPtr<IBitmap> DEFAULT_BUSINESS_AVATAR;
    static AutoPtr<IBitmap> DEFAULT_VOICEMAIL_AVATAR;

    /** Reusable components to avoid new allocations */
    static AutoPtr<IPaint> sPaint;
    static AutoPtr<IRect> sRect;
    static AutoPtr<ArrayOf<Char32> > sFirstChar;

    AutoPtr<IPaint> mPaint;

    String mDisplayName;
    String mIdentifier;
    Int32 mContactType;
    Float mScale;
    Float mOffset;
    Boolean mIsCircle;
};

} // Lettertiles
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LETTERTILES_LETTERTILEDRAWABLE_H__
