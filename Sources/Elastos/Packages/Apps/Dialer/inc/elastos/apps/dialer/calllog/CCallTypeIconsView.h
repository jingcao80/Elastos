#ifndef __ELASTOS_APPS_DIALER_CCALLTYPEICONSVIEW_H__
#define __ELASTOS_APPS_DIALER_CCALLTYPEICONSVIEW_H__

#include "_Elastos_Apps_Dialer_CallLog_CCallTypeIconsView.h"
#include "elastos/droid/View/View.h"

using Elastos::Droid::View::View;

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

/**
 * Helper class to perform operations related to call types.
 */
CarClass(CCallTypeIconsView)
    , public View
    , public ICallTypeIconsView
{
private:
    class Resources
        : public Object
    {
    public:
        /**
         * Configures the call icon drawables.
         * A single white call arrow which points down and left is used as a basis for all of the
         * call arrow icons, applying rotation and colors as needed.
         *
         * @param context The current context.
         */
        Resources(
            /* [in] */ IContext* context);
    public:
        /**
         * Drawable representing an incoming answered call.
         */
        AutoPtr<IDrawable> mIncoming;

        /**
         * Drawable respresenting an outgoing call.
         */
        AutoPtr<IDrawable> mOutgoing;

        /**
         * Drawable representing an incoming missed call.
         */
        AutoPtr<IDrawable> mMissed;

        /**
         * Drawable representing a voicemail.
         */
        AutoPtr<IDrawable> mVoicemail;

        /**
         * Drawable repesenting a video call.
         */
        AutoPtr<IDrawable> mVideoCall;

        /**
         * The margin to use for icons.
         */
        Int32 mIconMargin;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCallTypeIconsView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Clear();

    CARAPI Add(
        /* [in] */ Int32 callType);

    /**
     * Determines whether the video call icon will be shown.
     *
     * @param showVideo True where the video icon should be shown.
     */
    CARAPI SetShowVideo(
        /* [in] */ Boolean showVideo);

    /**
     * Determines if the video icon should be shown.
     *
     * @return True if the video icon should be shown.
     */
    CARAPI IsVideoShown(
        /* [out] */ Boolean* result);

    // @NeededForTesting
    CARAPI GetCount(
        /* [out] */ Int32* count);

    // @NeededForTesting
    CARAPI GetCallType(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);

protected:
    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(AutoPtr<IDrawable>) GetCallTypeDrawable(
        /* [in] */ Int32 callType);

private:
    AutoPtr<IList> mCallTypes; // = Lists.newArrayListWithCapacity(3);
    Boolean mShowVideo; // = false;
    AutoPtr<Resources> mResources;
    Int32 mWidth;
    Int32 mHeight;
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CCALLTYPEICONSVIEW_H__
