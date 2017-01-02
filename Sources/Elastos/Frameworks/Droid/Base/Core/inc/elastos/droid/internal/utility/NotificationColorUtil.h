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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_NOTIFICATIONCOLORUTIL_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_NOTIFICATIONCOLORUTIL_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/utility/ImageUtils.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Text::Style::ITextAppearanceSpan;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IWeakHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class NotificationColorUtil
    : public Object
    , public INotificationColorUtil
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<INotificationColorUtil>) GetInstance(
        /* [in] */ IContext* context);

    /**
     * Checks whether a Bitmap is a small grayscale icon.
     * Grayscale here means "very close to a perfect gray"; icon means "no larger than 64dp".
     *
     * @param bitmap The bitmap to test.
     * @return True if the bitmap is grayscale; FALSE if it is color or too large to examine.
     */
    CARAPI IsGrayscaleIcon(
        /* [in] */ IBitmap* bitmap,
        /* [out] */ Boolean* result);

    /**
     * Checks whether a Drawable is a small grayscale icon.
     * Grayscale here means "very close to a perfect gray"; icon means "no larger than 64dp".
     *
     * @param d The drawable to test.
     * @return True if the bitmap is grayscale; FALSE if it is color or too large to examine.
     */
    CARAPI IsGrayscaleIcon(
        /* [in] */ IDrawable* d,
        /* [out] */ Boolean* result);

    /**
     * Checks whether a drawable with a resoure id is a small grayscale icon.
     * Grayscale here means "very close to a perfect gray"; icon means "no larger than 64dp".
     *
     * @param context The context to load the drawable from.
     * @return True if the bitmap is grayscale; FALSE if it is color or too large to examine.
     */
    CARAPI IsGrayscaleIcon(
        /* [in] */ IContext* context,
        /* [in] */ Int32 drawableResId,
        /* [out] */ Boolean* result);

    /**
     * Inverts all the grayscale colors set by {@link android.text.style.TextAppearanceSpan}s on
     * the text.
     *
     * @param charSequence The text to process.
     * @return The color inverted text.
     */
    CARAPI InvertCharSequenceColors(
        /* [in] */ ICharSequence* charSequence,
        /* [out] */ ICharSequence** outCs);

private:
    NotificationColorUtil(
        /* [in] */ IContext* context);

    CARAPI_(AutoPtr<ITextAppearanceSpan>) ProcessTextAppearanceSpan(
        /* [in] */ ITextAppearanceSpan* span);

    CARAPI_(Int32) ProcessColor(
        /* [in] */ Int32 color);

private:
    static const String TAG;

    static Object sLock;

    static AutoPtr<INotificationColorUtil> sInstance;

    AutoPtr<Elastos::Droid::Internal::Utility::ImageUtils> mImageUtils;
    AutoPtr<IWeakHashMap> mGrayscaleBitmapCache; //WeakHashMap<Bitmap, Pair<Boolean, Integer>>

    Int32 mGrayscaleIconMaxSize; // @dimen/notification_large_icon_width (64dp)
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_NOTIFICATIONCOLORUTIL_H__
