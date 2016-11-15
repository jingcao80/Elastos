
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/AutoCompleteTextView.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/inputmethod/CCompletionInfo.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include "elastos/droid/widget/CListPopupWindow.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/Selection.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Selection;
using Elastos::Droid::Text::INoCopySpan;
using Elastos::Droid::Text::EIID_INoCopySpan;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::InputMethod::CCompletionInfo;
using Elastos::Droid::View::InputMethod::CInputMethodManager;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(AutoCompleteTextView::MyWatcher, Object, ITextWatcher)

AutoCompleteTextView::MyWatcher::MyWatcher(
    /* [in] */ AutoCompleteTextView* host)
    : mHost(host)
{}

ECode AutoCompleteTextView::MyWatcher::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    mHost->DoAfterTextChanged();

    return NOERROR;
}

ECode AutoCompleteTextView::MyWatcher::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    mHost->DoBeforeTextChanged();

    return NOERROR;
}

ECode AutoCompleteTextView::MyWatcher::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(AutoCompleteTextView::DropDownItemClickListener, Object, IAdapterViewOnItemClickListener)

AutoCompleteTextView::DropDownItemClickListener::DropDownItemClickListener(
    /* [in] */ AutoCompleteTextView* host)
    : mHost(host)
{}

ECode AutoCompleteTextView::DropDownItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    mHost->PerformCompletion(v, position, id);

    return NOERROR;
}

CAR_INTERFACE_IMPL(AutoCompleteTextView::PassThroughClickListener, Object, IViewOnClickListener)

AutoCompleteTextView::PassThroughClickListener::PassThroughClickListener(
    /* [in] */ AutoCompleteTextView* host)
    : mHost(host)
{}

ECode AutoCompleteTextView::PassThroughClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->OnClickImpl();

    if (mWrapped != NULL)
        mWrapped->OnClick(v);

    return NOERROR;
}

AutoCompleteTextView::PopupDataSetObserverOnchangedRunnable::PopupDataSetObserverOnchangedRunnable(
    /* [in] */ AutoCompleteTextView* host)
{
    mHost = host;
}

ECode AutoCompleteTextView::PopupDataSetObserverOnchangedRunnable::Run()
{
   if (mHost->mAdapter != NULL) {
       Int32 count;
       IAdapter::Probe(mHost->mAdapter)->GetCount(&count);
       mHost->UpdateDropDownForFilter(count);
   }
   return NOERROR;
}

AutoCompleteTextView::PopupDataSetObserver::PopupDataSetObserver(
    /* [in] */ AutoCompleteTextView* host)
{
    if (host != NULL) {
        host->GetWeakReference((IWeakReference**)&mWeakHost);
    }
}

ECode AutoCompleteTextView::PopupDataSetObserver::OnChanged()
{
    if (mWeakHost != NULL) {
        AutoPtr<IInterface> obj;
        mWeakHost->Resolve(EIID_IInterface, (IInterface**)&obj);
        AutoPtr<IAutoCompleteTextView> tmp = IAutoCompleteTextView::Probe(obj);
        AutoCompleteTextView* impl = (AutoCompleteTextView*)tmp.Get();
        if (impl != NULL && impl->mAdapter != NULL) {
            // If the popup is not showing already, showing it will cause
            // the list of data set observers attached to the adapter to
            // change. We can't do it from here, because we are in the middle
            // of iterating throught he list of observers.
            impl->mPopRunnable = new PopupDataSetObserverOnchangedRunnable(impl);
            Boolean rst;
            impl->Post(impl->mPopRunnable, &rst);
        }
    }

    return NOERROR;
}

ECode AutoCompleteTextView::PopupDataSetObserver::OnInvalidated()
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(AutoCompleteTextView::PopupWindowOnDismissListener, Object, IPopupWindowOnDismissListener)

AutoCompleteTextView::PopupWindowOnDismissListener::PopupWindowOnDismissListener(
    /* [in] */ IAutoCompleteTextViewOnDismissListener* l)
    : mDismissListener(l)
{}

ECode AutoCompleteTextView::PopupWindowOnDismissListener::OnDismiss()
{
    return mDismissListener->OnDismiss();
}

const String AutoCompleteTextView::TAG("AutoCompleteTextView");

CAR_INTERFACE_IMPL(AutoCompleteTextView, EditText, IAutoCompleteTextView)

AutoCompleteTextView::AutoCompleteTextView()
    : mHintResource(0)
    , mThreshold(0)
    , mDropDownAnchorId(0)
    , mDropDownDismissedOnCompletion(TRUE)
    , mLastKeyCode(FALSE)
    , mOpenBefore(FALSE)
    , mBlockCompletion(FALSE)
    , mPopupCanBeUpdated(TRUE)
{
}

ECode AutoCompleteTextView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode AutoCompleteTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::autoCompleteTextViewStyle);
}

ECode AutoCompleteTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return constructor(context, attrs, defStyle, 0);
}

