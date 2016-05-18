
#ifndef __ELASTOS_DROID_WIDGET_TOAST_H__
#define __ELASTOS_DROID_WIDGET_TOAST_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::App::IITransientNotification;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Widget {
/**
 * A toast is a view containing a quick little message for the user.  The toast class
 * helps you create and show those.
 * {@more}
 *
 * <p>
 * When the view is shown to the user, appears as a floating view over the
 * application.  It will never receive focus.  The user will probably be in the
 * middle of typing something else.  The idea is to be as unobtrusive as
 * possible, while still showing the user the information you want them to see.
 * Two examples are the volume control, and the brief message saying that your
 * settings have been saved.
 * <p>
 * The easiest way to use this class is to call one of the static methods that constructs
 * everything you need and returns a new Toast object.
 */
class Toast
    : public Object
    , public IToast
{
public:
    class TN
        : public Object
        , public IITransientNotification
        , public IBinder
    {
        friend class Toast;
    private:
        /*
         * The member mShow in TN is a sub-TN
         * so you need implement it here
         */
        class ShowAction
            : public Runnable
        {
        public:
            ShowAction(
                /* [in] */ TN* host);

            CARAPI Run();

        private:
            TN* mHost;
        };

        /*
         * The member mHide in TN is a sub-TN
         * so you need implement it here
         */
        class HideAction
            : public Runnable
        {
        public:
            HideAction(
                /* [in] */ TN* host);

            CARAPI Run();

        private:
            TN* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("Toast::TN::TransientNotification")

        TN();

        ~TN();

        CARAPI constructor();

        CARAPI Show();

        CARAPI Hide();

        CARAPI SetNextView(
            /* [in] */ IView* nextView);

        CARAPI SetGravity(
            /* [in] */ Int32 gravity);

        CARAPI GetGravity(
            /* [out] */ Int32* gravity);

        CARAPI SetX(
            /* [in] */ Int32 x);

        CARAPI GetX(
            /* [out] */ Int32* x);

        CARAPI SetY(
            /* [in] */ Int32 y);

        CARAPI GetY(
            /* [out] */ Int32* y);

        CARAPI SetHorizontalMargin(
            /* [in] */ Float horizontalMargin);

        CARAPI GetHorizontalMargin(
            /* [out] */ Float* horizontalMargin);

        CARAPI SetVerticalMargin(
            /* [in] */ Float verticalMargin);

        CARAPI GetVerticalMargin(
            /* [out] */ Float* verticalMargin);

    private:
        CARAPI HandleShow();

        CARAPI HandleHide();

    private:
        CARAPI TrySendAccessibilityEvent();

    public:
        AutoPtr<IView> mView;
        AutoPtr<IView> mNextView;

    private:
        AutoPtr<IRunnable> mShow;
        AutoPtr<IRunnable> mHide;
        AutoPtr<IWindowManagerLayoutParams> mParams; //= new WindowManager.LayoutParams();
        AutoPtr<IWindowManager> mWM;

        AutoPtr<IHandler> mHandler;// = new Handler();

        Int32 mGravity;
        Int32 mX;
        Int32 mY;
        Float mHorizontalMargin;
        Float mVerticalMargin;
    };

public:
    CAR_INTERFACE_DECL()

    Toast();

    ~Toast();

    /**
     * Construct an empty Toast object.  You must call {@link #setView} before you
     * can call {@link #show}.
     *
     * @param context  The context to use.  Usually your {@link android.app.Application}
     *                 or {@link android.app.Activity} object.
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
    * Show the view for the specified duration.
    */
    CARAPI Show();

    /**
    * Close the view if it's showing, or don't show it if it isn't showing yet.
    * You do not normally have to call this.  Normally view will disappear on its own
    * after the appropriate duration.
    */
    CARAPI Cancel();

    /**
    * Set the view to show.
    * @see #getView
    */
    CARAPI SetView(
        /* [in] */ IView* view);

    /**
    * Return the view.
    * @see #setView
    */
    CARAPI GetView(
        /* [out] */ IView** view);

    /**
    * Set how long to show the view for.
    * @see #LENGTH_SHORT
    * @see #LENGTH_LONG
    */
    CARAPI SetDuration(
        /* [in] */ Int32 duration);

    /**
    * Return the duration.
    * @see #setDuration
    */
    CARAPI GetDuration(
        /* [out] */ Int32* duration);

    /**
    * Set the margins of the view.
    *
    * @param horizontalMargin The horizontal margin, in percentage of the
    *        container width, between the container's edges and the
    *        notification
    * @param verticalMargin The vertical margin, in percentage of the
    *        container height, between the container's edges and the
    *        notification
    */
    CARAPI SetMargin(
        /* [in] */ Float horizontalMargin,
        /* [in] */ Float verticalMargin);

    /**
    * Return the horizontal margin.
    */
    CARAPI GetHorizontalMargin(
        /* [out] */ Float* horizontalMargin);

    /**
    * Return the vertical margin.
    */
    CARAPI GetVerticalMargin(
        /* [out] */ Float* verticalMargin);

    /**
    * Set the location at which the notification should appear on the screen.
    * @see android.view.Gravity
    * @see #getGravity
    */
    CARAPI SetGravity(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 xOffset,
        /* [in] */ Int32 yOffset);

    /**
    * Get the location at which the notification should appear on the screen.
    * @see android.view.Gravity
    * @see #getGravity
    */
    CARAPI GetGravity(
        /* [out] */ Int32* gravity);

    /**
    * Return the X offset in pixels to apply to the gravity's location.
    */
    CARAPI GetXOffset(
        /* [out] */ Int32* xOffset);

    /**
    * Return the Y offset in pixels to apply to the gravity's location.
    */
    CARAPI GetYOffset(
        /* [out] */ Int32* yOffset);

    /**
     * Make a standard toast that just contains a text view.
     *
     * @param context  The context to use.  Usually your {@link android.app.Application}
     *                 or {@link android.app.Activity} object.
     * @param text     The text to show.  Can be formatted text.
     * @param duration How long to display the message.  Either {@link #LENGTH_SHORT} or
     *                 {@link #LENGTH_LONG}
     *
     */
    static CARAPI MakeText(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 duration,
        /* [out] */ IToast** toast);

    /**
     * Make a standard toast that just contains a text view with the text from a resource.
     *
     * @param context  The context to use.  Usually your {@link android.app.Application}
     *                 or {@link android.app.Activity} object.
     * @param resId    The resource id of the string resource to use.  Can be formatted text.
     * @param duration How long to display the message.  Either {@link #LENGTH_SHORT} or
     *                 {@link #LENGTH_LONG}
     *
     * @throws Resources.NotFoundException if the resource can't be found.
     */
    static CARAPI MakeText(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resId,
        /* [in] */ Int32 duration,
        /* [out] */ IToast** toast);

    /**
     * Update the text in a Toast that was previously created using one of the makeText() methods.
     * @param resId The new text for the Toast.
     */
    CARAPI SetText(
        /* [in] */ Int32 resId);

    /**
     * Update the text in a Toast that was previously created using one of the makeText() methods.
     * @param s The new text for the Toast.
     */
    CARAPI SetText(
        /* [in] */ ICharSequence* s);

private:
    static AutoPtr<IINotificationManager> GetService();

protected:
    static const String TAG;
    static const Boolean localLOGV;

    AutoPtr<IContext> mContext;
    AutoPtr<TN> mTN;

    Int32 mDuration;
    AutoPtr<IView> mNextView;

private:
    static AutoPtr<IINotificationManager> sService;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif
