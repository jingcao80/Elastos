
#include "elastos/droid/widget/Editor.h"
#include <elastos/core/Math.h>
#include <elastos/core/Character.h>
#include <elastos/core/StringUtils.h>
#include "Elastos.Droid.InputMethodService.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.App.h"
#include "elastos/droid/R.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CClipDataHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/text/CStaticLayout.h"
#include "elastos/droid/text/DynamicLayout.h"
#include "elastos/droid/text/style/CSuggestionRangeSpan.h"
#include "elastos/droid/text/method/CMetaKeyKeyListenerHelper.h"
#include "elastos/droid/text/method/CWordIterator.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/CDragShadowBuilder.h"
#include "elastos/droid/view/inputmethod/CExtractedText.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include "elastos/droid/view/inputmethod/CCursorAnchorInfo.h"
#include "elastos/droid/widget/CSpellChecker.h"
#include "elastos/droid/widget/CLinearLayout.h"
#include "elastos/droid/widget/TextView.h"
#include "elastos/droid/widget/CPopupWindow.h"
#include "elastos/droid/widget/CListView.h"
#include "elastos/droid/widget/internal/EditableInputConnection.h"

using Elastos::Core::Character;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;

//using Elastos::Text::IBreakIterator;
using Elastos::Droid::R;
using Elastos::Droid::Internal::Utility::GrowingArrayUtils;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CClipDataHelper;
using Elastos::Droid::Content::IClipDataItem;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IClipDataHelper;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::EIID_ISpanWatcher;
using Elastos::Droid::Text::EIID_IParcelableSpan;
using Elastos::Droid::Text::EIID_IInputFilter;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Selection;
using Elastos::Droid::Text::DynamicLayout;
using Elastos::Droid::Text::IDynamicLayout;
using Elastos::Droid::Text::IStaticLayout;
using Elastos::Droid::Text::CStaticLayout;
using Elastos::Droid::Text::Style::CSuggestionRangeSpan;
using Elastos::Droid::Text::Style::EIID_ISuggestionSpan;
using Elastos::Droid::Text::Style::EIID_IURLSpan;
using Elastos::Droid::Text::Method::CWordIterator;
using Elastos::Droid::Text::Method::ITransformationMethod;
using Elastos::Droid::Text::Method::IPasswordTransformationMethod;
using Elastos::Droid::Text::Method::IMetaKeyKeyListener;
using Elastos::Droid::Text::Method::IMetaKeyKeyListenerHelper;
using Elastos::Droid::Text::Method::CMetaKeyKeyListenerHelper;
using Elastos::Droid::Graphics::Color;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::Internal::View::Menu::IMenuBuilder;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::EIID_IViewManager;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IOnTouchModeChangeListener;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::View;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::EIID_IActionModeCallback;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::IHardwareCanvas;
using Elastos::Droid::View::CDragShadowBuilder;
using Elastos::Droid::View::InputMethod::ICursorAnchorInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::InputMethod::CExtractedText;
using Elastos::Droid::View::InputMethod::CCursorAnchorInfo;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::CInputMethodManager;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::CListView;
using Elastos::Droid::Widget::CSpellChecker;
using Elastos::Droid::Widget::Internal::EditableInputConnection;
using Elastos::Droid::InputMethodService::EIID_IExtractEditText;
using Elastos::Droid::InputMethodService::IExtractEditText;

namespace Elastos {
namespace Droid {
namespace Widget {

//==============================================================================
//              ActionPopupShowerRunnable
//==============================================================================

ECode ActionPopupShowerRunnable::Run()
{
    if (mHost != NULL) {
        mHost->Show();
    }
    return NOERROR;
}

//==============================================================================
//              InsertionHandleViewHiderRunnable
//==============================================================================

ECode InsertionHandleViewHiderRunnable::Run()
{
    if (mHost != NULL) {
        mHost->Hide();
    }
    return NOERROR;
}

//==============================================================================
//              ShowSuggestionRunnable
//==============================================================================

ECode ShowSuggestionRunnable::Run()
{
    if (mHost != NULL) {
        mHost->ShowSuggestions();
    }
    return NOERROR;
}

//==============================================================================
//              InputContentType
//==============================================================================

InputContentType::InputContentType()
    : mImeOptions(IEditorInfo::IME_NULL)
    , mImeActionId(0)
    , mEnterDown(FALSE)
{
}

//==============================================================================
//              InputMethodState
//==============================================================================
InputMethodState::InputMethodState()
    : mBatchEditNesting(0)
    , mCursorChanged(FALSE)
    , mSelectionModeChanged(FALSE)
    , mContentChanged(FALSE)
    , mChangedStart(0)
    , mChangedEnd(0)
    , mChangedDelta(0)
{
    CRect::New((IRect**)&mCursorRectInWindow);
    mTmpOffset = ArrayOf<Float>::Alloc(2);
    CExtractedText::New((IExtractedText**)&mExtractedText);
}

//==============================================================================
//              CustomPopupWindow
//==============================================================================
CustomPopupWindow::CustomPopupWindow(
    /* [in] */ IContext* context,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Editor* editor,
    /* [in] */ SuggestionsPopupWindow* owner)
    : PopupWindow()
    , mEditor(editor)
    , mOwner(owner)
{
    constructor(context, NULL, defStyleAttr);
}

ECode CustomPopupWindow::Dismiss()
{
    PopupWindow::Dismiss();

    AutoPtr<PositionListener> pl = mEditor->GetPositionListener();
    pl->RemoveSubscriber(mOwner);

    // Safe cast since show() checks that mTextView->GetText() is an Editable
    AutoPtr<ICharSequence> text;
    mEditor->mTextView->GetText((ICharSequence**)&text);
    if (text != NULL && ISpannable::Probe(text)) {
        ISpannable* spannable = ISpannable::Probe(text);
        spannable->RemoveSpan(mEditor->mSuggestionRangeSpan);
    }

    mEditor->mTextView->SetCursorVisible(mOwner->mCursorWasVisibleBeforeSuggestions);
    if (mEditor->HasInsertionController()) {
        AutoPtr<InsertionPointCursorController> controller = mEditor->GetInsertionController();
        if (controller)
            controller->Show();
    }

    return NOERROR;
}
//==============================================================================
//              MyPopupWindow
//==============================================================================
MyPopupWindow::MyPopupWindow(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : PopupWindow()
{
    constructor(context, attrs, defStyle);
}

//==============================================================================
//              PinnedPopupWindow
//==============================================================================
CAR_INTERFACE_IMPL(PinnedPopupWindow, Object, ITextViewPositionListener)

PinnedPopupWindow::PinnedPopupWindow(
    /* [in] */ Editor* editor)
    : mEditor(editor)
{
}

PinnedPopupWindow::~PinnedPopupWindow()
{
}

ECode PinnedPopupWindow::Init()
{
    CreatePopupWindow();

    assert(mPopupWindow != NULL);

    mPopupWindow->SetWindowLayoutType(IWindowManagerLayoutParams::TYPE_APPLICATION_SUB_PANEL);
    mPopupWindow->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
    mPopupWindow->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);

    InitContentView();

    assert(mContentView != NULL);

    AutoPtr<IViewGroupLayoutParams> wrapContent;
    CViewGroupLayoutParams::New(
            IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT,
            (IViewGroupLayoutParams**)&wrapContent);
    IView::Probe(mContentView)->SetLayoutParams(wrapContent);

    mPopupWindow->SetContentView((IView*)mContentView->Probe(EIID_IView));
    return NOERROR;
}

ECode PinnedPopupWindow::Show()
{
    AutoPtr<PositionListener> pl = mEditor->GetPositionListener();
    pl->AddSubscriber(this, FALSE);

    ComputeLocalPosition();

    AutoPtr<PositionListener> positionListener = mEditor->GetPositionListener();
    UpdatePosition(positionListener->GetPositionX(), positionListener->GetPositionY());
    return NOERROR;
}

ECode PinnedPopupWindow::MeasureContent()
{
    AutoPtr<IResources> resources;
    mEditor->mTextView->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> displayMetrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
    Int32 width, height;
    displayMetrics->GetWidthPixels(&width);
    displayMetrics->GetHeightPixels(&height);
    IView::Probe(mContentView)->Measure(
        View::View::MeasureSpec::MakeMeasureSpec(width, View::View::MeasureSpec::AT_MOST),
        View::View::MeasureSpec::MakeMeasureSpec(height, View::View::MeasureSpec::AT_MOST));
    return NOERROR;
}

ECode PinnedPopupWindow::ComputeLocalPosition()
{
    MeasureContent();
    Int32 width;
    IView::Probe(mContentView)->GetMeasuredWidth(&width);
    Int32 offset = GetTextOffset();

    AutoPtr<ILayout> layout;
    mEditor->mTextView->GetLayout((ILayout**)&layout);
    Float horizontal;
    layout->GetPrimaryHorizontal(offset, &horizontal);
    mPositionX = (Int32) (horizontal - width / 2.0f);
    mPositionX += mEditor->mTextView->ViewportToContentHorizontalOffset();

    Int32 line;
    layout->GetLineForOffset(offset, &line);
    mPositionY = GetVerticalLocalPosition(line);
    mPositionY += mEditor->mTextView->ViewportToContentVerticalOffset();
    return NOERROR;
}

void PinnedPopupWindow::UpdatePosition(
    /* [in] */ Int32 parentPositionX,
    /* [in] */ Int32 parentPositionY)
{
    Int32 positionX = parentPositionX + mPositionX;
    Int32 positionY = parentPositionY + mPositionY;

    positionY = ClipVertically(positionY);

    // Horizontal clipping
    AutoPtr<IResources> resources;
    mEditor->mTextView->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> displayMetrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
    Int32 widthPixels;
    displayMetrics->GetWidthPixels(&widthPixels);

    Int32 width;
    IView::Probe(mContentView)->GetMeasuredWidth(&width);
    positionX = Elastos::Core::Math::Min(widthPixels - width, positionX);
    positionX = Elastos::Core::Math::Max(0, positionX);

    if (IsShowing()) {
        mPopupWindow->Update(positionX, positionY, -1, -1);
    }
    else {
        mPopupWindow->ShowAtLocation((IView*)(mEditor->mTextView->Probe(EIID_IView)),
            IGravity::NO_GRAVITY, positionX, positionY);
    }
}

ECode PinnedPopupWindow::Hide()
{
    mPopupWindow->Dismiss();
    AutoPtr<PositionListener> positionListener = mEditor->GetPositionListener();
    positionListener->RemoveSubscriber(this);
    return NOERROR;
}

ECode PinnedPopupWindow::UpdatePosition(
    /* [in] */ Int32 parentPositionX,
    /* [in] */ Int32 parentPositionY,
    /* [in] */ Boolean parentPositionChanged,
    /* [in] */ Boolean parentScrolled)
{
    // Either parentPositionChanged or parentScrolled is TRUE, check if still visible
    if (IsShowing() && mEditor->IsOffsetVisible(GetTextOffset())) {
        if (parentScrolled)
            ComputeLocalPosition();
        UpdatePosition(parentPositionX, parentPositionY);
    }
    else {
        Hide();
    }
    return NOERROR;
}

Boolean PinnedPopupWindow::IsShowing()
{
    Boolean isShowing;
    mPopupWindow->IsShowing(&isShowing);
    return isShowing;
}

//==============================================================================
//              EasyEditPopupWindow
//==============================================================================
CAR_INTERFACE_IMPL(EasyEditPopupWindow, PinnedPopupWindow, IViewOnClickListener)

const Int32 EasyEditPopupWindow::POPUP_TEXT_LAYOUT = R::layout::text_edit_action_popup_text;

EasyEditPopupWindow::EasyEditPopupWindow(
    /* [in] */ Editor* editor)
    : PinnedPopupWindow(editor)
    , mDeleteTextView(NULL)
{
}

ECode EasyEditPopupWindow::CreatePopupWindow()
{
    AutoPtr<IContext> context;
    mEditor->mTextView->GetContext((IContext**)&context);
    mPopupWindow = new MyPopupWindow(context, NULL, R::attr::textSelectHandleWindowStyle);
    mPopupWindow->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NOT_NEEDED);
    mPopupWindow->SetClippingEnabled(TRUE);
    return NOERROR;
}

ECode EasyEditPopupWindow::InitContentView()
{
    AutoPtr<IContext> context;
    mEditor->mTextView->GetContext((IContext**)&context);
    AutoPtr<ILinearLayout> linearLayout;
    CLinearLayout::New(context, (ILinearLayout**)&linearLayout);
    linearLayout->SetOrientation(ILinearLayout::HORIZONTAL);

    mContentView = IViewGroup::Probe(linearLayout);
    IView::Probe(mContentView)->SetBackgroundResource(R::drawable::text_edit_side_paste_window);

    AutoPtr<IInterface> obj;
    context->GetSystemService(
        IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);

    AutoPtr<IViewGroupLayoutParams> wrapContent;
    CViewGroupLayoutParams::New(
        IViewGroupLayoutParams::WRAP_CONTENT,
        IViewGroupLayoutParams::WRAP_CONTENT,
        (IViewGroupLayoutParams**)&wrapContent);

    inflater->Inflate(POPUP_TEXT_LAYOUT, NULL, (IView**)&mDeleteTextView);
    IView::Probe(mDeleteTextView)->SetLayoutParams(wrapContent);
    mDeleteTextView->SetText(R::string::delete_);
    IView::Probe(mDeleteTextView)->SetOnClickListener(this);
    mContentView->AddView(IView::Probe(mDeleteTextView));
    return NOERROR;
}

void EasyEditPopupWindow::SetEasyEditSpan(
    /* [in] */ IEasyEditSpan* easyEditSpan)
{
    mEasyEditSpan = easyEditSpan;
}

void EasyEditPopupWindow::SetOnDeleteListener(
    /* [in] */ IEasyEditDeleteListener* listener)
{
    mOnDeleteListener = listener;
}

ECode EasyEditPopupWindow::OnClick(
    /* [in] */ IView* view)
{
    Boolean isDeleteEnabled;
    if (view == IView::Probe(mDeleteTextView)
        && mEasyEditSpan != NULL
        && (mEasyEditSpan->IsDeleteEnabled(&isDeleteEnabled), isDeleteEnabled)
        && mOnDeleteListener != NULL) {
        mOnDeleteListener->OnDeleteClick(mEasyEditSpan);
    }
    return NOERROR;
}

ECode EasyEditPopupWindow::Hide()
{
    if (mEasyEditSpan != NULL) {
        mEasyEditSpan->SetDeleteEnabled(FALSE);
    }
    mOnDeleteListener = NULL;
    PinnedPopupWindow::Hide();
    return NOERROR;
}

Int32 EasyEditPopupWindow::GetTextOffset()
{
    // Place the pop-up at the end of the span
    AutoPtr<ICharSequence> text;
    mEditor->mTextView->GetText((ICharSequence**)&text);
    if (text != NULL && IEditable::Probe(text)) {
        IEditable* editable = IEditable::Probe(text);
        Int32 end;
        ISpanned::Probe(editable)->GetSpanEnd(mEasyEditSpan, &end);
        return end;
    }
    return 0;
}

Int32 EasyEditPopupWindow::GetVerticalLocalPosition(
    /* [in] */ Int32 line)
{
    Int32 result = 0;
    AutoPtr<ILayout> layout;
    mEditor->mTextView->GetLayout((ILayout**)&layout);
    if (layout)
        layout->GetLineBottom(line, &result);
    return result;
}

Int32 EasyEditPopupWindow::ClipVertically(
    /* [in] */ Int32 positionY)
{
    // As we display the pop-up below the span, no vertical clipping is required.
    return positionY;
}

//==============================================================================
//              SuggestionInfo
//==============================================================================
SuggestionInfo::SuggestionInfo()
    : mSuggestionStart(0)
    , mSuggestionEnd(0)
    , mSuggestionIndex(-1)
{
}

//==============================================================================
//              SuggestionAdapter
//==============================================================================

SuggestionAdapter::SuggestionAdapter(
    /* [in] */ Editor* editor,
    /* [in] */ SuggestionsPopupWindow* popupWindow)
    : mEditor(editor)
    , mPopupWindow(popupWindow)
{
    AutoPtr<IContext> context;
    mEditor->mTextView->GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
}

ECode SuggestionAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = mPopupWindow->mNumberOfSuggestions;
    return NOERROR;
}

ECode SuggestionAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    *item = (IObject*)((*mPopupWindow->mSuggestionInfos)[position]);
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode SuggestionAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    *id = position;
    return NOERROR;
}

ECode SuggestionAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ITextView> textView = ITextView::Probe(convertView);
    if (textView == NULL) {
        TextView* tv = (TextView*)ITextView::Probe(mEditor->mTextView);
        AutoPtr<IView> view;
        mInflater->Inflate(tv->mTextEditSuggestionItemLayout, parent, FALSE, (IView**)&view);
        textView = ITextView::Probe(view);
    }

    AutoPtr<SuggestionInfo> suggestionInfo = (*mPopupWindow->mSuggestionInfos)[position];
    textView->SetText(ICharSequence::Probe(suggestionInfo->mText));

    if (suggestionInfo->mSuggestionIndex == SuggestionsPopupWindow::ADD_TO_DICTIONARY
        || suggestionInfo->mSuggestionIndex == SuggestionsPopupWindow::DELETE_TEXT) {
       IView::Probe(textView)->SetBackgroundColor(IColor::TRANSPARENT);
    }
    else {
       IView::Probe(textView)->SetBackgroundColor(IColor::WHITE);
    }

    *result = IView::Probe(textView);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//==============================================================================
//              SuggestionSpanComparator
//==============================================================================
CAR_INTERFACE_IMPL(SuggestionSpanComparator, Object, IComparator)

SuggestionSpanComparator::SuggestionSpanComparator(
    /* [in] */ SuggestionsPopupWindow* host)
    : mHost(host)
{
}

ECode SuggestionSpanComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    AutoPtr<ISuggestionSpan> span1 = ISuggestionSpan::Probe(lhs);
    AutoPtr<ISuggestionSpan> span2 = ISuggestionSpan::Probe(rhs);
    if (!span1 || !span2) {
        return E_INVALID_ARGUMENT;
    }

    Int32 flag1, flag2;
    span1->GetFlags(&flag1);
    span2->GetFlags(&flag2);
    if (flag1 != flag2) {
        // The order here should match what is used in updateDrawState
        Boolean easy1 = (flag1 & ISuggestionSpan::FLAG_EASY_CORRECT) != 0;
        Boolean easy2 = (flag2 & ISuggestionSpan::FLAG_EASY_CORRECT) != 0;
        Boolean misspelled1 = (flag1 & ISuggestionSpan::FLAG_MISSPELLED) != 0;
        Boolean misspelled2 = (flag2 & ISuggestionSpan::FLAG_MISSPELLED) != 0;
        if (easy1 && !misspelled1) { *result = -1; return NOERROR; }
        if (easy2 && !misspelled2) { *result = 1; return NOERROR; }
        if (misspelled1) { *result = -1; return NOERROR; }
        if (misspelled2) { *result = 1; return NOERROR; }
    }

    HashMap<AutoPtr<ISuggestionSpan>, AutoPtr<IInteger32> >::Iterator iter1 = mHost->mSpansLengths.Find(span1);
    HashMap<AutoPtr<ISuggestionSpan>, AutoPtr<IInteger32> >::Iterator iter2 = mHost->mSpansLengths.Find(span2);
    if (iter1 == mHost->mSpansLengths.End() || iter2 == mHost->mSpansLengths.End())
        return NOERROR;

    AutoPtr<IInteger32> integer1 = iter1->mSecond;
    AutoPtr<IInteger32> integer2 = iter2->mSecond;
    assert(integer1 && integer2);
    Int32 value1, value2;
    integer1->GetValue(&value1);
    integer2->GetValue(&value2);
    *result = value1 - value2;
    return NOERROR;
}

//==============================================================================
//              SuggestionsPopupWindow
//==============================================================================

const Int32 SuggestionsPopupWindow::MAX_NUMBER_SUGGESTIONS = ISuggestionSpan::SUGGESTIONS_MAX_SIZE;
const Int32 SuggestionsPopupWindow::ADD_TO_DICTIONARY = -1;
const Int32 SuggestionsPopupWindow::DELETE_TEXT = -2;

CAR_INTERFACE_IMPL(SuggestionsPopupWindow, PinnedPopupWindow, IAdapterViewOnItemClickListener)

SuggestionsPopupWindow::SuggestionsPopupWindow(
    /* [in] */ Editor* editor)
    : PinnedPopupWindow(editor)
    , mNumberOfSuggestions(0)
    , mIsShowingUp(FALSE)
{
    mCursorWasVisibleBeforeSuggestions = mEditor->mCursorVisible;
    mSuggestionSpanComparator = new SuggestionSpanComparator(this);
}

SuggestionsPopupWindow::~SuggestionsPopupWindow()
{
}

ECode SuggestionsPopupWindow::CreatePopupWindow()
{
    AutoPtr<IContext> context;
    mEditor->mTextView->GetContext((IContext**)&context);
    mPopupWindow = new CustomPopupWindow(
            context, R::attr::textSuggestionsWindowStyle, mEditor, this);
    mPopupWindow->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NOT_NEEDED);
    mPopupWindow->SetFocusable(TRUE);
    mPopupWindow->SetClippingEnabled(FALSE);
    return NOERROR;
}

ECode SuggestionsPopupWindow::InitContentView()
{
    AutoPtr<IContext> context;
    mEditor->mTextView->GetContext((IContext**)&context);
    AutoPtr<IListView> listView;
    CListView::New(context, (IListView**)&listView);

    mSuggestionsAdapter = new SuggestionAdapter(mEditor, this);
    IAdapterView::Probe(listView)->SetAdapter(IAdapter::Probe(mSuggestionsAdapter));
    IAdapterView::Probe(listView)->SetOnItemClickListener(this);
    mContentView = IViewGroup::Probe(listView);

    // Inflate the suggestion items once and for all. + 2 for add to dictionary and delete
    mSuggestionInfos = ArrayOf<SuggestionInfo*>::Alloc(MAX_NUMBER_SUGGESTIONS + 2);
    for (Int32 i = 0; i < mSuggestionInfos->GetLength(); i++) {
        AutoPtr<SuggestionInfo> info = new SuggestionInfo();
        mSuggestionInfos->Set(i, info);
    }
    return NOERROR;
}

Boolean SuggestionsPopupWindow::IsShowingUp()
{
    return mIsShowingUp;
}

void SuggestionsPopupWindow::OnParentLostFocus()
{
    mIsShowingUp = FALSE;
}

AutoPtr<ArrayOf<ISuggestionSpan*> > SuggestionsPopupWindow::GetSuggestionSpans()
{
    AutoPtr<ArrayOf<ISuggestionSpan*> > suggestionSpans;
    Int32 pos;
    mEditor->mTextView->GetSelectionStart(&pos);
    AutoPtr<ICharSequence> text;
    mEditor->mTextView->GetText((ICharSequence**)&text);
    if (text && ISpannable::Probe(text)) {
        ISpannable* spannable = ISpannable::Probe(text);
        AutoPtr< ArrayOf<IInterface*> > temp;
        ISpanned::Probe(spannable)->GetSpans(pos, pos, EIID_ISuggestionSpan, (ArrayOf<IInterface*>**)&temp);
        suggestionSpans = ArrayOf<ISuggestionSpan*>::Alloc(temp->GetLength());
        for (Int32 i = 0; i < temp->GetLength(); i++){
            suggestionSpans->Set(i, ISuggestionSpan::Probe((*temp)[i]));
        }
        mSpansLengths.Clear();
        Int32 start, end;
        for (Int32 i = 0; i < suggestionSpans->GetLength(); ++i) {
            AutoPtr<ISuggestionSpan> suggestionSpan = ISuggestionSpan::Probe((*suggestionSpans)[i]);
            assert(suggestionSpan != NULL);
            ISpanned::Probe(spannable)->GetSpanStart(suggestionSpan, &start);
            ISpanned::Probe(spannable)->GetSpanEnd(suggestionSpan, &end);

            AutoPtr<IInteger32> integer;
            CInteger32::New(end - start, (IInteger32**)&integer);
            //mSpansLengths[suggestionSpan] = integer;
        }

        // The suggestions are sorted according to their types (easy correction first, then
        // misspelled) and to the length of the text that they cover (shorter first).
//        Arrays.sort(suggestionSpans, mSuggestionSpanComparator);
        QuickSort(suggestionSpans, 0, suggestionSpans->GetLength() - 1);
    }

    return suggestionSpans;
}

void SuggestionsPopupWindow::QuickSort(ArrayOf<ISuggestionSpan*>* array, Int32 low, Int32 high)
{
    if (array == NULL || low >= high || low < 0)
        return;

    AutoPtr<ISuggestionSpan> privot = (*array)[low];
    Int32 i = low, j = high;
    Int32 result;
    while (i < j) {
        while (i < j) {
            mSuggestionSpanComparator->Compare(
                    (*array)[j]->Probe(EIID_IInterface),
                    privot->Probe(EIID_IInterface),
                    &result);
            if (result >= 0){
                --j;
            }
        }

        if (i < j) {
            array->Set(i++, (*array)[j]);
        }

        while (i < j) {
            mSuggestionSpanComparator->Compare(
                    (*array)[i]->Probe(EIID_IInterface),
                    privot->Probe(EIID_IInterface),
                    &result);
            if (result <= 0){
                ++i;
            }
        }

        if (i < j) {
            array->Set(j--, (*array)[i]);
        }
    }

    array->Set(i, privot);

    QuickSort(array, low, i - 1);
    QuickSort(array, i + 1, high);
}