ECode AutoCompleteTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(EditText::constructor(context, attrs, defStyleAttr, defStyleRes))

    CListPopupWindow::New(
        context, attrs, defStyleAttr, defStyleRes,
        (IListPopupWindow**)&mPopup);

    mPopup->SetSoftInputMode(IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE);
    mPopup->SetPromptPosition(IListPopupWindow::POSITION_PROMPT_BELOW);
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::AutoCompleteTextView);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes,(ITypedArray**)&a);

    a->GetInt32(R::styleable::AutoCompleteTextView_completionThreshold, 2, &mThreshold);

    AutoPtr<IDrawable> dropDownSelector;
    a->GetDrawable(R::styleable::AutoCompleteTextView_dropDownSelector,
        (IDrawable**)&dropDownSelector);
    mPopup->SetListSelector(dropDownSelector);

    // Get the anchor's id now, but the view won't be ready, so wait to actually get the
    // view and store it in mDropDownAnchorView lazily in getDropDownAnchorView later.
    // Defaults to NO_ID, in which case the getDropDownAnchorView method will simply return
    // this TextView, as a default anchoring point.
    a->GetResourceId(R::styleable::AutoCompleteTextView_dropDownAnchor,
            IView::NO_ID, &mDropDownAnchorId);

    // For dropdown width, the developer can specify a specific width, or MATCH_PARENT
    // (for full screen width) or WRAP_CONTENT (to match the width of the anchored view).
    Int32 dropDownWidth;
    a->GetLayoutDimension(
            R::styleable::AutoCompleteTextView_dropDownWidth,
            IViewGroupLayoutParams::WRAP_CONTENT, &dropDownWidth);
    mPopup->SetWidth(dropDownWidth);

    Int32 dropDownHeight;
    a->GetLayoutDimension(
            R::styleable::AutoCompleteTextView_dropDownHeight,
            IViewGroupLayoutParams::WRAP_CONTENT, &dropDownHeight);
    mPopup->SetHeight(dropDownHeight);


    a->GetResourceId(R::styleable::AutoCompleteTextView_completionHintView,
            R::layout::simple_dropdown_hint, &mHintResource);

    AutoPtr<DropDownItemClickListener> listener = new DropDownItemClickListener(this);
    mPopup->SetOnItemClickListener(listener);


    AutoPtr<ICharSequence> completionHint;
    a->GetText(R::styleable::AutoCompleteTextView_completionHint, (ICharSequence**)&completionHint);
    SetCompletionHint(completionHint);


    // Always turn on the auto complete input type flag, since it
    // makes no sense to use this widget without it.
    Int32 inputType;
    GetInputType(&inputType);
    if ((inputType&IInputType::TYPE_MASK_CLASS)
            == IInputType::TYPE_CLASS_TEXT) {
        inputType |= IInputType::TYPE_TEXT_FLAG_AUTO_COMPLETE;
        SetRawInputType(inputType);
    }

    a->Recycle();

    SetFocusable(TRUE);

    AutoPtr<MyWatcher> myWatcher = new MyWatcher(this);
    AddTextChangedListener(myWatcher);

    mPassThroughClickListener = new PassThroughClickListener(this);
    EditText::SetOnClickListener(mPassThroughClickListener);
    return NOERROR;
}

ECode AutoCompleteTextView::SetOnClickListener(
    /* [in] */ IViewOnClickListener* listener)
{
    mPassThroughClickListener->mWrapped = listener;

    return NOERROR;
}

/**
 * Private hook into the on click event, dispatched from {@link PassThroughClickListener}
 */
void AutoCompleteTextView::OnClickImpl()
{
    // If the dropdown is showing, bring the keyboard to the front
    // when the user touches the text field.
    Boolean isPopupShowing;
    if (IsPopupShowing(&isPopupShowing), isPopupShowing) {
        EnsureImeVisible(TRUE);
    }
}

/**
 * <p>Sets the optional hint text that is displayed at the bottom of the
 * the matching list.  This can be used as a cue to the user on how to
 * best use the list, or to provide extra information.</p>
 *
 * @param hint the text to be displayed to the user
 *
 * @attr ref android.R.styleable#AutoCompleteTextView_completionHint
 */
ECode AutoCompleteTextView::SetCompletionHint(
    /* [in] */ ICharSequence* hint)
{
    mHintText = hint;

    if (hint != NULL) {
        if (mHintView == NULL) {
            AutoPtr<ILayoutInflater> layoutInflater;
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            LayoutInflater::From(context, (ILayoutInflater**)&layoutInflater);
            AutoPtr<IView> view;
            layoutInflater->Inflate(mHintResource, NULL, (IView**)&view);
            AutoPtr<IView> viewObj;
            view->FindViewById(R::id::text1, (IView**)&viewObj);
            AutoPtr<ITextView> hintView = ITextView::Probe(viewObj);
            hintView->SetText(mHintText);
            mHintView = hintView;
            mPopup->SetPromptView(viewObj);
        } else {
            mHintView->SetText(hint);
        }
    } else {
        mPopup->SetPromptView(NULL);
        mHintView = NULL;
    }
    return NOERROR;
}

/**
 * Gets the optional hint text displayed at the bottom of the the matching list.
 *
 * @return The hint text, if any
 *
 * @see #setCompletionHint(CharSequence)
 *
 * @attr ref android.R.styleable#AutoCompleteTextView_completionHint
 */
ECode AutoCompleteTextView::GetCompletionHint(
    /* [out] */ ICharSequence** hint)
{
    VALIDATE_NOT_NULL(hint)

    *hint = mHintText;
    REFCOUNT_ADD(*hint)
    return NOERROR;
}

/**
 * <p>Returns the current width for the auto-complete drop down list. This can
 * be a fixed width, or {@link ViewGroup.LayoutParams#MATCH_PARENT} to fill the screen, or
 * {@link ViewGroup.LayoutParams#WRAP_CONTENT} to fit the width of its anchor view.</p>
 *
 * @return the width for the drop down list
 *
 * @attr ref android.R.styleable#AutoCompleteTextView_dropDownWidth
 */
ECode AutoCompleteTextView::GetDropDownWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    return mPopup->GetWidth(width);
}

/**
 * <p>Sets the current width for the auto-complete drop down list. This can
 * be a fixed width, or {@link ViewGroup.LayoutParams#MATCH_PARENT} to fill the screen, or
 * {@link ViewGroup.LayoutParams#WRAP_CONTENT} to fit the width of its anchor view.</p>
 *
 * @param width the width to use
 *
 * @attr ref android.R.styleable#AutoCompleteTextView_dropDownWidth
 */
ECode AutoCompleteTextView::SetDropDownWidth(
    /* [in] */ Int32 width)
{
    return mPopup->SetWidth(width);
}

