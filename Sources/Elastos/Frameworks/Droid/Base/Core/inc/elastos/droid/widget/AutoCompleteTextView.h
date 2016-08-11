
#ifndef __ELASTOS_DROID_WIDGET_AUTOCOMPLETETEXTVIEW_H__
#define __ELASTOS_DROID_WIDGET_AUTOCOMPLETETEXTVIEW_H__

#include "elastos/droid/widget/EditText.h"
#include "elastos/droid/widget/ListView.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/database/DataSetObserver.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::Os::Runnable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Widget {

class AutoCompleteTextView
    : public EditText
    , public IAutoCompleteTextView
    , public IFilterListener
{
private:
    /**
     * This is used to watch for edits to the text view.  Note that we call
     * to methods on the auto complete text view class so that we can access
     * private vars without going through thunks.
     */
    class MyWatcher
        : public ITextWatcher
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        MyWatcher(
            /* [in] */ AutoCompleteTextView* host);

        virtual CARAPI AfterTextChanged(
            /* [in] */ IEditable* s);

        virtual CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        virtual CARAPI OnTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 before,
            /* [in] */ Int32 count);

    private:
        AutoCompleteTextView* mHost;
    };

    class DropDownItemClickListener
            : public Object
            , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        DropDownItemClickListener(
            /* [in] */ AutoCompleteTextView* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* v,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        AutoCompleteTextView* mHost;
    };

    /**
     * Allows us a private hook into the on click event without preventing users from setting
     * their own click listener.
     */
    class PassThroughClickListener
            : public Object
            , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        PassThroughClickListener(
            /* [in] */ AutoCompleteTextView* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    public:
        AutoPtr<IViewOnClickListener> mWrapped;

    private:
        AutoCompleteTextView* mHost;
    };

    class PopupDataSetObserverOnchangedRunnable
        : public Runnable
    {
    public:
        PopupDataSetObserverOnchangedRunnable(
            /* [in] */ AutoCompleteTextView* host);

        CARAPI Run();

    private:
        AutoCompleteTextView* mHost;
    };

    class PopupDataSetObserver
        : public DataSetObserver
    {
    public:
        PopupDataSetObserver(
            /* [in] */ AutoCompleteTextView* host);

        CARAPI OnChanged();

        CARAPI OnInvalidated();

    private:
        AutoPtr<IWeakReference> mHost;
    };

    class PopupWindowOnDismissListener
        : public Object
        , public IPopupWindowOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI OnDismiss();

        PopupWindowOnDismissListener(
            /* [in] */ IAutoCompleteTextViewOnDismissListener* l);
    private:
        AutoPtr<IAutoCompleteTextViewOnDismissListener> mDismissListener;
    };

