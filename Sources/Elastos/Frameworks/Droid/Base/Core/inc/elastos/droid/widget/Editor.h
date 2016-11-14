
#ifndef __ELASTOS_DROID_WIDGET_EDITOR_H__
#define __ELASTOS_DROID_WIDGET_EDITOR_H__

#define HASH_FOR_TEXT_STYLE
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/UndoOperation.h"
#include "elastos/droid/view/ViewTreeObserver.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/text/style/CSuggestionSpan.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/widget/PopupWindow.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/HandlerRunnable.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IUndoManager;
using Elastos::Droid::Content::IUndoOwner;
using Elastos::Droid::Content::UndoOperation;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Os::HandlerRunnable;
using Elastos::Droid::Os::Handler;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::EIID_IBundle;
using Elastos::Droid::Text::ISpanWatcher;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::ILayout;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::Style::ISuggestionSpan;
using Elastos::Droid::Text::Style::IEasyEditSpan;
using Elastos::Droid::Text::Style::ITextAppearanceSpan;
using Elastos::Droid::Text::Style::ISuggestionRangeSpan;
using Elastos::Droid::Text::Method::IWordIterator;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::View::ViewTreeObserver;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IRenderNode;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IDisplayList;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IDragEvent;
using Elastos::Droid::View::IDragShadowBuilder;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::View::IOnTouchModeChangeListener;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::InputMethod::IExtractedTextRequest;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::ICorrectionInfo;
using Elastos::Droid::View::InputMethod::ICursorAnchorInfoBuilder;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::PopupWindow;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::IComparator;

namespace Elastos {
namespace Droid {
namespace Widget {

//==============================================================================
//              SuggestionInfo
//==============================================================================

class Editor;

class SuggestionInfo
    : public Object
{
public:
    SuggestionInfo(
        /* [in] */ Editor* host);

    // range of actual suggestion within text
    Int32 mSuggestionStart;
    Int32 mSuggestionEnd;

    // the SuggestionSpan that this TextView represents
    AutoPtr<ISuggestionSpan> mSuggestionSpan;

    // the index of this suggestion inside suggestionSpan
    Int32 mSuggestionIndex;

    AutoPtr<ISpannableStringBuilder> mText;
    AutoPtr<ITextAppearanceSpan> mHighlightSpan;

    Editor* mHost;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Widget::SuggestionInfo, IInterface)

namespace Elastos {
namespace Droid {
namespace Widget {

class TextDisplayList
    : public Object
{
public:
    friend class Editor;

    TO_STRING_IMPL("TextDisplayList");

    TextDisplayList(
        /* [in] */ const String& name);

    ~TextDisplayList();

    CARAPI_(Boolean) NeedsRecord();

private:
    AutoPtr<IRenderNode> mDisplayList;

    Boolean mIsDirty;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Widget::TextDisplayList, IInterface)

namespace Elastos {
namespace Droid {
namespace Widget {

class Editor;
class TextView;
class ActionPopupWindow;
class SuggestionsPopupWindow;
class InsertionHandleView;
class HandleView;

//==============================================================================
//              EditorRunnable
//==============================================================================

class ActionPopupShowerRunnable
    : public Runnable
{
public:
    ActionPopupShowerRunnable(
        /* [in] */ ActionPopupWindow* host)
        : mHost(host)
    {}

    virtual ECode Run();
private:
    AutoPtr<ActionPopupWindow> mHost;
};

class InsertionHandleViewHiderRunnable
    : public Runnable
{
public:
    InsertionHandleViewHiderRunnable(
        /* [in] */ InsertionHandleView* host)
        : mHost(host)
    {}

    virtual ECode Run();

private:
    InsertionHandleView* mHost;
};

class ShowSuggestionRunnable
    : public Runnable
{
public:
    ShowSuggestionRunnable(
        /* [in] */ Editor* host)
        : mHost(host)
    {}

    virtual ECode Run();

private:
    Editor* mHost;
};

//==============================================================================
//              InputContentType
//==============================================================================
class InputContentType
    : public Object
{
public:
    InputContentType();

    Int32 mImeOptions;
    String mPrivateImeOptions;
    AutoPtr<ICharSequence> mImeActionLabel;
    Int32 mImeActionId;
    AutoPtr<IBundle> mExtras;
    AutoPtr<IOnEditorActionListener> mOnEditorActionListener;
    Boolean mEnterDown;
};

//==============================================================================
//              InputMethodState
//==============================================================================

class InputMethodState
    : public Object
{
public:
    InputMethodState();

    AutoPtr<IRect> mCursorRectInWindow;// = new Rect();
    AutoPtr<ArrayOf<Float> > mTmpOffset;// = new Float[2];
    AutoPtr<IExtractedTextRequest> mExtractedTextRequest;
    AutoPtr<IExtractedText> mExtractedText;// = new ExtractedText();
    Int32 mBatchEditNesting;
    Boolean mCursorChanged;
    Boolean mSelectionModeChanged;
    Boolean mContentChanged;
    Int32 mChangedStart;
    Int32 mChangedEnd;
    Int32 mChangedDelta;
};

//==============================================================================
//              CustomPopupWindow
//==============================================================================
class CustomPopupWindow
    : public PopupWindow
{
public:
    TO_STRING_IMPL("CustomPopupWindow");

    CustomPopupWindow(
        /* [in] */ IContext* context,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Editor* editor,
        /* [in] */ SuggestionsPopupWindow* owner);

    virtual CARAPI Dismiss();

private:
    Editor* mEditor;
    SuggestionsPopupWindow* mOwner;
};

//==============================================================================
//              PinnedPopupWindow
//==============================================================================
class PinnedPopupWindow
    : public Object
    , public ITextViewPositionListener
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("PinnedPopupWindow");

    PinnedPopupWindow(
        /* [in] */ Editor* editor);

    virtual ~PinnedPopupWindow();

    virtual CARAPI Show();

    virtual CARAPI Hide();

    virtual CARAPI UpdatePosition(
        /* [in] */ Int32 parentPositionX,
        /* [in] */ Int32 parentPositionY,
        /* [in] */ Boolean parentPositionChanged,
        /* [in] */ Boolean parentScrolled);

