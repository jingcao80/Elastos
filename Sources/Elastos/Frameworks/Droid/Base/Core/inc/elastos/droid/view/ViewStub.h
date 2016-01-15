
#ifndef __ELASTOS_DROID_VIEW_VIEWSTUB_H__
#define __ELASTOS_DROID_VIEW_VIEWSTUB_H__

#include "elastos/droid/view/View.h"

namespace Elastos {
namespace Droid {
namespace View {

class ViewStub
    : public View
    , public IViewStub
{
public:
    CAR_INTERFACE_DECL();

    ViewStub();

    ~ViewStub();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Creates a new ViewStub with the specified layout resource.
     *
     * @param context The application's environment.
     * @param layoutResource The reference to a layout resource that will be inflated.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 layoutResource);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // /@SuppressWarnings({"UnusedDeclaration"})
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * Returns the id taken by the inflated view. If the inflated id is
     * {@link View#NO_ID}, the inflated view keeps its original id.
     *
     * @return A positive integer used to identify the inflated view or
     *         {@link #NO_ID} if the inflated view should keep its id.
     *
     * @see #setInflatedId(Int32)
     * @attr ref android.R.styleable#ViewStub_inflatedId
     */
    CARAPI GetInflatedId(
        /* [out] */ Int32* id);

    /**
     * Defines the id taken by the inflated view. If the inflated id is
     * {@link View#NO_ID}, the inflated view keeps its original id.
     *
     * @param inflatedId A positive integer used to identify the inflated view or
     *                   {@link #NO_ID} if the inflated view should keep its id.
     *
     * @see #getInflatedId()
     * @attr ref android.R.styleable#ViewStub_inflatedId
     */
    //@android.view.RemotableViewMethod
    CARAPI SetInflatedId(
        /* [in] */ Int32 inflatedId);

    /**
     * Returns the layout resource that will be used by {@link #setVisibility(Int32)} or
     * {@link #inflate()} to replace this StubbedView
     * in its parent by another view.
     *
     * @return The layout resource identifier used to inflate the new View.
     *
     * @see #setLayoutResource(Int32)
     * @see #setVisibility(Int32)
     * @see #inflate()
     * @attr ref android.R.styleable#ViewStub_layout
     */
    CARAPI GetLayoutResource(
        /* [out] */ Int32* resource);

    /**
     * Specifies the layout resource to inflate when this StubbedView becomes visible or invisible
     * or when {@link #inflate()} is invoked. The View created by inflating the layout resource is
     * used to replace this StubbedView in its parent.
     *
     * @param layoutResource A valid layout resource identifier (different from 0.)
     *
     * @see #getLayoutResource()
     * @see #setVisibility(Int32)
     * @see #inflate()
     * @attr ref android.R.styleable#ViewStub_layout
     */
    //@android.view.RemotableViewMethod
    CARAPI SetLayoutResource(
        /* [in] */ Int32 layoutResource);

    /**
     * Set {@link LayoutInflater} to use in {@link #inflate()}, or {@code null}
     * to use the default.
     */
    CARAPI SetLayoutInflater(
        /* [in] */ ILayoutInflater* inflater);

    /**
     * Get current {@link LayoutInflater} used in {@link #inflate()}.
     */
    CARAPI GetLayoutInflater(
        /* [out] */ ILayoutInflater** inflater);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    /**
     * When visibility is set to {@link #VISIBLE} or {@link #INVISIBLE},
     * {@link #inflate()} is invoked and this StubbedView is replaced in its parent
     * by the inflated layout resource. After that calls to this function are passed
     * through to the inflated view.
     *
     * @param visibility One of {@link #VISIBLE}, {@link #INVISIBLE}, or {@link #GONE}.
     *
     * @see #inflate()
     */
    //@Override
    //@android.view.RemotableViewMethod
    CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    /**
     * Inflates the layout resource identified by {@link #getLayoutResource()}
     * and replaces this StubbedView in its parent by the inflated layout resource.
     *
     * @return The inflated layout resource.
     *
     */
    CARAPI Inflate(
        /* [out] */ IView** retView);

    /**
     * Specifies the inflate listener to be notified after this ViewStub successfully
     * inflated its layout resource.
     *
     * @param inflateListener The OnInflateListener to notify of successful inflation.
     *
     * @see android.view.ViewStub.OnInflateListener
     */
    CARAPI SetOnInflateListener(
        /* [in] */ IViewStubOnInflateListener* inflateListener);

protected:
    //@Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI_(void) DispatchDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) Initialize(
        /* [in] */ IContext* context);

private:
    Int32 mLayoutResource;
    Int32 mInflatedId;

    //WeakReference<View>
    AutoPtr<IWeakReference> mInflatedViewRef;

    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<IViewStubOnInflateListener> mInflateListener;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_VIEWSTUB_H__
