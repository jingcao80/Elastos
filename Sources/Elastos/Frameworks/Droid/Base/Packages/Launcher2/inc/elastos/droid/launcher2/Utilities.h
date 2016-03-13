
#ifndef  __ELASTOS_DROID_LAUNCHER2_UTILITIES_H__
#define  __ELASTOS_DROID_LAUNCHER2_UTILITIES_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Various utilities shared amongst the Launcher's classes.
 */
class Utilities
{
public:
    /**
     * Returns a bitmap suitable for the all apps view. Used to convert pre-ICS
     * icon bitmaps that are stored in the database (which were 74x74 pixels at hdpi size)
     * to the proper size (48dp)
     */
    static CARAPI_(AutoPtr<IBitmap>) CreateIconBitmap(
        /* [in] */ IBitmap* icon,
        /* [in] */ IContext* context);


    /**
     * Returns a bitmap suitable for the all apps view.
     */
    static CARAPI_(AutoPtr<IBitmap>) CreateIconBitmap(
        /* [in] */ IDrawable* icon,
        /* [in] */ IContext* context);

    static CARAPI DrawSelectedAllAppsBitmap(
        /* [in] */ ICanvas* dest,
        /* [in] */ Int32 destWidth,
        /* [in] */ Int32 destHeight,
        /* [in] */ Boolean pressed,
        /* [in] */ IBitmap* src);

    /**
     * Returns a Bitmap representing the thumbnail of the specified Bitmap.
     * The size of the thumbnail is defined by the dimension
     * android.R.dimen.launcher_application_icon_size.
     *
     * @param bitmap The bitmap to get a thumbnail of.
     * @param context The application's context.
     *
     * @return A thumbnail for the specified bitmap or the bitmap itself if the
     *         thumbnail could not be created.
     */
    static CARAPI_(AutoPtr<IBitmap>) ResampleIconBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<IBitmap>) DrawDisabledBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ IContext* context);

    /** Only works for positive numbers. */
    static CARAPI_(Int32) RoundToPow2(
        /* [in] */ Int32 n);

    static CARAPI_(Int32) GenerateRandomId();

private:
    static CARAPI_(Boolean) InitStaticBlock();

    static CARAPI_(void) InitStatics(
        /* [in] */ IContext* context);

private:
    //@SuppressWarnings("unused")
    static const String TAG;

    static Int32 sIconWidth;
    static Int32 sIconHeight;
    static Int32 sIconTextureWidth;
    static Int32 sIconTextureHeight;

    static AutoPtr<IPaint> sBlurPaint;
    static AutoPtr<IPaint> sGlowColorPressedPaint;
    static AutoPtr<IPaint> sGlowColorFocusedPaint;
    static AutoPtr<IPaint> sDisabledPaint;
    static AutoPtr<IRect> sOldBounds;
    static AutoPtr<ICanvas> sCanvas;
    static Object sCanvasLock;

    static AutoPtr<ArrayOf<Int32> > sColors;
    static Int32 sColorIndex;

    static Boolean initStaticBlock;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_UTILITIES_H__