    CARAPI_(Boolean) IsShowing();

    virtual CARAPI constructor();

protected:
    CARAPI MeasureContent();

    virtual CARAPI CreatePopupWindow() = 0;
    virtual CARAPI InitContentView() = 0;
    virtual CARAPI_(Int32) GetTextOffset() = 0;
    virtual CARAPI_(Int32) GetVerticalLocalPosition(
        /* [in] */ Int32 line) = 0;
    virtual CARAPI_(Int32) ClipVertically(
        /* [in] */ Int32 positionY) = 0;

private:
    CARAPI ComputeLocalPosition();

    CARAPI_(void) UpdatePosition(
        /* [in] */ Int32 parentPositionX,
        /* [in] */ Int32 parentPositionY);

protected:
    Editor* mEditor;
    AutoPtr<IPopupWindow> mPopupWindow;
    AutoPtr<IViewGroup> mContentView;
    Int32 mPositionX;
    Int32 mPositionY;
};

//==============================================================================
//              EasyEditPopupWindow
//==============================================================================
class EasyEditPopupWindow
    : public PinnedPopupWindow
{
private:
    class ClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ClickListener(
            /* [in] */ EasyEditPopupWindow* host);

        virtual CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        EasyEditPopupWindow* mHost;
    };

public:
    TO_STRING_IMPL("EasyEditPopupWindow");

    EasyEditPopupWindow(
        /* [in] */ Editor* editor);

    virtual CARAPI CreatePopupWindow();

    virtual CARAPI InitContentView();

    virtual CARAPI_(Int32) GetTextOffset();

    virtual CARAPI_(Int32) GetVerticalLocalPosition(
        /* [in] */ Int32 line);

    virtual CARAPI_(Int32) ClipVertically(
        /* [in] */ Int32 positionY);

    CARAPI_(void) SetEasyEditSpan(
        /* [in] */ IEasyEditSpan* easyEditSpan);

    virtual CARAPI OnClick(
        /* [in] */ IView* view);

    CARAPI Hide();

private:
    CARAPI_(void) SetOnDeleteListener(
        /* [in] */ IEasyEditDeleteListener* listener);

private:
    friend class Editor;
    static const Int32 POPUP_TEXT_LAYOUT;
    AutoPtr<ITextView> mDeleteTextView;
    AutoPtr<IEasyEditSpan> mEasyEditSpan;
    AutoPtr<IEasyEditDeleteListener> mOnDeleteListener;
};

//==============================================================================
//              SuggestionAdapter
//==============================================================================
class SuggestionAdapter
    : public BaseAdapter
{
public:
    TO_STRING_IMPL("SuggestionAdapter");

    SuggestionAdapter(
        /* [in] */ Editor* editor,
        /* [in] */ SuggestionsPopupWindow* popupWindow);

    virtual CARAPI GetCount(
        /* [out] */ Int32* count);

    virtual CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    virtual CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

    virtual CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** result);
private:
    Editor* mEditor;
    SuggestionsPopupWindow* mPopupWindow;
    AutoPtr<ILayoutInflater> mInflater;
};

//==============================================================================
//              SuggestionSpanComparator
//==============================================================================
class SuggestionsPopupWindow;

class SuggestionSpanComparator
    : public Object
    , public IComparator
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("SuggestionSpanComparator");

    SuggestionSpanComparator(
        /* [in] */ SuggestionsPopupWindow* host);

    CARAPI Compare(
        /* [in] */ IInterface* lhs,
        /* [in] */ IInterface* rhs,
        /* [out] */ Int32* result);
private:
    SuggestionsPopupWindow* mHost;
};

//==============================================================================
//              SuggestionsPopupWindow
//==============================================================================
class SuggestionsPopupWindow
    : public PinnedPopupWindow
{
private:
    class ItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ItemClickListener(
            /* [in] */ SuggestionsPopupWindow* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        SuggestionsPopupWindow* mHost;
    };

public:
    TO_STRING_IMPL("SuggestionsPopupWindow");

    typedef HashMap<AutoPtr<ISuggestionSpan>, AutoPtr<IInteger32> > SuggestionSpanHashMap;
    typedef typename SuggestionSpanHashMap::Iterator SuggestionSpanIterator;

public:
    SuggestionsPopupWindow(
        /* [in] */ Editor* editor);

    virtual ~SuggestionsPopupWindow();

    virtual CARAPI CreatePopupWindow();

    virtual CARAPI InitContentView();

    virtual CARAPI_(Int32) GetTextOffset();

    virtual CARAPI_(Int32) GetVerticalLocalPosition(
        /* [in] */ Int32 line);

    virtual CARAPI_(Int32) ClipVertically(
        /* [in] */ Int32 positionY);

    CARAPI_(Boolean) IsShowingUp();

    CARAPI_(void) OnParentLostFocus();

    CARAPI_(AutoPtr<ArrayOf<ISuggestionSpan*> >) GetSuggestionSpans();

    CARAPI Show();

    CARAPI Hide();

    virtual CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

protected:
    CARAPI MeasureContent();

private:
    CARAPI_(Boolean) UpdateSuggestions();

    CARAPI_(void) HighlightTextDifferences(
        /* [in] */ SuggestionInfo* suggestionInfo,
        /* [in] */ Int32 unionStart,
        /* [in] */ Int32 unionEnd);

private:
    friend class SuggestionSpanComparator;
    friend class CustomPopupWindow;
    friend class SuggestionAdapter;

    static const Int32 MAX_NUMBER_SUGGESTIONS;// = SuggestionSpan.SUGGESTIONS_MAX_SIZE;
    static const Int32 ADD_TO_DICTIONARY;// = -1;
    static const Int32 DELETE_TEXT;// = -2;
    AutoPtr<ArrayOf<SuggestionInfo *> > mSuggestionInfos;
    Int32 mNumberOfSuggestions;
    Boolean mCursorWasVisibleBeforeSuggestions;
    Boolean mIsShowingUp;// = false;
    AutoPtr<SuggestionAdapter> mSuggestionsAdapter;
    AutoPtr<SuggestionSpanComparator> mSuggestionSpanComparator;
    SuggestionSpanHashMap mSpansLengths;
};

/**
 * An ActionMode Callback class that is used to provide actions while in text selection mode.
 *
 * The default callback provides a subset of Select All, Cut, Copy and Paste actions, depending
 * on which of these this TextView supports.
 */
class SelectionActionModeCallback
    : public Object
    , public IActionModeCallback
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("SelectionActionModeCallback");

