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

#ifndef __ELASTOS_DROID_VIEW_VIEWTREEOBSERVER_H__
#define __ELASTOS_DROID_VIEW_VIEWTREEOBSERVER_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace View {

class ViewTreeObserver
    : public Object
    , public IViewTreeObserver
{
public:
    class InternalInsetsInfo
        : public Object
        , public IInternalInsetsInfo
    {
    public:
        CAR_INTERFACE_DECL()

        InternalInsetsInfo();

        /**
         * Set which parts of the window can be touched: either
         * {@link #TOUCHABLE_INSETS_FRAME}, {@link #TOUCHABLE_INSETS_CONTENT},
         * or {@link #TOUCHABLE_INSETS_VISIBLE}.
         */
        CARAPI SetTouchableInsets(
            /* [in] */ Int32 val);

        CARAPI GetTouchableInsets(
            /* [out] */ Int32* result);

        CARAPI GetContentInsets(
            /* [out] */ IRect** result);

        CARAPI GetVisibleInsets(
            /* [out] */ IRect** result);

        CARAPI GetTouchableRegion(
            /* [out] */ IRegion** result);

        CARAPI Reset();

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);

        CARAPI Set(
            /* [in] */ InternalInsetsInfo* other);

    public:
        /**
         * Offsets from the frame of the window at which the content of
         * windows behind it should be placed.
         */
        AutoPtr<IRect> mContentInsets;

        /**
         * Offsets from the fram of the window at which windows behind it
         * are visible.
         */
        AutoPtr<IRect> mVisibleInsets;

        /**
         * Touchable region defined relative to the origin of the frame of the window.
         * Only used when {@link #setTouchableInsets(int)} is called with
         * the option {@link #TOUCHABLE_INSETS_REGION}.
         */
        AutoPtr<IRegion> mTouchableRegion;

        Int32 mTouchableInsets;
    };