/**
 * <p>Returns the current height for the auto-complete drop down list. This can
 * be a fixed height, or {@link ViewGroup.LayoutParams#MATCH_PARENT} to fill
 * the screen, or {@link ViewGroup.LayoutParams#WRAP_CONTENT} to fit the height
 * of the drop down's content.</p>
 *
 * @return the height for the drop down list
 *
 * @attr ref android.R.styleable#AutoCompleteTextView_dropDownHeight
 */
ECode AutoCompleteTextView::GetDropDownHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    return mPopup->GetHeight(height);
}

/**
 * <p>Sets the current height for the auto-complete drop down list. This can
 * be a fixed height, or {@link ViewGroup.LayoutParams#MATCH_PARENT} to fill
 * the screen, or {@link ViewGroup.LayoutParams#WRAP_CONTENT} to fit the height
 * of the drop down's content.</p>
 *
 * @param height the height to use
 *
 * @attr ref android.R.styleable#AutoCompleteTextView_dropDownHeight
 */
ECode AutoCompleteTextView::SetDropDownHeight(
    /* [in] */ Int32 height)
{
    mPopup->SetHeight(height);

    return NOERROR;
}

/**
 * <p>Returns the id for the view that the auto-complete drop down list is anchored to.</p>
 *
 * @return the view's id, or {@link View#NO_ID} if none specified
 *
 * @attr ref android.R.styleable#AutoCompleteTextView_dropDownAnchor
 */
ECode AutoCompleteTextView::GetDropDownAnchor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDropDownAnchorId;
    return NOERROR;
}

/**
 * <p>Sets the view to which the auto-complete drop down list should anchor. The view
 * corresponding to this id will not be loaded until the next time it is needed to avoid
 * loading a view which is not yet instantiated.</p>
 *
 * @param id the id to anchor the drop down list view to
 *
 * @attr ref android.R.styleable#AutoCompleteTextView_dropDownAnchor
 */
ECode AutoCompleteTextView::SetDropDownAnchor(
    /* [in] */ Int32 id)
{
    mDropDownAnchorId = id;
    mPopup->SetAnchorView(NULL);

    return NOERROR;
}

/**
 * <p>Gets the background of the auto-complete drop-down list.</p>
 *
 * @return the background drawable
 *
 * @attr ref android.R.styleable#PopupWindow_popupBackground
 */
ECode AutoCompleteTextView::GetDropDownBackground(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    return mPopup->GetBackground(drawable);
}

/**
 * <p>Sets the background of the auto-complete drop-down list.</p>
 *
 * @param d the drawable to set as the background
 *
 * @attr ref android.R.styleable#PopupWindow_popupBackground
 */
ECode AutoCompleteTextView::SetDropDownBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    return mPopup->SetBackgroundDrawable(d);
}

/**
 * <p>Sets the background of the auto-complete drop-down list.</p>
 *
 * @param id the id of the drawable to set as the background
 *
 * @attr ref android.R.styleable#PopupWindow_popupBackground
 */
ECode AutoCompleteTextView::SetDropDownBackgroundResource(
    /* [in] */ Int32 id)
{
    AutoPtr<IDrawable> drawable;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetDrawable(id, (IDrawable**)&drawable);

    return mPopup->SetBackgroundDrawable(drawable);
}

/**
 * <p>Sets the vertical offset used for the auto-complete drop-down list.</p>
 *
 * @param offset the vertical offset
 */
ECode AutoCompleteTextView::SetDropDownVerticalOffset(
    /* [in] */ Int32 offset)
{
    mPopup->SetVerticalOffset(offset);

    return NOERROR;
}

/**
 * <p>Gets the vertical offset used for the auto-complete drop-down list.</p>
 *
 * @return the vertical offset
 */
ECode AutoCompleteTextView::GetDropDownVerticalOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)
    return mPopup->GetVerticalOffset(offset);
}

/**
 * <p>Sets the horizontal offset used for the auto-complete drop-down list.</p>
 *
 * @param offset the horizontal offset
 */
ECode AutoCompleteTextView::SetDropDownHorizontalOffset(
    /* [in] */ Int32 offset)
{
    mPopup->SetHorizontalOffset(offset);

    return NOERROR;
}

/**
 * <p>Gets the horizontal offset used for the auto-complete drop-down list.</p>
 *
 * @return the horizontal offset
 */
ECode AutoCompleteTextView::GetDropDownHorizontalOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)
    return mPopup->GetHorizontalOffset(offset);
}

/**
 * <p>Sets the animation style of the auto-complete drop-down list.</p>
 *
 * <p>If the drop-down is showing, calling this method will take effect only
 * the next time the drop-down is shown.</p>
 *
 * @param animationStyle animation style to use when the drop-down appears
 *      and disappears.  Set to -1 for the default animation, 0 for no
 *      animation, or a resource identifier for an explicit animation.
 *
 * @hide Pending API council approval
 */
ECode AutoCompleteTextView::SetDropDownAnimationStyle(
    /* [in] */ Int32 animationStyle)
{
    mPopup->SetAnimationStyle(animationStyle);

    return NOERROR;
}

/**
 * <p>Returns the animation style that is used when the drop-down list appears and disappears
 * </p>
 *
 * @return the animation style that is used when the drop-down list appears and disappears
 *
 * @hide Pending API council approval
 */
ECode AutoCompleteTextView::GetDropDownAnimationStyle(
    /* [out] */ Int32* animationStyle)
{
    VALIDATE_NOT_NULL(animationStyle)
    return mPopup->GetAnimationStyle(animationStyle);
}

/**
 * @return Whether the drop-down is visible as long as there is {@link #enoughToFilter()}
 *
 * @hide Pending API council approval
 */
ECode AutoCompleteTextView::IsDropDownAlwaysVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible)
    return mPopup->IsDropDownAlwaysVisible(visible);
}