    SelectionActionModeCallback(
        /* [in] */ Editor* editor);

    CARAPI OnCreateActionMode(
        /* [in] */ IActionMode* mode,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    CARAPI OnPrepareActionMode(
        /* [in] */ IActionMode* mode,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    CARAPI OnActionItemClicked(
        /* [in] */ IActionMode* mode,
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    CARAPI OnDestroyActionMode(
        /* [in] */ IActionMode* mode);

private:
    Editor*   mEditor;
};

//==============================================================================
//              ActionPopupWindow
//==============================================================================
class ActionPopupWindow
    : public PinnedPopupWindow
{
private:
    class ClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ClickListener(
            /* [in] */ ActionPopupWindow* host);

        virtual CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        ActionPopupWindow* mHost;
    };
public:

    TO_STRING_IMPL("ActionPopupWindow");

    ActionPopupWindow(
        /* [in] */ Editor* editor);

    virtual ~ActionPopupWindow();

    virtual CARAPI CreatePopupWindow();

    virtual CARAPI InitContentView();

    virtual CARAPI_(Int32) GetTextOffset();

    virtual CARAPI_(Int32) GetVerticalLocalPosition(
        /* [in] */ Int32 line);

    virtual CARAPI_(Int32) ClipVertically(
        /* [in] */ Int32 positionY);

    CARAPI Show();

    CARAPI OnClick(
        /* [in] */ IView* view);

private:
    static const Int32 POPUP_TEXT_LAYOUT;// = com.android.internal.R.layout.text_edit_action_popup_text;
    AutoPtr<ITextView> mPasteTextView;
    AutoPtr<ITextView> mReplaceTextView;
};

//==============================================================================
//              HandleView
//==============================================================================

class HandleView
    : public Elastos::Droid::View::View
    , public ITextViewPositionListener
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("HandleView");

    HandleView();

    CARAPI constructor(
        /* [in] */ IDrawable* drawableLtr,
        /* [in] */ IDrawable* drawableRtl,
        /* [in] */ Editor* editor);

    CARAPI_(Boolean) OffsetHasBeenChanged();

    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI_(void) Show();

    virtual CARAPI_(void) Hide();

    virtual CARAPI_(void) ShowActionPopupWindow(
        /* [in] */ Int32 delay);

    CARAPI_(Boolean) IsShowing();

    virtual CARAPI_(Int32) GetCurrentCursorOffset() = 0;

    virtual CARAPI_(void) UpdatePosition(
        /* [in] */ Float x,
        /* [in] */ Float y) = 0;

    CARAPI_(Boolean) IsDragging();

    virtual CARAPI_(void) OnHandleMoved();

    virtual CARAPI_(void) OnDetached();

protected:
    CARAPI_(void) UpdateDrawable();

    virtual CARAPI_(Int32) GetHotspotX(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Boolean isRtlRun) = 0;

    virtual CARAPI_(Int32) GetHorizontalGravity(
        /* [in] */ Boolean isRtlRun) = 0;

    CARAPI_(void) Dismiss();

    virtual CARAPI_(void) HideActionPopupWindow();

    virtual CARAPI_(void) UpdateSelection(
        /* [in] */ Int32 offset) = 0;

    virtual CARAPI UpdatePosition(
        /* [in] */ Int32 parentPositionX,
        /* [in] */ Int32 parentPositionY,
        /* [in] */ Boolean parentPositionChanged,
        /* [in] */ Boolean parentScrolled);

    CARAPI_(void) PositionAtCursorOffset(
        /* [in] */ Int32 offset,
        /* [in] */ Boolean parentScrolled);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI_(Int32) GetCursorOffset();

private:
    CARAPI_(void) StartTouchUpFilter(
        /* [in] */ Int32 offset);

    CARAPI_(void) AddPositionToTouchUpFilter(
        /* [in] */ Int32 offset);

    CARAPI_(void) FilterOnTouchUp();

    CARAPI_(Boolean) IsVisible();

    CARAPI_(Int32) GetPreferredWidth();

    CARAPI_(Int32) GetPreferredHeight();

    CARAPI_(Int32) GetHorizontalOffset();

protected:
    Editor* mEditor;

protected:
    AutoPtr<IDrawable> mDrawable;
    AutoPtr<IDrawable> mDrawableLtr;
    AutoPtr<IDrawable> mDrawableRtl;
    Int32 mHotspotX;
    Int32 mHorizontalGravity;
    // Transient action popup window for Paste and Replace actions
    AutoPtr<ActionPopupWindow> mActionPopupWindow;

private:
    // Touch-up filter: number of previous positions remembered
    static const Int32 HISTORY_SIZE;
    static const Int32 TOUCH_UP_FILTER_DELAY_AFTER;
    static const Int32 TOUCH_UP_FILTER_DELAY_BEFORE;

    AutoPtr<IPopupWindow> mContainer;

    // Position with respect to the parent TextView
    Int32 mPositionX;
    Int32 mPositionY;

    Boolean mIsDragging;

    // Offset from touch position to mPosition
    Float mTouchToWindowOffsetX;
    Float mTouchToWindowOffsetY;

    // Offsets the hotspot point up, so that cursor is not hidden by the finger when moving up
    Float mTouchOffsetY;

    // Where the touch position should be on the handle to ensure a maximum cursor visibility
    Float mIdealVerticalOffset;

    // Parent's (TextView) previous position in window
    Int32 mLastParentX;
    Int32 mLastParentY;

    // Previous text character offset
    Int32 mPreviousOffset;// = -1;

    // Previous text character offset
    Boolean mPositionHasChanged;// = true;

    // Used to delay the appearance of the action popup window
    AutoPtr<IRunnable> mActionPopupShower;

    Int32 mMinSize;

    AutoPtr<ArrayOf<Int64> > mPreviousOffsetsTimes;//= new long[HISTORY_SIZE];
    AutoPtr<ArrayOf<Int32> > mPreviousOffsets;// = new Int32[HISTORY_SIZE];
    Int32 mPreviousOffsetIndex;// = 0;
    Int32 mNumberPreviousOffsets;// = 0;
};

//==============================================================================
//              InsertionHandleView
//==============================================================================
class InsertionHandleView
    : public HandleView
{
public:
    TO_STRING_IMPL("InsertionHandleView");

    InsertionHandleView();

    CARAPI constructor(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Editor* editor);

    virtual void Show();

    void ShowWithActionPopup();

    virtual CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI_(Int32) GetCurrentCursorOffset();

    virtual CARAPI_(void) UpdateSelection(
        /* [in] */ Int32 offset);

    virtual CARAPI_(void) UpdatePosition(
        /* [in] */ Float x,
        /* [in] */ Float y);

    virtual CARAPI_(void) OnHandleMoved();

    virtual CARAPI_(void) OnDetached();

protected:
    CARAPI_(Int32) GetHotspotX(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Boolean isRtlRun);

    CARAPI_(Int32) GetHorizontalGravity(
        /* [in] */ Boolean isRtlRun);

    CARAPI_(Int32) GetCursorOffset();

private:
    CARAPI_(void) HideAfterDelay();

    CARAPI_(void) RemoveHiderCallback();


private:
    static const Int32 DELAY_BEFORE_HANDLE_FADES_OUT;
    static const Int32 RECENT_CUT_COPY_DURATION;

    // Used to detect taps on the insertion handle, which will affect the ActionPopupWindow
    Float mDownPositionX;
    Float mDownPositionY;
    AutoPtr<IRunnable> mHider;
};

//==============================================================================
//              SelectionStartHandleView
//==============================================================================
class SelectionStartHandleView
    : public HandleView
{
public:
    TO_STRING_IMPL("SelectionStartHandleView");

    SelectionStartHandleView();

    CARAPI constructor(
        /* [in] */ IDrawable* drawableLtr,
        /* [in] */ IDrawable* drawableRtl,
        /* [in] */ Editor* editor);

    virtual CARAPI_(Int32) GetCurrentCursorOffset();

    virtual CARAPI_(void) UpdateSelection(
        /* [in] */ Int32 offset);

    virtual CARAPI_(void) UpdatePosition(
        /* [in] */ Float x,
        /* [in] */ Float y);

    AutoPtr<ActionPopupWindow> GetActionPopupWindow();

protected:
    CARAPI_(Int32) GetHotspotX(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Boolean isRtlRun);

    CARAPI_(Int32) GetHorizontalGravity(
        /* [in] */ Boolean isRtlRun);
};

//==============================================================================
//              SelectionEndHandleView
//==============================================================================
class SelectionEndHandleView
    : public HandleView
{
public:
    TO_STRING_IMPL("SelectionEndHandleView");

    SelectionEndHandleView();

    CARAPI constructor(
        /* [in] */ IDrawable* drawableLtr,
        /* [in] */ IDrawable* drawableRtl,
        /* [in] */ Editor* editor);

    virtual CARAPI_(Int32) GetCurrentCursorOffset();

    virtual CARAPI_(void) UpdateSelection(
        /* [in] */ Int32 offset);

    virtual CARAPI_(void) UpdatePosition(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) SetActionPopupWindow(
        /* [in] */ ActionPopupWindow* actionPopupWindow);

protected:
    CARAPI_(Int32) GetHotspotX(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Boolean isRtlRun);

    CARAPI_(Int32) GetHorizontalGravity(
        /* [in] */ Boolean isRtlRun);

};

//==============================================================================
//              CursorController
//==============================================================================
/**
 * A CursorController instance can be used to control a cursor in the text.
 */
class CursorController
    : public Object
    , public IOnTouchModeChangeListener
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("CursorController");