public:
    CAR_INTERFACE_DECL()

    ViewTreeObserver();

    CARAPI constructor();

    CARAPI Merge(
        /* [in] */ IViewTreeObserver* observer);

    /**
     * Register a callback to be invoked when the view hierarchy is attached to a window.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    CARAPI AddOnWindowAttachListener(
        /* [in] */ IOnWindowAttachListener* listener);

    /**
     * Remove a previously installed window attach callback.
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnWindowAttachListener(android.view.ViewTreeObserver.OnWindowAttachListener)
     */
    CARAPI RemoveOnWindowAttachListener(
        /* [in] */ IOnWindowAttachListener* victim);

    /**
     * Register a callback to be invoked when the window focus state within the view tree changes.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    CARAPI AddOnWindowFocusChangeListener(
        /* [in] */ IOnWindowFocusChangeListener* listener);

    /**
     * Remove a previously installed window focus change callback.
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnWindowFocusChangeListener(android.view.ViewTreeObserver.OnWindowFocusChangeListener)
     */
    CARAPI RemoveOnWindowFocusChangeListener(
        /* [in] */ IOnWindowFocusChangeListener* victim);

    /**
     * Register a callback to be invoked when the focus state within the view tree changes.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    CARAPI AddOnGlobalFocusChangeListener(
        /* [in] */ IOnGlobalFocusChangeListener* listener);

    /**
     * Remove a previously installed focus change callback.
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnGlobalFocusChangeListener(OnGlobalFocusChangeListener)
     */
    CARAPI RemoveOnGlobalFocusChangeListener(
        /* [in] */ IOnGlobalFocusChangeListener* victim);

    /**
     * Register a callback to be invoked when the global layout state or the visibility of views
     * within the view tree changes
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    CARAPI AddOnGlobalLayoutListener(
        /* [in] */ IOnGlobalLayoutListener* listener);

    /**
     * Remove a previously installed global layout callback
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnGlobalLayoutListener(OnGlobalLayoutListener)
     */
    CARAPI RemoveGlobalOnLayoutListener(
        /* [in] */ IOnGlobalLayoutListener* victim);

    /**
     * Remove a previously installed global layout callback
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnGlobalLayoutListener(OnGlobalLayoutListener)
     */
    CARAPI RemoveOnGlobalLayoutListener(
        /* [in] */ IOnGlobalLayoutListener*  victim);

    /**
     * Register a callback to be invoked when the view tree is about to be drawn
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    CARAPI AddOnPreDrawListener(
        /* [in] */ IOnPreDrawListener* listener);

    /**
     * Remove a previously installed pre-draw callback
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnPreDrawListener(OnPreDrawListener)
     */
    CARAPI RemoveOnPreDrawListener(
        /* [in] */ IOnPreDrawListener* victim);

    /**
     * <p>Register a callback to be invoked when the view tree is about to be drawn.</p>
     * <p><strong>Note:</strong> this method <strong>cannot</strong> be invoked from
     * {@link android.view.ViewTreeObserver.OnDrawListener#onDraw()}.</p>
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    CARAPI AddOnDrawListener(
        /* [in] */ IOnDrawListener* listener);

    /**
     * <p>Remove a previously installed pre-draw callback.</p>
     * <p><strong>Note:</strong> this method <strong>cannot</strong> be invoked from
     * {@link android.view.ViewTreeObserver.OnDrawListener#onDraw()}.</p>
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnDrawListener(OnDrawListener)
     */
    CARAPI RemoveOnDrawListener(
        /* [in] */ IOnDrawListener* victim);

    /**
     * Register a callback to be invoked when a view has been scrolled.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    CARAPI AddOnScrollChangedListener(
        /* [in] */ IOnScrollChangedListener* listener);

    /**
     * Remove a previously installed scroll-changed callback
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnScrollChangedListener(OnScrollChangedListener)
     */
    CARAPI RemoveOnScrollChangedListener(
        /* [in] */ IOnScrollChangedListener* victim);

    /**
     * Register a callback to be invoked when the invoked when the touch mode changes.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    CARAPI AddOnTouchModeChangeListener(
        /* [in] */ IOnTouchModeChangeListener* listener);

    /**
     * Remove a previously installed touch mode change callback
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnTouchModeChangeListener(OnTouchModeChangeListener)
     */
    CARAPI RemoveOnTouchModeChangeListener(
        /* [in] */ IOnTouchModeChangeListener* victim);

    /**
     * Register a callback to be invoked when the invoked when it is time to
     * compute the window's internal insets.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * We are not yet ready to commit to this API and support it, so
     * @hide
     */
    CARAPI AddOnComputeInternalInsetsListener(
        /* [in] */ IOnComputeInternalInsetsListener* listener);

    /**
     * Remove a previously installed internal insets computation callback
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnComputeInternalInsetsListener(OnComputeInternalInsetsListener)
     *
     * We are not yet ready to commit to this API and support it, so
     * @hide
     */
    CARAPI RemoveOnComputeInternalInsetsListener(
        /* [in] */ IOnComputeInternalInsetsListener* victim);

    /**
     * Indicates whether this ViewTreeObserver is alive. When an observer is not alive,
     * any call to a method (except this one) will throw an exception.
     *
     * If an application keeps a long-lived reference to this ViewTreeObserver, it should
     * always check for the result of this method before calling any other method.
     *
     * @return True if this object is alive and be used, false otherwise.
     */
    CARAPI IsAlive(
        /* [out] */ Boolean* result);

    /**
     * Notifies registered listeners that window has been attached/detached.
     */
    CARAPI DispatchOnWindowAttachedChange(
        /* [in] */ Boolean attached);

    /**
     * Notifies registered listeners that window focus has changed.
     */
    CARAPI DispatchOnWindowFocusChange(
        /* [in] */ Boolean hasFocus);

    /**
     * Notifies registered listeners that focus has changed.
     */
    CARAPI DispatchOnGlobalFocusChange(
        /* [in] */ IView* oldFocus,
        /* [in] */ IView* newFocus);

    /**
     * Notifies registered listeners that a global layout happened. This can be called
     * manually if you are forcing a layout on a View or a hierarchy of Views that are
     * not attached to a Window or in the GONE state.
     */
    CARAPI DispatchOnGlobalLayout();

    /**
     * Returns whether there are listeners for on pre-draw events.
     */
    CARAPI HasOnPreDrawListeners(
        /* [out] */ Boolean* result);

    /**
     * Notifies registered listeners that the drawing pass is about to start. If a
     * listener returns true, then the drawing pass is canceled and rescheduled. This can
     * be called manually if you are forcing the drawing on a View or a hierarchy of Views
     * that are not attached to a Window or in the GONE state.
     *
     * @return True if the current draw should be canceled and resceduled, false otherwise.
     */
    CARAPI DispatchOnPreDraw(
        /* [out] */ Boolean* result);

    /**
     * Notifies registered listeners that the drawing pass is about to start.
     */
    CARAPI DispatchOnDraw();

    /**
     * Notifies registered listeners that the touch mode has changed.
     *
     * @param inTouchMode True if the touch mode is now enabled, false otherwise.
     */
    CARAPI DispatchOnTouchModeChanged(
        /* [in] */ Boolean inTouchMode);

    /**
     * Notifies registered listeners that something has scrolled.
     */
    CARAPI DispatchOnScrollChanged();

    /**
     * Returns whether there are listeners for computing internal insets.
     */
    CARAPI HasComputeInternalInsetsListeners(
        /* [out] */ Boolean* result);

    /**
     * Calls all listeners to compute the current insets.
     */
    CARAPI DispatchOnComputeInternalInsets(
        /* [in] */ IInternalInsetsInfo* inoutInfo);

