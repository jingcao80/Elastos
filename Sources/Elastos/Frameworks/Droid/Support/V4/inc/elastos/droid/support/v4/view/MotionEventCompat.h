
#ifndef __ELASTOS_DROID_SUPPORT_V4_VIEW_MOTIONEVENTCOMPAT_H__
#define __ELASTOS_DROID_SUPPORT_V4_VIEW_MOTIONEVENTCOMPAT_H__

#include "_Elastos.Droid.Support.V4.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

/**
 * Helper for accessing features in {@link MotionEvent} introduced
 * after API level 4 in a backwards compatible fashion.
 */
class MotionEventCompat
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    /**
     * Interface implementation that doesn't use anything about v4 APIs.
     */
    class BaseMotionEventVersionImpl
        : public Object
        , public IMotionEventVersionImpl
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI FindPointerIndex(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 pointerId,
            /* [out] */ Int32* index);

        // @Override
        CARAPI GetPointerId(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 pointerIndex,
            /* [out] */ Int32* id);

        // @Override
        CARAPI GetX(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 pointerIndex,
            /* [out] */ Float* x);

        // @Override
        CARAPI GetY(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 pointerIndex,
            /* [out] */ Float* y);

        // @Override
        CARAPI GetPointerCount(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Int32* count);
    };

    /**
     * Interface implementation for devices with at least v11 APIs.
     */
    class EclairMotionEventVersionImpl
        : public Object
        , public IMotionEventVersionImpl
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI FindPointerIndex(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 pointerId,
            /* [out] */ Int32* index);

        // @Override
        CARAPI GetPointerId(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 pointerIndex,
            /* [out] */ Int32* id);

        // @Override
        CARAPI GetX(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 pointerIndex,
            /* [out] */ Float* x);

        // @Override
        CARAPI GetY(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 pointerIndex,
            /* [out] */ Float* y);

        // @Override
        CARAPI GetPointerCount(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Int32* count);
    };

public:
    /**
     * Call {@link MotionEvent#getAction}, returning only the {@link #ACTION_MASK}
     * portion.
     */
    static CARAPI_(Int32) GetActionMasked(
        /* [in] */ IMotionEvent* event);

    /**
     * Call {@link MotionEvent#getAction}, returning only the pointer index
     * portion
     */
    static CARAPI_(Int32) GetActionIndex(
        /* [in] */ IMotionEvent* event);

    /**
     * Call {@link MotionEvent#findPointerIndex(int)}.
     * If running on a pre-{@link android.os.Build.VERSION_CODES#ECLAIR} device,
     * does nothing and returns -1.
     */
    static CARAPI_(Int32) FindPointerIndex(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 pointerId);

    /**
     * Call {@link MotionEvent#getPointerId(int)}.
     * If running on a pre-{@link android.os.Build.VERSION_CODES#ECLAIR} device,
     * {@link IndexOutOfBoundsException} is thrown.
     */
    static CARAPI_(Int32) GetPointerId(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 pointerIndex);

    /**
     * Call {@link MotionEvent#getX(int)}.
     * If running on a pre-{@link android.os.Build.VERSION_CODES#ECLAIR} device,
     * {@link IndexOutOfBoundsException} is thrown.
     */
    static CARAPI_(Float) GetX(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 pointerIndex);

    /**
     * Call {@link MotionEvent#getY(int)}.
     * If running on a pre-{@link android.os.Build.VERSION_CODES#ECLAIR} device,
     * {@link IndexOutOfBoundsException} is thrown.
     */
    static CARAPI_(Float) GetY(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 pointerIndex);

    /**
     * The number of pointers of data contained in this event.  Always
     * >= 1.
     */
    static CARAPI_(Int32) GetPointerCount(
        /* [in] */ IMotionEvent* event);

public:
    /**
     * Select the correct implementation to use for the current platform.
     */
    static AutoPtr<IMotionEventVersionImpl> IMPL;
    static StaticInitializer sInitializer;

    // -------------------------------------------------------------------

    /**
     * Synonym for {@link MotionEvent#ACTION_MASK}.
     */
    static const Int32 ACTION_MASK = 0xff;

    /**
     * Synonym for {@link MotionEvent#ACTION_POINTER_DOWN}.
     */
    static const Int32 ACTION_POINTER_DOWN = 5;

    /**
     * Synonym for {@link MotionEvent#ACTION_POINTER_UP}.
     */
    static const Int32 ACTION_POINTER_UP = 6;

    /**
     * Synonym for {@link MotionEvent#ACTION_HOVER_MOVE}.
     */
    static const Int32 ACTION_HOVER_MOVE = 7;

    /**
     * Synonym for {@link MotionEvent#ACTION_SCROLL}.
     */
    static const Int32 ACTION_SCROLL = 8;

    /**
     * Synonym for {@link MotionEvent#ACTION_POINTER_INDEX_MASK}.
     */
    static const Int32 ACTION_POINTER_INDEX_MASK  = 0xff00;

    /**
     * Synonym for {@link MotionEvent#ACTION_POINTER_INDEX_SHIFT}.
     */
    static const Int32 ACTION_POINTER_INDEX_SHIFT = 8;

    /**
     * Constant for {@link #getActionMasked}: The pointer is not down but has entered the
     * boundaries of a window or view.
     * <p>
     * This action is always delivered to the window or view under the pointer.
     * </p><p>
     * This action is not a touch event so it is delivered to
     * {@link android.view.View#onGenericMotionEvent(MotionEvent)} rather than
     * {@link android.view.View#onTouchEvent(MotionEvent)}.
     * </p>
     */
    static const Int32 ACTION_HOVER_ENTER = 9;

    /**
     * Constant for {@link #getActionMasked}: The pointer is not down but has exited the
     * boundaries of a window or view.
     * <p>
     * This action is always delivered to the window or view that was previously under the pointer.
     * </p><p>
     * This action is not a touch event so it is delivered to
     * {@link android.view.View#onGenericMotionEvent(MotionEvent)} rather than
     * {@link android.view.View#onTouchEvent(MotionEvent)}.
     * </p>
     */
    static const Int32 ACTION_HOVER_EXIT = 10;
};

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SUPPORT_V4_VIEW_MOTIONEVENTCOMPAT_H__