    CursorController(
        /* [in] */ Editor* editor);

    /**
     * Makes the cursor controller visible on screen.
     * See also {@link #hide()}.
     */
    virtual CARAPI Show();

    /**
     * Hide the cursor controller from screen.
     * See also {@link #show()}.
     */
    virtual CARAPI Hide();

    /**
     * Called when the view is detached from window. Perform house keeping task, such as
     * stopping Runnable thread that would otherwise keep a reference on the context, thus
     * preventing the activity from being recycled.
     */
    virtual CARAPI OnDetached();

    /**
     * Callback method to be invoked when the touch mode changes.
     *
     * @param isInTouchMode True if the view hierarchy is now in touch mode, false  otherwise.
     */
    virtual CARAPI OnTouchModeChanged(
        /* [in]*/ Boolean isInTouchMode);

protected:
    Editor* mEditor;
};

//==============================================================================
//              InsertionPointCursorController
//==============================================================================

class InsertionPointCursorController
    : public CursorController
{
public:
    TO_STRING_IMPL("InsertionPointCursorController");

    InsertionPointCursorController(
        /* [in] */ Editor* editor);

    virtual CARAPI Show();

    virtual CARAPI Hide();

    virtual CARAPI OnTouchModeChanged(
        /* [in]*/ Boolean isInTouchMode);

    virtual CARAPI OnDetached();

    CARAPI_(void) ShowWithActionPopup();

private:
    AutoPtr<InsertionHandleView> GetHandle();

private:
    AutoPtr<InsertionHandleView> mHandle;
};

//==============================================================================
//              SelectionModifierCursorController
//==============================================================================

class SelectionModifierCursorController
    : public CursorController
{
public:
    TO_STRING_IMPL("SelectionModifierCursorController");

    SelectionModifierCursorController(
        /* [in] */ Editor* editor);

    virtual CARAPI Show();

    virtual CARAPI Hide();

    virtual CARAPI OnTouchModeChanged(
        /* [in]*/ Boolean isInTouchMode);

    virtual CARAPI OnDetached();

    CARAPI_(Int32) GetMinTouchOffset();

    CARAPI_(Int32) GetMaxTouchOffset();

    CARAPI_(void) ResetTouchOffsets();

    CARAPI_(void) OnTouchEvent(
        /* [in]*/ IMotionEvent* event);

    /**
     * @return true iff this controller is currently used to move the selection start.
     */
    Boolean IsSelectionStartDragged();

private:
    CARAPI_(void) InitDrawables();

    CARAPI_(void) InitHandles();

    void UpdateMinAndMaxOffsets(
        /* [in]*/ IMotionEvent* event);

private:
    static const Int32 DELAY_BEFORE_REPLACE_ACTION;

    // The cursor controller handles, lazily created when shown.
    AutoPtr<SelectionStartHandleView> mStartHandle;
    AutoPtr<SelectionEndHandleView> mEndHandle;

    // The offsets of that last touch down event. Remembered to start selection there.
    Int32 mMinTouchOffset;
    Int32 mMaxTouchOffset;

    // Double tap detection
    Int64 mPreviousTapUpTime;
    Float mDownPositionX;
    Float mDownPositionY;
    Boolean mGestureStayedInTapRegion;
};

//==============================================================================
//              PositionListener
//==============================================================================

class PositionListener
    : public Object
    , public IOnPreDrawListener
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("PositionListener");