public:
    CAR_INTERFACE_DECL()

    AutoCompleteTextView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    virtual CARAPI SetOnClickListener(
        /* [in] */ IViewOnClickListener* listener);

    /**
     * <p>Sets the optional hint text that is displayed at the bottom of the
     * the matching list.  This can be used as a cue to the user on how to
     * best use the list, or to provide extra information.</p>
     *
     * @param hint the text to be displayed to the user
     *
     * @attr ref android.R.styleable#AutoCompleteTextView_completionHint
     */
    virtual CARAPI SetCompletionHint(
        /* [in] */ ICharSequence* hint);

    /**
     * Gets the optional hint text displayed at the bottom of the the matching list.
     *
     * @return The hint text, if any
     *
     * @see #setCompletionHint(CharSequence)
     *
     * @attr ref android.R.styleable#AutoCompleteTextView_completionHint
     */
    virtual CARAPI GetCompletionHint(
        /* [out] */ ICharSequence** hint);
    /**
     * <p>Returns the current width for the auto-complete drop down list. This can
     * be a fixed width, or {@link ViewGroup.LayoutParams#MATCH_PARENT} to fill the screen, or
     * {@link ViewGroup.LayoutParams#WRAP_CONTENT} to fit the width of its anchor view.</p>
     *
     * @return the width for the drop down list
     *
     * @attr ref android.R.styleable#AutoCompleteTextView_dropDownWidth
     */
    virtual CARAPI GetDropDownWidth(
        /* [out] */ Int32* width);

    /**
     * <p>Sets the current width for the auto-complete drop down list. This can
     * be a fixed width, or {@link ViewGroup.LayoutParams#MATCH_PARENT} to fill the screen, or
     * {@link ViewGroup.LayoutParams#WRAP_CONTENT} to fit the width of its anchor view.</p>
     *
     * @param width the width to use
     *
     * @attr ref android.R.styleable#AutoCompleteTextView_dropDownWidth
     */
    virtual CARAPI SetDropDownWidth(
        /* [in] */ Int32 width);

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
    virtual CARAPI GetDropDownHeight(
        /* [out] */ Int32* height);

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
    virtual CARAPI SetDropDownHeight(
        /* [in] */ Int32 height);

    /**
     * <p>Returns the id for the view that the auto-complete drop down list is anchored to.</p>
     *
     * @return the view's id, or {@link View#NO_ID} if none specified
     *
     * @attr ref android.R.styleable#AutoCompleteTextView_dropDownAnchor
     */
    virtual CARAPI GetDropDownAnchor(
        /* [out] */ Int32* result);

    /**
     * <p>Sets the view to which the auto-complete drop down list should anchor. The view
     * corresponding to this id will not be loaded until the next time it is needed to avoid
     * loading a view which is not yet instantiated.</p>
     *
     * @param id the id to anchor the drop down list view to
     *
     * @attr ref android.R.styleable#AutoCompleteTextView_dropDownAnchor
     */
    virtual CARAPI SetDropDownAnchor(
        /* [in] */ Int32 id);

    /**
     * <p>Gets the background of the auto-complete drop-down list.</p>
     *
     * @return the background drawable
     *
     * @attr ref android.R.styleable#PopupWindow_popupBackground
     */
    virtual CARAPI GetDropDownBackground(
        /* [out] */ IDrawable** drawable);

    /**
     * <p>Sets the background of the auto-complete drop-down list.</p>
     *
     * @param d the drawable to set as the background
     *
     * @attr ref android.R.styleable#PopupWindow_popupBackground
     */
    virtual CARAPI SetDropDownBackgroundDrawable(
        /* [in] */ IDrawable* d);

    /**
     * <p>Sets the background of the auto-complete drop-down list.</p>
     *
     * @param id the id of the drawable to set as the background
     *
     * @attr ref android.R.styleable#PopupWindow_popupBackground
     */
    virtual CARAPI SetDropDownBackgroundResource(
        /* [in] */ Int32 id);

    /**
     * <p>Sets the vertical offset used for the auto-complete drop-down list.</p>
     *
     * @param offset the vertical offset
     */
    virtual CARAPI SetDropDownVerticalOffset(
        /* [in] */ Int32 offset);

    /**
     * <p>Gets the vertical offset used for the auto-complete drop-down list.</p>
     *
     * @return the vertical offset
     */
    virtual CARAPI GetDropDownVerticalOffset(
        /* [out] */ Int32* offset);

    /**
     * <p>Sets the horizontal offset used for the auto-complete drop-down list.</p>
     *
     * @param offset the horizontal offset
     */
    virtual CARAPI SetDropDownHorizontalOffset(
        /* [in] */ Int32 offset);

    /**
     * <p>Gets the horizontal offset used for the auto-complete drop-down list.</p>
     *
     * @return the horizontal offset
     */
    virtual CARAPI GetDropDownHorizontalOffset(
        /* [out] */ Int32* offset);

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
    virtual CARAPI SetDropDownAnimationStyle(
        /* [in] */ Int32 animationStyle);

    /**
     * <p>Returns the animation style that is used when the drop-down list appears and disappears
     * </p>
     *
     * @return the animation style that is used when the drop-down list appears and disappears
     *
     * @hide Pending API council approval
     */
    virtual CARAPI GetDropDownAnimationStyle(
        /* [out] */ Int32* animationStyle);

    /**
     * @return Whether the drop-down is visible as long as there is {@link #enoughToFilter()}
     *
     * @hide Pending API council approval
     */
    virtual CARAPI IsDropDownAlwaysVisible(
        /* [out] */ Boolean* visible);

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
    virtual CARAPI SetDropDownAlwaysVisible(
        /* [in] */ Boolean dropDownAlwaysVisible);

    /**
     * Checks whether the drop-down is dismissed when a suggestion is clicked.
     *
     * @hide Pending API council approval
     */
    virtual CARAPI IsDropDownDismissedOnCompletion(
        /* [out] */ Boolean* dropDownDismissedOnCompletion);

    /**
     * Sets whether the drop-down is dismissed when a suggestion is clicked. This is
     * TRUE by default.
     *
     * @param dropDownDismissedOnCompletion Whether to dismiss the drop-down.
     *
     * @hide Pending API council approval
     */
    virtual CARAPI SetDropDownDismissedOnCompletion(
        /* [in] */ Boolean dropDownDismissedOnCompletion);

    /**
     * <p>Returns the number of characters the user must type before the drop
     * down list is shown.</p>
     *
     * @return the minimum number of characters to type to show the drop down
     *
     * @see #setThreshold(Int32)
     */
    virtual CARAPI GetThreshold(
        /* [out] */ Int32* threshold);

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
    virtual CARAPI SetThreshold(
        /* [in] */ Int32 threshold);

    virtual CARAPI GetItemClickListener(
        /* [out] */ IAdapterViewOnItemClickListener** l);

    /**
     * <p>Sets the listener that will be notified when the user clicks an item
     * in the drop down list.</p>
     *
     * @param l the item click listener
     */
    virtual CARAPI SetOnItemClickListener(
        /* [in] */ IAdapterViewOnItemClickListener* l);

    /**
     * <p>Sets the listener that will be notified when the user selects an item
     * in the drop down list.</p>
     *
     * @param l the item selected listener
     */
    virtual CARAPI SetOnItemSelectedListener(
        /* [in] */ IAdapterViewOnItemSelectedListener* l);

    /**
     * <p>Returns the listener that is notified whenever the user clicks an item
     * in the drop down list.</p>
     *
     * @return the item click listener
     */
    virtual CARAPI GetOnItemClickListener(
        /* [out] */ IAdapterViewOnItemClickListener** l);

    /**
     * <p>Returns the listener that is notified whenever the user selects an
     * item in the drop down list.</p>
     *
     * @return the item selected listener
     */
    virtual CARAPI GetOnItemSelectedListener(
        /* [out] */ IAdapterViewOnItemSelectedListener** l);

    virtual CARAPI GetItemSelectedListener(
        /* [out] */ IAdapterViewOnItemSelectedListener** l)
    {
        return NOERROR;
    }

    /**
     * Set a listener that will be invoked whenever the AutoCompleteTextView's
     * list of completions is dismissed.
     * @param dismissListener Listener to invoke when completions are dismissed
     */
    virtual CARAPI SetOnDismissListener(
        /* [in] */ IAutoCompleteTextViewOnDismissListener* dismissListener);

    /**
     * <p>Returns a filterable list adapter used for auto completion.</p>
     *
     * @return a data adapter used for auto completion
     */
    virtual CARAPI GetAdapter(
        /* [out] */ IListAdapter** adapter);

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
    virtual CARAPI SetAdapter(
        /* [in] */ IListAdapter* adapter);

    virtual CARAPI OnKeyPreIme(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean* result);

    virtual CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean* result);

    virtual CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean* result);

    /**
     * Returns <code>TRUE</code> if the amount of text in the field meets
     * or exceeds the {@link #getThreshold} requirement.  You can override
     * this to impose a different standard for when filtering will be
     * triggered.
     */
    virtual CARAPI EnoughToFilter(
        /* [in] */ Boolean* result);

    /**
     * <p>Indicates whether the popup menu is showing.</p>
     *
     * @return TRUE if the popup menu is showing, FALSE otherwise
     */
    virtual CARAPI IsPopupShowing(
        /* [out] */ Boolean* isPopupShowing);

    /**
     * <p>Clear the list selection.  This may only be temporary, as user input will often bring
     * it back.
     */
    virtual CARAPI ClearListSelection();

    /**
     * Set the position of the dropdown view selection.
     *
     * @param position The position to move the selector to.
     */
    virtual CARAPI SetListSelection(
        /* [in] */ Int32 position);

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
    virtual CARAPI GetListSelection(
        /* [out] */ Int32* selection);

    /**
     * <p>Performs the text completion by converting the selected item from
     * the drop down list into a string, replacing the text box's content with
     * this string and finally dismissing the drop down menu.</p>
     */
    virtual CARAPI PerformCompletion();

    virtual CARAPI OnCommitCompletion(
        /* [in] */ ICompletionInfo* completion);

    /**
     * Identifies whether the view is currently performing a text completion, so subclasses
     * can decide whether to respond to text changed events.
     */
    virtual CARAPI IsPerformingCompletion(
        /* [out] */ Boolean* isPerformingCompletion);

    using EditText::SetText;
    /**
     * Like {@link #setText(CharSequence)}, except that it can disable filtering.
     *
     * @param filter If <code>FALSE</code>, no filtering will be performed
     *        as a result of this call.
     *
     * @hide Pending API council approval.
     */
    virtual CARAPI SetTextFilter(
        /* [in] */ ICharSequence* text,
        /* [in] */ Boolean filter)
    {
        return NOERROR;
    }

    virtual CARAPI SetText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Boolean filter);

    /** {@inheritDoc} */
    virtual CARAPI OnFilterComplete(
        /* [in] */ Int32 count);

    virtual CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    /**
     * <p>Closes the drop down if present on screen.</p>
     */
    virtual CARAPI DismissDropDown();

    /**
     * Issues a runnable to show the dropdown as soon as possible.
     *
     * @hide internal used only by SearchDialog
     */
    virtual CARAPI ShowDropDownAfterLayout();

    /**
     * Ensures that the drop down is not obscuring the IME.
     * @param visible whether the ime should be in front. If FALSE, the ime is pushed to
     * the background.
     * @hide internal used only here and SearchDialog
     */
    virtual CARAPI EnsureImeVisible(
        /* [in] */ Boolean visible);

    /**
     * @hide internal used only here and SearchDialog
     */
    virtual CARAPI IsInputMethodNotNeeded(
        /* [out] */ Boolean* isInputMethodNotNeeded);

    /**
     * <p>Displays the drop down on screen.</p>
     */
    virtual CARAPI ShowDropDown();

    /**
     * Forces outside touches to be ignored. Normally if {@link #isDropDownAlwaysVisible()} is
     * FALSE, we allow outside touch to dismiss the dropdown. If this is set to TRUE, then we
     * ignore outside touch even when the drop down is not set to always visible.
     *
     * @hide used only by SearchDialog
     */
    virtual CARAPI SetForceIgnoreOutsideTouch(
        /* [in] */ Boolean forceIgnoreOutsideTouch);

    /**
     * Sets the validator used to perform text validation.
     *
     * @param validator The validator used to validate the text entered in this widget.
     *
     * @see #getValidator()
     * @see #performValidation()
     */
    virtual CARAPI SetValidator(
        /* [in] */ IValidator* validator);

    /**
     * Returns the Validator set with {@link #setValidator},
     * or <code>NULL</code> if it was not set.
     *
     * @see #setValidator(android.widget.AutoCompleteTextView.Validator)
     * @see #performValidation()
     */
    virtual CARAPI GetValidator(
        /* [out] */ IValidator** validator);

    /**
     * If a validator was set on this view and the current string is not valid,
     * ask the validator to fix it.
     *
     * @see #getValidator()
     * @see #setValidator(android.widget.AutoCompleteTextView.Validator)
     */
    virtual CARAPI PerformValidation();

