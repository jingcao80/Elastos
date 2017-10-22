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

#ifndef __ELASTOS_DROID_WIDGET_SEARCHVIEW_H__
#define __ELASTOS_DROID_WIDGET_SEARCHVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/widget/AutoCompleteTextView.h"

using Elastos::Droid::App::IActionKeyInfo;
using Elastos::Droid::App::ISearchableInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewOnFocusChangeListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnLayoutChangeListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ICollapsibleActionView;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::Widget::ISearchViewOnQueryTextListener;
using Elastos::Droid::Widget::ISearchViewOnSuggestionListener;
using Elastos::Droid::Widget::ICursorAdapter;
using Elastos::Droid::Widget::ISearchViewOnCloseListener;
using Elastos::Droid::Widget::IAutoCompleteTextView;
using Elastos::Droid::Widget::ISearchView;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IWeakHashMap;

namespace Elastos {
namespace Droid {
namespace Widget {

class SearchView
    : public LinearLayout
    , public ISearchView
    , public ICollapsibleActionView
{
private:
    class ShowImeRunnable
        : public Runnable
    {
    public:
        ShowImeRunnable(
            /* [in] */ SearchView* host);

        CARAPI Run();

    private:
        SearchView* mHost;
    };

    class UpdateDrawableStateRunnable
        : public Runnable
    {
    public:
        UpdateDrawableStateRunnable(
            /* [in] */ SearchView* host);

        CARAPI Run();

    private:
        SearchView* mHost;
    };

    class ReleaseCursorRunnable
        : public Runnable
    {
    public:
        ReleaseCursorRunnable(
            /* [in] */ SearchView* host);

        CARAPI Run();

    private:
        SearchView* mHost;
    };

    class SearchViewClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        SearchViewClickListener(
            /* [in] */ SearchView* host);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        SearchView* mHost;
    };

    class SearchViewKeyListener
        : public Object
        , public IViewOnKeyListener
    {
    public:
        SearchViewKeyListener(
            /* [in] */ SearchView* host);

        CAR_INTERFACE_DECL()

        CARAPI OnKey(
            /* [in] */ IView* v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        SearchView* mHost;
    };

    class SearchViewEditorActionListener
        : public Object
        , public IOnEditorActionListener
    {
    public:
        SearchViewEditorActionListener(
            /* [in] */ SearchView* host);

        CAR_INTERFACE_DECL()

        CARAPI OnEditorAction(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 actionId,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        SearchView* mHost;
    };

    class SearchViewItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        SearchViewItemClickListener(
            /* [in] */ SearchView* host);

        CAR_INTERFACE_DECL()

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        SearchView* mHost;
    };

    class SearchViewItemSelectedListener
        : public Object
        , public IAdapterViewOnItemSelectedListener
    {
    public:
        SearchViewItemSelectedListener(
            /* [in] */ SearchView* host);

        CAR_INTERFACE_DECL()

        CARAPI OnItemSelected(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnNothingSelected(
            /* [in] */ IAdapterView* parent);

    private:
        SearchView* mHost;
    };

    class SearchViewTextWatcher
        : public Object
        , public ITextWatcher
    {
    public:
        SearchViewTextWatcher(
            /* [in] */ SearchView* host);

        CAR_INTERFACE_DECL()

        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 before,
            /* [in] */ Int32 count);

        CARAPI AfterTextChanged(
            /* [in] */ IEditable* s);

    private:
        SearchView* mHost;
    };

    class SearchViewFocusChangeListener
        : public Object
        , public IViewOnFocusChangeListener
    {
    public:
        SearchViewFocusChangeListener(
            /* [in] */ SearchView* host);

        CAR_INTERFACE_DECL()

        CARAPI OnFocusChange(
            /* [in] */ IView* v,
            /* [in] */ Boolean hasFocus);

    private:
        SearchView* mHost;
    };

    class SearchViewLayoutChangeListener
        : public Object
        , public IViewOnLayoutChangeListener
    {
    public:
        SearchViewLayoutChangeListener(
            /* [in] */ SearchView* host);

        CAR_INTERFACE_DECL()

        CARAPI OnLayoutChange(
            /* [in] */ IView* v,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ Int32 oldLeft,
            /* [in] */ Int32 oldTop,
            /* [in] */ Int32 oldRight,
            /* [in] */ Int32 oldBottom);

    private:
        SearchView* mHost;
    };

public:
    class _SearchAutoComplete
        : public AutoCompleteTextView
        , public ISearchViewSearchAutoComplete
    {
    public:
        CAR_INTERFACE_DECL()

        _SearchAutoComplete();

        CARAPI constructor(
            /* [in] */ IContext* context);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ Int32 defStyleAttrs);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ Int32 defStyleAttrs,
            /* [in] */ Int32 defStyleRes);

        CARAPI SetSearchView(
            /* [in] */ ISearchView* searchView);

        CARAPI SetThreshold(
            /* [in] */ Int32 threshold);

        CARAPI PerformCompletion();

        CARAPI OnWindowFocusChanged(
            /* [in] */ Boolean hasWindowFocus);

        CARAPI EnoughToFilter(
            /* [out] */ Boolean* result);