/**
 * Sets whether the drop-down should remain visible as long as there is there is
 * {@link #enoughToFilter()}.  This is useful if an unknown number of results are expected
 * to show up in the adapter sometime in the future.
 *
 * The drop-down will occupy the entire screen below {@link #getDropDownAnchor} regardless
 * of the size or content of the list.  {@link #getDropDownBackground()} will fill any space
 * that is not used by the list.
 *
 * @param dropDownAlwaysVisible Whether to keep the drop-down visible.
 *
 * @hide Pending API council approval
 */
ECode AutoCompleteTextView::SetDropDownAlwaysVisible(
    /* [in] */ Boolean dropDownAlwaysVisible)
{
    mPopup->SetDropDownAlwaysVisible(dropDownAlwaysVisible);

    return NOERROR;
}

/**
 * Checks whether the drop-down is dismissed when a suggestion is clicked.
 *
 * @hide Pending API council approval
 */
ECode AutoCompleteTextView::IsDropDownDismissedOnCompletion(
    /* [out] */ Boolean* completion)
{
    VALIDATE_NOT_NULL(completion)
    *completion = mDropDownDismissedOnCompletion;
    return NOERROR;
}

/**
 * Sets whether the drop-down is dismissed when a suggestion is clicked. This is
 * TRUE by default.
 *
 * @param dropDownDismissedOnCompletion Whether to dismiss the drop-down.
 *
 * @hide Pending API council approval
 */
ECode AutoCompleteTextView::SetDropDownDismissedOnCompletion(
    /* [in] */ Boolean dropDownDismissedOnCompletion)
{
    mDropDownDismissedOnCompletion = dropDownDismissedOnCompletion;

    return NOERROR;
}

/**
 * <p>Returns the number of characters the user must type before the drop
 * down list is shown.</p>
 *
 * @return the minimum number of characters to type to show the drop down
 *
 * @see #setThreshold(Int32)
 */
ECode AutoCompleteTextView::GetThreshold(
    /* [out] */ Int32* threshold)
{
    VALIDATE_NOT_NULL(threshold)
    *threshold = mThreshold;
    return NOERROR;
}

/**
 * <p>Specifies the minimum number of characters the user has to type in the
 * edit box before the drop down list is shown.</p>
 *
 * <p>When <code>threshold</code> is less than or equals 0, a threshold of
 * 1 is applied.</p>
 *
 * @param threshold the number of characters to type before the drop down
 *                  is shown
 *
 * @see #getThreshold()
 *
 * @attr ref android.R.styleable#AutoCompleteTextView_completionThreshold
 */
ECode AutoCompleteTextView::SetThreshold(
    /* [in] */ Int32 threshold)
{
    if (threshold <= 0) {
        threshold = 1;
    }

    mThreshold = threshold;

    return NOERROR;
}

/**
 * <p>Sets the listener that will be notified when the user clicks an item
 * in the drop down list.</p>
 *
 * @param l the item click listener
 */
ECode AutoCompleteTextView::SetOnItemClickListener(
    /* [in] */ IAdapterViewOnItemClickListener* l)
{
    mItemClickListener = l;

    return NOERROR;
}

ECode AutoCompleteTextView::GetItemClickListener(
    /* [out] */ IAdapterViewOnItemClickListener** l)
{
    VALIDATE_NOT_NULL(l)
    *l = mItemClickListener;
    REFCOUNT_ADD(*l)
    return NOERROR;
}

/**
 * <p>Sets the listener that will be notified when the user selects an item
 * in the drop down list.</p>
 *
 * @param l the item selected listener
 */
ECode AutoCompleteTextView::SetOnItemSelectedListener(
    /* [in] */ IAdapterViewOnItemSelectedListener* l)
{
    mItemSelectedListener = l;

    return NOERROR;
}

/**
 * <p>Returns the listener that is notified whenever the user clicks an item
 * in the drop down list.</p>
 *
 * @return the item click listener
 */
ECode AutoCompleteTextView::GetOnItemClickListener(
    /* [out] */ IAdapterViewOnItemClickListener** l)
{
    VALIDATE_NOT_NULL(l)
    *l = mItemClickListener;
    REFCOUNT_ADD(*l)
    return NOERROR;
}

/**
 * <p>Returns the listener that is notified whenever the user selects an
 * item in the drop down list.</p>
 *
 * @return the item selected listener
 */
ECode AutoCompleteTextView::GetOnItemSelectedListener(
    /* [out] */ IAdapterViewOnItemSelectedListener** l)
{
    VALIDATE_NOT_NULL(l)
    *l = mItemSelectedListener;
    REFCOUNT_ADD(*l)
    return NOERROR;
}

/**
 * Set a listener that will be invoked whenever the AutoCompleteTextView's
 * list of completions is dismissed.
 * @param dismissListener Listener to invoke when completions are dismissed
 */
ECode AutoCompleteTextView::SetOnDismissListener(
    /* [in] */ IAutoCompleteTextViewOnDismissListener* dismissListener)
{
    AutoPtr<IPopupWindowOnDismissListener> wrappedListener = NULL;
    if (dismissListener != NULL) {
        wrappedListener = new PopupWindowOnDismissListener(dismissListener);
    }
    mPopup->SetOnDismissListener(wrappedListener);
    return NOERROR;
}

/**
 * <p>Returns a filterable list adapter used for auto completion.</p>
 *
 * @return a data adapter used for auto completion
 */
ECode AutoCompleteTextView::GetAdapter(
    /* [out] */ IListAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter)
    *adapter = mAdapter;
    REFCOUNT_ADD(*adapter)
    return NOERROR;
}

/**
 * <p>Changes the list of data used for auto completion. The provided list
 * must be a filterable list adapter.</p>
 *
 * <p>The caller is still responsible for managing any resources used by the adapter.
 * Notably, when the AutoCompleteTextView is closed or released, the adapter is not notified.
 * A common case is the use of {@link android.widget.CursorAdapter}, which
 * contains a {@link android.database.Cursor} that must be closed.  This can be done
 * automatically (see
 * {@link android.app.Activity#startManagingCursor(android.database.Cursor)
 * startManagingCursor()}),
 * or by manually closing the cursor when the AutoCompleteTextView is dismissed.</p>
 *
 * @param adapter the adapter holding the auto completion data
 *
 * @see #getAdapter()
 * @see android.widget.Filterable
 * @see android.widget.ListAdapter
 */