ECode SuggestionsPopupWindow::Show()
{
    AutoPtr<ICharSequence> text;
    mEditor->mTextView->GetText((ICharSequence**)&text);
    if (text && IEditable::Probe(text)) {
        return NOERROR;
    }

    if (UpdateSuggestions()) {
        mCursorWasVisibleBeforeSuggestions = mEditor->mCursorVisible;
        mEditor->mTextView->SetCursorVisible(FALSE);
        mIsShowingUp = TRUE;
        PinnedPopupWindow::Show();
    }
    return NOERROR;
}

ECode SuggestionsPopupWindow::MeasureContent()
{
    AutoPtr<IResources> resources;
    mEditor->mTextView->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> displayMetrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
    Int32 widthPixels, heightPixels;
    displayMetrics->GetWidthPixels(&widthPixels);
    displayMetrics->GetHeightPixels(&heightPixels);

    Int32 horizontalMeasure = View::View::MeasureSpec::MakeMeasureSpec(
            widthPixels, View::View::MeasureSpec::AT_MOST);
    Int32 verticalMeasure = View::View::MeasureSpec::MakeMeasureSpec(
            heightPixels, View::View::MeasureSpec::AT_MOST);

    Int32 width = 0, measuredWidth;
    AutoPtr<IView> view;
    for (Int32 i = 0; i < mNumberOfSuggestions; i++) {
        AutoPtr<IView> newView;
        mSuggestionsAdapter->GetView(i, view, mContentView, (IView**)&newView);
        AutoPtr<IViewGroupLayoutParams> params;
        newView->GetLayoutParams((IViewGroupLayoutParams**)&params);
        params->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
        newView->Measure(horizontalMeasure, verticalMeasure);
        newView->GetMeasuredWidth(&measuredWidth);
        width = Elastos::Core::Math::Max(width, measuredWidth);
        view = newView;
    }

    // Enforce the width based on actual text widths
    IView::Probe(mContentView)->Measure(
        View::View::MeasureSpec::MakeMeasureSpec(width, View::View::MeasureSpec::EXACTLY),
        verticalMeasure);

    AutoPtr<IDrawable> popupBackground;
    mPopupWindow->GetBackground((IDrawable**)&popupBackground);
    if (popupBackground != NULL) {
        if (mEditor->mTempRect == NULL) {
            CRect::New((IRect**)&mEditor->mTempRect);
        }
        Boolean isPadding;
        popupBackground->GetPadding(mEditor->mTempRect, &isPadding);
        Int32 left, right;
        mEditor->mTempRect->GetLeft(&left);
        mEditor->mTempRect->GetRight(&right);
        width += left + right;
    }
    mPopupWindow->SetWidth(width);
    return NOERROR;
}

Int32 SuggestionsPopupWindow::GetTextOffset()
{
    Int32 offset;
    mEditor->mTextView->GetSelectionStart(&offset);
    return offset;
}

Int32 SuggestionsPopupWindow::GetVerticalLocalPosition(
    /* [in] */ Int32 line)
{
    Int32 result = 0;
    AutoPtr<ILayout> layout;
    mEditor->mTextView->GetLayout((ILayout**)&layout);
    if (layout)
        layout->GetLineBottom(line, &result);

    return result;
}

Int32 SuggestionsPopupWindow::ClipVertically(
    /* [in] */ Int32 positionY)
{
    AutoPtr<IResources> resources;
    mEditor->mTextView->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> displayMetrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
    Int32 heightPixels;
    displayMetrics->GetHeightPixels(&heightPixels);

    Int32 height;
    IView::Probe(mContentView)->GetMeasuredHeight(&height);
    return Elastos::Core::Math::Min(positionY, heightPixels - height);
}

ECode SuggestionsPopupWindow::Hide()
{
    PinnedPopupWindow::Hide();
    return NOERROR;
}

Boolean SuggestionsPopupWindow::UpdateSuggestions()
{
    AutoPtr<ICharSequence> text;
    mEditor->mTextView->GetText((ICharSequence**)&text);
    ISpannable* spannable = ISpannable::Probe(text);
    AutoPtr<ArrayOf<ISuggestionSpan*> > suggestionSpans = GetSuggestionSpans();

    // Suggestions are shown after a delay: the underlying spans may have been removed
    if (NULL == suggestionSpans || suggestionSpans->GetLength() == 0)
        return FALSE;
    Int32 nbSpans = suggestionSpans->GetLength();

    mNumberOfSuggestions = 0;
    Int32 spanUnionStart;
    Int32 spanUnionEnd = 0;
    text->GetLength(&spanUnionStart);
    AutoPtr<ISuggestionSpan> misspelledSpan;
    Int32 underlineColor = 0;

    Int32 spanStart, spanEnd, otherSpanStart, otherSpanEnd;
    Int32 flags, textLength;
    for (Int32 spanIndex = 0; spanIndex < nbSpans; spanIndex++) {
        AutoPtr<ISuggestionSpan> suggestionSpan = (*suggestionSpans)[spanIndex];
        ISpanned::Probe(spannable)->GetSpanStart(suggestionSpan, &spanStart);
        ISpanned::Probe(spannable)->GetSpanEnd(suggestionSpan, &spanEnd);

        spanUnionStart = Elastos::Core::Math::Min(spanStart, spanUnionStart);
        spanUnionEnd = Elastos::Core::Math::Max(spanEnd, spanUnionEnd);

        suggestionSpan->GetFlags(&flags);
        if ((flags & ISuggestionSpan::FLAG_MISSPELLED) != 0) {
            misspelledSpan = suggestionSpan;
        }

        // The first span dictates the background color of the highlighted text
        if (spanIndex == 0) {
            suggestionSpan->GetUnderlineColor(&underlineColor);
        }

        AutoPtr<ArrayOf<String> > suggestions;
        suggestionSpan->GetSuggestions((ArrayOf<String>**)&suggestions);
        if (suggestions != NULL) {
            Int32 nbSuggestions = suggestions->GetLength();
            String info;
            for (Int32 suggestionIndex = 0; suggestionIndex < nbSuggestions; suggestionIndex++) {
                String suggestion = (*suggestions)[suggestionIndex];

                Boolean suggestionIsDuplicate = FALSE;
                for (Int32 i = 0; i < mNumberOfSuggestions; i++) {
                    ICharSequence::Probe((*mSuggestionInfos)[i]->mText)->ToString(&info);
                    if (info.Equals(suggestion)) {
                        AutoPtr<ISuggestionSpan> otherSuggestionSpan = (*mSuggestionInfos)[i]->mSuggestionSpan;
                        ISpanned::Probe(spannable)->GetSpanStart(otherSuggestionSpan, &otherSpanStart);
                        ISpanned::Probe(spannable)->GetSpanEnd(otherSuggestionSpan, &otherSpanEnd);
                        if (spanStart == otherSpanStart && spanEnd == otherSpanEnd) {
                            suggestionIsDuplicate = TRUE;
                            break;
                        }
                    }
                }

                if (!suggestionIsDuplicate) {
                    AutoPtr<SuggestionInfo> suggestionInfo = (*mSuggestionInfos)[mNumberOfSuggestions];
                    suggestionInfo->mSuggestionSpan = suggestionSpan;
                    suggestionInfo->mSuggestionIndex = suggestionIndex;
                    ICharSequence::Probe(suggestionInfo->mText)->GetLength(&textLength);
                    AutoPtr<ICharSequence> seq;
                    CString::New(suggestion, (ICharSequence**)&seq);
                    IEditable::Probe(suggestionInfo->mText)->Replace(0, textLength, seq);

                    mNumberOfSuggestions++;

                    if (mNumberOfSuggestions == MAX_NUMBER_SUGGESTIONS) {
                        // Also end outer for loop
                        spanIndex = nbSpans;
                        break;
                    }
                }
            }
        }
    }

    for (Int32 i = 0; i < mNumberOfSuggestions; i++) {
        HighlightTextDifferences((*mSuggestionInfos)[i], spanUnionStart, spanUnionEnd);
    }

    AutoPtr<IContext> context;
    mEditor->mTextView->GetContext((IContext**)&context);
    // Add "Add to dictionary" item if there is a span with the misspelled flag
    if (misspelledSpan != NULL) {
        Int32 misspelledStart, misspelledEnd;
        ISpanned::Probe(spannable)->GetSpanStart(misspelledSpan, &misspelledStart);
        ISpanned::Probe(spannable)->GetSpanEnd(misspelledSpan, &misspelledEnd);
        if (misspelledStart >= 0 && misspelledEnd > misspelledStart) {
            AutoPtr<SuggestionInfo> suggestionInfo = (*mSuggestionInfos)[mNumberOfSuggestions];
            suggestionInfo->mSuggestionSpan = misspelledSpan;
            suggestionInfo->mSuggestionIndex = ADD_TO_DICTIONARY;

            String infoStr;
            context->GetString(R::string::addToDictionary, &infoStr);
            ICharSequence::Probe(suggestionInfo->mText)->GetLength(&textLength);
            AutoPtr<ICharSequence> seq;
            CString::New(infoStr, (ICharSequence**)&seq);
            IEditable::Probe(suggestionInfo->mText)->Replace(0, textLength, seq);
            ISpannable::Probe(suggestionInfo->mText)->SetSpan(suggestionInfo->mHighlightSpan, 0, 0,
                    ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);

            mNumberOfSuggestions++;
        }
    }

    // Delete item
    AutoPtr<SuggestionInfo> suggestionInfo = (*mSuggestionInfos)[mNumberOfSuggestions];
    suggestionInfo->mSuggestionSpan = NULL;
    suggestionInfo->mSuggestionIndex = DELETE_TEXT;
    String infoStr;
    context->GetString(R::string::deleteText, &infoStr);
    AutoPtr<ICharSequence> seq;
    CString::New(infoStr, (ICharSequence**)&seq);
    ICharSequence::Probe(suggestionInfo->mText)->GetLength(&textLength);
    IEditable::Probe(suggestionInfo->mText)->Replace(0, textLength, seq);
    ISpannable::Probe(suggestionInfo->mText)->SetSpan(suggestionInfo->mHighlightSpan, 0, 0,
            ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
    mNumberOfSuggestions++;

    if (mEditor->mSuggestionRangeSpan == NULL) {
        CSuggestionRangeSpan::New((ISuggestionRangeSpan**)&mEditor->mSuggestionRangeSpan);
    }
    if (underlineColor == 0) {
        // Fallback on the default highlight color when the first span does not provide one
        mEditor->mSuggestionRangeSpan->SetBackgroundColor(mEditor->mTextView->mHighlightColor);
    }
    else {
        Float BACKGROUND_TRANSPARENCY = 0.4f;
        Int32 alpha = Color::Alpha(underlineColor);
        Int32 newAlpha = (Int32) (alpha * BACKGROUND_TRANSPARENCY);
        mEditor->mSuggestionRangeSpan->SetBackgroundColor(
                (underlineColor & 0x00FFFFFF) + (newAlpha << 24));
    }
    spannable->SetSpan(mEditor->mSuggestionRangeSpan, spanUnionStart, spanUnionEnd,
        ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);

    mSuggestionsAdapter->NotifyDataSetChanged();
    return TRUE;
}

void SuggestionsPopupWindow::HighlightTextDifferences(
    /* [in] */ SuggestionInfo* suggestionInfo,
    /* [in] */ Int32 unionStart,
    /* [in] */ Int32 unionEnd)
{
    AutoPtr<ICharSequence> seq;
    mEditor->mTextView->GetText((ICharSequence**)&seq);
    ISpannable* text = ISpannable::Probe(seq);
    Int32 spanStart, spanEnd;
    ISpanned::Probe(text)->GetSpanStart(suggestionInfo->mSuggestionSpan, &spanStart);
    ISpanned::Probe(text)->GetSpanEnd(suggestionInfo->mSuggestionSpan, &spanEnd);

    // Adjust the start/end of the suggestion span
    Int32 textLength;
    ICharSequence::Probe(suggestionInfo->mText)->GetLength(&textLength);
    suggestionInfo->mSuggestionStart = spanStart - unionStart;
    suggestionInfo->mSuggestionEnd = suggestionInfo->mSuggestionStart + textLength;
    ISpannable::Probe(suggestionInfo->mText)->SetSpan(suggestionInfo->mHighlightSpan, 0,
            textLength, ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);

    // Add the text before and after the span.
    String textAsString;
    ICharSequence::Probe(text)->ToString(&textAsString);
    String subStr = textAsString.Substring(unionStart, spanStart);

    AutoPtr<ICharSequence> cs;
    CString::New(subStr, (ICharSequence**)&cs);
    IEditable::Probe(suggestionInfo->mText)->Insert(0, cs);

    cs = NULL;
    subStr = textAsString.Substring(spanEnd, unionEnd);
    CString::New(subStr, (ICharSequence**)&cs);
    IEditable::Probe(suggestionInfo->mText)->Append(cs);
}

ECode SuggestionsPopupWindow::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IContext> context;
    mEditor->mTextView->GetContext((IContext**)&context);
    AutoPtr<ICharSequence> seq;
    mEditor->mTextView->GetText((ICharSequence**)&seq);
    IEditable* editable = IEditable::Probe(seq);

    AutoPtr<SuggestionInfo> suggestionInfo = (*mSuggestionInfos)[position];

    if (suggestionInfo->mSuggestionIndex == DELETE_TEXT) {
        Int32 spanUnionStart, spanUnionEnd;
        ISpanned::Probe(editable)->GetSpanStart(mEditor->mSuggestionRangeSpan, &spanUnionStart);
        ISpanned::Probe(editable)->GetSpanEnd(mEditor->mSuggestionRangeSpan, &spanUnionEnd);
        if (spanUnionStart >= 0 && spanUnionEnd > spanUnionStart) {
            Int32 length;
            ICharSequence::Probe(editable)->GetLength(&length);
            if (spanUnionEnd < length) {
                Char32 endChar, startChar = '0';
                ICharSequence::Probe(editable)->GetCharAt(spanUnionEnd, &endChar);
                if (spanUnionStart > 0) {
                    ICharSequence::Probe(editable)->GetCharAt(spanUnionStart - 1, &startChar);
                }
                // Do not leave two adjacent spaces after deletion, or one at beginning of text
                if (Character::IsSpaceChar(endChar) &&
                        (spanUnionStart == 0 || Character::IsSpaceChar(startChar))) {
                    spanUnionEnd = spanUnionEnd + 1;
                }
            }
            mEditor->mTextView->DeleteText_internal(spanUnionStart, spanUnionEnd);
        }
        Hide();
        return NOERROR;
    }

    Int32 spanStart, spanEnd;
    ISpanned::Probe(editable)->GetSpanStart(suggestionInfo->mSuggestionSpan, &spanStart);
    ISpanned::Probe(editable)->GetSpanEnd(suggestionInfo->mSuggestionSpan, &spanEnd);
    if (spanStart < 0 || spanEnd <= spanStart) {
        // Span has been removed
        Hide();
        return NOERROR;
    }

    String originalText;
    ICharSequence::Probe(editable)->ToString(&originalText);
    originalText = originalText.Substring(spanStart, spanEnd);

    if (suggestionInfo->mSuggestionIndex == ADD_TO_DICTIONARY) {
        AutoPtr<ILocale> locale;
        mEditor->mTextView->GetTextServicesLocale((ILocale**)&locale);
        String localeStr;
        locale->ToString(&localeStr);
        AutoPtr<IIntent> intent;
        CIntent::New(ISettings::ACTION_USER_DICTIONARY_INSERT, (IIntent**)&intent);
        intent->PutExtra(String("word"), originalText);
        intent->PutExtra(String("locale"), localeStr);
        // Put a listener to replace the original text with a word which the user
        // modified in a user dictionary dialog.
        Int32 flags;
        intent->GetFlags(&flags);
        intent->SetFlags(flags | IIntent::FLAG_ACTIVITY_NEW_TASK);
        context->StartActivity(intent);
        // There is no way to know if the word was indeed added. Re-check.
        // TODO The ExtractEditText should remove the span in the original text instead
        ISpannable::Probe(editable)->RemoveSpan(suggestionInfo->mSuggestionSpan);
        Selection::SetSelection(ISpannable::Probe(editable), spanEnd);
        mEditor->UpdateSpellCheckSpans(spanStart, spanEnd, FALSE);
    }
    else {
        // SuggestionSpans are removed by replace: save them before
        AutoPtr<ArrayOf<IInterface*> > temp;
        ISpanned::Probe(editable)->GetSpans(spanStart, spanEnd,
                EIID_ISuggestionSpan, (ArrayOf<IInterface*>**)&temp);
        AutoPtr<ArrayOf<ISuggestionSpan*> > suggestionSpans = ArrayOf<ISuggestionSpan*>::Alloc(temp->GetLength());
        for(Int32 i = 0; i < temp->GetLength(); i++) {
            suggestionSpans->Set(i, ISuggestionSpan::Probe((*temp)[i]));
        }
        Int32 length = suggestionSpans->GetLength();

        AutoPtr<ArrayOf<Int32> > suggestionSpansStarts = ArrayOf<Int32>::Alloc(length);
        AutoPtr<ArrayOf<Int32> > suggestionSpansEnds = ArrayOf<Int32>::Alloc(length);
        AutoPtr<ArrayOf<Int32> > suggestionSpansFlags = ArrayOf<Int32>::Alloc(length);

        Int32 start, end, flags;
        for (Int32 i = 0; i < length; i++) {
            ISuggestionSpan* suggestionSpan = (*suggestionSpans)[i];
            ISpanned::Probe(editable)->GetSpanStart(suggestionSpan, &start);
            ISpanned::Probe(editable)->GetSpanEnd(suggestionSpan, &end);
            ISpanned::Probe(editable)->GetSpanFlags(suggestionSpan, &flags);
            (*suggestionSpansStarts)[i] = start;
            (*suggestionSpansEnds)[i] = end;
            (*suggestionSpansFlags)[i] = flags;

            // Remove potential misspelled flags
            Int32 suggestionSpanFlags;
            suggestionSpan->GetFlags(&suggestionSpanFlags);
            if ((suggestionSpanFlags & ISuggestionSpan::FLAG_MISSPELLED) > 0) {
                suggestionSpanFlags &= ~ISuggestionSpan::FLAG_MISSPELLED;
                suggestionSpanFlags &= ~ISuggestionSpan::FLAG_EASY_CORRECT;
                suggestionSpan->SetFlags(suggestionSpanFlags);
            }
        }

        Int32 suggestionStart = suggestionInfo->mSuggestionStart;
        Int32 suggestionEnd = suggestionInfo->mSuggestionEnd;
        AutoPtr<ICharSequence> subSeq;
        ICharSequence::Probe(suggestionInfo->mText)->SubSequence(suggestionStart, suggestionEnd, (ICharSequence**)&subSeq);
        mEditor->mTextView->ReplaceText_internal(spanStart, spanEnd, subSeq);

        // Notify source IME of the suggestion pick. Do this before swaping texts.
        AutoPtr<IContext> ctx;
        IView::Probe(mEditor->mTextView)->GetContext((IContext**)&ctx);
        suggestionInfo->mSuggestionSpan->NotifySelection(
                        ctx, originalText, suggestionInfo->mSuggestionIndex);

        // Swap text content between actual text and Suggestion span
        AutoPtr<ArrayOf<String> > suggestions;
        suggestionInfo->mSuggestionSpan->GetSuggestions((ArrayOf<String>**)&suggestions);
        suggestions->Set(suggestionInfo->mSuggestionIndex, originalText);

        // Restore previous SuggestionSpans
        Int32 realSuggestionLength;
        AutoPtr<ICharSequence> csq;
        mEditor->mTextView->GetText((ICharSequence**)&csq);
        csq->GetLength(&realSuggestionLength);

        Int32 seqLen;
        subSeq->GetLength(&seqLen);
        Int32 lengthDifference = seqLen - (spanEnd - spanStart);
        Int32 realSuggestionDiff = realSuggestionLength - (spanEnd - spanStart);

        if (realSuggestionDiff < lengthDifference) {
            lengthDifference = realSuggestionDiff;
        }

        for (Int32 i = 0; i < length; i++) {
            // Only spans that include the modified region make sense after replacement
            // Spans partially included in the replaced region are removed, there is no
            // way to assign them a valid range after replacement
            if ((*suggestionSpansStarts)[i] <= spanStart &&
                    (*suggestionSpansEnds)[i] >= spanEnd) {
                // When the SpansEnd beyond the length of mTextView here should avoid it.
                csq = NULL;
                mEditor->mTextView->GetText((ICharSequence**)&csq);
                Int32 nTextLen;
                csq->GetLength(&nTextLen);
                Int32 spansEnd = (*suggestionSpansEnds)[i] + lengthDifference;
                Int32 realSpansEnd = spansEnd > nTextLen ? nTextLen : spansEnd;

                mEditor->mTextView->SetSpan_internal(
                        (*suggestionSpans)[i]->Probe(EIID_IInterface),
                        (*suggestionSpansStarts)[i],
                        realSpansEnd,
                        (*suggestionSpansFlags)[i]);
            }
        }

        // Move cursor at the end of the replaced word
        Int32 newCursorPosition = spanEnd + lengthDifference;
        // When the SpansEnd beyond the length of mTextView here should avoid it.
        csq = NULL;
        mEditor->mTextView->GetText((ICharSequence**)&csq);
        Int32 textLen;
        csq->GetLength(&textLen);
        Int32 realNewCursorPosition = newCursorPosition > textLen ? textLen
                : newCursorPosition;
        mEditor->mTextView->SetCursorPosition_internal(realNewCursorPosition, realNewCursorPosition);
    }

    Hide();
    return NOERROR;
}

//==============================================================================
//              SelectionActionModeCallback
//==============================================================================
CAR_INTERFACE_IMPL(SelectionActionModeCallback, Object, IActionModeCallback)

SelectionActionModeCallback::SelectionActionModeCallback(
    /* [in] */ Editor* editor)
    : mEditor(editor)
{
}

ECode SelectionActionModeCallback::OnCreateActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    assert(mode != NULL && menu != NULL && result != NULL);
    AutoPtr<IContext> ctx;
    IView::Probe(mEditor->mTextView)->GetContext((IContext**)&ctx);
    assert(ctx != NULL);
    AutoPtr<IApplicationInfo> info;
    ctx->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 targetSdkVersion;
    info->GetTargetSdkVersion(&targetSdkVersion);
    Boolean legacy = targetSdkVersion < Build::VERSION_CODES::LOLLIPOP;
    AutoPtr<IContext> context;
    if (!legacy && IMenuBuilder::Probe(menu)) {
        IMenuBuilder::Probe(menu)->GetContext((IContext**)&context);
    } else {
        IView::Probe(mEditor->mTextView)->GetContext((IContext**)&context);
    }

    AutoPtr<ITypedArray> styledAttributes;
    AutoPtr<ArrayOf<Int32> > values = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::SelectionModeDrawables),
        ArraySize(R::styleable::SelectionModeDrawables));

    ctx->ObtainStyledAttributes(
            values, (ITypedArray**)&styledAttributes);
    assert(styledAttributes != NULL);

    String text;
    ctx->GetString(R::string::textSelectionCABTitle, &text);
    AutoPtr<ICharSequence> title;
    CString::New(text, (ICharSequence**)&title);
    mode->SetTitle(title);
    mode->SetSubtitle((ICharSequence*)NULL);
    mode->SetTitleOptionalHint(TRUE);

    AutoPtr<IMenuItem> menuItem;
    menu->Add(0, R::id::selectAll, 0, R::string::selectAll, (IMenuItem**)&menuItem);
    Int32 id = 0;
    styledAttributes->GetResourceId(R::styleable::SelectionModeDrawables_actionModeSelectAllDrawable, 0, &id);
    menuItem->SetIcon(id);
    menuItem->SetAlphabeticShortcut('a');
    menuItem->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_ALWAYS | IMenuItem::SHOW_AS_ACTION_WITH_TEXT);

    if (mEditor->mTextView->CanCut()) {
        AutoPtr<IMenuItem> item, temp;
        menu->Add(0, R::id::cut, 0, R::string::cut, (IMenuItem**)&item);

        Int32 id = 0;
        styledAttributes->GetResourceId(R::styleable::SelectionModeDrawables_actionModeCutDrawable, 0, &id);
        item->SetIcon(id);
        item->SetAlphabeticShortcut('x');
        item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_ALWAYS | IMenuItem::SHOW_AS_ACTION_WITH_TEXT);
    }

    if (mEditor->mTextView->CanCopy()) {
        AutoPtr<IMenuItem> item, temp;
        menu->Add(0, R::id::copy, 0, R::string::copy, (IMenuItem**)&item);

        Int32 id = 0;
        styledAttributes->GetResourceId(R::styleable::SelectionModeDrawables_actionModeCopyDrawable, 0, &id);
        item->SetIcon(id);
        item->SetAlphabeticShortcut('c');
        item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_ALWAYS | IMenuItem::SHOW_AS_ACTION_WITH_TEXT);
    }

    if (mEditor->mTextView->CanPaste()) {
        AutoPtr<IMenuItem> item;
        menu->Add(0, R::id::paste, 0, R::string::paste, (IMenuItem**)&item);

        Int32 id = 0;
        styledAttributes->GetResourceId(R::styleable::SelectionModeDrawables_actionModePasteDrawable, 0, &id);
        item->SetIcon(id);
        item->SetAlphabeticShortcut('v');
        item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_ALWAYS | IMenuItem::SHOW_AS_ACTION_WITH_TEXT);
    }

    styledAttributes->Recycle();

    if (mEditor->mCustomSelectionActionModeCallback != NULL) {
        Boolean state = FALSE;
        mEditor->mCustomSelectionActionModeCallback->OnCreateActionMode(mode, menu, &state);
        if (!state) {
            // The custom mode can choose to cancel the action mode
            *result = FALSE;
            return NOERROR;
        }
    }

    Boolean has = FALSE;
    AutoPtr<IView> view;
    if ((menu->HasVisibleItems(&has), has) || (mode->GetCustomView((IView**)&view), view.Get()) != NULL) {
        mEditor->GetSelectionController()->Show();
        mEditor->mTextView->SetHasTransientState(TRUE);
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode SelectionActionModeCallback::OnPrepareActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    assert(result != NULL);
    if (mEditor->mCustomSelectionActionModeCallback != NULL) {
        return mEditor->mCustomSelectionActionModeCallback->OnPrepareActionMode(mode, menu, result);
    }

    *result = TRUE;
    return NOERROR;
}