        CARAPI OnKeyPreIme(
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        CARAPI_(Boolean) IsEmpty();

    protected:
        CARAPI_(void) ReplaceText(
            /* [in] */ ICharSequence* text);

        CARAPI_(void) OnFocusChanged(
            /* [in] */ Boolean focused,
            /* [in] */ Int32 direction,
            /* [in] */ IRect* previouslyFocusedRect);

    private:
        Int32 mThreshold;
        AutoPtr<ISearchView> mSearchView;
        friend class SearchView;
    };

public:
    CAR_INTERFACE_DECL()

    SearchView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttrs,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetSearchableInfo(
        /* [in] */ ISearchableInfo* searchable);

    CARAPI SetAppSearchData(
        /* [in] */ IBundle* appSearchData);

    CARAPI SetImeOptions(
        /* [in] */ Int32 imeOptions);

    CARAPI GetImeOptions(
        /* [out] */ Int32* imeOptions);

    CARAPI SetInputType(
        /* [in] */ Int32 inputType);

    CARAPI GetInputType(
        /* [out] */ Int32* type);

    CARAPI RequestFocus(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result);

    CARAPI ClearFocus();

    CARAPI SetOnQueryTextListener(
        /* [in] */ ISearchViewOnQueryTextListener* listener);

    CARAPI SetOnCloseListener(
        /* [in] */ ISearchViewOnCloseListener* listener);

    CARAPI SetOnQueryTextFocusChangeListener(
        /* [in] */ IViewOnFocusChangeListener* listener);

    CARAPI SetOnSuggestionListener(
        /* [in] */ ISearchViewOnSuggestionListener* listener);

    CARAPI SetOnSearchClickListener(
        /* [in] */ IViewOnClickListener* listener);

    CARAPI GetQuery(
        /* [out] */ ICharSequence** str);

    CARAPI SetQuery(
        /* [in] */ ICharSequence* query,
        /* [in] */ Boolean submit);

    CARAPI SetQueryHint(
        /* [in] */ ICharSequence* hint);

    CARAPI GetQueryHint(
        /* [out] */ ICharSequence** hint);

    CARAPI SetIconifiedByDefault(
        /* [in] */ Boolean iconified);

    CARAPI IsIconfiedByDefault(
        /* [out] */ Boolean* result);

    CARAPI SetIconified(
        /* [in] */ Boolean iconify);

    CARAPI IsIconified(
        /* [out] */ Boolean* result);

    CARAPI SetSubmitButtonEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsSubmitButtonEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetQueryRefinementEnabled(
        /* [in] */ Boolean enable);

    CARAPI IsQueryRefinementEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetSuggestionsAdapter(
        /* [in] */ ICursorAdapter* adapter);

    CARAPI GetSuggestionsAdapter(
        /* [out] */ ICursorAdapter** adapter);

    CARAPI SetMaxWidth(
        /* [in] */ Int32 maxpixels);

    CARAPI GetMaxWidth(
        /* [out] */ Int32* max);

    CARAPI_(void) OnQueryRefine(
        /* [in] */ ICharSequence* queryText);

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) OnSuggestionsKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    CARAPI OnActionViewCollapsed();

    CARAPI OnActionViewExpanded();

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    CARAPI OnTextFocusChanged();

    static CARAPI_(Boolean) IsLandscapeMode(
        /* [in] */ IContext* context);

protected:
    CARAPI_(Int32) GetSuggestionRowLayout();

    CARAPI_(Int32) GetSuggestionCommitIconResId();

    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI OnDetachedFromWindow();

private:
    CARAPI_(Int32) GetPreferredWidth();

    CARAPI_(void) UpdateViewsVisibility(
        /* [in] */ Boolean collapsed);

    CARAPI_(Boolean) HasVoiceSearch();

    CARAPI_(Boolean) IsSubmitAreaEnabled();

    CARAPI_(void) UpdateSubmitButton(
        /* [in] */ Boolean hasText);

    CARAPI_(void) UpdateSubmitArea();

    CARAPI_(void) UpdateCloseButton();

    CARAPI_(void) PostUpdateFocusedState();

    CARAPI_(void) UpdateFocusedState();

    CARAPI_(void) SetImeVisibility(
        /* [in] */ Boolean visible);

    CARAPI_(String) GetActionKeyMessage(
        /* [in] */ ICursor* c,
        /* [in] */ IActionKeyInfo* actionKey);

    CARAPI_(AutoPtr<ICharSequence>) GetDecoratedHint(
        /* [in] */ ICharSequence* hintText);

    CARAPI_(void) UpdateQueryHint();

    CARAPI_(void) UpdateSearchAutoComplete();

    CARAPI_(void) UpdateVoiceButton(
        /* [in] */ Boolean empty);

    CARAPI_(void) OnTextChanged(
        /* [in] */ ICharSequence* newText);

    CARAPI_(void) OnSubmitQuery();

    CARAPI_(void) DismissSuggestions();

    CARAPI_(void) OnCloseClicked();

    CARAPI_(void) OnSearchClicked();

    CARAPI_(void) OnVoiceClicked();