ECode AutoCompleteTextView::SetAdapter(
    /* [in] */ IListAdapter* adapter)
{
    if (mObserver == NULL) {
        mObserver = new PopupDataSetObserver(this);
    }
    else if (mAdapter != NULL) {
        IAdapter::Probe(mAdapter)->UnregisterDataSetObserver(mObserver);
    }
    mAdapter = adapter;
    if (mAdapter != NULL) {
        //noinspection unchecked
        if(mFilter) mFilter = NULL;
        AutoPtr<IFilterable> fTemp = IFilterable::Probe(mAdapter);
        fTemp->GetFilter((IFilter**)&mFilter);
        IAdapter::Probe(adapter)->RegisterDataSetObserver(mObserver);
    }
    else {
        mFilter = NULL;
    }

    mPopup->SetAdapter(mAdapter);

    return NOERROR;
}

ECode AutoCompleteTextView::OnKeyPreIme(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Boolean* result)
{
    Boolean visible;
    mPopup->IsDropDownAlwaysVisible(&visible);
    Boolean isPopupShowing;
    if (keyCode == IKeyEvent::KEYCODE_BACK && (IsPopupShowing(&isPopupShowing), isPopupShowing)
            && !visible) {
        // special case for the back key, we do not even try to send it
        // to the drop down list but instead, consume it immediately

        Int32 action;
        event->GetAction(&action);

        Int32 count;
        event->GetRepeatCount(&count);

        if (action == IKeyEvent::ACTION_DOWN && count == 0) {
            AutoPtr<IDispatcherState> state ;
            GetKeyDispatcherState((IDispatcherState**)&state);
            if (state != NULL) {
                state->StartTracking(event, (IAutoCompleteTextView*)this);
            }
            *result = TRUE;
            return NOERROR;
        }
        else if (action == IKeyEvent::ACTION_UP) {
            AutoPtr<IDispatcherState> state;
            GetKeyDispatcherState((IDispatcherState**)&state);
            if (state != NULL) {
                state->HandleUpEvent(event);
            }

            Boolean tracking, canceled;
            event->IsTracking(&tracking);
            event->IsCanceled(&canceled);
            if (tracking && !canceled) {
                DismissDropDown();
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    return EditText::OnKeyPreIme(keyCode, event, result);
}

ECode AutoCompleteTextView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Boolean* result)
{
    Boolean consumed;
    mPopup->OnKeyUp(keyCode, event, &consumed);
    if (consumed) {
        switch (keyCode) {
            // if the list accepts the key events and the key event
            // was a click, the text view gets the selected item
            // from the drop down as its content
            case IKeyEvent::KEYCODE_ENTER:
            case IKeyEvent::KEYCODE_DPAD_CENTER:
            case IKeyEvent::KEYCODE_TAB:
                Boolean res;
                event->HasNoModifiers(&res);
                if (res) {
                    PerformCompletion();
                }
                *result = TRUE;
                return NOERROR;
        }
    }

    Boolean res;
    event->HasNoModifiers(&res);
    Boolean isPopupShowing;
    if ((IsPopupShowing(&isPopupShowing), isPopupShowing) && keyCode == IKeyEvent::KEYCODE_TAB && res) {
        PerformCompletion();
        *result = TRUE;
        return NOERROR;
    }

    return EditText::OnKeyUp(keyCode, event, result);
}

ECode AutoCompleteTextView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Boolean* result)
{
    Boolean res;
    mPopup->OnKeyDown(keyCode, event, &res);
    if (res) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean isPopupShowing;
    if (IsPopupShowing(&isPopupShowing), !isPopupShowing) {
        switch(keyCode) {
            case IKeyEvent::KEYCODE_DPAD_DOWN:
                event->HasNoModifiers(&res);
                if (res) {
                    PerformValidation();
                }
        }
    }

    event->HasNoModifiers(&res);
    if ((IsPopupShowing(&isPopupShowing), isPopupShowing) && (keyCode == IKeyEvent::KEYCODE_TAB) && res) {
        *result = TRUE;
        return NOERROR;
    }

    mLastKeyCode = keyCode;
    Boolean handled;
    EditText::OnKeyDown(keyCode, event, &handled);
    mLastKeyCode = IKeyEvent::KEYCODE_UNKNOWN;

    if (handled && (IsPopupShowing(&isPopupShowing), isPopupShowing)) {
        ClearListSelection();
    }

    *result = handled;
    return NOERROR;
}

/**
 * Returns <code>TRUE</code> if the amount of text in the field meets
 * or exceeds the {@link #getThreshold} requirement.  You can override
 * this to impose a different standard for when filtering will be
 * triggered.
 */
ECode AutoCompleteTextView::EnoughToFilter(
    /* [in] */ Boolean* result)
{
    /*if (DEBUG) Log.v(TAG, "Enough to filter: len=" + getText().length()
            + " threshold=" + mThreshold);*/

    AutoPtr<ICharSequence> chars;
    GetText((ICharSequence**)&chars);
    Int32 len;
    chars->GetLength(&len);
    *result = len >= mThreshold;
    return NOERROR;
}

/**
 * This is used to watch for edits to the text view.  Note that we call
 * to methods on the auto complete text view class so that we can access
 * private vars without going through thunks.
 */

void AutoCompleteTextView::DoBeforeTextChanged()
{
    if (mBlockCompletion) return;

    // when text is changed, inserted or deleted, we attempt to show
    // the drop down
    IsPopupShowing(&mOpenBefore);
    //if (DEBUG) Log.v(TAG, "before text changed: open=" + mOpenBefore);
}

void AutoCompleteTextView::DoAfterTextChanged()
{
    if (mBlockCompletion) return;

    // if the list was open before the keystroke, but closed afterwards,
    // then something in the keystroke processing (an input filter perhaps)
    // called performCompletion() and we shouldn't do any more processing.
    /*if (DEBUG) Log.v(TAG, "after text changed: openBefore=" + mOpenBefore
            + " open=" + isPopupShowing());*/
    Boolean isPopupShowing;
    if (mOpenBefore && (IsPopupShowing(&isPopupShowing), !isPopupShowing)) {
        return;
    }

    // the drop down is shown only when a minimum number of characters
    // was typed in the text view
    Boolean enough;
    if (EnoughToFilter(&enough), enough) {
        if (mFilter != NULL) {
            mPopupCanBeUpdated = TRUE;
            AutoPtr<ICharSequence> chars;
            GetText((ICharSequence**)&chars);
            PerformFiltering(chars, mLastKeyCode);
        }
    }
    else {
        // drop down is automatically dismissed when enough characters
        // are deleted from the text view

        Boolean res;
        mPopup->IsDropDownAlwaysVisible(&res);
        if (!res) {
            DismissDropDown();
        }
        if (mFilter != NULL) {
            mFilter->DoFilter(NULL);
        }
    }
}

/**
 * <p>Indicates whether the popup menu is showing.</p>
 *
 * @return TRUE if the popup menu is showing, FALSE otherwise
 */
ECode AutoCompleteTextView::IsPopupShowing(
    /* [out] */ Boolean* isPopupShowing)
{
    VALIDATE_NOT_NULL(isPopupShowing)
    return mPopup->IsShowing(isPopupShowing);
}

/**
 * <p>Converts the selected item from the drop down list into a sequence
 * of character that can be used in the edit box.</p>
 *
 * @param selectedItem the item selected by the user for completion
 *
 * @return a sequence of characters representing the selected suggestion
 */
AutoPtr<ICharSequence> AutoCompleteTextView::ConvertSelectionToString(
    /* [in] */ IInterface* selectedItem)
{
    AutoPtr<ICharSequence> cs;
    mFilter->ConvertResultToString(selectedItem, (ICharSequence**)&cs);
    return cs;
}

/**
 * <p>Clear the list selection.  This may only be temporary, as user input will often bring
 * it back.
 */
ECode AutoCompleteTextView::ClearListSelection()
{
    return mPopup->ClearListSelection();
}

/**
 * Set the position of the dropdown view selection.
 *
 * @param position The position to move the selector to.
 */
ECode AutoCompleteTextView::SetListSelection(
    /* [in] */ Int32 position)
{
    return mPopup->SetSelection(position);
}

/**
 * Get the position of the dropdown view selection, if there is one.  Returns
 * {@link ListView#INVALID_POSITION ListView.INVALID_POSITION} if there is no dropdown or if
 * there is no selection.
 *
 * @return the position of the current selection, if there is one, or
 * {@link ListView#INVALID_POSITION ListView.INVALID_POSITION} if not.
 *
 * @see ListView#getSelectedItemPosition()
 */
ECode AutoCompleteTextView::GetListSelection(
    /* [out] */ Int32* selection)
{
    VALIDATE_NOT_NULL(selection)
    return mPopup->GetSelectedItemPosition(selection);
}

/**
 * <p>Starts filtering the content of the drop down list. The filtering
 * pattern is the content of the edit box. Subclasses should override this
 * method to filter with a different pattern, for instance a substring of
 * <code>text</code>.</p>
 *
 * @param text the filtering pattern
 * @param keyCode the last character inserted in the edit box; beware that
 * this will be NULL when text is being added through a soft input method.
 */
void AutoCompleteTextView::PerformFiltering(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 keyCode)
{
    mFilter->DoFilter(text, this);
}

/**
 * <p>Performs the text completion by converting the selected item from
 * the drop down list into a string, replacing the text box's content with
 * this string and finally dismissing the drop down menu.</p>
 */
ECode AutoCompleteTextView::PerformCompletion()
{
    PerformCompletion(NULL, -1, -1);

    return NOERROR;
}

ECode AutoCompleteTextView::OnCommitCompletion(
    /* [in] */ ICompletionInfo* completion)
{
    Boolean isPopupShowing;
    if (IsPopupShowing(&isPopupShowing), isPopupShowing) {
        Int32 pos;
        Boolean rst;
        completion->GetPosition(&pos);
        mPopup->PerformItemClick(pos, &rst);
    }

    return NOERROR;
}

void AutoCompleteTextView::PerformCompletion(
    /* [in] */ IView* selectedView,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Boolean isPopupShowing;
    if (IsPopupShowing(&isPopupShowing), isPopupShowing) {
        AutoPtr<IInterface> selectedItem;
        if (position < 0) {
            mPopup->GetSelectedItem((IInterface**)&selectedItem);
        } else {
            IAdapter::Probe(mAdapter)->GetItem(position, (IInterface**)&selectedItem);
        }
        if (selectedItem == NULL) {
            //Log.w(TAG, "performCompletion: no selected item");
            return;
        }

        mBlockCompletion = TRUE;
        ReplaceText(ConvertSelectionToString(selectedItem));
        mBlockCompletion = FALSE;

        if (mItemClickListener != NULL) {

            if (selectedView == NULL || position < 0) {
                mPopup->GetSelectedView(&selectedView);
                mPopup->GetSelectedItemPosition(&position);
                mPopup->GetSelectedItemId(&id);
            }

            AutoPtr<IListView> v;
            mPopup->GetListView((IListView**)&v);

            mItemClickListener->OnItemClick(IAdapterView::Probe(v), selectedView, position, id);
        }
    }

    Boolean visible;
    mPopup->IsDropDownAlwaysVisible(&visible);

    if (mDropDownDismissedOnCompletion && !visible) {
        DismissDropDown();
    }
}

/**
 * Identifies whether the view is currently performing a text completion, so subclasses
 * can decide whether to respond to text changed events.
 */
ECode AutoCompleteTextView::IsPerformingCompletion(
    /* [out] */ Boolean* isPerformingCompletion)
{
    VALIDATE_NOT_NULL(isPerformingCompletion)
    *isPerformingCompletion = mBlockCompletion;
    return NOERROR;
}

/**
 * Like {@link #setText(CharSequence)}, except that it can disable filtering.
 *
 * @param filter If <code>FALSE</code>, no filtering will be performed
 *        as a result of this call.
 *
 * @hide Pending API council approval.
 */
ECode AutoCompleteTextView::SetText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Boolean filter)
{
    if (filter) {
        SetText(text);
    }
    else {
        mBlockCompletion = TRUE;
        SetText(text);
        mBlockCompletion = FALSE;
    }

    return NOERROR;
}

