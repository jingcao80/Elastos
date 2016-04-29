
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_UTILITIES_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_UTILITIES_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

/* Common code */
class Utilities
{
public:
    /**
     * Calculates a consistent animation duration (ms) for all animations depending on the movement
     * of the object being animated.
     */
    static CARAPI_(Int32) CalculateTranslationAnimationDuration(
        /* [in] */ Int32 distancePx);

    static CARAPI_(Int32) CalculateTranslationAnimationDuration(
        /* [in] */ Int32 distancePx,
        /* [in] */ Int32 minDuration);

    /** Scales a rect about its centroid */
    static CARAPI_(void) scaleRectAboutCenter(
        /* [in] */ IRect* r,
        /* [in] */ Float scale);

    /** Maps a coorindate in a descendant view into the parent. */
    static CARAPI_(Float) MapCoordInDescendentToSelf(
        /* [in] */ IView* descendant,
        /* [in] */ IView* root,
        /* [in] */ ArrayOf<Float>* coord,
        /* [in] */ Boolean includeRootScroll);

    /** Maps a coordinate in the root to a descendent. */
    static CARAPI_(Float) MapCoordInSelfToDescendent(
        /* [in] */ IView* descendant,
        /* [in] */ IView* root,
        /* [in] */ ArrayOf<Float>* coord,
        /* [in] */ IMatrix* tmpInverseMatrix);

    /** Calculates the constrast between two colors, using the algorithm provided by the WCAG v2. */
    static CARAPI_(Float) ComputeContrastBetweenColors(
        /* [in] */ Int32 bg,
        /* [in] */ Int32 fg);

    /** Returns the base color overlaid with another overlay color with a specified alpha. */
    static CARAPI_(Int32) GetColorWithOverlay(
        /* [in] */ Int32 value,
        /* [in] */ Int32 baseColor,
        /* [in] */ Int32 overlayColor,
        /* [in] */ Float overlayAlpha);

    /** Sets some private shadow properties. */
    static CARAPI_(void) SetShadowProperty(
        /* [in] */ const String& property,
        /* [in] */ const String& value);

    /** Returns whether the specified intent is a document. */
    static CARAPI_(Boolean) IsDocument(
        /* [in] */ IIntent* intent);

private:
    // Reflection methods for altering shadows
#if 0 //TODO
    private static Method sPropertyMethod;
    static {
        try {
            Class<?> c = Class.forName("android.view.GLES20Canvas");
            sPropertyMethod = c.getDeclaredMethod("setProperty", String.class, String.class);
            if (!sPropertyMethod.isAccessible()) sPropertyMethod.setAccessible(true);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
    }
#endif
};

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_UTILITIES_H__