ECode SelectionActionModeCallback::OnActionItemClicked(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    assert(result != NULL);
    if (mEditor->mCustomSelectionActionModeCallback != NULL) {
        Boolean state = FALSE;
        if ((mEditor->mCustomSelectionActionModeCallback->OnActionItemClicked(mode, item, &state), state)) {
            *result = TRUE;
            return NOERROR;
        }
    }

    assert(item != NULL);
    Int32 id = 0;
    item->GetItemId(&id);
    mEditor->mTextView->OnTextContextMenuItem(id, result);
    return NOERROR;
}

ECode SelectionActionModeCallback::OnDestroyActionMode(
    /* [in] */ IActionMode* mode)
{
    if (mEditor->mCustomSelectionActionModeCallback != NULL) {
        mEditor->mCustomSelectionActionModeCallback->OnDestroyActionMode(mode);
    }

    /*
     * If we're ending this mode because we're detaching from a window,
     * we still have selection state to preserve. Don't clear it, we'll
     * bring back the selection mode when (if) we get reattached.
     */
    if (!mEditor->mPreserveDetachedSelection) {
        AutoPtr<ICharSequence> text;
        Int32 end;
        mEditor->mTextView->GetSelectionEnd(&end);
        mEditor->mTextView->GetText((ICharSequence**)&text);
        Selection::SetSelection(ISpannable::Probe(text), end);
        mEditor->mTextView->SetHasTransientState(FALSE);
    }

    if (mEditor->mSelectionModifierCursorController != NULL) {
        mEditor->mSelectionModifierCursorController->Hide();
    }

    mEditor->mSelectionActionMode = NULL;
    return NOERROR;
}

//==============================================================================
//              ActionPopupWindow
//==============================================================================

const Int32 ActionPopupWindow::POPUP_TEXT_LAYOUT = R::layout::text_edit_action_popup_text;

CAR_INTERFACE_IMPL(ActionPopupWindow, PinnedPopupWindow, IViewOnClickListener)

ActionPopupWindow::ActionPopupWindow(
    /* [in] */ Editor* editor)
    : PinnedPopupWindow(editor)
{
}

ActionPopupWindow::~ActionPopupWindow()
{
}

ECode ActionPopupWindow::CreatePopupWindow()
{
    AutoPtr<IContext> context;
    mEditor->mTextView->GetContext((IContext**)&context);
    mPopupWindow = new MyPopupWindow(context, NULL, R::attr::textSelectHandleWindowStyle);
    mPopupWindow->SetClippingEnabled(TRUE);
    return NOERROR;
}

ECode ActionPopupWindow::InitContentView()
{
    AutoPtr<IContext> context;
    mEditor->mTextView->GetContext((IContext**)&context);
    AutoPtr<ILinearLayout> linearLayout;
    CLinearLayout::New(context, (ILinearLayout**)&linearLayout);
    linearLayout->SetOrientation(ILinearLayout::HORIZONTAL);

    mContentView = IViewGroup::Probe(linearLayout);
    IView::Probe(mContentView)->SetBackgroundResource(R::drawable::text_edit_paste_window);

    AutoPtr<IInterface> obj;
    context->GetSystemService(
        IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);

    AutoPtr<IViewGroupLayoutParams> wrapContent;
    CViewGroupLayoutParams::New(
        IViewGroupLayoutParams::WRAP_CONTENT,
        IViewGroupLayoutParams::WRAP_CONTENT,
        (IViewGroupLayoutParams**)&wrapContent);

    AutoPtr<IView> temp;
    inflater->Inflate(POPUP_TEXT_LAYOUT, NULL, (IView**)&temp);
    mPasteTextView = ITextView::Probe(temp);
    IView::Probe(mPasteTextView)->SetLayoutParams(wrapContent);
    mPasteTextView->SetText(R::string::paste);
    IView::Probe(mPasteTextView)->SetOnClickListener(this);
    mContentView->AddView(IView::Probe(mPasteTextView));

    temp = NULL;
    inflater->Inflate(POPUP_TEXT_LAYOUT, NULL, (IView**)&temp);
    mReplaceTextView = ITextView::Probe(temp);
    IView::Probe(mReplaceTextView)->SetLayoutParams(wrapContent);
    mReplaceTextView->SetText(R::string::replace);
    IView::Probe(mReplaceTextView)->SetOnClickListener(this);
    mContentView->AddView(IView::Probe(mReplaceTextView));

    return NOERROR;
}

ECode ActionPopupWindow::Show()
{
    Boolean canPaste = mEditor->mTextView->CanPaste();
    Boolean isSuggestionsEnabled;
    mEditor->mTextView->IsSuggestionsEnabled(&isSuggestionsEnabled);
    Boolean canSuggest = isSuggestionsEnabled && mEditor->IsCursorInsideSuggestionSpan();
    IView::Probe(mPasteTextView)->SetVisibility(canPaste ? IView::VISIBLE : IView::GONE);
    IView::Probe(mReplaceTextView)->SetVisibility(canSuggest ? IView::VISIBLE : IView::GONE);

    if (!canPaste && !canSuggest) return NOERROR;

    Show();
    return NOERROR;
}

ECode ActionPopupWindow::OnClick(
    /* [in] */ IView* view)
{
    if (view == IView::Probe(mPasteTextView) && mEditor->mTextView->CanPaste()) {
        Boolean res;
        mEditor->mTextView->OnTextContextMenuItem(R::id::paste, &res);
        Hide();
    } else if (view == IView::Probe(mReplaceTextView)) {
        Int32 middle = GetTextOffset();
        mEditor->StopSelectionActionMode();
        AutoPtr<ICharSequence> seq;
        mEditor->mTextView->GetText((ICharSequence**)&seq);
        ISpannable* spannable = ISpannable::Probe(seq);
        Selection::SetSelection(spannable, middle);
        mEditor->ShowSuggestions();
    }
    return NOERROR;
}

Int32 ActionPopupWindow::GetTextOffset()
{
    Int32 start, end;
    mEditor->mTextView->GetSelectionStart(&start);
    mEditor->mTextView->GetSelectionEnd(&end);
    return (start + end) / 2;
}

Int32 ActionPopupWindow::GetVerticalLocalPosition(
    /* [in] */ Int32 line)
{
    Int32 result = 0;
    AutoPtr<ILayout> layout;
    mEditor->mTextView->GetLayout((ILayout**)&layout);
    if (layout) {
        layout->GetLineTop(line, &result);
    }

    Int32 h;
    IView::Probe(mContentView)->GetMeasuredHeight(&h);
    result += h;
    return result;
}

Int32 ActionPopupWindow::ClipVertically(
    /* [in] */ Int32 positionY)
{
    if (positionY < 0) {
        Int32 offset = GetTextOffset();
        AutoPtr<ILayout> layout;
        mEditor->mTextView->GetLayout((ILayout**)&layout);
        Int32 line, top, bottom, h;
        IView::Probe(mContentView)->GetMeasuredHeight(&h);
        layout->GetLineForOffset(offset, &line);
        layout->GetLineBottom(line, &bottom);
        layout->GetLineTop(line, &top);
        positionY += bottom - top;
        positionY += h;

        // Assumes insertion and selection handles share the same height
        AutoPtr<IContext> ctx;
        IView::Probe(mEditor->mTextView)->GetContext((IContext**)&ctx);
        AutoPtr<IDrawable> handle;
        ctx->GetDrawable(mEditor->mTextView->mTextSelectHandleRes, (IDrawable**)&handle);
        Int32 height;
        handle->GetIntrinsicHeight(&height);
        positionY += height;
    }

    return positionY;
}

//==============================================================================
//              HandleView
//==============================================================================

const Int32 HandleView::HISTORY_SIZE;
const Int32 HandleView::TOUCH_UP_FILTER_DELAY_AFTER;
const Int32 HandleView::TOUCH_UP_FILTER_DELAY_BEFORE;

HandleView::HandleView()
    : View()
    , mEditor(NULL)
    , mHotspotX(0)
    , mHorizontalGravity(0)
    , mPositionX(0)
    , mPositionY(0)
    , mIsDragging(FALSE)
    , mTouchToWindowOffsetX(0)
    , mTouchToWindowOffsetY(0)
    , mTouchOffsetY(0)
    , mIdealVerticalOffset(0)
    , mLastParentX(0)
    , mLastParentY(0)
    , mPreviousOffset(-1)
    , mPositionHasChanged(TRUE)
    , mMinSize(0)
    , mPreviousOffsetIndex(0)
    , mNumberPreviousOffsets(0)
{
    mPreviousOffsetsTimes = ArrayOf<Int64>::Alloc(HISTORY_SIZE);
    mPreviousOffsets = ArrayOf<Int32>::Alloc(HISTORY_SIZE);
}

ECode HandleView::constructor(
    /* [in] */ IDrawable* drawableLtr,
    /* [in] */ IDrawable* drawableRtl,
    /* [in] */ Editor* editor)
{
    AutoPtr<IContext> ctx;
    mEditor->mTextView->GetContext((IContext**)&ctx);
    View::constructor(ctx);
    mEditor = editor;
    CPopupWindow::New(ctx, NULL, R::attr::textSelectHandleWindowStyle, (IPopupWindow**)&mContainer);
    mContainer->SetSplitTouchEnabled(TRUE);
    mContainer->SetClippingEnabled(FALSE);
    mContainer->SetWindowLayoutType(IWindowManagerLayoutParams::TYPE_APPLICATION_SUB_PANEL);
    mContainer->SetContentView((IView*)this->Probe(EIID_IView));

    mDrawableLtr = drawableLtr;
    mDrawableRtl = drawableRtl;

    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::text_handle_min_size, &mMinSize);
    UpdateDrawable();
    assert(0);
    Int32 handleHeight;// = GetPreferredHeight();
    mTouchOffsetY = -0.3f * handleHeight;
    mIdealVerticalOffset = 0.7f * handleHeight;
    return NOERROR;
}

void HandleView::UpdateDrawable()
{
    Int32 offset = GetCurrentCursorOffset();
    AutoPtr<ILayout> layout;
    mEditor->mTextView->GetLayout((ILayout**)&layout);
    Boolean isRtlCharAtOffset;
    layout->IsRtlCharAt(offset, &isRtlCharAtOffset);
    mDrawable = isRtlCharAtOffset ? mDrawableRtl : mDrawableLtr;
    mHotspotX = GetHotspotX(mDrawable, isRtlCharAtOffset);
    mHorizontalGravity = GetHorizontalGravity(isRtlCharAtOffset);
}

void HandleView::StartTouchUpFilter(
    /* [in] */ Int32 offset)
{
    mNumberPreviousOffsets = 0;
    AddPositionToTouchUpFilter(offset);
}

void HandleView::AddPositionToTouchUpFilter(
    /* [in] */ Int32 offset)
{
    mPreviousOffsetIndex = (mPreviousOffsetIndex + 1) % HISTORY_SIZE;
    (*mPreviousOffsets)[mPreviousOffsetIndex] = offset;
    (*mPreviousOffsetsTimes)[mPreviousOffsetIndex] = SystemClock::GetUptimeMillis();
    mNumberPreviousOffsets++;
}

void HandleView::FilterOnTouchUp()
{
    Int64 now = SystemClock::GetUptimeMillis();
    Int32 i = 0;
    Int32 index = mPreviousOffsetIndex;
    Int32 iMax = Elastos::Core::Math::Min(mNumberPreviousOffsets, HISTORY_SIZE);
    while (i < iMax && (now - (*mPreviousOffsetsTimes)[index]) < TOUCH_UP_FILTER_DELAY_AFTER) {
        i++;
        index = (mPreviousOffsetIndex - i + HISTORY_SIZE) % HISTORY_SIZE;
    }

    if (i > 0 && i < iMax &&
            (now - (*mPreviousOffsetsTimes)[index]) > TOUCH_UP_FILTER_DELAY_BEFORE) {
        PositionAtCursorOffset((*mPreviousOffsets)[index], FALSE);
    }
}

Boolean HandleView::OffsetHasBeenChanged()
{
    return mNumberPreviousOffsets > 1;
}

void HandleView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    SetMeasuredDimension(GetPreferredWidth(), GetPreferredHeight());
}

Int32 HandleView::GetPreferredWidth()
{
    Int32 width;
    mDrawable->GetIntrinsicWidth(&width);
    return Elastos::Core::Math::Max(width, mMinSize);
}

Int32 HandleView::GetPreferredHeight()
{
    Int32 height;
    mDrawable->GetIntrinsicHeight(&height);
    return Elastos::Core::Math::Max(height, mMinSize);
}

void HandleView::Show()
{
    if (IsShowing()) return;

    AutoPtr<PositionListener> listener = mEditor->GetPositionListener();
    listener->AddSubscriber(this, TRUE);

    // Make sure the offset is always considered new, even when focusing at same position
    mPreviousOffset = -1;
    PositionAtCursorOffset(GetCurrentCursorOffset(), FALSE);

    HideActionPopupWindow();
}

void HandleView::Dismiss()
{
    mIsDragging = FALSE;
    if (IsShowing()) {
        mContainer->Dismiss();
    }
    OnDetached();
}

void HandleView::Hide()
{
    Dismiss();

    AutoPtr<PositionListener> listener = mEditor->GetPositionListener();
    listener->RemoveSubscriber(this);
}

void HandleView::ShowActionPopupWindow(
    /* [in] */ Int32 delay)
{
    if (mActionPopupWindow == NULL) {
        mActionPopupWindow = new ActionPopupWindow(mEditor);
        mActionPopupWindow->constructor();
    }

    if (mActionPopupShower == NULL) {
        mActionPopupShower = new ActionPopupShowerRunnable(mActionPopupWindow.Get());
    }
    else {
        Boolean res;
        mEditor->mTextView->RemoveCallbacks(mActionPopupShower, &res);
    }

    Boolean res;
    mEditor->mTextView->PostDelayed(mActionPopupShower, delay, &res);
}

void HandleView::HideActionPopupWindow()
{
    if (mActionPopupShower != NULL) {
        Boolean res;
        mEditor->mTextView->RemoveCallbacks(mActionPopupShower, &res);
    }
    if (mActionPopupWindow != NULL) {
        mActionPopupWindow->Hide();
    }
}

Boolean HandleView::IsShowing()
{
    Boolean isShowing;
    mContainer->IsShowing(&isShowing);
    return isShowing;
}

Boolean HandleView::IsVisible()
{
    // Always show a dragging handle.
     if (mIsDragging) {
         return TRUE;
     }

     if (mEditor->mTextView->IsInBatchEditMode()) {
         return FALSE;
     }

     return mEditor->IsPositionVisible(mPositionX + mHotspotX, mPositionY);
}

void HandleView::PositionAtCursorOffset(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean parentScrolled)
{
    // A HandleView relies on the layout, which may be nulled by external methods
    AutoPtr<ILayout> layout;
    mEditor->mTextView->GetLayout((ILayout**)&layout);
    if (layout == NULL) {
        // Will update controllers' state, hiding them and stopping selection mode if needed
        mEditor->PrepareCursorControllers();
        return;
    }

    Boolean offsetChanged = offset != mPreviousOffset;
    if (offsetChanged || parentScrolled) {
        if (offsetChanged) {
            UpdateSelection(offset);
            AddPositionToTouchUpFilter(offset);
        }
        Int32 line;
        layout->GetLineForOffset(offset, &line);

        Float hor;
        layout->GetPrimaryHorizontal(offset, &hor);
        Int32 bottom;
        layout->GetLineBottom(line, &bottom);
        mPositionX = (Int32) (hor - 0.5f - mHotspotX - GetHorizontalOffset() + GetCursorOffset());
        mPositionY = bottom;

        // Take TextView's padding and scroll into account.
        mPositionX += mEditor->mTextView->ViewportToContentHorizontalOffset();
        mPositionY += mEditor->mTextView->ViewportToContentVerticalOffset();

        mPreviousOffset = offset;
        mPositionHasChanged = TRUE;
    }
}

ECode HandleView::UpdatePosition(
    /* [in] */ Int32 parentPositionX,
    /* [in] */ Int32 parentPositionY,
    /* [in] */ Boolean parentPositionChanged,
    /* [in] */ Boolean parentScrolled)
{
    PositionAtCursorOffset(GetCurrentCursorOffset(), parentScrolled);
    if (parentPositionChanged || mPositionHasChanged) {
        if (mIsDragging) {
            // Update touchToWindow offset in case of parent scrolling while dragging
            if (parentPositionX != mLastParentX || parentPositionY != mLastParentY) {
                mTouchToWindowOffsetX += parentPositionX - mLastParentX;
                mTouchToWindowOffsetY += parentPositionY - mLastParentY;
                mLastParentX = parentPositionX;
                mLastParentY = parentPositionY;
            }

            OnHandleMoved();
        }

        if (IsVisible()) {
            Int32 positionX = parentPositionX + mPositionX;
            Int32 positionY = parentPositionY + mPositionY;
            if (IsShowing()) {
                mContainer->Update(positionX, positionY, -1, -1);
            } else {
                mContainer->ShowAtLocation(
                        (IView*)mEditor->mTextView->Probe(EIID_IView),
                        IGravity::NO_GRAVITY,
                        positionX, positionY);
            }
        } else {
            if (IsShowing()) {
                Dismiss();
            }
        }

        mPositionHasChanged = FALSE;
    }
    return NOERROR;
}

void HandleView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    Int32 drawWidth, drawHeight;
    mDrawable->GetIntrinsicWidth(&drawWidth);
    Int32 left = GetHorizontalOffset();
    mDrawable->GetIntrinsicHeight(&drawHeight);
    mDrawable->SetBounds(left, 0, left + drawWidth, drawHeight);
    mDrawable->Draw(canvas);
}

Int32 HandleView::GetHorizontalOffset()
{
    Int32 width = GetPreferredWidth();
    Int32 drawWidth;
    mDrawable->GetIntrinsicWidth(&drawWidth);
    Int32 left;
    switch (mHorizontalGravity) {
        case IGravity::LEFT:
            left = 0;
            break;
        default:
        case IGravity::CENTER:
            left = (width - drawWidth) / 2;
            break;
        case IGravity::RIGHT:
            left = width - drawWidth;
            break;
    }
    return left;
}

Int32 HandleView::GetCursorOffset()
{
    return 0;
}

ECode HandleView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    Int32 mask;
    event->GetActionMasked(&mask);
    Float rawX, rawY;
    event->GetRawX(&rawX);
    event->GetRawY(&rawY);
    switch (mask) {
        case IMotionEvent::ACTION_DOWN: {
            StartTouchUpFilter(GetCurrentCursorOffset());
            mTouchToWindowOffsetX = rawX - mPositionX;
            mTouchToWindowOffsetY = rawY - mPositionY;

            AutoPtr<PositionListener> positionListener = mEditor->GetPositionListener();
            mLastParentX = positionListener->GetPositionX();
            mLastParentY = positionListener->GetPositionY();
            mIsDragging = true;
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            // Vertical hysteresis: vertical down movement tends to snap to ideal offset
            Float previousVerticalOffset = mTouchToWindowOffsetY - mLastParentY;
            Float currentVerticalOffset = rawY - mPositionY - mLastParentY;
            Float newVerticalOffset;
            if (previousVerticalOffset < mIdealVerticalOffset) {
                newVerticalOffset = Elastos::Core::Math::Min(currentVerticalOffset, mIdealVerticalOffset);
                newVerticalOffset = Elastos::Core::Math::Max(newVerticalOffset, previousVerticalOffset);
            } else {
                newVerticalOffset = Elastos::Core::Math::Max(currentVerticalOffset, mIdealVerticalOffset);
                newVerticalOffset = Elastos::Core::Math::Min(newVerticalOffset, previousVerticalOffset);
            }
            mTouchToWindowOffsetY = newVerticalOffset + mLastParentY;

            Float newPosX = rawX - mTouchToWindowOffsetX + mHotspotX;
            Float newPosY = rawY - mTouchToWindowOffsetY + mTouchOffsetY;

            UpdatePosition(newPosX, newPosY);
            break;
        }

        case IMotionEvent::ACTION_UP:
            FilterOnTouchUp();
            mIsDragging = FALSE;
            break;

        case IMotionEvent::ACTION_CANCEL:
            mIsDragging = FALSE;
            break;
    }
    return TRUE;
}

Boolean HandleView::IsDragging()
{
    return mIsDragging;
}

void HandleView::OnHandleMoved()
{
    HideActionPopupWindow();
}

void HandleView::OnDetached()
{
    HideActionPopupWindow();
}

//==============================================================================
//              InsertionHandleView
//==============================================================================

const Int32 InsertionHandleView::DELAY_BEFORE_HANDLE_FADES_OUT = 4000;
const Int32 InsertionHandleView::RECENT_CUT_COPY_DURATION = 15 * 1000; // seconds

InsertionHandleView::InsertionHandleView()
    : HandleView()
    , mDownPositionX(0)
    , mDownPositionY(0)
{
}

ECode InsertionHandleView::constructor(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Editor* editor)
{
    HandleView::constructor(drawable, drawable, editor);
    return NOERROR;
}

void InsertionHandleView::Show()
{
    HandleView::Show();

    Int64 durationSinceCutOrCopy =
            SystemClock::GetUptimeMillis() - TextView::LAST_CUT_OR_COPY_TIME;
    if (durationSinceCutOrCopy < RECENT_CUT_COPY_DURATION) {
        ShowActionPopupWindow(0);
    }

    HideAfterDelay();
}

void InsertionHandleView::ShowWithActionPopup()
{
    Show();
    ShowActionPopupWindow(0);
}

void InsertionHandleView::HideAfterDelay()
{
    if (mHider == NULL) {
        mHider = new InsertionHandleViewHiderRunnable(this);
    } else {
        RemoveHiderCallback();
    }
    Boolean res;
    mEditor->mTextView->PostDelayed(mHider, DELAY_BEFORE_HANDLE_FADES_OUT, &res);
}

void InsertionHandleView::RemoveHiderCallback()
{
    if (mHider != NULL) {
        Boolean res;
        mEditor->mTextView->RemoveCallbacks(mHider, &res);
    }
}

Int32 InsertionHandleView::GetHotspotX(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Boolean isRtlRun)
{
    assert(drawable);
    Int32 width;
    drawable->GetIntrinsicWidth(&width);
    return width / 2;
}

Int32 InsertionHandleView::GetHorizontalGravity(
    /* [in] */ Boolean isRtlRun)
{
    return IGravity::CENTER_HORIZONTAL;
}

Int32 InsertionHandleView::GetCursorOffset()
{
    Int32 offset = HandleView::GetCursorOffset();
    AutoPtr<IDrawable> cursor = mEditor->mCursorCount > 0 ? (*mEditor->mCursorDrawable)[0] : NULL;
    if (cursor != NULL) {
        Boolean isPadding;
        cursor->GetPadding(mEditor->mTempRect, &isPadding);
        Int32 width;
        cursor->GetIntrinsicWidth(&width);
        CRect* temp = (CRect*)mEditor->mTempRect.Get();
        offset += (width - temp->mLeft - temp->mRight) / 2;
    }
    return offset;
}