private:
    CARAPI CheckIsAlive();

    /**
     * Marks this ViewTreeObserver as not alive. After invoking this method, invoking
     * any other method but {@link #isAlive()} and {@link #kill()} will throw an Exception.
     *
     * @hide
     */
    CARAPI Kill();

private:
    //TODO: CopyOnWriteArrayList ----Thread-safe; List---Don't know.

    // Recursive listeners use CopyOnWriteArrayList
    AutoPtr< List<AutoPtr<IOnWindowFocusChangeListener> > > mOnWindowFocusListeners;
    AutoPtr< List<AutoPtr<IOnWindowAttachListener> > > mOnWindowAttachListeners;
    AutoPtr< List<AutoPtr<IOnGlobalFocusChangeListener> > > mOnGlobalFocusListeners;
    AutoPtr< List<AutoPtr<IOnTouchModeChangeListener> > > mOnTouchModeChangeListeners;

    // Non-recursive listeners use CopyOnWriteArray
    // Any listener invoked from ViewRootImpl.performTraversals() should not be recursive
    AutoPtr< List<AutoPtr<IOnGlobalLayoutListener> > > mOnGlobalLayoutListeners;
    AutoPtr< List<AutoPtr<IOnComputeInternalInsetsListener> > > mOnComputeInternalInsetsListeners;
    AutoPtr< List<AutoPtr<IOnScrollChangedListener> > > mOnScrollChangedListeners;
    AutoPtr< List<AutoPtr<IOnPreDrawListener> > > mOnPreDrawListeners;

    // These listeners cannot be mutated during dispatch
    AutoPtr< List<AutoPtr<IOnDrawListener> > > mOnDrawListeners;

    Boolean mAlive;

    /**
     * Copy on write array. This array is not thread safe, and only one loop can
     * iterate over this array at any given time. This class avoids allocations
     * until a concurrent modification happens.
     *
     * Usage:
     *
     * CopyOnWriteArray.Access<MyData> access = array.start();
     * try {
     *     for (int i = 0; i < access.size(); i++) {
     *         MyData d = access.get(i);
     *     }
     * } finally {
     *     access.end();
     * }
     */
    // static class CopyOnWriteArray<T> {
    //     private ArrayList<T> mData = new ArrayList<T>();
    //     private ArrayList<T> mDataCopy;

    //     private final Access<T> mAccess = new Access<T>();

    //     private boolean mStart;

    //     static class Access<T> {
    //         private ArrayList<T> mData;
    //         private int mSize;

    //         T get(int index) {
    //             return mData.get(index);
    //         }

    //         int size() {
    //             return mSize;
    //         }
    //     }

    //     CopyOnWriteArray() {
    //     }

    //     private ArrayList<T> getArray() {
    //         if (mStart) {
    //             if (mDataCopy == null) mDataCopy = new ArrayList<T>(mData);
    //             return mDataCopy;
    //         }
    //         return mData;
    //     }

    //     Access<T> start() {
    //         if (mStart) throw new IllegalStateException("Iteration already started");
    //         mStart = true;
    //         mDataCopy = null;
    //         mAccess.mData = mData;
    //         mAccess.mSize = mData.size();
    //         return mAccess;
    //     }

    //     void end() {
    //         if (!mStart) throw new IllegalStateException("Iteration not started");
    //         mStart = false;
    //         if (mDataCopy != null) {
    //             mData = mDataCopy;
    //         }
    //         mDataCopy = null;
    //     }

    //     int size() {
    //         return getArray().size();
    //     }

    //     void add(T item) {
    //         getArray().add(item);
    //     }

    //     void addAll(CopyOnWriteArray<T> array) {
    //         getArray().addAll(array.mData);
    //     }

    //     void remove(T item) {
    //         getArray().remove(item);
    //     }

    //     void clear() {
    //         getArray().clear();
    //     }
    // }
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_VIEWTREEOBSERVER_H__