    PositionListener(
        /* [in] */ Editor* editor);

    virtual CARAPI OnPreDraw(
        /* [out] */ Boolean* result);

    CARAPI_(void) AddSubscriber(
        /* [in] */ ITextViewPositionListener* positionListener,
        /* [in] */ Boolean canMove);

    CARAPI_(void) RemoveSubscriber(
        /* [in] */ ITextViewPositionListener* positionListener);

    CARAPI_(Int32) GetPositionX();

    CARAPI_(Int32) GetPositionY();

    CARAPI_(void) UpdatePosition();

    CARAPI OnScrollChanged();

private:
    // 3 handles
    // 3 ActionPopup [replace, suggestion, easyedit] (suggestionsPopup first hides the others)
    static const Int32 MAXIMUM_NUMBER_OF_LISTENERS;
    AutoPtr<ArrayOf<ITextViewPositionListener*> > mPositionListeners;//new TextViewPositionListener[MAXIMUM_NUMBER_OF_LISTENERS];
    AutoPtr<ArrayOf<Boolean> > mCanMove;//new boolean[MAXIMUM_NUMBER_OF_LISTENERS];
    Boolean mPositionHasChanged;// = true;
    // Absolute position of the TextView with respect to its parent window
    Int32 mPositionX;
    Int32 mPositionY;
    Int32 mNumberOfListeners;
    Boolean mScrollHasChanged;
    AutoPtr<ArrayOf<Int32> > mTempCoords;// new int[2];
    Editor* mEditor;
};

//==============================================================================
//              CorrectionHighlighter
//==============================================================================
class CorrectionHighlighter
    : public Object
{
public:
    TO_STRING_IMPL("CorrectionHighlighter");

    CorrectionHighlighter(
        /* [in] */ Editor* editor);

    void Highlight(
        /* [in] */ ICorrectionInfo* info);

    void Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 cursorOffsetVertical);

    Boolean UpdatePaint();

    Boolean UpdatePath();

    void Invalidate(
        /* [in] */ Boolean delayed);

    void StopAnimation();

private:
    AutoPtr<IPath> mPath;// = new Path();
    AutoPtr<IPaint> mPaint;// = new Paint(Paint.ANTI_ALIAS_FLAG);
    Int32 mStart;
    Int32 mEnd;
    Int64 mFadingStartTime;
    AutoPtr<IRectF> mTempRectF;
    static const Int32 FADE_OUT_DURATION;
    Editor* mEditor;
};

//==============================================================================
//              ErrorPopup
//==============================================================================
class ErrorPopup
    : public PopupWindow
{
public:
    TO_STRING_IMPL("ErrorPopup");

    ErrorPopup(
        /* [in]*/ TextView* textView,
        /* [in]*/ Int32 width,
        /* [in]*/ Int32 height);

    CARAPI_(void) FixDirection(
        /* [in]*/ Boolean above);

    CARAPI_(Int32) GetResourceId(
        /* [in]*/ Int32 currentId,
        /* [in]*/ Int32 index);

    virtual CARAPI Update(
        /* [in]*/ Int32 x,
        /* [in]*/ Int32 y,
        /* [in]*/ Int32 w,
        /* [in]*/ Int32 h,
        /* [in]*/ Boolean force);

    using PopupWindow::Update;

private:
    Boolean mAbove;// = false;
    TextView* mView;
    Int32 mPopupInlineErrorBackgroundId;// = 0;
    Int32 mPopupInlineErrorAboveBackgroundId;// = 0;
};

//==============================================================================
//              Blink
//==============================================================================

class Blink
    : public HandlerRunnable
{
public:
    TO_STRING_IMPL("Blink");

    Blink(
        /* [in]*/ Editor* editor);

    ~Blink();

    CARAPI Run();

    CARAPI_(void) Cancel();

    CARAPI_(void) Uncancel();

private:
    Boolean mCancelled;
    Editor* mEditor;
};

//==============================================================================
//              DragLocalState
//==============================================================================
class DragLocalState
    : public Object
    , public IDragLocalState
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("DragLocalState");

    DragLocalState(
        /* [in] */ TextView* sourceTextView,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

public:
    TextView* mSourceTextView;
    Int32 mStart;
    Int32 mEnd;
};

//==============================================================================
//              UndoInputFilter
//==============================================================================
class UndoInputFilter
    : public Object
    , public Elastos::Droid::Text::IInputFilter
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("UndoInputFilter");

    UndoInputFilter(
        /* [in] */ Editor* editor);

