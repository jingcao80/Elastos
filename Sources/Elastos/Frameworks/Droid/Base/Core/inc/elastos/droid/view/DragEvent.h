
#ifndef __ELASTOS_DROID_VIEW_DRAGEVENT_H__
#define __ELASTOS_DROID_VIEW_DRAGEVENT_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>
#include <elastos/core/Mutex.h>

using Elastos::Core::Mutex;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IClipDescription;

namespace Elastos {
namespace Droid {
namespace View {

class DragEvent
    : public Object
    , public IDragEvent
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    DragEvent();

    CARAPI Init(
        /* [in] */ Int32 action,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IClipDescription* description,
        /* [in] */ IClipData* data,
        /* [in] */ IInterface* localState,
        /* [in] */ Boolean result);

    /**
     * Inspect the action value of this event.
     * @return One of the following action constants, in the order in which they usually occur
     * during a drag and drop operation:
     * <ul>
     *  <li>{@link #ACTION_DRAG_STARTED}</li>
     *  <li>{@link #ACTION_DRAG_ENTERED}</li>
     *  <li>{@link #ACTION_DRAG_LOCATION}</li>
     *  <li>{@link #ACTION_DROP}</li>
     *  <li>{@link #ACTION_DRAG_EXITED}</li>
     *  <li>{@link #ACTION_DRAG_ENDED}</li>
     * </ul>
     */
    CARAPI GetAction(
        /* [out] */ Int32* action);

    CARAPI SetAction(
        /* [in] */ Int32 action);

    /**
     * Gets the X coordinate of the drag point. The value is only valid if the event action is
     * {@link #ACTION_DRAG_LOCATION} or {@link #ACTION_DROP}.
     * @return The current drag point's Y coordinate
     */
    CARAPI GetX(
        /* [out] */ Float* x);

    /**
     * Gets the Y coordinate of the drag point. The value is valid if the
     * event action is {@link #ACTION_DRAG_ENTERED}, {@link #ACTION_DRAG_LOCATION},
     * {@link #ACTION_DROP}, or {@link #ACTION_DRAG_EXITED}.
     * @return The current drag point's Y coordinate
     */
    CARAPI GetY(
        /* [out] */ Float* y);

    CARAPI SetX(
        /* [in] */ Float x);

    CARAPI SetY(
        /* [in] */ Float y);

    /**
     * Returns the {@link android.content.ClipData} object sent to the system as part of the call
     * to
     * {@link android.view.View#startDrag(ClipData,View.DragShadowBuilder,Object,int) startDrag()}.
     * This method only returns valid data if the event action is {@link #ACTION_DROP}.
     * @return The ClipData sent to the system by startDrag().
     */
    CARAPI GetClipData(
        /* [out] */ IClipData** clipData);

    /**
     * Returns the {@link android.content.ClipDescription} object contained in the
     * {@link android.content.ClipData} object sent to the system as part of the call to
     * {@link android.view.View#startDrag(ClipData,View.DragShadowBuilder,Object,int) startDrag()}.
     * The drag handler or listener for a View can use the metadata in this object to decide if the
     * View can accept the dragged View object's data.
     * <p>
     * This method returns valid data for all event actions.
     * @return The ClipDescription that was part of the ClipData sent to the system by startDrag().
     */
    CARAPI GetClipDescription(
        /* [out] */ IClipDescription** description);

    /**
     * Returns the local state object sent to the system as part of the call to
     * {@link android.view.View#startDrag(ClipData,View.DragShadowBuilder,Object,int) startDrag()}.
     * The object is intended to provide local information about the drag and drop operation. For
     * example, it can indicate whether the drag and drop operation is a copy or a move.
     * <p>
     *  This method returns valid data for all event actions.
     * </p>
     * @return The local state object sent to the system by startDrag().
     */
    CARAPI GetLocalState(
        /* [out] */ IInterface** localState);

    CARAPI SetLocalState(
        /* [in] */ IInterface* localState);

    /**
     * <p>
     * Returns an indication of the result of the drag and drop operation.
     * This method only returns valid data if the action type is {@link #ACTION_DRAG_ENDED}.
     * The return value depends on what happens after the user releases the drag shadow.
     * </p>
     * <p>
     * If the user releases the drag shadow on a View that can accept a drop, the system sends an
     * {@link #ACTION_DROP} event to the View object's drag event listener. If the listener
     * returns {@code true}, then getResult() will return {@code true}.
     * If the listener returns {@code false}, then getResult() returns {@code false}.
     * </p>
     * <p>
     * Notice that getResult() also returns {@code false} if no {@link #ACTION_DROP} is sent. This
     * happens, for example, when the user releases the drag shadow over an area outside of the
     * application. In this case, the system sends out {@link #ACTION_DRAG_ENDED} for the current
     * operation, but never sends out {@link #ACTION_DROP}.
     * </p>
     * @return {@code true} if a drag event listener returned {@code true} in response to
     * {@link #ACTION_DROP}. If the system did not send {@link #ACTION_DROP} before
     * {@link #ACTION_DRAG_ENDED}, or if the listener returned {@code false} in response to
     * {@link #ACTION_DROP}, then {@code false} is returned.
     */
    CARAPI GetResult(
        /* [out] */ Boolean* dragResult);

    /**
     * Recycle the DragEvent, to be re-used by a later caller.  After calling
     * this function you must never touch the event again.
     *
     * @hide
     */
    CARAPI Recycle();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Creates a {@link android.os.Parcel} object from this DragEvent object.
     * @param dest A {@link android.os.Parcel} object in which to put the DragEvent object.
     * @param flags Flags to store in the Parcel.
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    static CARAPI Obtain(
        /* [out] */ IDragEvent ** ppEvent);

    static CARAPI Obtain(
        /* [in] */ Int32 action,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IInterface * pLocalState,
        /* [in] */ IClipDescription * pDescription,
        /* [in] */ IClipData * pData,
        /* [in] */ Boolean result,
        /* [out] */ IDragEvent ** ppEvent);

    static CARAPI Obtain(
        /* [in] */ IDragEvent * pSource,
        /* [out] */ IDragEvent ** ppEvent);

    static AutoPtr<DragEvent> Obtain(
        /* [in] */ Int32 action,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IInterface* localState,
        /* [in] */ IClipDescription* description,
        /* [in] */ IClipData* data,
        /* [in] */ Boolean result);

private:
    friend class ViewRootImpl;

    static const Int32 MAX_RECYCLED = 10;
    static Mutex sRecyclerLock;
    static Int32 sRecyclerUsed;
    static AutoPtr<DragEvent> sRecyclerTop;
    static const Boolean TRACK_RECYCLED_LOCATION = FALSE;
    Boolean mRecycledLocation;
    Int32 mAction;
    Float mX, mY;
    AutoPtr<IClipDescription> mClipDescription;
    AutoPtr<IClipData> mClipData;
    AutoPtr<IInterface> mLocalState;
    Boolean mDragResult;
    AutoPtr<DragEvent> mNext;
    Boolean mRecycled;
};

}
}
}

#endif // __ELASTOS_DROID_VIEW_DRAGEVENT_H__