    CARAPI_(void) AdjustDropDownSizeAndPosition();

    CARAPI_(Boolean) OnItemClicked(
        /* [in] */ Int32 position,
        /* [in] */ Int32 actionKey,
        /* [in] */ const String& actionMsg);

    CARAPI_(Boolean) OnItemSelected(
        /* [in] */ Int32 position);

    CARAPI_(void) RewriteQueryFromSuggestion(
        /* [in] */ Int32 position);

    CARAPI_(Boolean) LaunchSuggestion(
        /* [in] */ Int32 position,
        /* [in] */ Int32 actionKey,
        /* [in] */ const String& actionMsg);

    CARAPI_(void) LaunchIntent(
        /* [in] */ IIntent* intent);

    CARAPI_(void) SetQuery(
        /* [in] */ ICharSequence* query);

    CARAPI_(void) LaunchQuerySearch(
        /* [in] */ Int32 actionKey,
        /* [in] */ const String& actionMsg,
        /* [in] */ const String& query);

    CARAPI_(AutoPtr<IIntent>) CreateIntent(
        /* [in] */ const String& action,
        /* [in] */ IUri* data,
        /* [in] */ const String& extraData,
        /* [in] */ const String& query,
        /* [in] */ Int32 actionKey,
        /* [in] */ const String& actionMsg);

   CARAPI_(AutoPtr<IIntent>) CreateVoiceWebSearchIntent(
       /* [in] */ IIntent* baseIntent,
       /* [in] */ ISearchableInfo* searchable);

   CARAPI_(AutoPtr<IIntent>) CreateVoiceAppSearchIntent(
       /* [in] */ IIntent* baseIntent,
       /* [in] */ ISearchableInfo* searchable);

    CARAPI_(AutoPtr<IIntent>) CreateIntentFromSuggestion(
        /* [in] */ ICursor* c,
        /* [in] */ Int32 actionKey,
        /* [in] */ const String& actionMsg);

    CARAPI_(void) ForceSuggestionQuery();

private:
    static const Boolean DBG;
    static const String IME_OPTION_NO_MICROPHONE;
    static const String SEARCHVIEW_NAME;

    AutoPtr<ISearchViewSearchAutoComplete> mQueryTextView;
    AutoPtr<IView> mSearchEditFrame;
    AutoPtr<IView> mSearchPlate;
    AutoPtr<IView> mSubmitArea;
    AutoPtr<IImageView> mSearchButton;
    AutoPtr<IImageView> mSubmitButton;
    AutoPtr<IImageView> mCloseButton;
    AutoPtr<IImageView> mVoiceButton;
    AutoPtr<IImageView> mSearchHintIcon;
    AutoPtr<IView> mDropDownAnchor;
    Int32 mSearchIconResId;

    // Resources used by SuggestionsAdapter to display suggestions.
    Int32 mSuggestionRowLayout;
    Int32 mSuggestionCommitIconResId;

    // Intents used for voice searching.
    AutoPtr<IIntent> mVoiceWebSearchIntent;
    AutoPtr<IIntent> mVoiceAppSearchIntent;

    AutoPtr<ISearchViewOnQueryTextListener> mOnQueryChangeListener;
    AutoPtr<ISearchViewOnCloseListener> mOnCloseListener;
    AutoPtr<IViewOnFocusChangeListener> mOnQueryTextFocusChangeListener;
    AutoPtr<ISearchViewOnSuggestionListener> mOnSuggestionListener;
    AutoPtr<IViewOnClickListener> mOnSearchClickListener;

    Boolean mIconifiedByDefault;
    Boolean mIconified;
    AutoPtr<ICursorAdapter> mSuggestionsAdapter;
    Boolean mSubmitButtonEnabled;
    AutoPtr<ICharSequence> mQueryHint;
    Boolean mQueryRefinement;
    Boolean mClearingFocus;
    Int32 mMaxWidth;
    Boolean mVoiceButtonEnabled;
    AutoPtr<ICharSequence> mOldQueryText;
    AutoPtr<ICharSequence> mUserQuery;
    Boolean mExpandedInActionView;
    Int32 mCollapsedImeOptions;
    AutoPtr<ISearchableInfo> mSearchable;
    AutoPtr<IBundle> mAppSearchData;

    AutoPtr<IRunnable> mShowImeRunnable;
    AutoPtr<IRunnable> mUpdateDrawableStateRunnable;
    AutoPtr<IRunnable> mReleaseCursorRunnable;

    AutoPtr<IViewOnClickListener> mOnClickListener;
    AutoPtr<IViewOnKeyListener> mTextKeyListener;
    AutoPtr<IOnEditorActionListener> mOnEditorActionListener;
    AutoPtr<IAdapterViewOnItemClickListener> mOnItemClickListener;
    AutoPtr<IAdapterViewOnItemSelectedListener> mOnItemSelectedListener;
    AutoPtr<ITextWatcher> mTextWatcher;
    // HashMap<String, IDrawableConstantState*> mOutsideDrawablesCache;
    AutoPtr<IWeakHashMap> mOutsideDrawablesCache;
};

} //namespace Widget
} //namespace Droid
} //namespace Elastos

#endif