    CARAPI Filter(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ISpanned* dest,
        /* [in] */ Int32 dstart,
        /* [in] */ Int32 dend,
        /* [out] */ ICharSequence** cs);

private:
    Editor* mEditor;
};

//==============================================================================
//              TextModifyOperation
//==============================================================================
class TextModifyOperation
    : public UndoOperation
    , public ITextModifyOperation
{
public:
    CAR_INTERFACE_DECL();

    TO_STRING_IMPL("TextModifyOperation");

    TextModifyOperation();

    CARAPI constructor(
        /* [in] */ IUndoOwner* owner);

    CARAPI Commit();

    CARAPI Undo();

    CARAPI Redo();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

private:
    CARAPI_(void) SwapText();

public:
    Int32 mRangeStart;
    Int32 mRangeEnd;
    AutoPtr<ICharSequence> mOldText;
};

//==============================================================================
//              Editor
//==============================================================================

/**
 * Helper class used by TextView to handle editable text views.
 *
 * @hide
 */
class Editor
    : public Object
    , public IEditor
{
private:
    class SpanController
        : public Object
        , public ISpanWatcher
    {
    private:
        class SpanControllerRunnable
            : public Runnable
        {
        public:
            TO_STRING_IMPL("Editor::SpanController::SpanControllerRunnable");

            SpanControllerRunnable(
                /* [in] */ SpanController* host);

            CARAPI Run();

        private:
            SpanController* mHost;
        };

        class SpanControllerListener
            : public Object
            , public IEasyEditDeleteListener
        {
        public:
            CAR_INTERFACE_DECL()

            TO_STRING_IMPL("Editor::SpanController::SpanControllerListener");

            SpanControllerListener(
                /* [in] */ SpanController* host);

            CARAPI OnDeleteClick(
                /* [in] */ IEasyEditSpan* span);
        private:
            SpanController* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("Editor::SpanController");

        SpanController(
            /* [in] */ Editor* host);

        CARAPI OnSpanAdded(
            /* [in] */ ISpannable* text,
            /* [in] */ IInterface* span,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end);

        CARAPI OnSpanRemoved(
            /* [in] */ ISpannable* text,
            /* [in] */ IInterface* span,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end);

        CARAPI OnSpanChanged(
            /* [in] */ ISpannable* text,
            /* [in] */ IInterface* span,
            /* [in] */ Int32 previousStart,
            /* [in] */ Int32 previousEnd,
            /* [in] */ Int32 newStart,
            /* [in] */ Int32 newEnd);

        CARAPI Hide();

    private:
        CARAPI_(Boolean) IsNonIntermediateSelectionSpan(
            /* [in] */ ISpannable* text,
            /* [in] */ IInterface* span);

        CARAPI_(void) SendEasySpanNotification(
            /* [in] */ Int32 textChangedType,
            /* [in] */ IEasyEditSpan* span);

    private:
        Editor* mHost;

        static const Int32 DISPLAY_TIMEOUT_MS;

        AutoPtr<EasyEditPopupWindow> mPopupWindow;

        AutoPtr<IRunnable> mHidePopup;
    };

    /**
     * A listener to call {@link InputMethodManager#updateCursorAnchorInfo(View, CursorAnchorInfo)}
     * while the input method is requesting the cursor/anchor position. Does nothing as long as
     * {@link InputMethodManager#isWatchingCursor(View)} returns false.
     */
    class CursorAnchorInfoNotifier
        : public Object
        , public ITextViewPositionListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("Editor::CursorAnchorInfoNotifier");

        CursorAnchorInfoNotifier(
            /* [in] */ Editor* editor);

        CARAPI UpdatePosition(
            /* [in] */ Int32 parentPositionX,
            /* [in] */ Int32 parentPositionY,
            /* [in] */ Boolean parentPositionChanged,
            /* [in] */ Boolean parentScrolled);

    private:
        Editor* mHost;
        AutoPtr<ICursorAnchorInfoBuilder> mSelectionInfoBuilder;
        AutoPtr< ArrayOf<Int32> > mTmpIntOffset;
        AutoPtr<IMatrix> mViewToScreenMatrix;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("Editor");

    Editor(
        /* [in] */ TextView* textView);

    virtual ~Editor();

    CARAPI SetError(
        /* [in] */ ICharSequence* error,
        /* [in] */ IDrawable* icon);

    CARAPI GetWordIterator(
        /* [out] */ IWordIterator** iterator);

    CARAPI PerformLongClick(
        /* [in] */ Boolean handled,
        /* [out] */ Boolean* res);

    CARAPI BeginBatchEdit();

    CARAPI EndBatchEdit();

    /**
     * Called by the framework in response to a text auto-correction (such as fixing a typo using a
     * a dictionnary) from the current input method, provided by it calling
     * {@link InputConnection#commitCorrection} InputConnection.commitCorrection()}. The default
     * implementation flashes the background of the corrected word to provide feedback to the user.
     *
     * @param info The auto correct info about the text that was corrected.
     */
    CARAPI OnCommitCorrection(
        /* [in] */ ICorrectionInfo* info);

    CARAPI AddSpanWatchers(
        /* [in] */ ISpannable* text);

public: /* package */

    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

    CARAPI_(void) CreateInputContentTypeIfNeeded();

    CARAPI_(void) CreateInputMethodStateIfNeeded();

    CARAPI_(Boolean) IsCursorVisible();

    CARAPI_(void) PrepareCursorControllers();

    /**
     * Hides the insertion controller and stops text selection mode, hiding the selection controller
     */
    CARAPI_(void) HideControllers();

    CARAPI_(void) OnScreenStateChanged(
        /* [in] */ Int32 screenState);

    CARAPI_(void) AdjustInputType(
        /* [in] */ Boolean password,
        /* [in] */ Boolean passwordInputType,
        /* [in] */ Boolean webPasswordInputType,
        /* [in] */ Boolean numberPasswordInputType);

    CARAPI_(void) SetFrame();

    CARAPI_(void) OnLocaleChanged();

    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction);

    CARAPI_(void) SendOnTextChanged(
        /* [in] */ Int32 start,
        /* [in] */ Int32 after);

    CARAPI_(void) OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    CARAPI_(void) OnTouchEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) EnsureEndedBatchEdit();

