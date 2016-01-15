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

    AutoPtr<ImageUtils> mImageUtils;
    AutoPtr<IWeakHashMap> mGrayscaleBitmapCache; //WeakHashMap<Bitmap, Pair<Boolean, Integer>>

    Int32 mGrayscaleIconMaxSize; // @dimen/notification_large_icon_width (64dp)
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_NOTIFICATIONCOLORUTIL_H__