protected:
    /**
     * <p>Converts the selected item from the drop down list into a sequence
     * of character that can be used in the edit box.</p>
     *
     * @param selectedItem the item selected by the user for completion
     *
     * @return a sequence of characters representing the selected suggestion
     */
    virtual CARAPI_(AutoPtr<ICharSequence>) ConvertSelectionToString(
        /* [in] */ IInterface* selectedItem);

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
    virtual CARAPI_(void) PerformFiltering(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 keyCode);

    /**
     * <p>Performs the text completion by replacing the current text by the
     * selected item. Subclasses should override this method to avoid replacing
     * the whole content of the edit box.</p>
     *
     * @param text the selected suggestion in the drop down list
     */
    virtual CARAPI_(void) ReplaceText(
        /* [in] */ ICharSequence* text);

    virtual CARAPI_(void) OnDisplayHint(
        /* [in] */ Int32 hint);

    virtual CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    virtual CARAPI OnAttachedToWindow() ;

    virtual CARAPI OnDetachedFromWindow();

    virtual CARAPI_(void) DoBeforeTextChanged();

    virtual CARAPI_(void) DoAfterTextChanged();

    virtual CARAPI_(Boolean) SetFrame(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    /**
     * Returns the Filter obtained from {@link Filterable#getFilter},
     * or <code>NULL</code> if {@link #setAdapter} was not called with
     * a Filterable.
     */
    virtual CARAPI_(AutoPtr<IFilter>) GetFilter();

private:
    /**
     * Private hook into the on click event, dispatched from {@link PassThroughClickListener}
     */
    CARAPI_(void) OnClickImpl();

    CARAPI_(void) PerformCompletion(
        /* [in] */ IView* selectedView,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI_(void) UpdateDropDownForFilter(
        /* [in] */ Int32 count);

    CARAPI_(void) BuildImeCompletions();

private:
    static const Boolean DEBUG = FALSE;
    static const String TAG;
    static const Int32 EXPAND_MAX = 3;

    AutoPtr<ICharSequence> mHintText;
    AutoPtr<ITextView> mHintView;
    Int32 mHintResource;
    AutoPtr<IListAdapter> mAdapter;
    AutoPtr<IFilter> mFilter;
    Int32 mThreshold;
    AutoPtr<IListPopupWindow> mPopup;
    Int32 mDropDownAnchorId;

    AutoPtr<IAdapterViewOnItemClickListener> mItemClickListener;
    AutoPtr<IAdapterViewOnItemSelectedListener> mItemSelectedListener;
    Boolean mDropDownDismissedOnCompletion;
    Int32 mLastKeyCode;
    Boolean mOpenBefore;
    AutoPtr<IValidator> mValidator;
    Boolean mBlockCompletion;

    // When set, an update in the underlying adapter will update the result list popup.
    // Set to false when the list is hidden to prevent asynchronous updates to popup the list again.
    Boolean mPopupCanBeUpdated;

    AutoPtr<PassThroughClickListener> mPassThroughClickListener;
    AutoPtr<PopupDataSetObserver> mObserver;
    AutoPtr<PopupDataSetObserverOnchangedRunnable> mPopRunnable;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos
#endif