    CARAPI_(void) FinishBatchEdit(
        /* [in] */ InputMethodState* ims);

    CARAPI_(Boolean) ExtractText(
        /* [in] */ IExtractedTextRequest* request,
        /* [in] */ IExtractedText* outText);

    CARAPI_(Boolean) ReportExtractedText();

    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ ILayout* layout,
        /* [in] */ IPath* highlight,
        /* [in] */ IPaint* highlightPaint,
        /* [in] */ Int32 cursorOffsetVertical);

    /**
     * Invalidates all the sub-display lists that overlap the specified character range
     */
    CARAPI_(void) InvalidateTextDisplayList(
        /* [in] */ ILayout* layout,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI_(void) InvalidateTextDisplayList();

    CARAPI_(void) UpdateCursorsPositions();

    /**
     * @return true if the selection mode was actually started.
     */
    CARAPI_(Boolean) StartSelectionActionMode();

    CARAPI_(void) OnTouchUpEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * @return True if this view supports insertion handles.
     */
    CARAPI_(Boolean) HasInsertionController();

    /**
     * @return True if this view supports selection handles.
     */
    CARAPI_(Boolean) HasSelectionController();

    CARAPI_(AutoPtr<InsertionPointCursorController>) GetInsertionController();

    CARAPI_(AutoPtr<SelectionModifierCursorController>) GetSelectionController();

    CARAPI_(void) ShowSuggestions();

    CARAPI_(Boolean) AreSuggestionsShown();

    CARAPI OnScrollChanged();

    CARAPI_(void) MakeBlink();

protected:
    CARAPI_(void) StopSelectionActionMode();

    CARAPI_(void) OnDrop(
        /* [in] */ IDragEvent* event);

private:
    CARAPI_(void) DestroyDisplayListsData();

    CARAPI_(void) ShowError();

    CARAPI_(void) SetErrorIcon(
        /* [in] */ IDrawable* icon);

    CARAPI_(void) HideError();

    /**
     * Returns the Y offset to make the pointy top of the error point
     * at the middle of the error icon.
     */
    CARAPI_(Int32) GetErrorX();

    /**
     * Returns the Y offset to make the pointy top of the error point
     * at the bottom of the error icon.
     */
    CARAPI_(Int32) GetErrorY();

    CARAPI_(void) HideInsertionPointCursorController();

    CARAPI_(void) HideSpanControllers();

    CARAPI_(void) HideCursorControllers();

    CARAPI_(void) UpdateSpellCheckSpans(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Boolean createSpellChecker);

    CARAPI_(void) SuspendBlink();

    CARAPI_(void) ResumeBlink();

    CARAPI_(void) ChooseSize(
        /* [in] */ PopupWindow* pop,
        /* [in] */ ICharSequence* text,
        /* [in] */ TextView* tv);

    /**
     * Unlike {@link TextView#textCanBeSelected()}, this method is based on the <i>current</i> state
     * of the TextView. textCanBeSelected() has to be true (this is one of the conditions to have
     * a selection controller (see {@link #prepareCursorControllers()}), but this is not sufficient.
     */
    CARAPI_(Boolean) CanSelectText();

    /**
     * It would be better to rely on the input type for everything. A password inputType should have
     * a password transformation. We should hence use isPasswordInputType instead of this method.
     *
     * We should:
     * - Call setInputType in setKeyListener instead of changing the input type directly (which
     * would install the correct transformation).
     * - Refuse the installation of a non-password transformation in setTransformation if the input
     * type is password.
     *
     * However, this is like this for legacy reasons and we cannot break existing apps. This method
     * is useful since it matches what the user can see (obfuscated text or not).
     *
     * @return true if the current transformation method is of the password type.
     */
    CARAPI_(Boolean) HasPasswordTransformationMethod();

    /**
     * Adjusts selection to the word under last touch offset.
     * Return true if the operation was successfully performed.
     */
    CARAPI_(Boolean) SelectCurrentWord();

    CARAPI_(Int64) GetCharRange(
        /* [in] */ Int32 offset);

    CARAPI_(Boolean) TouchPositionIsInSelection();

    CARAPI_(AutoPtr<PositionListener>) GetPositionListener();

    CARAPI_(Boolean) IsPositionVisible(
        /* [in] */ Float positionX,
        /* [in] */ Float positionY);

    CARAPI_(Boolean) IsOffsetVisible(
        /* [in] */ Int32 offset);

    /** Returns true if the screen coordinates position (x,y) corresponds to a character displayed
     * in the view. Returns false when the position is in the empty space of left/right of text.
     */
    CARAPI_(Boolean) IsPositionOnText(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(Int64) GetLastTouchOffsets();

    /**
     * Downgrades to simple suggestions all the easy correction spans that are not a spell check
     * span.
     */
    CARAPI_(void) DowngradeEasyCorrectionSpans();

    CARAPI_(Int32) GetLastTapPosition();

    CARAPI_(Boolean) ExtractTextInternal(
        /* [in] */ IExtractedTextRequest* request,
        /* [in] */ Int32 partialStartOffset,
        /* [in] */ Int32 partialEndOffset,
        /* [in] */ Int32 delta,
        /* [in] */ IExtractedText* outText);

    CARAPI_(void) DrawHardwareAccelerated(
        /* [in] */ ICanvas* canvas,
        /* [in] */ ILayout* layout,
        /* [in] */ IPath* highlight,
        /* [in] */ IPaint* highlightPaint,
        /* [in] */ Int32 cursorOffsetVertical);

    CARAPI_(Int32) GetAvailableDisplayListIndex(
        /* [in] */ ArrayOf<Int32>* blockIndices,
        /* [in] */ Int32 numberOfBlocks,
        /* [in] */ Int32 searchStartIndex);

    CARAPI_(void) DrawCursor(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 cursorOffsetVertical);

    CARAPI_(Float) GetPrimaryHorizontal(
        /* [in] */ ILayout* layout,
        /* [in] */ ILayout* hintLayout,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean clamped);

    CARAPI_(Boolean) ExtractedTextModeWillBeStarted();

    /**
     * @return <code>true</code> if the cursor/current selection overlaps a {@link SuggestionSpan}.
     */
    CARAPI_(Boolean) IsCursorInsideSuggestionSpan();

    /**
     * @return <code>true</code> if the cursor is inside an {@link SuggestionSpan} with
     * {@link SuggestionSpan#FLAG_EASY_CORRECT} set.
     */
    CARAPI_(Boolean) IsCursorInsideEasyCorrectionSpan();

    CARAPI_(void) UpdateCursorPosition(
        /* [in] */ Int32 cursorIndex,
        /* [in] */ Int32 top,
        /* [in] */ Int32 bottom,
        /* [in] */ Float horizontal);

    /**
     * @return True when the TextView isFocused and has a valid zero-length selection (cursor).
     */
    CARAPI_(Boolean) ShouldBlink();

    CARAPI_(AutoPtr<IDragShadowBuilder>) GetTextThumbnailBuilder(
        /* [in] */ ICharSequence* text);

    CARAPI_(void) SendUpdateSelection();

private:
    friend class TextView;
    friend class CustomPopupWindow;
    friend class PinnedPopupWindow;
    friend class EasyEditPopupWindow;
    friend class SuggestionsPopupWindow;
    friend class SuggestionAdapter;
    friend class ActionPopupWindow;
    friend class HandleView;
    friend class InsertionHandleView;
    friend class SuggestionInfo;
    friend class SelectionStartHandleView;
    friend class SelectionEndHandleView;
    friend class InsertionPointCursorController;
    friend class SelectionModifierCursorController;
    friend class PositionListener;
    friend class CorrectionHighlighter;
    friend class SpanController;
    friend class Blink;
    friend class SelectionActionModeCallback;
    friend class UndoInputFilter;

    static const String TAG;
    static const Boolean DEBUG_UNDO;
    static const Int32 BLINK;

    static AutoPtr<ArrayOf<Float> > TEMP_POSITION;
    static Object TEMP_POSITION_OBJECT;
    static const Int32 DRAG_SHADOW_MAX_TEXT_LENGTH;

    static const Int32 EXTRACT_NOTHING;
    static const Int32 EXTRACT_UNKNOWN;

    // Cursor Controllers.
    AutoPtr<IUndoManager> mUndoManager;
    AutoPtr<IUndoOwner> mUndoOwner;
    AutoPtr<Elastos::Droid::Text::IInputFilter> mUndoInputFilter;
    AutoPtr<InsertionPointCursorController> mInsertionPointCursorController;
    AutoPtr<SelectionModifierCursorController> mSelectionModifierCursorController;
    AutoPtr<IActionMode> mSelectionActionMode;
    Boolean mInsertionControllerEnabled;
    Boolean mSelectionControllerEnabled;

    // Used to highlight a word when it is corrected by the IME
    AutoPtr<CorrectionHighlighter> mCorrectionHighlighter;

    AutoPtr<InputContentType> mInputContentType;
    AutoPtr<InputMethodState> mInputMethodState;

    AutoPtr< ArrayOf<TextDisplayList*> > mTextDisplayLists;

    Boolean mFrozenWithFocus;
    Boolean mSelectionMoved;
    Boolean mTouchFocusSelected;
//
    AutoPtr<IKeyListener> mKeyListener;
    Int32 mInputType;// = EditorInfo.TYPE_NULL;

    Boolean mDiscardNextActionUp;
    Boolean mIgnoreActionUpEvent;

    Int64 mShowCursor;
    AutoPtr<Blink> mBlink;

    Boolean mCursorVisible;// = TRUE;
    Boolean mSelectAllOnFocus;
    Boolean mTextIsSelectable;

    AutoPtr<ICharSequence> mError;
    Boolean mErrorWasChanged;
    AutoPtr<ErrorPopup> mErrorPopup;

    /**
     * This flag is set if the TextView tries to display an error before it
     * is attached to the window (so its position is still unknown).
     * It causes the error to be shown later, when onAttachedToWindow()
     * is called.
     */
    Boolean mShowErrorAfterAttach;

    Boolean mInBatchEditControllers;
    Boolean mShowSoftInputOnFocus;// = TRUE;
    Boolean mPreserveDetachedSelection;
    Boolean mTemporaryDetach;

    AutoPtr<SuggestionsPopupWindow> mSuggestionsPopupWindow;
    AutoPtr<ISuggestionRangeSpan> mSuggestionRangeSpan;
    AutoPtr<IRunnable> mShowSuggestionRunnable;

    AutoPtr<ArrayOf<IDrawable *> > mCursorDrawable;// = new Drawable[2];
    Int32 mCursorCount; // Current number of used mCursorDrawable: 0 (resource=0), 1 or 2 (split)

    AutoPtr<IDrawable> mSelectHandleLeft;
    AutoPtr<IDrawable> mSelectHandleRight;
    AutoPtr<IDrawable> mSelectHandleCenter;

    // Global listener that detects changes in the global position of the TextView
    AutoPtr<PositionListener> mPositionListener;

    Float mLastDownPositionX;
    Float mLastDownPositionY;
    AutoPtr<IActionModeCallback> mCustomSelectionActionModeCallback;

    // Set when this TextView gained focus with some text selected. Will start selection mode.
    Boolean mCreatedWithASelection;

    // The span controller helps monitoring the changes to which the Editor needs to react:
    // - EasyEditSpans, for which we have some UI to display on attach and on hide
    // - SelectionSpans, for which we need to call updateSelection if an IME is attached
    AutoPtr<SpanController> mSpanController;

    AutoPtr<IWordIterator> mWordIterator;
    AutoPtr<ISpellChecker> mSpellChecker;

    AutoPtr<IRect> mTempRect;

    TextView* mTextView;
    AutoPtr<CursorAnchorInfoNotifier> mCursorAnchorInfoNotifier;//= new UserDictionaryListener();
    Object mTempPositionLock;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_EDITOR_H__
