
#ifndef __ELASTOS_DROID_WIDGET_SCROLLBARDRAWABLE_H__
#define __ELASTOS_DROID_WIDGET_SCROLLBARDRAWABLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/Drawable.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::IInsets;
using Elastos::Droid::Graphics::PorterDuffMode;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Widget {

class ScrollBarDrawable
    : public Elastos::Droid::Graphics::Drawable::Drawable
    , public IScrollBarDrawable
{
public:
    CAR_INTERFACE_DECL();

    ScrollBarDrawable();

    /**
     * Indicate whether the horizontal scrollbar track should always be drawn regardless of the
     * extent. Defaults to false.
     *
     * @param alwaysDrawTrack Set to true if the track should always be drawn
     */
    CARAPI SetAlwaysDrawHorizontalTrack(
        /* [in] */ Boolean alwaysDrawTrack);

    /**
     * Indicate whether the vertical scrollbar track should always be drawn regardless of the
     * extent. Defaults to false.
     *
     * @param alwaysDrawTrack Set to true if the track should always be drawn
     */
    CARAPI SetAlwaysDrawVerticalTrack(
        /* [in] */ Boolean alwaysDrawTrack);

    /**
     * Indicates whether the horizontal scrollbar track should always be drawn regardless of the
     * extent.
     */
    CARAPI GetAlwaysDrawHorizontalTrack(
        /* [out] */ Boolean* alwaysDrawTrack);

    /**
     * Indicates whether the vertical scrollbar track should always be drawn regardless of the
     * extent.
     */
    CARAPI GetAlwaysDrawVerticalTrack(
        /* [out] */ Boolean* alwaysTrack);

    CARAPI SetParameters(
        /* [in] */ Int32 range,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 extent,
        /* [in] */ Boolean vertical);

    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    CARAPI SetVerticalThumbDrawable(
        /* [in] */ IDrawable* thumb);

    CARAPI SetVerticalTrackDrawable(
        /* [in] */ IDrawable* track);

    CARAPI SetHorizontalThumbDrawable(
        /* [in] */ IDrawable* thumb);

    CARAPI SetHorizontalTrackDrawable(
        /* [in] */ IDrawable* track);

    CARAPI GetSize(
        /* [in] */ Boolean vertical,
        /* [out] */ Int32* size);

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    // overrides from Drawable.Callback

    CARAPI InvalidateDrawable(
        /* [in] */ IDrawable* who);

    CARAPI ScheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what,
        /* [in] */ Int64 when);

    CARAPI UnscheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what);

protected:
    virtual CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

    CARAPI_(void) DrawTrack(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* bounds,
        /* [in] */ Boolean vertical);

    CARAPI_(void) DrawThumb(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* bounds,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ Boolean vertical);

private:
    AutoPtr<IDrawable> mVerticalTrack;
    AutoPtr<IDrawable> mHorizontalTrack;
    AutoPtr<IDrawable> mVerticalThumb;
    AutoPtr<IDrawable> mHorizontalThumb;
    Int32 mRange;
    Int32 mOffset;
    Int32 mExtent;
    Boolean mVertical;
    Boolean mChanged;
    Boolean mRangeChanged;
    AutoPtr<IRect> mTempBounds;
    Boolean mAlwaysDrawHorizontalTrack;
    Boolean mAlwaysDrawVerticalTrack;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_SCROLLBARDRAWABLE_H__