ECode InsertionHandleView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    HandleView::OnTouchEvent(ev, result);
    AutoPtr<IContext> context;
    mEditor->mTextView->GetContext((IContext**)&context);
    Int32 mask;
    ev->GetActionMasked(&mask);
    Float rawX, rawY;
    ev->GetRawX(&rawX);
    ev->GetRawY(&rawY);
    switch (mask) {
        case IMotionEvent::ACTION_DOWN:
            mDownPositionX = rawX;
            mDownPositionY = rawY;
            break;

        case IMotionEvent::ACTION_UP:
            if (!OffsetHasBeenChanged()) {
                Float deltaX = mDownPositionX - rawX;
                Float deltaY = mDownPositionY - rawY;
                Float distanceSquared = deltaX * deltaX + deltaY * deltaY;

                AutoPtr<IViewConfigurationHelper> helper;
                CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
                AutoPtr<IViewConfiguration> viewConfiguration;
                helper->Get(context, (IViewConfiguration**)&viewConfiguration);
                Int32 touchSlop;
                viewConfiguration->GetScaledTouchSlop(&touchSlop);

                if (distanceSquared < touchSlop * touchSlop) {
                    if (mActionPopupWindow != NULL && mActionPopupWindow->IsShowing()) {
                        // Tapping on the handle dismisses the displayed action popup
                        mActionPopupWindow->Hide();
                    } else {
                        ShowWithActionPopup();
                    }
                }
            }
            HideAfterDelay();
            break;

        case IMotionEvent::ACTION_CANCEL:
            HideAfterDelay();
            break;

        default:
            break;
    }

    return NOERROR;
}


Int32 InsertionHandleView::GetCurrentCursorOffset()
{
    Int32 result;
    mEditor->mTextView->GetSelectionStart(&result);
    return result;
}

void InsertionHandleView::UpdateSelection(
    /* [in] */ Int32 offset)
{
    AutoPtr<ICharSequence> text;
    mEditor->mTextView->GetText((ICharSequence**)&text);
    ISpannable* spannable = ISpannable::Probe(text);
    Selection::SetSelection(spannable, offset);
}

void InsertionHandleView::UpdatePosition(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Int32 position;
    mEditor->mTextView->GetOffsetForPosition(x, y, &position);
    PositionAtCursorOffset(position, FALSE);
}

void InsertionHandleView::OnHandleMoved()
{
    HandleView::OnHandleMoved();
    RemoveHiderCallback();
}

void InsertionHandleView::OnDetached()
{
    HandleView::OnDetached();
    RemoveHiderCallback();
}

//==============================================================================
//              SelectionStartHandleView
//==============================================================================

SelectionStartHandleView::SelectionStartHandleView()
    : HandleView()
{
}

ECode SelectionStartHandleView::constructor(
    /* [in] */ IDrawable* drawableLtr,
    /* [in] */ IDrawable* drawableRtl,
    /* [in] */ Editor* editor)
{
    return HandleView::constructor(drawableLtr, drawableRtl, editor);
}

Int32 SelectionStartHandleView::GetHotspotX(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Boolean isRtlRun)
{
    assert(drawable);
    Int32 width;
    drawable->GetIntrinsicWidth(&width);
    if (isRtlRun) {
        return width / 4;
    } else {
        return (width * 3) / 4;
    }
}

Int32 SelectionStartHandleView::GetHorizontalGravity(
    /* [in] */ Boolean isRtlRun)
{
    return isRtlRun ? IGravity::RIGHT : IGravity::LEFT;
}

Int32 SelectionStartHandleView::GetCurrentCursorOffset()
{
    Int32 result;
    mEditor->mTextView->GetSelectionStart(&result);
    return result;
}

void SelectionStartHandleView::UpdateSelection(
    /* [in] */ Int32 offset)
{
    AutoPtr<ICharSequence> text;
    mEditor->mTextView->GetText((ICharSequence**)&text);
    ISpannable* spannable = ISpannable::Probe(text);
    Int32 end;
    mEditor->mTextView->GetSelectionEnd(&end);
    Selection::SetSelection(spannable, offset, end);
    UpdateDrawable();
}

void SelectionStartHandleView::UpdatePosition(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Int32 offset, selectionEnd;
    mEditor->mTextView->GetOffsetForPosition(x, y, &offset);
    // Handles can not cross and selection is at least one character
    mEditor->mTextView->GetSelectionEnd(&selectionEnd);

    if (offset >= selectionEnd)
        offset = Elastos::Core::Math::Max(0, selectionEnd - 1);

    PositionAtCursorOffset(offset, FALSE);
}

AutoPtr<ActionPopupWindow> SelectionStartHandleView::GetActionPopupWindow()
{
    return mActionPopupWindow;
}

//==============================================================================
//              SelectionEndHandleView
//==============================================================================

SelectionEndHandleView::SelectionEndHandleView()
    : HandleView()
{
}

ECode SelectionEndHandleView::constructor(
    /* [in] */ IDrawable* drawableLtr,
    /* [in] */ IDrawable* drawableRtl,
    /* [in] */ Editor* editor)
{
    return HandleView::constructor(drawableLtr, drawableRtl, editor);
}

Int32 SelectionEndHandleView::GetHotspotX(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Boolean isRtlRun)
{
    assert(drawable);
    Int32 width;
    drawable->GetIntrinsicWidth(&width);
    if (isRtlRun) {
        return (width * 3) / 4;
    } else {
        return width / 4;
    }
}

Int32 SelectionEndHandleView::GetHorizontalGravity(
    /* [in] */ Boolean isRtlRun)
{
    return isRtlRun ? IGravity::LEFT : IGravity::RIGHT;
}

Int32 SelectionEndHandleView::GetCurrentCursorOffset()
{
    Int32 end;
    mEditor->mTextView->GetSelectionEnd(&end);
    return end;
}

void SelectionEndHandleView::UpdateSelection(
    /* [in] */ Int32 offset)
{
    AutoPtr<ICharSequence> text;
    mEditor->mTextView->GetText((ICharSequence**)&text);
    ISpannable* spannable = ISpannable::Probe(text);
    Int32 start;
    mEditor->mTextView->GetSelectionStart(&start);
    Selection::SetSelection(spannable, start, offset);
    UpdateDrawable();
}

void SelectionEndHandleView::UpdatePosition(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Int32 offset, selectionStart;
    mEditor->mTextView->GetOffsetForPosition(x, y, &offset);
    // Handles can not cross and selection is at least one character
    mEditor->mTextView->GetSelectionStart(&selectionStart);

    if (offset <= selectionStart) {
        Int32 length;
        mEditor->mTextView->GetLength(&length);
        offset = Elastos::Core::Math::Min(selectionStart + 1, length);
    }

    PositionAtCursorOffset(offset, FALSE);
}

void SelectionEndHandleView::SetActionPopupWindow(
    /* [in] */ ActionPopupWindow* actionPopupWindow)
{
    mActionPopupWindow = actionPopupWindow;
}

//==============================================================================
//              CursorController
//==============================================================================
CAR_INTERFACE_IMPL(CursorController, Object, IOnTouchModeChangeListener)

CursorController::CursorController(
    /* [in] */ Editor* editor)
    : mEditor(editor)
{
}

ECode CursorController::Show()
{
    assert(0 && "Error: Pure base methods CursorController::Show() is called.");
    return E_NOT_IMPLEMENTED;
}

ECode CursorController::Hide()
{
    assert(0 && "Error: Pure base methods CursorController::Hide() is called.");
    return E_NOT_IMPLEMENTED;
}

ECode CursorController::OnDetached()
{
    assert(0 && "Error: Pure base methods CursorController::OnDetached() is called.");
    return E_NOT_IMPLEMENTED;
}

ECode CursorController::OnTouchModeChanged(
    /* [in]*/ Boolean isInTouchMode)
{
    assert(0 && "Error: Pure base methods CursorController::OnTouchModeChanged() is called.");
    return E_NOT_IMPLEMENTED;
}

//==============================================================================
//              InsertionPointCursorController
//==============================================================================

InsertionPointCursorController::InsertionPointCursorController(
    /* [in] */ Editor* editor)
    : CursorController(editor)
{
}

ECode InsertionPointCursorController::Show()
{
    AutoPtr<InsertionHandleView> handle = GetHandle();
    handle->Show();
    return NOERROR;
}

void InsertionPointCursorController::ShowWithActionPopup()
{
    AutoPtr<InsertionHandleView> handle = GetHandle();
    handle->ShowWithActionPopup();
}

ECode InsertionPointCursorController::Hide()
{
    if (mHandle != NULL) {
        mHandle->Hide();
    }
    return NOERROR;
}

ECode InsertionPointCursorController::OnTouchModeChanged(
    /* [in] */ Boolean isInTouchMode)
{
    if (!isInTouchMode) {
        Hide();
    }
    return NOERROR;
}

AutoPtr<InsertionHandleView> InsertionPointCursorController::GetHandle()
{
    if (mEditor->mSelectHandleCenter == NULL) {
        AutoPtr<IContext> ctx;
        mEditor->mTextView->GetContext((IContext**)&ctx);
        ctx->GetDrawable(
                mEditor->mTextView->mTextSelectHandleRes,
                (IDrawable**)&mEditor->mSelectHandleCenter);
    }
    if (mHandle == NULL) {
        mHandle = new InsertionHandleView();
        mHandle->constructor(mEditor->mSelectHandleCenter, mEditor);
    }
    return mHandle;
}

ECode InsertionPointCursorController::OnDetached()
{
    AutoPtr<IViewTreeObserver> observer;
    mEditor->mTextView->GetViewTreeObserver((IViewTreeObserver**)&observer);
    observer->RemoveOnTouchModeChangeListener(this);

    if (mHandle != NULL) mHandle->OnDetached();
    return NOERROR;
}

//==============================================================================
//              SelectionModifierCursorController
//==============================================================================
const Int32 SelectionModifierCursorController::DELAY_BEFORE_REPLACE_ACTION = 200; // milliseconds

SelectionModifierCursorController::SelectionModifierCursorController(
    /* [in] */ Editor* editor)
    : CursorController(editor)
    , mMinTouchOffset(0)
    , mMaxTouchOffset(0)
    , mPreviousTapUpTime(0)
    , mDownPositionX(0.0)
    , mDownPositionY(0.0)
    , mGestureStayedInTapRegion(FALSE)
{
    ResetTouchOffsets();
}

ECode SelectionModifierCursorController::Show()
{
    if (mEditor->mTextView->IsInBatchEditMode()) {
        return NOERROR;
    }
    InitDrawables();
    InitHandles();
    mEditor->HideInsertionPointCursorController();
    return NOERROR;
}

void SelectionModifierCursorController::InitDrawables()
{
    AutoPtr<IContext> ctx;
    mEditor->mTextView->GetContext((IContext**)&ctx);
    if (mEditor->mSelectHandleLeft == NULL) {
        ctx->GetDrawable(
                mEditor->mTextView->mTextSelectHandleLeftRes,
                (IDrawable**)&mEditor->mSelectHandleLeft);
    }
    if (mEditor->mSelectHandleRight == NULL) {
        ctx->GetDrawable(
                mEditor->mTextView->mTextSelectHandleRightRes,
                (IDrawable**)&mEditor->mSelectHandleRight);
    }
}

void SelectionModifierCursorController::InitHandles()
{
    // Lazy object creation has to be done before updatePosition() is called.
    if (mStartHandle == NULL) {
        mStartHandle = new SelectionStartHandleView();
        mStartHandle->constructor(mEditor->mSelectHandleLeft, mEditor->mSelectHandleRight, mEditor);
    }
    if (mEndHandle == NULL) {
        mEndHandle = new SelectionEndHandleView();
        mEndHandle->constructor(mEditor->mSelectHandleRight, mEditor->mSelectHandleLeft, mEditor);
    }

    mStartHandle->Show();
    mEndHandle->Show();

    // Make sure both left and right handles share the same ActionPopupWindow (so that
    // moving any of the handles hides the action popup).
    mStartHandle->ShowActionPopupWindow(DELAY_BEFORE_REPLACE_ACTION);
    AutoPtr<ActionPopupWindow> popuoWindow = mStartHandle->GetActionPopupWindow();
    mEndHandle->SetActionPopupWindow(popuoWindow.Get());

    mEditor->HideInsertionPointCursorController();
}

ECode SelectionModifierCursorController::Hide()
{
    if (mStartHandle != NULL) mStartHandle->Hide();
    if (mEndHandle != NULL) mEndHandle->Hide();
    return NOERROR;
}

void SelectionModifierCursorController::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    AutoPtr<IContext> context;
    mEditor->mTextView->GetContext((IContext**)&context);

    // This is done even when the View does not have focus, so that Int64 presses can start
    // selection and tap can move cursor from this tap position.
    Int32 mask;
    event->GetActionMasked(&mask);
    Float x, y;
    event->GetX(&x);
    event->GetY(&y);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);

    switch (mask) {
        case IMotionEvent::ACTION_DOWN:
            // Remember finger down position, to be able to start selection from there
            mEditor->mTextView->GetOffsetForPosition(x, y, &mMinTouchOffset);
            mEditor->mTextView->GetOffsetForPosition(x, y, &mMaxTouchOffset);

            // Double tap detection
            if (mGestureStayedInTapRegion) {
                Int64 duration = SystemClock::GetUptimeMillis() - mPreviousTapUpTime;
                Int32 timeout;
                helper->GetDoubleTapTimeout(&timeout);
                if (duration <= timeout) {
                    Float deltaX = x - mDownPositionX;
                    Float deltaY = y - mDownPositionY;
                    Float distanceSquared = deltaX * deltaX + deltaY * deltaY;

                    AutoPtr<IViewConfiguration> viewConfiguration;
                    helper->Get(context, (IViewConfiguration**)&viewConfiguration);
                    Int32 doubleTapSlop;
                    viewConfiguration->GetScaledDoubleTapSlop(&doubleTapSlop);
                    Boolean stayedInArea = distanceSquared < doubleTapSlop * doubleTapSlop;

                    if (stayedInArea && mEditor->IsPositionOnText(x, y)) {
                        mEditor->StartSelectionActionMode();
                        mEditor->mDiscardNextActionUp = TRUE;
                    }
                }
            }

            mDownPositionX = x;
            mDownPositionY = y;
            mGestureStayedInTapRegion = TRUE;
            break;

        case IMotionEvent::ACTION_POINTER_DOWN:
        case IMotionEvent::ACTION_POINTER_UP: {
                // Handle multi-point gestures. Keep min and max offset positions.
                // Only activated for devices that correctly handle multi-touch.
                AutoPtr<IPackageManager> packageManager;
                context->GetPackageManager((IPackageManager**)&packageManager);
                Boolean hasFeature;
                packageManager->HasSystemFeature(
                        IPackageManager::FEATURE_TOUCHSCREEN_MULTITOUCH_DISTINCT, &hasFeature);
                if (hasFeature) {
                    UpdateMinAndMaxOffsets(event);
                }
            }
            break;

        case IMotionEvent::ACTION_MOVE:
            if (mGestureStayedInTapRegion) {
                Float deltaX = x- mDownPositionX;
                Float deltaY = y - mDownPositionY;
                Float distanceSquared = deltaX * deltaX + deltaY * deltaY;

                AutoPtr<IViewConfiguration> viewConfiguration;
                helper->Get(context, (IViewConfiguration**)&viewConfiguration);
                Int32 doubleTapTouchSlop;
                viewConfiguration->GetScaledDoubleTapTouchSlop(&doubleTapTouchSlop);

                if (distanceSquared > doubleTapTouchSlop * doubleTapTouchSlop) {
                    mGestureStayedInTapRegion = FALSE;
                }
            }
            break;

        case IMotionEvent::ACTION_UP:
            mPreviousTapUpTime = SystemClock::GetUptimeMillis();
            break;
    }
}

/**
 * @param event
 */
void SelectionModifierCursorController::UpdateMinAndMaxOffsets(
    /* [in] */ IMotionEvent* event)
{
    Int32 pointerCount;
    event->GetPointerCount(&pointerCount);
    for (Int32 index = 0; index < pointerCount; index++) {
        Float x, y;
        event->GetX(index, &x);
        event->GetY(index, &y);
        Int32 offset;
        mEditor->mTextView->GetOffsetForPosition(x, y, &offset);
        if (offset < mMinTouchOffset) mMinTouchOffset = offset;
        if (offset > mMaxTouchOffset) mMaxTouchOffset = offset;
    }
}

Int32 SelectionModifierCursorController::GetMinTouchOffset()
{
    return mMinTouchOffset;
}

Int32 SelectionModifierCursorController::GetMaxTouchOffset()
{
    return mMaxTouchOffset;
}

void SelectionModifierCursorController::ResetTouchOffsets()
{
    mMinTouchOffset = mMaxTouchOffset = -1;
}

/**
 * @return TRUE iff this controller is currently used to move the selection start.
 */
Boolean SelectionModifierCursorController::IsSelectionStartDragged()
{
    return mStartHandle != NULL && mStartHandle->IsDragging();
}

ECode SelectionModifierCursorController::OnTouchModeChanged(
    /* [in] */ Boolean isInTouchMode)
{
    if (!isInTouchMode) {
        Hide();
    }
    return NOERROR;
}

ECode SelectionModifierCursorController::OnDetached()
{
    AutoPtr<IViewTreeObserver> observer;
    mEditor->mTextView->GetViewTreeObserver((IViewTreeObserver**)&observer);
    observer->RemoveOnTouchModeChangeListener(this);

    if (mStartHandle != NULL) mStartHandle->OnDetached();
    if (mEndHandle != NULL) mEndHandle->OnDetached();
    return NOERROR;
}

//==============================================================================
//              PositionListener
//==============================================================================

const Int32 PositionListener::MAXIMUM_NUMBER_OF_LISTENERS;

CAR_INTERFACE_IMPL(PositionListener, Object, IOnPreDrawListener);

PositionListener::PositionListener(
    /* [in] */ Editor* editor)
    : mPositionHasChanged(TRUE)
    , mPositionX(0)
    , mPositionY(0)
    , mNumberOfListeners(0)
    , mScrollHasChanged(0)
    , mEditor(editor)
{
    mPositionListeners = ArrayOf<ITextViewPositionListener*>::Alloc(MAXIMUM_NUMBER_OF_LISTENERS);
    mCanMove = ArrayOf<Boolean>::Alloc(MAXIMUM_NUMBER_OF_LISTENERS);
    mTempCoords = ArrayOf<Int32>::Alloc(2);
}

void PositionListener::AddSubscriber(
    /* [in] */ ITextViewPositionListener* positionListener,
    /* [in] */ Boolean canMove)
{
    if (mNumberOfListeners == 0) {
        UpdatePosition();
        AutoPtr<IViewTreeObserver> observer;
        mEditor->mTextView->GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->AddOnPreDrawListener(this);
    }

    Int32 emptySlotIndex = -1;
    for (Int32 i = 0; i < MAXIMUM_NUMBER_OF_LISTENERS; i++) {
        AutoPtr<ITextViewPositionListener> listener = (*mPositionListeners)[i];
        if (listener.Get() == positionListener) {
            return;
        } else if (emptySlotIndex < 0 && listener == NULL) {
            emptySlotIndex = i;
        }
    }

    mPositionListeners->Set(emptySlotIndex, positionListener);
    (*mCanMove)[emptySlotIndex] = canMove;
    mNumberOfListeners++;
}

void PositionListener::RemoveSubscriber(
    /* [in] */ ITextViewPositionListener* positionListener)
{
    for (Int32 i = 0; i < MAXIMUM_NUMBER_OF_LISTENERS; i++) {
        if ((*mPositionListeners)[i] == positionListener) {
            mPositionListeners->Set(i, NULL);
            mNumberOfListeners--;
            break;
        }
    }

    if (mNumberOfListeners == 0) {
        AutoPtr<IViewTreeObserver> observer;
        mEditor->mTextView->GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->RemoveOnPreDrawListener(this);
    }
}

Int32 PositionListener::GetPositionX()
{
    return mPositionX;
}

Int32 PositionListener::GetPositionY()
{
    return mPositionY;
}

ECode PositionListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    UpdatePosition();

    for (Int32 i = 0; i < MAXIMUM_NUMBER_OF_LISTENERS; i++) {
        if (mPositionHasChanged || mScrollHasChanged || (*mCanMove)[i]) {
            AutoPtr<ITextViewPositionListener> positionListener = (*mPositionListeners)[i];
            if (positionListener != NULL) {
                positionListener->UpdatePosition(mPositionX, mPositionY,
                        mPositionHasChanged, mScrollHasChanged);
            }
        }
    }

    mScrollHasChanged = FALSE;
    *result = TRUE;
    return NOERROR;
}

void PositionListener::UpdatePosition()
{
    mEditor->mTextView->GetLocationInWindow(mTempCoords);

    mPositionHasChanged = (*mTempCoords)[0] != mPositionX || (*mTempCoords)[1] != mPositionY;

    mPositionX = (*mTempCoords)[0];
    mPositionY = (*mTempCoords)[1];
}

void PositionListener::OnScrollChanged()
{
    mScrollHasChanged = TRUE;
}

//==============================================================================
//              CorrectionHighlighter
//==============================================================================

const Int32 CorrectionHighlighter::FADE_OUT_DURATION;

CorrectionHighlighter::CorrectionHighlighter(
    /* [in] */ Editor* editor)
    : mStart(0)
    , mEnd(0)
    , mFadingStartTime(0)
    , mEditor(editor)
{
    CPath::New((IPath**)&mPath);
    CPaint::New((IPaint**)&mPaint);

    AutoPtr<IResources> resources;
    mEditor->mTextView->GetResources((IResources**)&resources);
    AutoPtr<ICompatibilityInfo> info;
    resources->GetCompatibilityInfo((ICompatibilityInfo**)&info);
    Float scale;
    info->GetApplicationScale(&scale);
    mPaint->SetCompatibilityScaling(scale);
    mPaint->SetStyle(Elastos::Droid::Graphics::PaintStyle_FILL);
}

void CorrectionHighlighter::Highlight(
    /* [in] */ ICorrectionInfo* info)
{
    assert(info);
    info->GetOffset(&mStart);
    AutoPtr<ICharSequence> next;
    info->GetNewText((ICharSequence**)&next);
    Int32 length;
    next->GetLength(&length);
    mEnd = mStart + length;
    mFadingStartTime = SystemClock::GetUptimeMillis();

    if (mStart < 0 || mEnd < 0) {
        StopAnimation();
    }
}

void CorrectionHighlighter::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 cursorOffsetVertical)
{
    assert(canvas);
    if (UpdatePath() && UpdatePaint()) {
        if (cursorOffsetVertical != 0) {
            canvas->Translate(0, cursorOffsetVertical);
        }

        canvas->DrawPath(mPath, mPaint);

        if (cursorOffsetVertical != 0) {
            canvas->Translate(0, -cursorOffsetVertical);
        }
        Invalidate(TRUE); // TODO invalidate cursor region only
    } else {
        StopAnimation();
        Invalidate(FALSE); // TODO invalidate cursor region only
    }
}

Boolean CorrectionHighlighter::UpdatePaint()
{
    Int64 duration = SystemClock::GetUptimeMillis() - mFadingStartTime;
    if (duration > FADE_OUT_DURATION) return FALSE;

    Int32 highlightColorAlpha = Color::Alpha(mEditor->mTextView->mHighlightColor);

    Float coef = 1.0f - (Float) duration / FADE_OUT_DURATION;
    Int32 color = (mEditor->mTextView->mHighlightColor & 0x00FFFFFF) +
            ((Int32) (highlightColorAlpha * coef) << 24);
    mPaint->SetColor(color);
    return TRUE;
}

Boolean CorrectionHighlighter::UpdatePath()
{
    AutoPtr<ILayout> layout;
    mEditor->mTextView->GetLayout((ILayout**)&layout);
    if (layout == NULL) return FALSE;

    // Update in case text is edited while the animation is run
    Int32 length;
    mEditor->mTextView->GetLength(&length);
    Int32 start = Elastos::Core::Math::Min(length, mStart);
    Int32 end = Elastos::Core::Math::Min(length, mEnd);

    mPath->Reset();
    layout->GetSelectionPath(start, end, mPath);
    return TRUE;
}

void CorrectionHighlighter::Invalidate(
    /* [in] */ Boolean delayed)
{
    AutoPtr<ILayout> layout;
    mEditor->mTextView->GetLayout((ILayout**)&layout);
    if (layout == NULL) return;

    if (mTempRectF == NULL) {
        CRectF::New((IRectF**)&mTempRectF);
    }
    mPath->ComputeBounds(mTempRectF, FALSE);

    Int32 left, top;
    mEditor->mTextView->GetCompoundPaddingLeft(&left);
    mEditor->mTextView->GetExtendedPaddingTop(&top);
    top += mEditor->mTextView->GetVerticalOffset(TRUE);

    Float l, t, r, b;
    mTempRectF->Get(&l, &t, &r, &b);
    if (delayed) {
        mEditor->mTextView->PostInvalidateOnAnimation(
                left + (Int32) l, top + (Int32) t,
                left + (Int32) r, top + (Int32) b);
    } else {
        mEditor->mTextView->PostInvalidate(
                (Int32) l, (Int32) t, (Int32) r, (Int32) b);
    }
}

void CorrectionHighlighter::StopAnimation()
{
    mEditor->mCorrectionHighlighter = NULL;
}

//==============================================================================
//              ErrorPopup
//==============================================================================
ErrorPopup::ErrorPopup(
    /* [in]*/ TextView* textView,
    /* [in]*/ Int32 width,
    /* [in]*/ Int32 height)
    : PopupWindow()
    , mAbove(0)
    , mView(textView)
    , mPopupInlineErrorBackgroundId(0)
    , mPopupInlineErrorAboveBackgroundId(0)
{
    // Make sure the TextView has a background set as it will be used the first time it is
    // shown and positionned. Initialized with below background, which should have
    // dimensions identical to the above version for this to work (and is more likely).
    PopupWindow::constructor(IView::Probe(textView), width, height);
    mPopupInlineErrorBackgroundId = GetResourceId(mPopupInlineErrorBackgroundId,
            R::styleable::Theme_errorMessageBackground);
    mView->SetBackgroundResource(mPopupInlineErrorBackgroundId);
}