/**
 * <p>Performs the text completion by replacing the current text by the
 * selected item. Subclasses should override this method to avoid replacing
 * the whole content of the edit box.</p>
 *
 * @param text the selected suggestion in the drop down list
 */
void AutoCompleteTextView::ReplaceText(
    /* [in] */ ICharSequence* text)
{
    ClearComposingText();

    SetText(text);
    // make sure we keep the caret at the end of the text view
    AutoPtr<ICharSequence> chars;
    GetText((ICharSequence**)&chars);
    AutoPtr<IEditable> spannable = IEditable::Probe(chars);
    assert(spannable != NULL);
    Int32 len = 0;
    ICharSequence::Probe(spannable)->GetLength(&len);
    Selection::SetSelection(ISpannable::Probe(spannable), len);
}

/** {@inheritDoc} */
ECode AutoCompleteTextView::OnFilterComplete(
    /* [in] */ Int32 count)
{
    UpdateDropDownForFilter(count);

    return NOERROR;
}

void AutoCompleteTextView::UpdateDropDownForFilter(
    /* [in] */ Int32 count)
{
    // Not attached to window, don't update drop-down
    Int32 visible;
    GetWindowVisibility(&visible);
    if (visible == IView::GONE) return;

    /*
     * This checks enoughToFilter() again because filtering requests
     * are asynchronous, so the result may come back after enough text
     * has since been deleted to make it no longer appropriate
     * to filter.
     */

    Boolean dropDownAlwaysVisible;
    mPopup->IsDropDownAlwaysVisible(&dropDownAlwaysVisible);

    Boolean enoughToFilter, isPopupShowing;
    EnoughToFilter(&enoughToFilter);
    if ((count > 0 || dropDownAlwaysVisible) && enoughToFilter) {
        Boolean hasFocus, hasWindowFocus;
        HasFocus(&hasFocus);
        HasWindowFocus(&hasWindowFocus);
        if (hasFocus && hasWindowFocus && mPopupCanBeUpdated) {
            ShowDropDown();
        }
    } else if (!dropDownAlwaysVisible && (IsPopupShowing(&isPopupShowing), isPopupShowing)) {
        DismissDropDown();
        // When the filter text is changed, the first update from the adapter may show an empty
        // count (when the query is being performed on the network). Future updates when some
        // content has been retrieved should still be able to update the list.
        mPopupCanBeUpdated = TRUE;
    }
}

ECode AutoCompleteTextView::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    EditText::OnWindowFocusChanged(hasWindowFocus);

    Boolean v;
    mPopup->IsDropDownAlwaysVisible(&v);

    if (!hasWindowFocus && !v) {
        DismissDropDown();
    }

    return NOERROR;
}

void AutoCompleteTextView::OnDisplayHint(
    /* [in] */ Int32 hint)
{
    EditText::OnDisplayHint(hint);
    switch (hint) {
        case IView::INVISIBLE:

            Boolean visible;
            mPopup->IsDropDownAlwaysVisible(&visible);
            if (!visible) {
                DismissDropDown();
            }
            break;
    }
}

void AutoCompleteTextView::OnFocusChanged(
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    EditText::OnFocusChanged(focused, direction, previouslyFocusedRect);
    // Perform validation if the view is losing focus.
    if (!focused) {
        PerformValidation();
    }

    Boolean visible;
    mPopup->IsDropDownAlwaysVisible(&visible);

    if (!focused && !visible) {
        DismissDropDown();
    }
}

ECode AutoCompleteTextView::OnAttachedToWindow()
{
    return EditText::OnAttachedToWindow();
}

ECode AutoCompleteTextView::OnDetachedFromWindow()
{
    DismissDropDown();
    return EditText::OnDetachedFromWindow();
}

/**
 * <p>Closes the drop down if present on screen.</p>
 */
ECode AutoCompleteTextView::DismissDropDown()
{
    AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
     if (imm != NULL) {
        imm->DisplayCompletions(this, NULL);
     }
    mPopup->Dismiss();
    mPopupCanBeUpdated = FALSE;
    return NOERROR;
}

Boolean AutoCompleteTextView::SetFrame(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Boolean result = EditText::SetFrame(l, t, r, b);

    Boolean isPopupShowing;
    if (IsPopupShowing(&isPopupShowing), isPopupShowing) {
        ShowDropDown();
    }

    return result;
}



/**
 * Issues a runnable to show the dropdown as soon as possible.
 *
 * @hide internal used only by SearchDialog
 */
ECode AutoCompleteTextView::ShowDropDownAfterLayout()
{
    mPopup->PostShow();

    return NOERROR;
}

/**
 * Ensures that the drop down is not obscuring the IME.
 * @param visible whether the ime should be in front. If FALSE, the ime is pushed to
 * the background.
 * @hide internal used only here and SearchDialog
 */