void ErrorPopup::FixDirection(
    /* [in]*/ Boolean above)
{
    mAbove = above;

    if (above) {
        mPopupInlineErrorAboveBackgroundId =
            GetResourceId(mPopupInlineErrorAboveBackgroundId,
                    R::styleable::Theme_errorMessageAboveBackground);
    } else {
        mPopupInlineErrorBackgroundId = GetResourceId(mPopupInlineErrorBackgroundId,
                R::styleable::Theme_errorMessageBackground);
    }

    mView->SetBackgroundResource(above ? mPopupInlineErrorAboveBackgroundId :
        mPopupInlineErrorBackgroundId);
}

Int32 ErrorPopup::GetResourceId(
    /* [in]*/ Int32 currentId,
    /* [in]*/ Int32 index)
{
    if (currentId == 0) {
        AutoPtr<IContext> context;
        mView->GetContext((IContext**)&context);
        AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                const_cast<Int32 *>(R::styleable::Theme),
                ArraySize(R::styleable::Theme));
        AutoPtr<ITypedArray> styledAttributes;
        context->ObtainStyledAttributes(attrIds, (ITypedArray**)&styledAttributes);

        styledAttributes->GetResourceId(index, 0, &currentId);
        styledAttributes->Recycle();
    }
    return currentId;
}

ECode ErrorPopup::Update(
    /* [in]*/ Int32 x,
    /* [in]*/ Int32 y,
    /* [in]*/ Int32 w,
    /* [in]*/ Int32 h,
    /* [in]*/ Boolean force)
{
    PopupWindow::Update(x, y, w, h, force);

    Boolean above;
    IsAboveAnchor(&above);
    if (above != mAbove) {
        FixDirection(above);
    }
    return NOERROR;
}

//==============================================================================
//              Blink
//==============================================================================

Blink::Blink(
    /* [in]*/ Editor* editor)
    : mCancelled(FALSE)
    , mEditor(editor)
{
}

Blink::~Blink()
{
}

ECode Blink::Run()
{
    if (mCancelled) {
       return NOERROR;
    }

    RemoveCallbacks(this);

    if (mEditor->ShouldBlink()) {
        AutoPtr<ILayout> layout;
        mEditor->mTextView->GetLayout((ILayout**)&layout);
        if (layout != NULL) {
            mEditor->mTextView->InvalidateCursorPath();
        }

        Boolean result = FALSE;
        PostAtTime(this, SystemClock::GetUptimeMillis() + Editor::BLINK, &result);
    }

    return NOERROR;
}

void Blink::Cancel()
{
    if (!mCancelled) {
        RemoveCallbacks(this);
        mCancelled = TRUE;
    }
}

void Blink::Uncancel()
{
    mCancelled = FALSE;
}

//==============================================================================
//              DragLocalState
//==============================================================================

CAR_INTERFACE_IMPL(DragLocalState, Object, IDragLocalState)

DragLocalState::DragLocalState(
    /* [in] */ TextView* sourceTextView,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
    : mSourceTextView(sourceTextView)
    , mStart(start)
    , mEnd(end)
{
}

//==============================================================================
//              Editor
//==============================================================================

CAR_INTERFACE_IMPL(Editor, Object, IEditor)

const String Editor::TAG("Editor");
const Boolean Editor::DEBUG_UNDO = FALSE;
const Int32 Editor::BLINK;

AutoPtr<ArrayOf<Float> > Editor::TEMP_POSITION = ArrayOf<Float>::Alloc(2);
const Int32 Editor::DRAG_SHADOW_MAX_TEXT_LENGTH;
const Int32 Editor::EXTRACT_NOTHING;
const Int32 Editor::EXTRACT_UNKNOWN;

Editor::Editor(
    /* [in] */ TextView* textView)
    : mInsertionControllerEnabled(FALSE)
    , mSelectionControllerEnabled(FALSE)
    , mFrozenWithFocus(FALSE)
    , mSelectionMoved(FALSE)
    , mTouchFocusSelected(FALSE)
    , mInputType(IInputType::TYPE_NULL)
    , mDiscardNextActionUp(FALSE)
    , mIgnoreActionUpEvent(FALSE)
    , mShowCursor(0)
    , mCursorVisible(TRUE)
    , mSelectAllOnFocus(FALSE)
    , mTextIsSelectable(FALSE)
    , mErrorWasChanged(FALSE)
    , mErrorPopup(NULL)
    , mShowErrorAfterAttach(FALSE)
    , mInBatchEditControllers(FALSE)
    , mShowSoftInputOnFocus(TRUE)
    , mPreserveDetachedSelection(FALSE)
    , mTemporaryDetach(FALSE)
    , mCursorCount(0)
    , mLastDownPositionX(0.0)
    , mLastDownPositionY(0.0)
    , mCreatedWithASelection(FALSE)
    , mTextView(textView)
{
    mCursorDrawable = ArrayOf<IDrawable *>::Alloc(2);
}

Editor::~Editor()
{
}

ECode Editor::OnAttachedToWindow()
{
    if (mShowErrorAfterAttach) {
        ShowError();
        mShowErrorAfterAttach = FALSE;
    }
    mTemporaryDetach = FALSE;

    AutoPtr<IViewTreeObserver> observer;
    mTextView->GetViewTreeObserver((IViewTreeObserver**)&observer);
    // No need to create the controller.
    // The get method will add the listener on controller creation.
    if (mInsertionPointCursorController != NULL) {
        observer->AddOnTouchModeChangeListener(mInsertionPointCursorController);
    }
    if (mSelectionModifierCursorController != NULL) {
        mSelectionModifierCursorController->ResetTouchOffsets();
        observer->AddOnTouchModeChangeListener(mSelectionModifierCursorController);
    }

    Int32 length;
    mTextView->GetLength(&length);
    UpdateSpellCheckSpans(0, length, TRUE);
    Boolean hasTransientState;
    ;
    Int32 start, end;
    if ((IView::Probe(mTextView)->HasTransientState(&hasTransientState), hasTransientState) &&
        (mTextView->GetSelectionStart(&start), start)
        != (mTextView->GetSelectionEnd(&end), end)) {
        // Since transient state is reference counted make sure it stays matched
        // with our own calls to it for managing selection.
        // The action mode callback will set this back again when/if the action mode starts.
        mTextView->SetHasTransientState(FALSE);

        // We had an active selection from before, start the selection mode.
        StartSelectionActionMode();
    }

    GetPositionListener()->AddSubscriber(mCursorAnchorInfoNotifier, TRUE);

    return NOERROR;
}

ECode Editor::OnDetachedFromWindow()
{
    GetPositionListener()->RemoveSubscriber(mCursorAnchorInfoNotifier);

    if (mError != NULL) {
        HideError();
    }

    if (mBlink != NULL) {
        mBlink->RemoveCallbacks(mBlink);
    }

    if (mInsertionPointCursorController != NULL) {
        mInsertionPointCursorController->OnDetached();
    }

    if (mSelectionModifierCursorController != NULL) {
        mSelectionModifierCursorController->OnDetached();
    }

    if (mShowSuggestionRunnable != NULL) {
        Boolean isRemoved;
        mTextView->RemoveCallbacks(mShowSuggestionRunnable, &isRemoved);
    }
    DestroyDisplayListsData();

    if (mSpellChecker != NULL) {
        mSpellChecker->CloseSession();
        // Forces the creation of a new SpellChecker next time this window is created.
        // Will handle the cases where the settings has been changed in the meantime.
        mSpellChecker = NULL;
    }

    mPreserveDetachedSelection = TRUE;
    HideControllers();
    mPreserveDetachedSelection = FALSE;
    mTemporaryDetach = FALSE;
    return NOERROR;
}

void Editor::DestroyDisplayListsData()
{
    if (mTextDisplayLists != NULL) {
        for (Int32 i = 0; i < mTextDisplayLists->GetLength(); i++) {
            AutoPtr<IRenderNode> displayList;
            if ((*mTextDisplayLists)[i] != NULL) {
                displayList = (*mTextDisplayLists)[i]->mDisplayList;
            }
            Boolean isValid;
            if (displayList != NULL && (displayList->IsValid(&isValid), isValid)) {
                displayList->DestroyDisplayListData();
            }
        }
    }
}

void Editor::ShowError()
{
    AutoPtr<IBinder> token;
    mTextView->GetWindowToken((IBinder**)&token);
    if (token == NULL) {
        mShowErrorAfterAttach = TRUE;
        return;
    }

    if (mErrorPopup == NULL) {
        AutoPtr<IContext> context;
        mTextView->GetContext((IContext**)&context);
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(context, (ILayoutInflater**)&inflater);

        AutoPtr<ITextView> err;
        inflater->Inflate(R::layout::textview_hint, NULL, (IView**)&err);

        AutoPtr<IResources> resources;
        mTextView->GetResources((IResources**)&resources);
        AutoPtr<IDisplayMetrics> metrics;
        resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
        Float scale;
        metrics->GetDensity(&scale);

        mErrorPopup = new ErrorPopup((TextView*)err.Get(),
                (Int32)(200 * scale + 0.5f), (Int32)(50 * scale + 0.5f));
        mErrorPopup->SetFocusable(FALSE);
        // The user is entering text, so the input method is needed.  We
        // don't want the popup to be displayed on top of it.
        mErrorPopup->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NEEDED);
    }

    AutoPtr<IView> cv;
    mErrorPopup->GetContentView((IView**)&cv);
    TextView* tv = (TextView*)ITextView::Probe(cv);
    ChooseSize(mErrorPopup, mError, tv);
    tv->SetText(mError);

    mErrorPopup->ShowAsDropDown(IView::Probe(mTextView),
            GetErrorX(), GetErrorY());
    Boolean isAboveAnchor;
    mErrorPopup->IsAboveAnchor(&isAboveAnchor);
    mErrorPopup->FixDirection(isAboveAnchor);
}

ECode Editor::SetError(
    /* [in] */ ICharSequence* error,
    /* [in] */ IDrawable* icon)
{
    mError = TextUtils::StringOrSpannedString(error);
    mErrorWasChanged = TRUE;

    if (mError == NULL) {
        SetErrorIcon(NULL);
        if (mErrorPopup != NULL) {
            Boolean bval;
            mErrorPopup->IsShowing(&bval);
            if (bval) {
                mErrorPopup->Dismiss();
            }

            mErrorPopup = NULL;
        }

        mShowErrorAfterAttach = FALSE;
    }
    else {
        SetErrorIcon(icon);
        Boolean isFocused;
        if (mTextView->IsFocused(&isFocused), isFocused) {
            ShowError();
        }
    }
    return NOERROR;
}

void Editor::SetErrorIcon(
    /* [in] */ IDrawable* icon)
{
    AutoPtr<Drawables> dr = mTextView->mDrawables;
    if (dr == NULL) {
        AutoPtr<IContext> ctx;
        IView::Probe(mTextView)->GetContext((IContext**)&ctx);
        mTextView->mDrawables = dr = new Drawables(ctx);
    }
    ITextView* tv = (ITextView*)(mTextView->Probe(EIID_ITextView));

    dr->SetErrorDrawable(icon, tv);
    mTextView->ResetResolvedDrawables();
    mTextView->Invalidate();
    mTextView->RequestLayout();
}

void Editor::HideError()
{
    if (mErrorPopup != NULL) {
        Boolean bval;
        mErrorPopup->IsShowing(&bval);
        if (bval) {
            mErrorPopup->Dismiss();
        }

        SetErrorIcon(NULL);
    }

    mShowErrorAfterAttach = FALSE;
}

Int32 Editor::GetErrorX()
{
    /*
     * The "25" is the distance between the point and the right edge
     * of the background
     */
    AutoPtr<IResources> resources;
    mTextView->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float scale;
    metrics->GetDensity(&scale);

    AutoPtr<Drawables> dr = mTextView->mDrawables;
    Int32 layoutDirection;
    mTextView->GetLayoutDirection(&layoutDirection);
    Int32 errorX;
    Int32 offset;
    switch (layoutDirection) {
        default:
        case IView::LAYOUT_DIRECTION_LTR: {
            Int32 w;
            mErrorPopup->GetWidth(&w);
            offset = - (dr != NULL ? dr->mDrawableSizeRight : 0) / 2 + (Int32) (25 * scale + 0.5f);
            Int32 width, right;
            mTextView->GetWidth(&width);
            mTextView->GetPaddingRight(&right);
            errorX = width - w - right + offset;
            break;
        }
        case IView::LAYOUT_DIRECTION_RTL:
            offset = (dr != NULL ? dr->mDrawableSizeLeft : 0) / 2 - (Int32) (25 * scale + 0.5f);
            Int32 left;
            mTextView->GetPaddingLeft(&left);
            errorX = left + offset;
            break;
    }

    return errorX;
}

Int32 Editor::GetErrorY()
{
    /*
     * Compound, not extended, because the icon is not clipped
     * if the text height is smaller.
     */
    Int32 compoundPaddingTop, bottom, top, paddingBOttom;
    mTextView->GetCompoundPaddingTop(&compoundPaddingTop);
    mTextView->GetBottom(&bottom);
    mTextView->GetTop(&top);
    mTextView->GetCompoundPaddingBottom(&paddingBOttom);
    Int32 vspace = bottom - top - paddingBOttom - compoundPaddingTop;

    AutoPtr<Drawables> dr = mTextView->mDrawables;
    Int32 layoutDirection;
    mTextView->GetLayoutDirection(&layoutDirection);
    Int32 height;
    switch (layoutDirection) {
        default:
        case IView::LAYOUT_DIRECTION_LTR:
            height = (dr != NULL ? dr->mDrawableHeightRight : 0);
            break;
        case IView::LAYOUT_DIRECTION_RTL:
            height = (dr != NULL ? dr->mDrawableHeightLeft : 0);
            break;
    }

    Int32 icontop = compoundPaddingTop + (vspace - height) / 2;

    AutoPtr<IResources> resources;
    mTextView->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float scale;
    metrics->GetDensity(&scale);
    Int32 last;
    mTextView->GetHeight(&last);
    return icontop + height - last - (Int32) (2 * scale + 0.5f);
}

void Editor::CreateInputContentTypeIfNeeded()
{
    if (mInputContentType == NULL) {
        mInputContentType = new InputContentType();
    }
}

void Editor::CreateInputMethodStateIfNeeded()
{
    if (mInputMethodState == NULL) {
        mInputMethodState = new InputMethodState();
    }
}

Boolean Editor::IsCursorVisible()
{
    // The default value is TRUE, even when there is no associated Editor
    return mCursorVisible && mTextView->IsTextEditable();
}

void Editor::PrepareCursorControllers()
{
    Boolean windowSupportsHandles = FALSE;

    AutoPtr<IView> trv;
    mTextView->GetRootView((IView**)&trv);
    AutoPtr<IViewGroupLayoutParams> params;
    trv->GetLayoutParams((IViewGroupLayoutParams**)&params);
    IWindowManagerLayoutParams* windowParams = IWindowManagerLayoutParams::Probe(params.Get());
    if (windowParams) {
        Int32 type;
        windowParams->GetType(&type);
        windowSupportsHandles = type < IWindowManagerLayoutParams::FIRST_SUB_WINDOW
                || type > IWindowManagerLayoutParams::LAST_SUB_WINDOW;
    }

    AutoPtr<ILayout> layout;
    mTextView->GetLayout((ILayout**)&layout);
    Boolean enabled = windowSupportsHandles && layout != NULL;
    mInsertionControllerEnabled = enabled && IsCursorVisible();
    mSelectionControllerEnabled = enabled && mTextView->TextCanBeSelected();

    if (!mInsertionControllerEnabled) {
        HideInsertionPointCursorController();
        if (mInsertionPointCursorController != NULL) {
            mInsertionPointCursorController->OnDetached();
            mInsertionPointCursorController = NULL;
        }
    }

    if (!mSelectionControllerEnabled) {
        StopSelectionActionMode();
        if (mSelectionModifierCursorController != NULL) {
            mSelectionModifierCursorController->OnDetached();
            mSelectionModifierCursorController = NULL;
        }
    }
}

void Editor::HideInsertionPointCursorController()
{
    if (mInsertionPointCursorController != NULL) {
        mInsertionPointCursorController->Hide();
    }
}

void Editor::HideControllers()
{
    HideCursorControllers();
    HideSpanControllers();
}

void Editor::HideSpanControllers()
{
    if (mSpanController != NULL) {
        mSpanController->Hide();
    }
}

void Editor::HideCursorControllers()
{
    if (mSuggestionsPopupWindow != NULL && !mSuggestionsPopupWindow->IsShowingUp()) {
        // Should be done before hide insertion point controller since it triggers a show of it
        mSuggestionsPopupWindow->Hide();
    }
    HideInsertionPointCursorController();
    StopSelectionActionMode();
}

void Editor::UpdateSpellCheckSpans(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Boolean createSpellChecker)
{
    // Remove spans whose adjacent characters are text not punctuation
    mTextView->RemoveAdjacentSuggestionSpans(start);
    mTextView->RemoveAdjacentSuggestionSpans(end);
    Boolean isSuggestionsEnabled;
    if (mTextView->IsTextEditable()
        && (mTextView->IsSuggestionsEnabled(&isSuggestionsEnabled), isSuggestionsEnabled)
        && !(IExtractEditText::Probe(mTextView))) {
        if (mSpellChecker == NULL && createSpellChecker) {
            CSpellChecker::New(mTextView, (ISpellChecker**)&mSpellChecker);
        }
        if (mSpellChecker != NULL) {
            mSpellChecker->SpellCheck(start, end);
        }
    }
}

void Editor::OnScreenStateChanged(
    /* [in] */ Int32 screenState)
{
    switch (screenState) {
        case IView::SCREEN_STATE_ON:
            ResumeBlink();
            break;
        case IView::SCREEN_STATE_OFF:
            SuspendBlink();
            break;
    }
}

void Editor::SuspendBlink()
{
    if (mBlink != NULL) {
        mBlink->Cancel();
    }
}

void Editor::ResumeBlink()
{
    if (mBlink != NULL) {
        mBlink->Uncancel();
        MakeBlink();
    }
}

void Editor::AdjustInputType(
    /* [in] */ Boolean password,
    /* [in] */ Boolean passwordInputType,
    /* [in] */ Boolean webPasswordInputType,
    /* [in] */ Boolean numberPasswordInputType)
{
    // mInputType has been set from inputType, possibly modified by mInputMethod.
    // Specialize mInputType to [web]password if we have a text class and the original input
    // type was a password.
    if ((mInputType & IInputType::TYPE_MASK_CLASS) == IInputType::TYPE_CLASS_TEXT) {
        if (password || passwordInputType) {
            mInputType = (mInputType & ~(IInputType::TYPE_MASK_VARIATION))
                    | IInputType::TYPE_TEXT_VARIATION_PASSWORD;
        }
        if (webPasswordInputType) {
            mInputType = (mInputType & ~(IInputType::TYPE_MASK_VARIATION))
                    | IInputType::TYPE_TEXT_VARIATION_WEB_PASSWORD;
        }
    } else if ((mInputType & IInputType::TYPE_MASK_CLASS) == IInputType::TYPE_CLASS_NUMBER) {
        if (numberPasswordInputType) {
            mInputType = (mInputType & ~(IInputType::TYPE_MASK_VARIATION))
                    | IInputType::TYPE_NUMBER_VARIATION_PASSWORD;
        }
    }
}

void Editor::ChooseSize(
    /* [in] */ PopupWindow* pop,
    /* [in] */ ICharSequence* text,
    /* [in] */ TextView* tv)
{
    Int32 left, right, top, bottom;
    tv->GetPaddingLeft(&left);
    tv->GetPaddingRight(&right);
    Int32 wid = left + right;
    tv->GetPaddingTop(&top);
    tv->GetPaddingBottom(&bottom);
    Int32 ht = top + bottom;

    AutoPtr<IResources> resources;
    mTextView->GetResources((IResources**)&resources);
    Int32 defaultWidthInPixels;
    resources->GetDimensionPixelSize(
        R::dimen::textview_error_popup_default_width, &defaultWidthInPixels);
    AutoPtr<ITextPaint> paint;
    tv->GetPaint((ITextPaint**)&paint);
    AutoPtr<ILayout> layout;
    CStaticLayout::New(text, paint, defaultWidthInPixels,
        Elastos::Droid::Text::ALIGN_NORMAL, 1, 0, TRUE, (IStaticLayout**)&layout);
    Float max = 0;
    Float lineWidth;
    Int32 lineCount;
    layout->GetLineCount(&lineCount);
    for (Int32 i = 0; i < lineCount; i++) {
        layout->GetLineWidth(i, &lineWidth);
        max = Elastos::Core::Math::Max(max, lineWidth);
    }

    /*
     * Now set the popup size to be big enough for the text plus the border capped
     * to DEFAULT_MAX_POPUP_WIDTH
     */
    Int32 height;
    layout->GetHeight(&height);
    pop->SetWidth(wid + (Int32) Elastos::Core::Math::Ceil(max));
    pop->SetHeight(ht + height);
}

void Editor::SetFrame()
{
    if (mErrorPopup != NULL) {
        AutoPtr<IView> cv;
        mErrorPopup->GetContentView((IView**)&cv);
        TextView* tv = (TextView*)ITextView::Probe(cv);
        assert(tv != NULL);
        ChooseSize(mErrorPopup, mError, tv);
        Int32 w, h;
        mErrorPopup->GetWidth(&w);
        mErrorPopup->GetHeight(&h);
        mErrorPopup->Update((IView*)(mTextView->Probe(EIID_IView)),
            GetErrorX(), GetErrorY(), w, h);
    }
}

Boolean Editor::CanSelectText()
{
    Int32 len;
    mTextView->GetLength(&len);
    return HasSelectionController() && len != 0;
}

Boolean Editor::HasPasswordTransformationMethod()
{
    AutoPtr<ITransformationMethod> method;
    mTextView->GetTransformationMethod((ITransformationMethod**)&method);
    return IPasswordTransformationMethod::Probe(method) != NULL;
}

Boolean Editor::SelectCurrentWord()
{
    if (!CanSelectText()) {
        return FALSE;
    }

    if (HasPasswordTransformationMethod()) {
        // Always select all on a password field.
        // Cut/copy menu entries are not available for passwords, but being able to select all
        // is however useful to delete or paste to replace the entire content.
        return mTextView->SelectAllText();
    }

    Int32 inputType;
    mTextView->GetInputType(&inputType);
    Int32 klass = inputType & IInputType::TYPE_MASK_CLASS;
    Int32 variation = inputType & IInputType::TYPE_MASK_VARIATION;

    // Specific text field types: select the entire text for these
    if (klass == IInputType::TYPE_CLASS_NUMBER ||
            klass == IInputType::TYPE_CLASS_PHONE ||
            klass == IInputType::TYPE_CLASS_DATETIME ||
            variation == IInputType::TYPE_TEXT_VARIATION_URI ||
            variation == IInputType::TYPE_TEXT_VARIATION_EMAIL_ADDRESS ||
            variation == IInputType::TYPE_TEXT_VARIATION_WEB_EMAIL_ADDRESS ||
            variation == IInputType::TYPE_TEXT_VARIATION_FILTER) {
        return mTextView->SelectAllText();
    }

    Int64 lastTouchOffsets = GetLastTouchOffsets();
    Int32 minOffset = TextUtils::UnpackRangeStartFromInt64(lastTouchOffsets);
    Int32 maxOffset = TextUtils::UnpackRangeEndFromInt64(lastTouchOffsets);

    // Safety check in case standard touch event handling has been bypassed
    Int32 textLength;
    mTextView->GetLength(&textLength);
    if (minOffset < 0 || minOffset >= textLength) return FALSE;
    if (maxOffset < 0 || maxOffset >= textLength) return FALSE;

    Int32 selectionStart, selectionEnd;

    // If a URLSpan (web address, email, phone...) is found at that position, select it.
    AutoPtr<ArrayOf<IURLSpan*> > urlSpans;
    AutoPtr<ICharSequence> text;
    mTextView->GetText((ICharSequence**)&text);
    ISpanned* spanned = ISpanned::Probe(text);
    if (spanned) {
        AutoPtr<ArrayOf<IInterface*> > temp;
        spanned->GetSpans(minOffset, maxOffset, EIID_IURLSpan,
                (ArrayOf<IInterface*>**)&temp);
        urlSpans = ArrayOf<IURLSpan*>::Alloc(temp->GetLength());
        for(Int32 i = 0; i < temp->GetLength(); i++) {
            urlSpans->Set(i, IURLSpan::Probe((*temp)[i]));
        }
    }

    if (urlSpans != NULL && urlSpans->GetLength() >= 1) {
        AutoPtr<IURLSpan> urlSpan = (*urlSpans)[0];
        spanned->GetSpanStart(urlSpan, &selectionStart);
        spanned->GetSpanEnd(urlSpan, &selectionEnd);
    } else {
        AutoPtr<IWordIterator> wordIterator;
        GetWordIterator((IWordIterator**)&wordIterator);
        wordIterator->SetCharSequence(text, minOffset, maxOffset);

        wordIterator->GetBeginning(minOffset, &selectionStart);
        wordIterator->GetEnd(maxOffset, &selectionEnd);

        if (selectionStart == -1 //IBreakIterator::DONE
               || selectionEnd == -1 //IBreakIterator::DONE
               || selectionStart == selectionEnd) {
           // Possible when the word iterator does not properly handle the text's language
           Int64 range = GetCharRange(minOffset);
           selectionStart = TextUtils::UnpackRangeStartFromInt64(range);
           selectionEnd = TextUtils::UnpackRangeEndFromInt64(range);
        }
    }

    AutoPtr<ISpannable> spannable = ISpannable::Probe(text);
    Selection::SetSelection(spannable, selectionStart, selectionEnd);
    return selectionEnd > selectionStart;
}