ECode AutoCompleteTextView::EnsureImeVisible(
    /* [in] */ Boolean visible)
{
    mPopup->SetInputMethodMode(visible
        ? IPopupWindow::INPUT_METHOD_NEEDED : IPopupWindow::INPUT_METHOD_NOT_NEEDED);

    Boolean isVisible;
    mPopup->IsDropDownAlwaysVisible(&isVisible);

    Boolean enough;
    if (visible || (mFilter != NULL && (EnoughToFilter(&enough), enough))) {
        ShowDropDown();
    }

    return NOERROR;
}

/**
 * @hide internal used only here and SearchDialog
 */
ECode AutoCompleteTextView::IsInputMethodNotNeeded(
    /* [out] */ Boolean* isInputMethodNotNeeded)
{
    VALIDATE_NOT_NULL(isInputMethodNotNeeded)
    Int32 mode;
    mPopup->GetInputMethodMode(&mode);
    *isInputMethodNotNeeded = mode == IPopupWindow::INPUT_METHOD_NOT_NEEDED;
    return NOERROR;
}

/**
 * <p>Displays the drop down on screen.</p>
 */
ECode AutoCompleteTextView::ShowDropDown()
{
    BuildImeCompletions();

    AutoPtr<IView> view;
    mPopup->GetAnchorView((IView**)&view);
    if (view == NULL) {
        if (mDropDownAnchorId != IView::NO_ID) {
            AutoPtr<IView> anchorView;
            AutoPtr<IView> root;
            GetRootView((IView**)&root);
            root->FindViewById(mDropDownAnchorId, (IView**)&anchorView);
            mPopup->SetAnchorView(anchorView);
        } else {
            mPopup->SetAnchorView(this);
        }
    }
    Boolean isPopupShowing;
    if (IsPopupShowing(&isPopupShowing), !isPopupShowing) {
        // Make sure the list does not obscure the IME when shown for the first time.
        mPopup->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NEEDED);
        mPopup->SetListItemExpandMax(EXPAND_MAX);
    }
    mPopup->Show();
    AutoPtr<IListView> listView;

    mPopup->GetListView((IListView**)&listView);
    IView::Probe(listView)->SetOverScrollMode(IView::OVER_SCROLL_ALWAYS);

    return NOERROR;
}

/**
 * Forces outside touches to be ignored. Normally if {@link #isDropDownAlwaysVisible()} is
 * FALSE, we allow outside touch to dismiss the dropdown. If this is set to TRUE, then we
 * ignore outside touch even when the drop down is not set to always visible.
 *
 * @hide used only by SearchDialog
 */
ECode AutoCompleteTextView::SetForceIgnoreOutsideTouch(
    /* [in] */ Boolean forceIgnoreOutsideTouch)
{
    mPopup->SetForceIgnoreOutsideTouch(forceIgnoreOutsideTouch);

    return NOERROR;
}

void AutoCompleteTextView::BuildImeCompletions()
{
    AutoPtr<IListAdapter> adapter = mAdapter;
    if (adapter != NULL) {
        AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
        if (imm != NULL) {
            Int32 count = 0;
            IAdapter::Probe(adapter)->GetCount(&count);
            count = Elastos::Core::Math::Min(count, 20);
            AutoPtr<ArrayOf<ICompletionInfo*> > completions = ArrayOf<ICompletionInfo*>::Alloc(count);
            Int32 realCount = 0;

            for (Int32 i = 0; i < count; i++) {
                Boolean enabled = FALSE;
                adapter->IsEnabled(i, &enabled);
                if (enabled) {
                    AutoPtr<IInterface> item;
                    IAdapter::Probe(adapter)->GetItem(i, (IInterface**)&item);
                    Int64 id = 0;
                    IAdapter::Probe(adapter)->GetItemId(i, &id);
                    AutoPtr<ICompletionInfo> tmpInfo;
                    CCompletionInfo::New(id, realCount,
                           ConvertSelectionToString(item), (ICompletionInfo**)&tmpInfo);
                    completions->Set(realCount, tmpInfo);
                }
            }

            if (realCount != count) {
                AutoPtr<ArrayOf<ICompletionInfo*> > tmp = ArrayOf<ICompletionInfo*>::Alloc(realCount);
                tmp->Copy(completions, 0, realCount);
                completions = tmp;
            }

            imm->DisplayCompletions(this, completions);
        }
    }

}

/**
 * Sets the validator used to perform text validation.
 *
 * @param validator The validator used to validate the text entered in this widget.
 *
 * @see #getValidator()
 * @see #performValidation()
 */
ECode AutoCompleteTextView::SetValidator(
    /* [in] */ IValidator* validator)
{
    mValidator = validator;

    return NOERROR;
}

/**
 * Returns the Validator set with {@link #setValidator},
 * or <code>NULL</code> if it was not set.
 *
 * @see #setValidator(android.widget.AutoCompleteTextView.Validator)
 * @see #performValidation()
 */
ECode AutoCompleteTextView::GetValidator(
    /* [out] */ IValidator** validator)
{
    VALIDATE_NOT_NULL(validator)
    *validator = mValidator;
    REFCOUNT_ADD(*validator)
    return NOERROR;
}

/**
 * If a validator was set on this view and the current string is not valid,
 * ask the validator to fix it.
 *
 * @see #getValidator()
 * @see #setValidator(android.widget.AutoCompleteTextView.Validator)
 */
ECode AutoCompleteTextView::PerformValidation()
{
    if (mValidator == NULL) return NOERROR;

    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);

    Boolean res;
    mValidator->IsValid(text, &res);

    if (!TextUtils::IsEmpty(text) && !res) {
        AutoPtr<ICharSequence> str;
        mValidator->FixText(text, (ICharSequence**)&str);
        SetText(str);
    }

    return NOERROR;
}

/**
 * Returns the Filter obtained from {@link Filterable#getFilter},
 * or <code>NULL</code> if {@link #setAdapter} was not called with
 * a Filterable.
 */
AutoPtr<IFilter> AutoCompleteTextView::GetFilter()
{
    return mFilter;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