void Editor::OnLocaleChanged()
{
    // Will be re-created on demand in getWordIterator with the proper new locale
    mWordIterator = NULL;
}

ECode Editor::GetWordIterator(
    /* [out] */ IWordIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator)
    if (mWordIterator == NULL) {
        AutoPtr<ILocale> locale;
        mTextView->GetTextServicesLocale((ILocale**)&locale);
        CWordIterator::New(locale, (IWordIterator**)&mWordIterator);
    }
    *iterator = mWordIterator;
    REFCOUNT_ADD(*iterator)
    return NOERROR;
}

Int64 Editor::GetCharRange(
    /* [in] */ Int32 offset)
{
    AutoPtr<ICharSequence> text;
    mTextView->GetText((ICharSequence**)&text);
    Int32 textLength;
    mTextView->GetLength(&textLength);
    if (offset + 1 < textLength) {
        Char32 currentChar, nextChar;
        text->GetCharAt(offset, &currentChar);
        text->GetCharAt(offset + 1, &nextChar);
        if (Character::IsSurrogatePair(currentChar, nextChar)) {
            return TextUtils::PackRangeInInt64(offset,  offset + 2);
        }
    }
    if (offset < textLength) {
        return TextUtils::PackRangeInInt64(offset,  offset + 1);
    }
    if (offset - 2 >= 0) {
        Char32 previousChar, previousPreviousChar;
        text->GetCharAt(offset - 1, &previousChar);
        text->GetCharAt(offset - 2, &previousPreviousChar);
        if (Character::IsSurrogatePair(previousPreviousChar, previousChar)) {
            return TextUtils::PackRangeInInt64(offset - 2,  offset);
        }
    }
    if (offset - 1 >= 0) {
        return TextUtils::PackRangeInInt64(offset - 1,  offset);
    }
    return TextUtils::PackRangeInInt64(offset,  offset);
}

Boolean Editor::TouchPositionIsInSelection()
{
    Int32 selectionStart, selectionEnd;
    mTextView->GetSelectionStart(&selectionStart);
    mTextView->GetSelectionEnd(&selectionEnd);

    if (selectionStart == selectionEnd) {
        return FALSE;
    }

    if (selectionStart > selectionEnd) {
        Int32 tmp = selectionStart;
        selectionStart = selectionEnd;
        selectionEnd = tmp;
        AutoPtr<ICharSequence> text;
        mTextView->GetText((ICharSequence**)&text);
        Selection::SetSelection(ISpannable::Probe(text), selectionStart, selectionEnd);
    }

    AutoPtr<SelectionModifierCursorController> selectionController = GetSelectionController();
    Int32 minOffset = selectionController->GetMinTouchOffset();
    Int32 maxOffset = selectionController->GetMaxTouchOffset();

    return ((minOffset >= selectionStart) && (maxOffset < selectionEnd));
}

AutoPtr<PositionListener> Editor::GetPositionListener()
{
    if (mPositionListener == NULL) {
        mPositionListener = new PositionListener(this);
    }
    return mPositionListener;
}

Boolean Editor::IsPositionVisible(
    /* [in] */ Float positionX,
    /* [in] */ Float positionY)
{
    synchronized(TEMP_POSITION_OBJECT)
    {
        AutoLock lock(mTempPositionLock);
        AutoPtr<ArrayOf<Float> > position = TEMP_POSITION;
        (*position)[0] = positionX;
        (*position)[1] = positionY;
        IView* view = (IView*)(mTextView->Probe(EIID_IView));
        IView* textView = view;

        AutoPtr<IMatrix> matrix;
        Boolean isIdentity;
        Int32 sx, sy, w, h, l, t;

        while (view != NULL) {
            if (view != textView) {
                // Local scroll is already taken into account in positionX/Y
                view->GetScrollX(&sx);
                view->GetScrollY(&sy);
                (*position)[0] -= sx;
                (*position)[1] -= sy;
            }

            view->GetWidth(&w);
            view->GetHeight(&h);
            if ((*position)[0] < 0 || (*position)[1] < 0 ||
                    (*position)[0] > w || (*position)[1] > h) {
                return FALSE;
            }

            matrix = NULL;
            view->GetMatrix((IMatrix**)&matrix);
            matrix->IsIdentity(&isIdentity);
            if (!isIdentity) {
                matrix->MapPoints(position);
            }

            view->GetLeft(&l);
            view->GetTop(&t);
            (*position)[0] += l;
            (*position)[1] += t;

            AutoPtr<IViewParent> parent;
            view->GetParent((IViewParent**)&parent);
            if (IView::Probe(parent)) {
                view = IView::Probe(parent);
            } else {
                // We've reached the ViewRoot, stop iterating
                view = NULL;
            }
        }
    }

    // We've been able to walk up the view hierarchy and the position was never clipped
    return TRUE;
}

Boolean Editor::IsOffsetVisible(
    /* [in] */ Int32 offset)
{
    AutoPtr<ILayout> layout;
    mTextView->GetLayout((ILayout**)&layout);
    if (layout == NULL) return FALSE;

    Int32 line, lineBottom;
    layout->GetLineForOffset(offset, &line);
    layout->GetLineBottom(line, &lineBottom);
    Float hor;
    layout->GetPrimaryHorizontal(offset, &hor);
    Int32 primaryHorizontal = (Int32) hor;
    return IsPositionVisible(primaryHorizontal + mTextView->ViewportToContentHorizontalOffset(),
            lineBottom + mTextView->ViewportToContentVerticalOffset());
}

Boolean Editor::IsPositionOnText(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<ILayout> layout;
    mTextView->GetLayout((ILayout**)&layout);
    if (layout == NULL) return FALSE;

    Int32 line = mTextView->GetLineAtCoordinate(y);
    x = mTextView->ConvertToLocalHorizontalCoordinate(x);

    Float l, r;
    layout->GetLineLeft(line, &l);
    if (x < l) return FALSE;
    layout->GetLineRight(line, &r);
    if (x > r) return FALSE;
    return TRUE;
}

ECode Editor::PerformLongClick(
    /* [in] */ Boolean handled,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    // Long press in empty space moves cursor and shows the Paste affordance if available.
    if (!handled && !IsPositionOnText(mLastDownPositionX, mLastDownPositionY) &&
            mInsertionControllerEnabled) {
        Int32 offset;
        mTextView->GetOffsetForPosition(mLastDownPositionX, mLastDownPositionY, &offset);
        StopSelectionActionMode();
        AutoPtr<ICharSequence> text;
        mTextView->GetText((ICharSequence**)&text);
        Selection::SetSelection(ISpannable::Probe(text), offset);
        AutoPtr<InsertionPointCursorController> controller = GetInsertionController();
        controller->ShowWithActionPopup();
        handled = TRUE;
    }

    if (!handled && mSelectionActionMode != NULL) {
        if (TouchPositionIsInSelection()) {
            // Start a drag
            Int32 start, end;
            mTextView->GetSelectionStart(&start);
            mTextView->GetSelectionEnd(&end);
            AutoPtr<ICharSequence> selectedText = mTextView->GetTransformedText(start, end);
            AutoPtr<IClipDataHelper> helper;
            CClipDataHelper::AcquireSingleton((IClipDataHelper**)&helper);
            AutoPtr<IClipData> data;
            helper->NewPlainText(NULL, selectedText, (IClipData**)&data);

            AutoPtr<DragLocalState> localState = new DragLocalState(mTextView, start, end);
            AutoPtr<IDragShadowBuilder> builder = GetTextThumbnailBuilder(selectedText);
            if (builder == NULL) {
                *res = FALSE;
                return NOERROR;
            }
            Boolean isStartDrag;
            mTextView->StartDrag(data, builder, (IObject*)localState, 0, &isStartDrag);
            StopSelectionActionMode();
        } else {
            AutoPtr<SelectionModifierCursorController> controller = GetSelectionController();
            controller->Hide();
            SelectCurrentWord();
            controller->Show();
        }
        handled = TRUE;
    }

    // Start a new selection
    if (!handled) {
        handled = StartSelectionActionMode();
    }

    *res = handled;
    return NOERROR;
}

Int64 Editor::GetLastTouchOffsets()
{
    AutoPtr<SelectionModifierCursorController> selectionController = GetSelectionController();
    Int32 minOffset = selectionController->GetMinTouchOffset();
    Int32 maxOffset = selectionController->GetMaxTouchOffset();
    return TextUtils::PackRangeInInt64(minOffset, maxOffset);
}

void Editor::OnFocusChanged(
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction)
{
    mShowCursor = SystemClock::GetUptimeMillis();
    EnsureEndedBatchEdit();

    AutoPtr<ICharSequence> text;
    mTextView->GetText((ICharSequence**)&text);
    AutoPtr<ISpannable> spannable = ISpannable::Probe(text);
    if (focused) {
        Int32 selStart, selEnd;
        mTextView->GetSelectionStart(&selStart);
        mTextView->GetSelectionEnd(&selEnd);

        // SelectAllOnFocus fields are highlighted and not selected. Do not start text selection
        // mode for these, unless there was a specific selection already started.
        Int32 textLength;
        Boolean isFocusHighlighted = mSelectAllOnFocus && selStart == 0 &&
                selEnd == (mTextView->GetLength(&textLength), textLength);

        Boolean hasSelection;
        mCreatedWithASelection = mFrozenWithFocus
            && (mTextView->HasSelection(&hasSelection), hasSelection)
            && !isFocusHighlighted;

        if (!mFrozenWithFocus || (selStart < 0 || selEnd < 0)) {
            // If a tap was used to give focus to that view, move cursor at tap position.
            // Has to be done before onTakeFocus, which can be overloaded.
            Int32 lastTapPosition = GetLastTapPosition();
            if (lastTapPosition >= 0) {
                Selection::SetSelection(spannable, lastTapPosition);
            }

            // Note this may have to be moved out of the Editor class
            AutoPtr<IMovementMethod> mMovement;
            mTextView->GetMovementMethod((IMovementMethod**)&mMovement);
            if (mMovement != NULL) {
                mMovement->OnTakeFocus(
                        ITextView::Probe(mTextView),
                        spannable, direction);
            }

            // The DecorView does not have focus when the 'Done' ExtractEditText button is
            // pressed. Since it is the ViewAncestor's mView, it requests focus before
            // ExtractEditText clears focus, which gives focus to the ExtractEditText.
            // This special case ensure that we keep current selection in that case.
            // It would be better to know why the DecorView does not have focus at that time.
            if ((mTextView->Probe(EIID_IExtractEditText) || mSelectionMoved) &&
                    selStart >= 0 && selEnd >= 0) {
                /*
                 * Someone intentionally set the selection, so let them
                 * do whatever it is that they wanted to do instead of
                 * the default on-focus behavior.  We reset the selection
                 * here instead of just skipping the onTakeFocus() call
                 * because some movement methods do something other than
                 * just setting the selection in theirs and we still
                 * need to go through that path.
                 */
                Selection::SetSelection(spannable, selStart, selEnd);
            }

            if (mSelectAllOnFocus) {
                mTextView->SelectAllText();
            }

            mTouchFocusSelected = TRUE;
        }

        mFrozenWithFocus = FALSE;
        mSelectionMoved = FALSE;

        if (mError != NULL) {
            ShowError();
        }

        MakeBlink();
    }
    else {
        if (mError != NULL) {
            HideError();
        }
        // Don't leave us in the middle of a batch edit.
        mTextView->OnEndBatchEdit();

        if (mTextView->Probe(EIID_IExtractEditText)) {
            // terminateTextSelectionMode removes selection, which we want to keep when
            // ExtractEditText goes out of focus.
            Int32 selStart, selEnd;
            mTextView->GetSelectionStart(&selStart);
            mTextView->GetSelectionEnd(&selEnd);
            HideControllers();
            Selection::SetSelection(spannable, selStart, selEnd);
        }
        else {
            if (mTemporaryDetach) mPreserveDetachedSelection = TRUE;
            HideControllers();
            if (mTemporaryDetach) mPreserveDetachedSelection = FALSE;
            DowngradeEasyCorrectionSpans();
        }

        // No need to create the controller
        if (mSelectionModifierCursorController != NULL) {
            mSelectionModifierCursorController->ResetTouchOffsets();
        }
    }
}

void Editor::DowngradeEasyCorrectionSpans()
{
    AutoPtr<ICharSequence> text;
    mTextView->GetText((ICharSequence**)&text);
    ISpannable* spannable = ISpannable::Probe(text);
    if (spannable) {
        Int32 length;
        ICharSequence::Probe(spannable)->GetLength(&length);
        AutoPtr<ArrayOf<ISuggestionSpan*> > temp;
        ISpanned::Probe(spannable)->GetSpans(0, length, EIID_ISuggestionSpan, (ArrayOf<IInterface*>**)&temp);
        AutoPtr<ArrayOf<ISuggestionSpan*> > suggestionSpans = ArrayOf<ISuggestionSpan*>::Alloc(temp->GetLength());
        for(Int32 i = 0; i < temp->GetLength(); i++) {
            suggestionSpans->Set(i, ISuggestionSpan::Probe((*temp)[i]));
        }
        if (suggestionSpans != NULL) {
            Int32 flags;
            for (Int32 i = 0; i < suggestionSpans->GetLength(); i++) {
                (*suggestionSpans)[i]->GetFlags(&flags);
                if ((flags & ISuggestionSpan::FLAG_EASY_CORRECT) != 0
                        && (flags & ISuggestionSpan::FLAG_MISSPELLED) == 0) {
                    flags &= ~ISuggestionSpan::FLAG_EASY_CORRECT;
                    (*suggestionSpans)[i]->SetFlags(flags);
                }
            }
        }
    }
}

void Editor::SendOnTextChanged(
    /* [in] */ Int32 start,
    /* [in] */ Int32 after)
{
    UpdateSpellCheckSpans(start, start + after, FALSE);

    // Hide the controllers as soon as text is modified (typing, procedural...)
    // We do not hide the span controllers, since they can be added when a new text is
    // inserted into the text view (voice IME).
    HideCursorControllers();
}

Int32 Editor::GetLastTapPosition()
{
    // No need to create the controller at that point, no last tap position saved
    if (mSelectionModifierCursorController != NULL) {
        Int32 lastTapPosition = mSelectionModifierCursorController->GetMinTouchOffset();
        if (lastTapPosition >= 0) {
            // Safety check, should not be possible.
            Int32 length;
            mTextView->GetLength(&length);
            if (lastTapPosition > length) {
                lastTapPosition = length;
            }
            return lastTapPosition;
        }
    }

    return -1;
}

void Editor::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    if (hasWindowFocus) {
        if (mBlink != NULL) {
            mBlink->Uncancel();
            MakeBlink();
        }
    } else {
        if (mBlink != NULL) {
            mBlink->Cancel();
        }
        if (mInputContentType != NULL) {
            mInputContentType->mEnterDown = FALSE;
        }
        // Order matters! Must be done before onParentLostFocus to rely on isShowingUp
        HideControllers();
        if (mSuggestionsPopupWindow != NULL) {
            mSuggestionsPopupWindow->OnParentLostFocus();
        }

        // Don't leave us in the middle of a batch edit. Same as in onFocusChanged
        EnsureEndedBatchEdit();
    }
}

void Editor::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    assert(event);

    if (HasSelectionController()) {
        GetSelectionController()->OnTouchEvent(event);
    }

    if (mShowSuggestionRunnable != NULL) {
        Boolean res;
        mTextView->RemoveCallbacks(mShowSuggestionRunnable, &res);
        mShowSuggestionRunnable = NULL;
    }

    Int32 masked;
    event->GetActionMasked(&masked);
    if (masked == IMotionEvent::ACTION_DOWN) {
        event->GetX(&mLastDownPositionX);
        event->GetY(&mLastDownPositionY);

        // Reset this state; it will be re-set if super.onTouchEvent
        // causes focus to move to the view.
        mTouchFocusSelected = FALSE;
        mIgnoreActionUpEvent = FALSE;
    }
}

ECode Editor::BeginBatchEdit()
{
    mInBatchEditControllers = TRUE;
    AutoPtr<InputMethodState> ims = mInputMethodState;
    if (ims != NULL) {
        Int32 nesting = ++ims->mBatchEditNesting;
        if (nesting == 1) {
            ims->mCursorChanged = FALSE;
            ims->mChangedDelta = 0;
            if (ims->mContentChanged) {
                // We already have a pending change from somewhere else,
                // so turn this into a full update.
                ims->mChangedStart = 0;
                mTextView->GetLength(&ims->mChangedEnd);
            } else {
                ims->mChangedStart = EXTRACT_UNKNOWN;
                ims->mChangedEnd = EXTRACT_UNKNOWN;
                ims->mContentChanged = FALSE;
            }
            mTextView->OnBeginBatchEdit();
        }
    }
    return NOERROR;
}

ECode Editor::EndBatchEdit()
{
    mInBatchEditControllers = FALSE;
    AutoPtr<InputMethodState> ims = mInputMethodState;
    if (ims != NULL) {
        Int32 nesting = --ims->mBatchEditNesting;
        if (nesting == 0) {
            FinishBatchEdit(ims);
        }
    }
    return NOERROR;
}

void Editor::EnsureEndedBatchEdit()
{
    AutoPtr<InputMethodState> ims = mInputMethodState;
    if (ims != NULL && ims->mBatchEditNesting != 0) {
        ims->mBatchEditNesting = 0;
        FinishBatchEdit(ims);
    }
}

void Editor::FinishBatchEdit(
    /* [in] */ InputMethodState* ims)
{
    mTextView->OnEndBatchEdit();

    if (ims->mContentChanged || ims->mSelectionModeChanged) {
        mTextView->UpdateAfterEdit();
        ReportExtractedText();
    } else if (ims->mCursorChanged) {
        // Cheezy way to get us to report the current cursor location.
        mTextView->InvalidateCursor();
    }
    // sendUpdateSelection knows to avoid sending if the selection did
    // not actually change.
    SendUpdateSelection();
}

Boolean Editor::ExtractText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ IExtractedText* outText)
{
    return ExtractTextInternal(request, EXTRACT_UNKNOWN, EXTRACT_UNKNOWN,
            EXTRACT_UNKNOWN, outText);
}

Boolean Editor::ExtractTextInternal(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ Int32 partialStartOffset,
    /* [in] */ Int32 partialEndOffset,
    /* [in] */ Int32 delta,
    /* [in] */ IExtractedText* outText)
{
    AutoPtr<ICharSequence> content;
    mTextView->GetText((ICharSequence**)&content);
    if (content != NULL) {
        if (partialStartOffset != EXTRACT_NOTHING) {
            Int32 N;
            content->GetLength(&N);
            if (partialStartOffset < 0) {
                outText->SetPartialStartOffset(-1);
                outText->SetPartialEndOffset(-1);
                partialStartOffset = 0;
                partialEndOffset = N;
            } else {
                // Now use the delta to determine the actual amount of text
                // we need.
                partialEndOffset += delta;
                // Adjust offsets to ensure we contain full spans.
                if (ISpanned::Probe(content)) {
                    ISpanned* spanned = ISpanned::Probe(content);
                    AutoPtr<ArrayOf<IInterface*> > spans;
                    spanned->GetSpans(partialStartOffset, partialEndOffset,
                            EIID_IParcelableSpan, (ArrayOf<IInterface*>**)&spans);
                    if (spans) {
                        Int32 i = spans->GetLength();
                        Int32 j;
                        while (i > 0) {
                            i--;
                            spanned->GetSpanStart((*spans)[i], &j);
                            if (j < partialStartOffset) partialStartOffset = j;
                            spanned->GetSpanEnd((*spans)[i], &j);
                            if (j > partialEndOffset) partialEndOffset = j;
                        }
                    }
                }
                outText->SetPartialStartOffset(partialStartOffset);
                outText->SetPartialEndOffset(partialEndOffset - delta);

                if (partialStartOffset > N) {
                    partialStartOffset = N;
                } else if (partialStartOffset < 0) {
                    partialStartOffset = 0;
                }
                if (partialEndOffset > N) {
                    partialEndOffset = N;
                } else if (partialEndOffset < 0) {
                    partialEndOffset = 0;
                }
            }

            Int32 flags;
            request->GetFlags(&flags);
            if ((flags & IInputConnection::GET_TEXT_WITH_STYLES) != 0) {
                AutoPtr<ICharSequence> subSeq;
                content->SubSequence(partialStartOffset, partialEndOffset, (ICharSequence**)&subSeq);
                outText->SetText(subSeq);
            } else {
                String str = TextUtils::Substring(content, partialStartOffset, partialEndOffset);
                AutoPtr<ICharSequence> seq;
                CString::New(str, (ICharSequence**)&seq);
                outText->SetText(seq);
            }
        } else {
            outText->SetPartialStartOffset(0);
            outText->SetPartialEndOffset(0);
            AutoPtr<ICharSequence> seq;
            CString::New(String(""), (ICharSequence**)&seq);
            outText->SetText(seq);
        }
        outText->SetFlags(0);

        AutoPtr<IMetaKeyKeyListenerHelper> helper;
        CMetaKeyKeyListenerHelper::AcquireSingleton((IMetaKeyKeyListenerHelper**)&helper);
        Int32 state;
        helper->GetMetaState(content, IMetaKeyKeyListener::META_SELECTING, &state);
        if (state != 0) {
            Int32 flags;
            outText->GetFlags(&flags);
            outText->SetFlags(flags | IExtractedText::FLAG_SELECTING);
        }
        if (mTextView->IsSingleLine()) {
            Int32 flags;
            outText->GetFlags(&flags);
            outText->SetFlags(flags | IExtractedText::FLAG_SINGLE_LINE);
        }
        outText->SetStartOffset(0);
        Int32 selStart, selEnd;
        mTextView->GetSelectionStart(&selStart);
        outText->SetSelectionStart(selStart);
        mTextView->GetSelectionEnd(&selEnd);
        outText->SetSelectionEnd(selEnd);
        return TRUE;
    }
    return FALSE;
}

Boolean Editor::ReportExtractedText()
{
    AutoPtr<InputMethodState> ims = mInputMethodState;
    if (ims != NULL) {
        Boolean contentChanged = ims->mContentChanged;
        if (contentChanged || ims->mSelectionModeChanged) {
            ims->mContentChanged = FALSE;
            ims->mSelectionModeChanged = FALSE;
            AutoPtr<IExtractedTextRequest> req = ims->mExtractedTextRequest;
            if (req != NULL) {
                AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
                if (imm != NULL) {
//                    if (TextView::DEBUG_EXTRACT) Log.v(TextView.LOG_TAG,
//                            "Retrieving extracted start=" + ims->mChangedStart +
//                            " end=" + ims->mChangedEnd +
//                            " delta=" + ims->mChangedDelta);
                    if (ims->mChangedStart < 0 && !contentChanged) {
                        ims->mChangedStart = EXTRACT_NOTHING;
                    }
                    if (ExtractTextInternal(req, ims->mChangedStart, ims->mChangedEnd,
                            ims->mChangedDelta, ims->mExtractedText)) {
//                        if (TextView.DEBUG_EXTRACT) Log.v(TextView.LOG_TAG,
//                                "Reporting extracted start=" +
//                                ims->mExtractedText.partialStartOffset +
//                                " end=" + ims->mExtractedText.partialEndOffset +
//                                ": " + ims->mExtractedText.text);

                        Int32 token;
                        req->GetToken(&token);
                        imm->UpdateExtractedText(
                                IVIEW_PROBE(mTextView),
                                token, ims->mExtractedText);
                        ims->mChangedStart = EXTRACT_UNKNOWN;
                        ims->mChangedEnd = EXTRACT_UNKNOWN;
                        ims->mChangedDelta = 0;
                        ims->mContentChanged = FALSE;
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}

void Editor::SendUpdateSelection()
{
    if (NULL != mInputMethodState && mInputMethodState->mBatchEditNesting <= 0) {
        AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
        if (NULL != imm) {
            Int32 selectionStart, selectionEnd;
            mTextView->GetSelectionStart(&selectionStart);
            mTextView->GetSelectionEnd(&selectionEnd);
            Int32 candStart = -1;
            Int32 candEnd = -1;
            AutoPtr<ICharSequence> text;
            mTextView->GetText((ICharSequence**)&text);
            if (ISpannable::Probe(text)) {
                AutoPtr<ISpannable> sp = ISpannable::Probe(text);
                candStart = EditableInputConnection::GetComposingSpanStart(sp);
                candEnd = EditableInputConnection::GetComposingSpanEnd(sp);
            }
            // InputMethodManager#updateSelection skips sending the message if
            // none of the parameters have changed since the last time we called it.
            imm->UpdateSelection(mTextView,
                    selectionStart, selectionEnd, candStart, candEnd);
        }
    }
}

void Editor::OnDraw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ ILayout* layout,
    /* [in] */ IPath* highlight,
    /* [in] */ IPaint* highlightPaint,
    /* [in] */ Int32 cursorOffsetVertical)
{
    assert(canvas != NULL);
    assert(layout != NULL);
    Int32 selectionStart, selectionEnd;
    mTextView->GetSelectionStart(&selectionStart);
    mTextView->GetSelectionEnd(&selectionEnd);

    AutoPtr<InputMethodState> ims = mInputMethodState;
    if (ims != NULL && ims->mBatchEditNesting == 0) {
        AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
        if (imm != NULL) {
            Boolean active;
            imm->IsActive(IView::Probe(mTextView), &active);
            if (active) {
                Boolean UNUSED(reported) = FALSE;
                if (ims->mContentChanged || ims->mSelectionModeChanged) {
                    // We are in extract mode and the content has changed
                    // in some way... just report complete new text to the
                    // input method.
                    reported = ReportExtractedText();
                }
            }
        }
    }

    if (mCorrectionHighlighter != NULL) {
        mCorrectionHighlighter->Draw(canvas, cursorOffsetVertical);
    }

    if (highlight != NULL && selectionStart == selectionEnd && mCursorCount > 0) {
        DrawCursor(canvas, cursorOffsetVertical);
        // Rely on the drawable entirely, do not draw the cursor line.
        // Has to be done after the IMM related code above which relies on the highlight.
        highlight = NULL;
    }

    Boolean isAccelerated = FALSE;
    canvas->IsHardwareAccelerated(&isAccelerated);
    Boolean canHaveDisplayList;
    if ((mTextView->CanHaveDisplayList(&canHaveDisplayList), canHaveDisplayList)
        && isAccelerated) {
        DrawHardwareAccelerated(canvas, layout, highlight, highlightPaint,
                cursorOffsetVertical);
    } else {
        layout->Draw(canvas, highlight, highlightPaint, cursorOffsetVertical);
    }
}

void Editor::DrawHardwareAccelerated(
    /* [in] */ ICanvas* canvas,
    /* [in] */ ILayout* layout,
    /* [in] */ IPath* highlight,
    /* [in] */ IPaint* highlightPaint,
    /* [in] */ Int32 cursorOffsetVertical)
{
    assert(layout);

    Int64 lineRange;
    layout->GetLineRangeForDraw(canvas, &lineRange);
    Int32 firstLine = TextUtils::UnpackRangeStartFromInt64(lineRange);
    Int32 lastLine = TextUtils::UnpackRangeEndFromInt64(lineRange);
    if (lastLine < 0) return;

    layout->DrawBackground(canvas, highlight, highlightPaint, cursorOffsetVertical,
            firstLine, lastLine);

    if (IDynamicLayout::Probe(layout)) {
        if (mTextDisplayLists == NULL) {
            mTextDisplayLists = ArrayOf<TextDisplayList*>::Alloc(0);
        }

        IDynamicLayout* dynamicLayout = IDynamicLayout::Probe(layout);
        AutoPtr<ArrayOf<Int32> > blockEndLines, blockIndices;
        dynamicLayout->GetBlockEndLines((ArrayOf<Int32>**)&blockEndLines);
        dynamicLayout->GetBlockIndices((ArrayOf<Int32>**)&blockIndices);
        Int32 numberOfBlocks, indexFirstChangedBlock;
        dynamicLayout->GetNumberOfBlocks(&numberOfBlocks);
        dynamicLayout->GetIndexFirstChangedBlock(&indexFirstChangedBlock);

        Int32 endOfPreviousBlock = -1;
        Int32 searchStartIndex = 0;
        for (Int32 i = 0; i < numberOfBlocks; i++) {
            Int32 blockEndLine = (*blockEndLines)[i];
            Int32 blockIndex = (*blockIndices)[i];

            Boolean blockIsInvalid = blockIndex == DynamicLayout::INVALID_BLOCK_INDEX;
            if (blockIsInvalid) {
                blockIndex = GetAvailableDisplayListIndex(blockIndices, numberOfBlocks,
                        searchStartIndex);
                // Note how dynamic layout's internal block indices get updated from Editor
                (*blockIndices)[i] = blockIndex;
                searchStartIndex = blockIndex + 1;
            }

            if ((*mTextDisplayLists)[blockIndex] == NULL) {
                String name("Text ");
                name += StringUtils::ToString(blockIndex);
                AutoPtr<TextDisplayList> obj = new TextDisplayList(name);
                mTextDisplayLists->Set(blockIndex, obj);
            }

            Boolean blockDisplayListIsInvalid = (*mTextDisplayLists)[blockIndex]->NeedsRecord();
            AutoPtr<IRenderNode> blockDisplayList = (*mTextDisplayLists)[blockIndex]->mDisplayList;
            if (i >= indexFirstChangedBlock || blockDisplayListIsInvalid) {
                Int32 blockBeginLine = endOfPreviousBlock + 1;
                Int32 top, bottom;
                layout->GetLineTop(blockBeginLine, &top);
                layout->GetLineBottom(blockEndLine, &bottom);
                Int32 left = 0, right;
                mTextView->GetWidth(&right);
                Boolean isHorizontallyScrolling;
                if (mTextView->GetHorizontallyScrolling(&isHorizontallyScrolling), isHorizontallyScrolling) {
                    Float min = Elastos::Core::Math::FLOAT_MAX_VALUE;
                    Float max = Elastos::Core::Math::FLOAT_MIN_VALUE;
                    Float l, r;
                    for (Int32 line = blockBeginLine; line <= blockEndLine; line++) {
                        layout->GetLineLeft(line, &l);
                        layout->GetLineRight(line, &r);
                        min = Elastos::Core::Math::Min(min, l);
                        max = Elastos::Core::Math::Max(max, r);
                    }
                    left = (Int32) min;
                    right = (Int32) (max + 0.5f);
                }
               Boolean resTmp;
               if (blockDisplayListIsInvalid) {
                   AutoPtr<IHardwareCanvas> hardwareCanvas;
                   blockDisplayList->Start(right - left, bottom - top, (IHardwareCanvas**)&hardwareCanvas);
                   // try {
                   // drawText is always relative to TextView's origin, this translation brings
                   // this range of text back to the top left corner of the viewport
                    if (FAILED(ICanvas::Probe(hardwareCanvas)->Translate(-left, -top)))
                        goto finally;
                    if (FAILED(layout->DrawText(ICanvas::Probe(hardwareCanvas), blockBeginLine, blockEndLine)))
                        goto finally;
                       // No need to untranslate, previous context is popped after drawDisplayList
                   // } finally {
                    finally:
                       blockDisplayList->End(hardwareCanvas);
                       // Same as drawDisplayList below, handled by our TextView's parent
                       blockDisplayList->SetClipToBounds(FALSE, &resTmp);
                   // }
               }

                // Valid disply list whose index is >= indexFirstChangedBlock
                    // only needs to update its drawing location.
               blockDisplayList->SetLeftTopRightBottom(left, top, right, bottom, &resTmp);
//
            }
//

            IHardwareCanvas* hCanvas = IHardwareCanvas::Probe(canvas);
            Int32 tmp;
            hCanvas->DrawRenderNode(blockDisplayList, NULL, 0, &tmp);
//            ((HardwareCanvas) canvas).drawRenderNode(blockDisplayList, NULL,
//                    0  no child clipping, our TextView parent enforces it );

            endOfPreviousBlock = blockEndLine;
        }
        dynamicLayout->SetIndexFirstChangedBlock(numberOfBlocks);
    } else {
        // Boring layout is used for empty and hint text
        layout->DrawText(canvas, firstLine, lastLine);
    }
}

Int32 Editor::GetAvailableDisplayListIndex(
    /* [in] */ ArrayOf<Int32>* blockIndices,
    /* [in] */ Int32 numberOfBlocks,
    /* [in] */ Int32 searchStartIndex)
{
    Int32 length = mTextDisplayLists->GetLength();
    for (Int32 i = searchStartIndex; i < length; i++) {
        Boolean blockIndexFound = FALSE;
        for (Int32 j = 0; j < numberOfBlocks; j++) {
            if ((*blockIndices)[j] == i) {
                blockIndexFound = TRUE;
                break;
            }
        }
        if (blockIndexFound) continue;
        return i;
    }

    // No available index found, the pool has to grow
    mTextDisplayLists = GrowingArrayUtils::Append(mTextDisplayLists, length, (TextDisplayList*)NULL);
    return length;
}

void Editor::DrawCursor(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 cursorOffsetVertical)
{
    Boolean translate = cursorOffsetVertical != 0;
    if (translate) canvas->Translate(0, cursorOffsetVertical);
    for (Int32 i = 0; i < mCursorCount; i++) {
        (*mCursorDrawable)[i]->Draw(canvas);
    }
    if (translate) canvas->Translate(0, -cursorOffsetVertical);
}

void Editor::InvalidateTextDisplayList(
    /* [in] */ ILayout* layout,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (mTextDisplayLists != NULL && IDynamicLayout::Probe(layout)) {
        Int32 firstLine, lastLine;
        layout->GetLineForOffset(start, &firstLine);
        layout->GetLineForOffset(end, &lastLine);

        IDynamicLayout* dynamicLayout = IDynamicLayout::Probe(layout);
        AutoPtr<ArrayOf<Int32> > blockEndLines, blockIndices;
        dynamicLayout->GetBlockEndLines((ArrayOf<Int32>**)&blockEndLines);
        dynamicLayout->GetBlockIndices((ArrayOf<Int32>**)&blockIndices);
        Int32 numberOfBlocks;
        dynamicLayout->GetNumberOfBlocks(&numberOfBlocks);

        Int32 i = 0;
        // Skip the blocks before firstLine
        while (i < numberOfBlocks) {
            if ((*blockEndLines)[i] >= firstLine) break;
            i++;
        }

        // Invalidate all subsequent blocks until lastLine is passed
        while (i < numberOfBlocks) {
            Int32 blockIndex = (*blockIndices)[i];
            if (blockIndex != DynamicLayout::INVALID_BLOCK_INDEX) {
                (*mTextDisplayLists)[blockIndex]->mIsDirty = TRUE;
            }
            if ((*blockEndLines)[i] >= lastLine) break;
            i++;
        }
    }
}

void Editor::InvalidateTextDisplayList()
{
    if (mTextDisplayLists != NULL) {
        for (Int32 i = 0; i < mTextDisplayLists->GetLength(); i++) {
            if ((*mTextDisplayLists)[i] != NULL)
                (*mTextDisplayLists)[i]->mIsDirty = TRUE;
        }
    }
}

void Editor::UpdateCursorsPositions()
{
    if (mTextView->mCursorDrawableRes == 0) {
        mCursorCount = 0;
        return;
    }

    AutoPtr<ILayout> layout;
    mTextView->GetLayout((ILayout**)&layout);
    AutoPtr<ILayout> hintLayout = mTextView->GetHintLayout();
    Int32 offset, line;
    mTextView->GetSelectionStart(&offset);
    layout->GetLineForOffset(offset, &line);
    Int32 top, bottom;
    layout->GetLineTop(line, &top);
    layout->GetLineTop(line + 1, &bottom);

    Boolean result;
    layout->IsLevelBoundary(offset, &result);
    mCursorCount = result ? 2 : 1;

    Int32 middle = bottom;
    if (mCursorCount == 2) {
        // Similar to what is done in {@link Layout.#getCursorPath(Int32, Path, CharSequence)}
        middle = (top + bottom) >> 1;
    }

    Boolean clamped;
    layout->ShouldClampCursor(line, &clamped);
    UpdateCursorPosition(0, top, middle,
                GetPrimaryHorizontal(layout, hintLayout, offset, clamped));

    if (mCursorCount == 2) {
        Float hor;
        layout->GetSecondaryHorizontal(offset, clamped, &hor);
        UpdateCursorPosition(1, middle, bottom, hor);
    }
}

Float Editor::GetPrimaryHorizontal(
    /* [in] */ ILayout* layout,
    /* [in] */ ILayout* hintLayout,
    /* [in] */ Int32 offset,
    /* [in] */ Boolean clamped)
{
    Float value = 0.0;
    VALIDATE_NOT_NULL(layout);
    AutoPtr<ICharSequence> text;
    layout->GetText((ICharSequence**)&text);
    if (TextUtils::IsEmpty(text) && hintLayout != NULL) {
        text = NULL;
        hintLayout->GetText((ICharSequence**)&text);
        if (!TextUtils::IsEmpty(text)) {
            hintLayout->GetPrimaryHorizontal(offset, clamped, &value);
            return value;
        }
    }

    layout->GetPrimaryHorizontal(offset, clamped, &value);
    return value;
}

Boolean Editor::StartSelectionActionMode()
{
    if (mSelectionActionMode != NULL) {
        // Selection action mode is already started
        return FALSE;
    }

    Boolean requestFocus;
    if (!CanSelectText() || (mTextView->RequestFocus(&requestFocus), !requestFocus)) {
//        Log.w(TextView.LOG_TAG,
//                "TextView does not support text selection. Action mode cancelled.");
        return FALSE;
    }

    if ((mTextView->HasSelection(&requestFocus), !requestFocus)) {
        // There may already be a selection on device rotation
        if (!SelectCurrentWord()) {
            // No word found under cursor or text selection not permitted.
            return FALSE;
        }
    }

    Boolean willExtract = ExtractedTextModeWillBeStarted();

    // Do not start the action mode when extracted text will show up full screen, which would
    // immediately hide the newly created action bar and would be visually distracting.
    if (!willExtract) {
        AutoPtr<IActionModeCallback> actionModeCallback = new SelectionActionModeCallback(this);
        mTextView->StartActionMode(actionModeCallback, (IActionMode**)&mSelectionActionMode);
    }

    Boolean selectionStarted = mSelectionActionMode != NULL || willExtract;
    Boolean isTextSelectable;
    if (selectionStarted && (mTextView->IsTextSelectable(&isTextSelectable), !isTextSelectable)
        && mShowSoftInputOnFocus) {
        // Show the IME to be able to replace text, except when selecting non editable text.
        AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
        if (imm != NULL) {
            imm->ShowSoftInput(IVIEW_PROBE(mTextView), 0, NULL);
        }
    }

    return selectionStarted;
}

Boolean Editor::ExtractedTextModeWillBeStarted()
{
    if (NULL == mTextView->Probe(EIID_IExtractEditText)) {
        AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
        if (imm != NULL) {
            Boolean fullScreen;
            imm->IsFullscreenMode(&fullScreen);
            return fullScreen;
        }
    }
    return FALSE;
}

Boolean Editor::IsCursorInsideSuggestionSpan()
{
    AutoPtr<ICharSequence> text;
    mTextView->GetText((ICharSequence**)&text);
    ISpannable* spannable = ISpannable::Probe(text);
    if (!spannable) return FALSE;

    AutoPtr<ArrayOf<IInterface*> > suggestionSpans;
    Int32 selStart, selEnd;
    mTextView->GetSelectionStart(&selStart);
    mTextView->GetSelectionEnd(&selEnd);
    ISpanned::Probe(spannable)->GetSpans(selStart, selEnd,
            EIID_ISuggestionSpan, (ArrayOf<IInterface*>**)&suggestionSpans);
    return (suggestionSpans && suggestionSpans->GetLength() > 0);
}

Boolean Editor::IsCursorInsideEasyCorrectionSpan()
{
    AutoPtr<ICharSequence> text;
    mTextView->GetText((ICharSequence**)&text);
    ISpannable* spannable = ISpannable::Probe(text);
    if (spannable) {
        Int32 selStart, selEnd;
        mTextView->GetSelectionStart(&selStart);
        mTextView->GetSelectionEnd(&selEnd);
        AutoPtr<ArrayOf<IInterface*> > temp;
        ISpanned::Probe(spannable)->GetSpans(selStart, selEnd,
                EIID_ISuggestionSpan, (ArrayOf<IInterface*>**)&temp);
        AutoPtr<ArrayOf<ISuggestionSpan*> > suggestionSpans = ArrayOf<ISuggestionSpan*>::Alloc(temp->GetLength());
        for(Int32 i = 0; i < temp->GetLength(); i++) {
            suggestionSpans->Set(i, ISuggestionSpan::Probe((*temp)[i]));
        }

        if (suggestionSpans != NULL) {
            Int32 flags;
            for (Int32 i = 0; i < suggestionSpans->GetLength(); i++) {
                (*suggestionSpans)[i]->GetFlags(&flags);
                if ((flags & ISuggestionSpan::FLAG_EASY_CORRECT) != 0) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

void Editor::OnTouchUpEvent(
    /* [in] */ IMotionEvent* event)
{
    Boolean isTouch;
    Boolean selectAllGotFocus = mSelectAllOnFocus && (mTextView->DidTouchFocusSelect(&isTouch), isTouch);
    HideControllers();
    AutoPtr<ICharSequence> text;
    mTextView->GetText((ICharSequence**)&text);
    Int32 length;
    text->GetLength(&length);
    if (!selectAllGotFocus && length > 0) {
        Float x, y;
        event->GetX(&x);
        event->GetX(&y);
        // Move cursor
        Int32 offset;
        mTextView->GetOffsetForPosition(x, y, &offset);
        Selection::SetSelection(ISpannable::Probe(text), offset);

        if (mSpellChecker != NULL) {
            // When the cursor moves, the word that was typed may need spell check
            mSpellChecker->OnSelectionChanged();
        }
        if (!ExtractedTextModeWillBeStarted()) {

            if (IsCursorInsideEasyCorrectionSpan()) {
                mShowSuggestionRunnable = new ShowSuggestionRunnable(this);

                // removeCallbacks is performed on every touch
                AutoPtr<IViewConfigurationHelper> helper;
                CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
                Int32 doubleTapTimeout;
                helper->GetDoubleTapTimeout(&doubleTapTimeout);
                Boolean res;
                mTextView->PostDelayed(mShowSuggestionRunnable, doubleTapTimeout, &res);
            }
            else if (HasInsertionController()) {
                GetInsertionController()->Show();
            }
        }
    }
}

void Editor::StopSelectionActionMode()
{
    if (mSelectionActionMode != NULL) {
        // This will hide the mSelectionModifierCursorController
        mSelectionActionMode->Finish();
    }
}

Boolean Editor::HasInsertionController()
{
    return mInsertionControllerEnabled;
}

Boolean Editor::HasSelectionController()
{
    return mSelectionControllerEnabled;
}

AutoPtr<InsertionPointCursorController> Editor::GetInsertionController()
{
    if (!mInsertionControllerEnabled) {
        return NULL;
    }

    if (mInsertionPointCursorController == NULL) {
        mInsertionPointCursorController = new InsertionPointCursorController(this);

        AutoPtr<IViewTreeObserver> observer;
        mTextView->GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->AddOnTouchModeChangeListener(mInsertionPointCursorController);
    }

    return mInsertionPointCursorController;
}

AutoPtr<SelectionModifierCursorController> Editor::GetSelectionController()
{
    if (!mSelectionControllerEnabled) {
        return NULL;
    }

    if (mSelectionModifierCursorController == NULL) {
        mSelectionModifierCursorController = new SelectionModifierCursorController(this);

        AutoPtr<IViewTreeObserver> observer;
        mTextView->GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->AddOnTouchModeChangeListener(mSelectionModifierCursorController);
    }

    return mSelectionModifierCursorController;
}

void Editor::UpdateCursorPosition(
    /* [in] */ Int32 cursorIndex,
    /* [in] */ Int32 top,
    /* [in] */ Int32 bottom,
    /* [in] */ Float horizontal)
{
    if ((*mCursorDrawable)[cursorIndex] == NULL) {
        AutoPtr<IContext> ctx;
        mTextView->GetContext((IContext**)&ctx);
        AutoPtr<IDrawable> cursor;
        ctx->GetDrawable(mTextView->mCursorDrawableRes, (IDrawable**)&cursor);
        mCursorDrawable->Set(cursorIndex, cursor);
    }

    if (mTempRect == NULL)
        CRect::New((IRect**)&mTempRect);

    Boolean isPadding;
    (*mCursorDrawable)[cursorIndex]->GetPadding(mTempRect, &isPadding);
    Int32 width;
    (*mCursorDrawable)[cursorIndex]->GetIntrinsicWidth(&width);
    horizontal = Elastos::Core::Math::Max(0.5f, horizontal - 0.5f);

    Int32 l, t, r, b;
    mTempRect->Get(&l, &t, &r, &b);
    Int32 left = (Int32) (horizontal) - l;
    (*mCursorDrawable)[cursorIndex]->SetBounds(left, top - t, left + width,
            bottom + b);
}

ECode Editor::OnCommitCorrection(
    /* [in] */ ICorrectionInfo* info)
{
    if (mCorrectionHighlighter == NULL) {
        mCorrectionHighlighter = new CorrectionHighlighter(this);
    } else {
        mCorrectionHighlighter->Invalidate(FALSE);
    }

    mCorrectionHighlighter->Highlight(info);
    return NOERROR;
}

void Editor::ShowSuggestions()
{
    if (mSuggestionsPopupWindow == NULL) {
        mSuggestionsPopupWindow = new SuggestionsPopupWindow(this);
        mSuggestionsPopupWindow->Init();
    }
    HideControllers();
    mSuggestionsPopupWindow->Show();
}

Boolean Editor::AreSuggestionsShown()
{
    return mSuggestionsPopupWindow != NULL && mSuggestionsPopupWindow->IsShowing();
}

void Editor::OnScrollChanged()
{
    if (mPositionListener != NULL) {
        mPositionListener->OnScrollChanged();
    }
}

Boolean Editor::ShouldBlink()
{
    Boolean isFocused;
    if (!IsCursorVisible() || (mTextView->IsFocused(&isFocused), !isFocused)) return FALSE;

    Int32 start;
    mTextView->GetSelectionStart(&start);
    if (start < 0) return FALSE;

    Int32 end;
    mTextView->GetSelectionEnd(&end);
    if (end < 0) return FALSE;

    return start == end;
}

void Editor::MakeBlink()
{
    if (ShouldBlink()) {
        mShowCursor = SystemClock::GetUptimeMillis();
        if (mBlink == NULL) {
            mBlink = new Blink(this);
        }

        Boolean result = FALSE;
        mBlink->RemoveCallbacks(mBlink);
        mBlink->PostAtTime(mBlink, mShowCursor + BLINK, &result);
    }
    else {
        if (mBlink != NULL) {
            mBlink->RemoveCallbacks(mBlink);
        }
    }
}

AutoPtr<IDragShadowBuilder> Editor::GetTextThumbnailBuilder(
    /* [in] */ ICharSequence* textIn)
{
    AutoPtr<IContext> context;
    mTextView->GetContext((IContext**)&context);
    AutoPtr<IView> view;
    View::View::Inflate(context, R::layout::text_drag_thumbnail, NULL, (IView**)&view);
    AutoPtr<ITextView> shadowView = ITextView::Probe(view);

    if (shadowView == NULL) {
        return NULL;
//        throw new IllegalArgumentException("Unable to inflate text drag thumbnail");
    }
    Int32 length;
    textIn->GetLength(&length);

    AutoPtr<ICharSequence> text = textIn;
    if (length > DRAG_SHADOW_MAX_TEXT_LENGTH) {
        text = NULL;
        textIn->SubSequence(0, DRAG_SHADOW_MAX_TEXT_LENGTH, (ICharSequence**)&text);
    }
    shadowView->SetText(text);
    AutoPtr<IColorStateList> list;
    mTextView->GetTextColors((IColorStateList**)&list);
    shadowView->SetTextColor(list);
    AutoPtr<IContext> ctx;
    mTextView->GetContext((IContext**)&ctx);
    shadowView->SetTextAppearance(ctx, R::styleable::Theme_textAppearanceLarge);
    shadowView->SetGravity(IGravity::CENTER);

    AutoPtr<IViewGroupLayoutParams> layout;
    CViewGroupLayoutParams::New(
            IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT,
            (IViewGroupLayoutParams**)&layout);
    IView::Probe(shadowView)->SetLayoutParams(layout);

    Int32 size = View::View::MeasureSpec::MakeMeasureSpec(0, View::View::MeasureSpec::UNSPECIFIED);
    IView::Probe(shadowView)->Measure(size, size);

    Int32 mw, mh;
    IView::Probe(shadowView)->GetMeasuredWidth(&mw);
    IView::Probe(shadowView)->GetMeasuredHeight(&mh);
    IView::Probe(shadowView)->Layout(0, 0, mw, mh);
    IView::Probe(shadowView)->Invalidate();

    AutoPtr<IDragShadowBuilder> builder;
    CDragShadowBuilder::New(IView::Probe(shadowView), (IDragShadowBuilder**)&builder);
    return builder;
}

void Editor::OnDrop(
    /* [in] */ IDragEvent* event)
{
    assert(event);

    AutoPtr<IContext> context;
    mTextView->GetContext((IContext**)&context);
    StringBuilder content("");
    AutoPtr<IClipData> clipData;
    event->GetClipData((IClipData**)&clipData);
    Int32 itemCount;
    clipData->GetItemCount(&itemCount);
    AutoPtr<IClipDataItem> item;
    AutoPtr<ICharSequence> seq;
    for (Int32 i=0; i < itemCount; i++) {
        clipData->GetItemAt(i, (IClipDataItem**)&item);
        item->CoerceToStyledText(context, (ICharSequence**)&seq);
        content.Append(seq);
        seq = NULL;
        item = NULL;
    }

    Float x, y;
    event->GetX(&x);
    event->GetY(&y);
    Int32 offset;
    mTextView->GetOffsetForPosition(x, y, &offset);

    AutoPtr<IInterface> localState;
    event->GetLocalState((IInterface**)&localState);
    AutoPtr<DragLocalState> dragLocalState;
    if (IDragLocalState::Probe(localState)) {
        dragLocalState = (DragLocalState*)IDragLocalState::Probe(localState);
    }
    Boolean dragDropIntoItself = dragLocalState != NULL &&
            dragLocalState->mSourceTextView == mTextView;

    if (dragDropIntoItself) {
        if (offset >= dragLocalState->mStart && offset < dragLocalState->mEnd) {
            // A drop inside the original selection discards the drop.
            return;
        }
    }

    AutoPtr<ICharSequence> text;
    mTextView->GetText((ICharSequence**)&text);
    Int32 originalLength;
    text->GetLength(&originalLength);
    Int32 min = offset;
    Int32 max = offset;

    Selection::SetSelection(ISpannable::Probe(text), max);
    mTextView->ReplaceText_internal(min, max, content.ToCharSequence());

    if (dragDropIntoItself) {
        Int32 dragSourceStart = dragLocalState->mStart;
        Int32 dragSourceEnd = dragLocalState->mEnd;
        Int32 len;
        mTextView->GetLength(&len);
        if (max <= dragSourceStart) {
            // Inserting text before selection has shifted positions
            Int32 shift = len - originalLength;
            dragSourceStart += shift;
            dragSourceEnd += shift;
        }

        // Delete original selection
        mTextView->DeleteText_internal(dragSourceStart, dragSourceEnd);

        // Make sure we do not leave two adjacent spaces.
        Int32 prevCharIdx = Elastos::Core::Math::Max(0,  dragSourceStart - 1);
        Int32 nextCharIdx = Elastos::Core::Math::Min(len, dragSourceStart + 1);
        if (nextCharIdx > prevCharIdx + 1) {
            AutoPtr<ICharSequence> t = mTextView->GetTransformedText(prevCharIdx, nextCharIdx);
            Char32 char0, char1;
            t->GetCharAt(0, &char0);
            t->GetCharAt(1, &char1);
            if (Character::IsSpaceChar(char0) && Character::IsSpaceChar(char1)) {
                mTextView->DeleteText_internal(prevCharIdx, prevCharIdx + 1);
            }
        }
    }
}

ECode Editor::AddSpanWatchers(
    /* [in] */ ISpannable* text)
{
    assert(text != NULL);
    Int32 textLength;
    ICharSequence::Probe(text)->GetLength(&textLength);

    if (mKeyListener != NULL) {
        text->SetSpan(mKeyListener, 0, textLength, ISpanned::SPAN_INCLUSIVE_INCLUSIVE);
    }

    if (mSpanController == NULL) {
        mSpanController = new SpanController(this);
    }
    text->SetSpan(mSpanController->Probe(EIID_IInterface), 0, textLength, ISpanned::SPAN_INCLUSIVE_INCLUSIVE);
    return NOERROR;
}

////////////////////////////////////////////////////////////
//              Editor::SpanController
////////////////////////////////////////////////////////////
const Int32 Editor::SpanController::DISPLAY_TIMEOUT_MS;

CAR_INTERFACE_IMPL(Editor::SpanController, Object, ISpanWatcher)

Editor::SpanController::SpanController(
    /* [in] */ Editor* host)
    : mHost(host)
{}

ECode Editor::SpanController::OnSpanAdded(
    /* [in] */ ISpannable* text,
    /* [in] */ IInterface* span,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (IsNonIntermediateSelectionSpan(text, span)) {
        mHost->SendUpdateSelection();
    } else if (IEasyEditSpan::Probe(span)) {
        if (mPopupWindow == NULL) {
            mPopupWindow = new EasyEditPopupWindow(mHost);
            mHidePopup = new SpanControllerRunnable(this);
        }

        // Make sure there is only at most one EasyEditSpan in the text
        if (mPopupWindow->mEasyEditSpan != NULL) {
            mPopupWindow->mEasyEditSpan->SetDeleteEnabled(FALSE);
        }

        mPopupWindow->SetEasyEditSpan(IEasyEditSpan::Probe(span));
        AutoPtr<IEasyEditDeleteListener> listener = new SpanControllerListener(this);
        mPopupWindow->SetOnDeleteListener(listener);

        Int32 visible;
        mHost->mTextView->GetWindowVisibility(&visible);
        if (visible != IView::VISIBLE) {
            // The window is not visible yet, ignore the text change.
            return NOERROR;
        }

        AutoPtr<ILayout> layout;
        mHost->mTextView->GetLayout((ILayout**)&layout);
        if (layout == NULL) {
            // The view has not been laid out yet, ignore the text change
            return NOERROR;
        }

        if (mHost->ExtractedTextModeWillBeStarted()) {
            // The input is in extract mode. Do not handle the easy edit in
            // the original TextView, as the ExtractEditText will do
            return NOERROR;
        }

        mPopupWindow->Show();
        Boolean res;
        mHost->mTextView->RemoveCallbacks(mHidePopup, &res);
        mHost->mTextView->PostDelayed(mHidePopup, DISPLAY_TIMEOUT_MS, &res);
    }

    return NOERROR;
}

ECode Editor::SpanController::OnSpanRemoved(
    /* [in] */ ISpannable* text,
    /* [in] */ IInterface* span,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (IsNonIntermediateSelectionSpan(text, span)) {
        mHost->SendUpdateSelection();
    } else if (mPopupWindow != NULL && span == IInterface::Probe(mPopupWindow->mEasyEditSpan)) {
        Hide();
    }
    return NOERROR;
}

ECode Editor::SpanController::OnSpanChanged(
    /* [in] */ ISpannable* text,
    /* [in] */ IInterface* span,
    /* [in] */ Int32 previousStart,
    /* [in] */ Int32 previousEnd,
    /* [in] */ Int32 newStart,
    /* [in] */ Int32 newEnd)
{
    if (IsNonIntermediateSelectionSpan(text, span)) {
        mHost->SendUpdateSelection();
    } else if (mPopupWindow != NULL && IEasyEditSpan::Probe(span)) {
        AutoPtr<IEasyEditSpan> easyEditSpan = IEasyEditSpan::Probe(span);
        SendEasySpanNotification(IEasyEditSpan::TEXT_MODIFIED, easyEditSpan);
        text->RemoveSpan(easyEditSpan);
    }
    return NOERROR;
}

ECode Editor::SpanController::Hide()
{
    if (mPopupWindow != NULL) {
        mPopupWindow->Hide();
        Boolean res;
        mHost->mTextView->RemoveCallbacks(mHidePopup, &res);
    }
    return NOERROR;
}

Boolean Editor::SpanController::IsNonIntermediateSelectionSpan(
    /* [in] */ ISpannable* text,
    /* [in] */ IInterface* span)
{
    Int32 flags;
    return (IInterface::Probe(Selection::SELECTION_START) == IInterface::Probe(span)
        || IInterface::Probe(Selection::SELECTION_END) == IInterface::Probe(span))
        && ((ISpanned::Probe(text)->GetSpanFlags(IInterface::Probe(span), &flags), flags)
        & ISpanned::SPAN_INTERMEDIATE) == 0;
}

void Editor::SpanController::SendEasySpanNotification(
    /* [in] */ Int32 textChangedType,
    /* [in] */ IEasyEditSpan* span)
{
    //try {
        AutoPtr<IPendingIntent> pendingIntent;
        span->GetPendingIntent((IPendingIntent**)&pendingIntent);
        if (pendingIntent != NULL) {
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->PutExtra(IEasyEditSpan::EXTRA_TEXT_CHANGED_TYPE, textChangedType);
            AutoPtr<IContext> ctx;
            mHost->mTextView->GetContext((IContext**)&ctx);
            pendingIntent->Send(ctx, 0, intent);
        }
    //} catch (CanceledException e) {
        // This should not happen, as we should try to send the intent only once.
    //    Log.w(TAG, "PendingIntent for notification cannot be sent", e);
    //}
}

////////////////////////////////////////////////////////////
//             Editor::CursorAnchorInfoNotifier
////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(Editor::CursorAnchorInfoNotifier, Object, ITextViewPositionListener)

Editor::CursorAnchorInfoNotifier::CursorAnchorInfoNotifier(
    /* [in] */ Editor* editor)
    : mHost(editor)
{
    mSelectionInfoBuilder = new CCursorAnchorInfo::Builder();
    mTmpIntOffset = ArrayOf<Int32>::Alloc(2);
    CMatrix::New((IMatrix**)&mViewToScreenMatrix);
}

ECode Editor::CursorAnchorInfoNotifier::UpdatePosition(
    /* [in] */ Int32 parentPositionX,
    /* [in] */ Int32 parentPositionY,
    /* [in] */ Boolean parentPositionChanged,
    /* [in] */ Boolean parentScrolled)
{
    AutoPtr<InputMethodState> ims = mHost->mInputMethodState;
    if (ims == NULL || ims->mBatchEditNesting > 0) {
        return NOERROR;
    }
    AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
    if (NULL == imm) {
        return NOERROR;
    }
    Boolean isActive;
    if (imm->IsActive(IView::Probe(mHost->mTextView), &isActive), !isActive) {
        return NOERROR;
    }
    // Skip if the IME has not requested the cursor/anchor position.
    Boolean isCursorAnchorInfoEnabled;
    if (imm->IsCursorAnchorInfoEnabled(&isCursorAnchorInfoEnabled), !isCursorAnchorInfoEnabled) {
        return NOERROR;
    }
    AutoPtr<ILayout> layout;
    mHost->mTextView->GetLayout((ILayout**)&layout);
    if (layout == NULL) {
        return NOERROR;
    }

    AutoPtr<ICursorAnchorInfoBuilder> builder = mSelectionInfoBuilder;
    builder->Reset();

    Int32 selectionStart, selectionEnd;
    mHost->mTextView->GetSelectionStart(&selectionStart);
    mHost->mTextView->GetSelectionEnd(&selectionEnd);
    builder->SetSelectionRange(selectionStart, selectionEnd);

    // Construct transformation matrix from view local coordinates to screen coordinates.
    AutoPtr<IMatrix> matrix;
    mHost->mTextView->GetMatrix((IMatrix**)&matrix);
    mViewToScreenMatrix->Set(matrix);
    mHost->mTextView->GetLocationOnScreen(mTmpIntOffset);
    Boolean postTranslate;
    mViewToScreenMatrix->PostTranslate((*mTmpIntOffset)[0], (*mTmpIntOffset)[1], &postTranslate);
    builder->SetMatrix(mViewToScreenMatrix);

    Float viewportToContentHorizontalOffset =
            mHost->mTextView->ViewportToContentHorizontalOffset();
    Float viewportToContentVerticalOffset =
            mHost->mTextView->ViewportToContentVerticalOffset();

    AutoPtr<ICharSequence> text;
    mHost->mTextView->GetText((ICharSequence**)&text);
    if (ISpannable::Probe(text)) {
        AutoPtr<ISpannable> sp = ISpannable::Probe(text);
        Int32 composingTextStart = EditableInputConnection::GetComposingSpanStart(sp);
        Int32 composingTextEnd = EditableInputConnection::GetComposingSpanEnd(sp);
        if (composingTextEnd < composingTextStart) {
            Int32 temp = composingTextEnd;
            composingTextEnd = composingTextStart;
            composingTextStart = temp;
        }
        Boolean hasComposingText =
                (0 <= composingTextStart) && (composingTextStart < composingTextEnd);
        if (hasComposingText) {
            AutoPtr<ICharSequence> composingText;
            text->SubSequence(composingTextStart, composingTextEnd, (ICharSequence**)&composingText);
            builder->SetComposingText(composingTextStart, composingText);

            Int32 minLine, maxLine;
            layout->GetLineForOffset(composingTextStart, &minLine);
            layout->GetLineForOffset(composingTextEnd - 1, &maxLine);
            for (Int32 line = minLine; line <= maxLine; ++line) {
                Int32 lineStart, lineEnd;
                layout->GetLineStart(line, &lineStart);
                layout->GetLineEnd(line, &lineEnd);
                Int32 offsetStart = Elastos::Core::Math::Max(lineStart, composingTextStart);
                Int32 offsetEnd = Elastos::Core::Math::Min(lineEnd, composingTextEnd);
                Int32 direction;
                layout->GetParagraphDirection(line, &direction);
                Boolean ltrLine = direction == ILayout::DIR_LEFT_TO_RIGHT;
                AutoPtr< ArrayOf<Float> > widths = ArrayOf<Float>::Alloc(offsetEnd - offsetStart);
                AutoPtr<ITextPaint> paint;
                layout->GetPaint((ITextPaint**)&paint);
                IPaint::Probe(paint)->GetTextWidths(text, offsetStart, offsetEnd, widths, &direction);
                Int32 top, bottom;
                layout->GetLineTop(line, &top);
                layout->GetLineBottom(line, &bottom);

                for (Int32 offset = offsetStart; offset < offsetEnd; ++offset) {
                    Float charWidth = (*widths)[offset - offsetStart];
                    Boolean isRtl;
                    layout->IsRtlCharAt(offset, &isRtl);
                    Float primary, secondary;
                    layout->GetPrimaryHorizontal(offset, &primary);
                    layout->GetSecondaryHorizontal(offset, &secondary);
                    // TODO: This doesn't work perfectly for text with custom styles and
                    // TAB chars.
                    Float left, right;
                    if (ltrLine) {
                        if (isRtl) {
                            left = secondary - charWidth;
                            right = secondary;
                        } else {
                            left = primary;
                            right = primary + charWidth;
                        }
                    } else {
                        if (!isRtl) {
                            left = secondary;
                            right = secondary + charWidth;
                        } else {
                            left = primary - charWidth;
                            right = primary;
                        }
                    }
                    // TODO: Check top-right and bottom-left as well.
                    Float localLeft = left + viewportToContentHorizontalOffset;
                    Float localRight = right + viewportToContentHorizontalOffset;
                    Float localTop = top + viewportToContentVerticalOffset;
                    Float localBottom = bottom + viewportToContentVerticalOffset;
                    Boolean isTopLeftVisible = mHost->IsPositionVisible(localLeft, localTop);
                    Boolean isBottomRightVisible = mHost->IsPositionVisible(localRight, localBottom);
                    Int32 characterBoundsFlags = 0;
                    if (isTopLeftVisible || isBottomRightVisible) {
                        characterBoundsFlags |= ICursorAnchorInfo::FLAG_HAS_VISIBLE_REGION;
                    }
                    if (!isTopLeftVisible || !isTopLeftVisible) {
                        characterBoundsFlags |= ICursorAnchorInfo::FLAG_HAS_INVISIBLE_REGION;
                    }
                    if (isRtl) {
                        characterBoundsFlags |= ICursorAnchorInfo::FLAG_IS_RTL;
                    }
                    // Here offset is the index in Java chars.
                    builder->AddCharacterBounds(offset, localLeft, localTop, localRight,
                            localBottom, characterBoundsFlags);
                }
            }
        }
    }

    // Treat selectionStart as the insertion point.
    if (0 <= selectionStart) {
        Int32 offset = selectionStart;
        Int32 line;
        layout->GetLineForOffset(offset, &line);
        Float insertionMarkerX;
        layout->GetPrimaryHorizontal(offset, &insertionMarkerX);
        insertionMarkerX += viewportToContentHorizontalOffset;
        Int32 insertionMarkerTop;
        layout->GetLineTop(line, &insertionMarkerTop);
        insertionMarkerTop += viewportToContentVerticalOffset;
        Int32 insertionMarkerBaseline;
        layout->GetLineBaseline(line, &insertionMarkerBaseline);
        insertionMarkerBaseline += viewportToContentVerticalOffset;
        Int32 insertionMarkerBottom;
        layout->GetLineBottom(line, &insertionMarkerBottom);
        insertionMarkerBottom += viewportToContentVerticalOffset;
        Boolean isTopVisible = mHost->IsPositionVisible(insertionMarkerX, insertionMarkerTop);
        Boolean isBottomVisible = mHost->IsPositionVisible(insertionMarkerX, insertionMarkerBottom);
        Int32 insertionMarkerFlags = 0;
        if (isTopVisible || isBottomVisible) {
            insertionMarkerFlags |= ICursorAnchorInfo::FLAG_HAS_VISIBLE_REGION;
        }
        if (!isTopVisible || !isBottomVisible) {
            insertionMarkerFlags |= ICursorAnchorInfo::FLAG_HAS_INVISIBLE_REGION;
        }
        Boolean isRtlCharAt;
        if (layout->IsRtlCharAt(offset, &isRtlCharAt), isRtlCharAt) {
            insertionMarkerFlags |= ICursorAnchorInfo::FLAG_IS_RTL;
        }
        builder->SetInsertionMarkerLocation(insertionMarkerX, insertionMarkerTop,
                insertionMarkerBaseline, insertionMarkerBottom, insertionMarkerFlags);
    }
    AutoPtr<ICursorAnchorInfo> result;
    builder->Build((ICursorAnchorInfo**)&result);
    imm->UpdateCursorAnchorInfo(mHost->mTextView, result);
    return NOERROR;
}

////////////////////////////////////////////////////////////
//    Editor::SpanController::SpanControllerRunnable
////////////////////////////////////////////////////////////
Editor::SpanController::SpanControllerRunnable::SpanControllerRunnable(
    /* [in] */ SpanController* host)
    : mHost(host)
{}

ECode Editor::SpanController::SpanControllerRunnable::Run()
{
    mHost->Hide();
    return NOERROR;
}

////////////////////////////////////////////////////////////
//    Editor::SpanController::SpanControllerListener
////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(Editor::SpanController::SpanControllerListener, Object, IEasyEditDeleteListener)

Editor::SpanController::SpanControllerListener::SpanControllerListener(
    /* [in] */ SpanController* host)
    : mHost(host)
{}

ECode Editor::SpanController::SpanControllerListener::OnDeleteClick(
    /* [in] */ IEasyEditSpan* span)
{
    AutoPtr<ICharSequence> text;
    mHost->mHost->mTextView->GetText((ICharSequence**)&text);
    AutoPtr<IEditable> editable = IEditable::Probe(text);
    Int32 start, end;
    ISpanned::Probe(editable)->GetSpanStart(span, &start);
    ISpanned::Probe(editable)->GetSpanEnd(span, &end);
    if (start >= 0 && end >= 0) {
        mHost->SendEasySpanNotification(IEasyEditSpan::TEXT_DELETED, span);
        mHost->mHost->mTextView->DeleteText_internal(start, end);
    }
    ISpannable::Probe(editable)->RemoveSpan(span);
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
//                      UndoInputFilter
///////////////////////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(UndoInputFilter, Object, IInputFilter)

UndoInputFilter::UndoInputFilter(
    /* [in] */ Editor* editor)
    : mEditor(editor)
{
}

ECode UndoInputFilter::Filter(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISpanned* dest,
    /* [in] */ Int32 dstart,
    /* [in] */ Int32 dend,
    /* [out] */ ICharSequence** cs)
{
    VALIDATE_NOT_NULL(cs)
    // if (DEBUG_UNDO) {
    //     Log.d(TAG, "filter: source=" + source + " (" + start + "-" + end + ")");
    //     Log.d(TAG, "filter: dest=" + dest + " (" + dstart + "-" + dend + ")");
    // }
    AutoPtr<IUndoManager> um = mEditor->mUndoManager;
    Boolean isInUndo;
    if (um->IsInUndo(&isInUndo)) {
        //if (DEBUG_UNDO) Log.d(TAG, "*** skipping, currently performing undo/redo");
        *cs = NULL;
        return NOERROR;
    }
    String str("Edit text");
    AutoPtr<ICharSequence> text;
    CString::New(str, (ICharSequence**)&text);
    um->BeginUpdate(text);
    // TextModifyOperation op = um.getLastOperation(
    //         TextModifyOperation.class, mEditor.mUndoOwner, UndoManager.MERGE_MODE_UNIQUE);
    /*if (op != null) {
        if (DEBUG_UNDO) Log.d(TAG, "Last op: range=(" + op.mRangeStart + "-" + op.mRangeEnd
                + "), oldText=" + op.mOldText);
        // See if we can continue modifying this operation.
        if (op.mOldText == null) {
            // The current operation is an add...  are we adding more?  We are adding
            // more if we are either appending new text to the end of the last edit or
            // completely replacing some or all of the last edit.
            if (start < end && ((dstart >= op.mRangeStart && dend <= op.mRangeEnd)
                    || (dstart == op.mRangeEnd && dend == op.mRangeEnd))) {
                op.mRangeEnd = dstart + (end-start);
                um.endUpdate();
                if (DEBUG_UNDO) Log.d(TAG, "*** merging with last op, mRangeEnd="
                        + op.mRangeEnd);
                return null;
            }
        } else {
            // The current operation is a delete...  can we delete more?
            if (start == end && dend == op.mRangeStart-1) {
                SpannableStringBuilder str;
                if (op.mOldText instanceof SpannableString) {
                    str = (SpannableStringBuilder)op.mOldText;
                } else {
                    str = new SpannableStringBuilder(op.mOldText);
                }
                str.insert(0, dest, dstart, dend);
                op.mRangeStart = dstart;
                op.mOldText = str;
                um.endUpdate();
                if (DEBUG_UNDO) Log.d(TAG, "*** merging with last op, range=("
                        + op.mRangeStart + "-" + op.mRangeEnd
                        + "), oldText=" + op.mOldText);
                return null;
            }
        }*/

        // Couldn't add to the current undo operation, need to start a new
        // undo state for a new undo operation.
    //     Int32 commitState;
    //     um->CommitState((IUndoOwner*)NULL, &commitState);
    //     um->SetUndoLabel(text);
    // }

    // Create a new undo state reflecting the operation being performed.
    AutoPtr<TextModifyOperation> operation = new TextModifyOperation();
    operation->constructor(mEditor->mUndoOwner);
    operation->mRangeStart = dstart;
    if (start < end) {
        operation->mRangeEnd = dstart + (end-start);
    } else {
        operation->mRangeEnd = dstart;
    }
    if (dstart < dend) {
        ICharSequence::Probe(dest)->SubSequence(dstart, dend, (ICharSequence**)&operation->mOldText);
    }
    // if (DEBUG_UNDO) Log.d(TAG, "*** adding new op, range=(" + op.mRangeStart
    //         + "-" + op.mRangeEnd + "), oldText=" + op.mOldText);
    um->AddOperation(operation, IUndoManager::MERGE_MODE_NONE);
    um->EndUpdate();
    *cs = NULL;
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
//                      TextModifyOperation
///////////////////////////////////////////////////////////////////////

TextModifyOperation::TextModifyOperation()
    : mRangeStart(0)
    , mRangeEnd(0)
{
}

ECode TextModifyOperation::constructor(
    /* [in] */ IUndoOwner* owner)
{
    return UndoOperation::constructor(owner);
}

ECode TextModifyOperation::Commit()
{
    return NOERROR;
}

ECode TextModifyOperation::Undo()
{
    SwapText();
    return NOERROR;
}

ECode TextModifyOperation::Redo()
{
    SwapText();
    return NOERROR;
}

ECode TextModifyOperation::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mRangeStart);
    dest->WriteInt32(mRangeEnd);
    TextUtils::WriteToParcel(mOldText, dest);
    return NOERROR;
}

ECode TextModifyOperation::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mRangeStart);
    source->ReadInt32(&mRangeEnd);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mOldText);
    return NOERROR;
}

void TextModifyOperation::SwapText()
{
    AutoPtr<IInterface> obj;
    GetOwnerData((IInterface**)&obj);
    AutoPtr<TextView> tv = (TextView*)ITextView::Probe(obj);
    AutoPtr<ICharSequence> curText;
    tv->GetText((ICharSequence**)&curText);
    IEditable* editable = IEditable::Probe(curText);
    curText = NULL;
    if (mRangeStart >= mRangeEnd) {
        curText = NULL;
    } else {
        ICharSequence::Probe(editable)->SubSequence(mRangeStart, mRangeEnd, (ICharSequence**)&curText);
    }
    // if (DEBUG_UNDO) {
    //     Log.d(TAG, "Swap: range=(" + mRangeStart + "-" + mRangeEnd
    //             + "), oldText=" + mOldText);
    //     Log.d(TAG, "Swap: curText=" + curText);
    // }
    if (mOldText == NULL) {
        editable->Delete(mRangeStart, mRangeEnd);
        mRangeEnd = mRangeStart;
    } else {
        editable->Replace(mRangeStart, mRangeEnd, mOldText);
        Int32 length;
        mOldText->GetLength(&length);
        mRangeEnd = mRangeStart + length;
    }
    mOldText = curText;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
