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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/webkit/FindActionModeCallback.h"
#include "elastos/droid/webkit/CWebView.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::Selection;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::EIID_IActionModeCallback;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Core::IString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {

//===============================================================
//              FindActionModeCallback::NoAction
//===============================================================

CAR_INTERFACE_IMPL(FindActionModeCallback::NoAction, Object, IActionModeCallback);

ECode FindActionModeCallback::NoAction::OnCreateActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    if (result) *result = FALSE;
    return NOERROR;
}

ECode FindActionModeCallback::NoAction::OnPrepareActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    if (result) *result = FALSE;
    return NOERROR;
}

ECode FindActionModeCallback::NoAction::OnActionItemClicked(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    if (result) *result = FALSE;
    return NOERROR;
}

ECode FindActionModeCallback::NoAction::OnDestroyActionMode(
    /* [in] */ IActionMode* mode)
{
    return NOERROR;
}

ECode FindActionModeCallback::NoAction::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "FindActionModeCallback::NoAction";
    return NOERROR;
}

//===============================================================
//                   FindActionModeCallback::InnerListener
//===============================================================
CAR_INTERFACE_IMPL_2(FindActionModeCallback::InnerListener, Object, \
    IViewOnClickListener, IWebViewFindListener);

FindActionModeCallback::InnerListener::InnerListener(
    /* [in] */ FindActionModeCallback* host)
    : mHost(host)
{}

ECode FindActionModeCallback::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

ECode FindActionModeCallback::InnerListener::OnFindResultReceived(
    /* [in] */ Int32 activeMatchOrdinal,
    /* [in] */ Int32 numberOfMatches,
    /* [in] */ Boolean isDoneCounting)
{
    return mHost->OnFindResultReceived(activeMatchOrdinal, numberOfMatches, isDoneCounting);
}

//===============================================================
//                   FindActionModeCallback
//===============================================================

CAR_INTERFACE_IMPL_2(FindActionModeCallback, Object, IActionModeCallback, ITextWatcher);

FindActionModeCallback::FindActionModeCallback()
    : mMatchesFound(FALSE)
    , mNumberOfMatches(0)
    , mActiveMatchIndex(0)
{
}

ECode FindActionModeCallback::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<InnerListener> listener = new InnerListener(this);

    AutoPtr<ILayoutInflater> inflate;
    LayoutInflater::From(context, (ILayoutInflater**)&inflate);
    inflate->Inflate(R::layout::webview_find, NULL, (IView**)&mCustomView);
    AutoPtr<IView> editTextView;
    mCustomView->FindViewById(R::id::edit, (IView**)&editTextView);
    mEditText = IEditText::Probe(editTextView);
    AutoPtr<NoAction> action = new NoAction();
    ITextView::Probe(mEditText)->SetCustomSelectionActionModeCallback(action);
    editTextView->SetOnClickListener(listener);
    SetText(String(""));
    AutoPtr<IView> matchesView;
    mCustomView->FindViewById(R::id::matches, (IView**)&matchesView);
    mMatches = ITextView::Probe(matchesView);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    mInput = IInputMethodManager::Probe(obj);
    return context->GetResources((IResources**)&mResources);
}

void FindActionModeCallback::Finish()
{
    mActionMode->Finish();
}

/*
 * Place text in the text field so it can be searched for.  Need to press
 * the find next or find previous button to find all of the matches.
 */
void FindActionModeCallback::SetText(
    /* [in] */ const String& text)
{
    AutoPtr<ICharSequence> textWrapper;
    CString::New(text, (ICharSequence**)&textWrapper);
    ITextView::Probe(mEditText)->SetText(textWrapper);
    AutoPtr<ICharSequence> spancs;
    ITextView::Probe(mEditText)->GetText((ICharSequence**)&spancs);
    ISpannable* span = ISpannable::Probe(spancs);
    Int32 length;
    ICharSequence::Probe(span)->GetLength(&length);
    // Ideally, we would like to set the selection to the whole field,
    // but this brings up the Text selection CAB, which dismisses this
    // one.
    Selection::SetSelection(span, length, length);
    // Necessary each time we set the text, so that this will watch
    // changes to it.
    span->SetSpan(TO_IINTERFACE(this), 0, length, ISpanned::SPAN_INCLUSIVE_INCLUSIVE);
    mMatchesFound = FALSE;
}

/*
 * Set the WebView to search.  Must be non null, and set before calling
 * startActionMode.
 */
ECode FindActionModeCallback::SetWebView(
    /* [in] */ CWebView* webView)
{
    if (NULL == webView) {
        return E_ASSERTION_ERROR;
    }
    mWebView = webView;
    AutoPtr<InnerListener> listener = new InnerListener(this);
    mWebView->SetFindDialogFindListener(listener);
    return NOERROR;
}

ECode FindActionModeCallback::OnFindResultReceived(
    /* [in] */ Int32 activeMatchOrdinal,
    /* [in] */ Int32 numberOfMatches,
    /* [in] */ Boolean isDoneCounting)
{
    if (isDoneCounting) {
        UpdateMatchCount(activeMatchOrdinal, numberOfMatches, numberOfMatches == 0);
    }

    return NOERROR;
}

/*
 * Highlight all the instances of the string from mEditText in mWebView.
 */
void FindActionModeCallback::FindAll()
{
    if (mWebView == NULL) {
        //throw new AssertionError(
        //        "No WebView for FindActionModeCallback::findAll");
        Logger::E("FindActionModeCallback::FindAll", "No WebView for FindActionModeCallback::findAll");
        assert(0);
        return;
    }

    AutoPtr<ICharSequence> find;
    ITextView::Probe(mEditText)->GetText((ICharSequence**)&find);
    Int32 length = 0;
    find->GetLength(&length);
    if (0 == length) {
        mWebView->ClearMatches();
        IView::Probe(mMatches)->SetVisibility(IView::GONE);
        mMatchesFound = FALSE;
        Int32 all;
        mWebView->FindAll(String(NULL), &all);
    }
    else {
        mMatchesFound = TRUE;
        IView::Probe(mMatches)->SetVisibility(IView::INVISIBLE);
        mNumberOfMatches = 0;
        String str;
        find->ToString(&str);
        mWebView->FindAllAsync(str);
    }
}

void FindActionModeCallback::ShowSoftInput()
{
    AutoPtr<IView> view;
    IView::Probe(mEditText)->GetRootView((IView**)&view);
    mInput->StartGettingWindowFocus(view);
    mInput->FocusIn(IView::Probe(mEditText));
    Boolean result;
    mInput->ShowSoftInput(IView::Probe(mEditText), 0, &result);
}

void FindActionModeCallback::UpdateMatchCount(
    /* [in] */ Int32 matchIndex,
    /* [in] */ Int32 matchCount,
    /* [in] */ Boolean isEmptyFind)
{
    if (!isEmptyFind) {
        mNumberOfMatches = matchCount;
        mActiveMatchIndex = matchIndex;
        UpdateMatchesString();
    }
    else {
        IView::Probe(mMatches)->SetVisibility(IView::GONE);
        mNumberOfMatches = 0;
    }
}

// OnClickListener implementation

ECode FindActionModeCallback::OnClick(
    /* [in] */ IView* v)
{
    FindNext(TRUE);
    return NOERROR;
}

// ActionMode.Callback implementation

ECode FindActionModeCallback::OnCreateActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(mode);
    VALIDATE_NOT_NULL(menu);

    Boolean bFlag = FALSE;
    mode->IsUiFocusable(&bFlag);
    if (!bFlag) {
        // If the action mode we're running in is not focusable the user
        // will not be able to type into the find on page field. This
        // should only come up when we're running in a dialog which is
        // already less than ideal; disable the option for now.
        return FALSE;
    }

    mode->SetCustomView(mCustomView);
    AutoPtr<IMenuInflater> menuInflater;
    mode->GetMenuInflater((IMenuInflater**)&menuInflater);
    menuInflater->Inflate(R::menu::webview_find, menu);
    mActionMode = mode;
    AutoPtr<ICharSequence> editcs;
    ITextView::Probe(mEditText)->GetText((ICharSequence**)&editcs);
    IEditable* edit = IEditable::Probe(editcs);
    Int32 length;
    ICharSequence::Probe(edit)->GetLength(&length);
    Selection::SetSelection(ISpannable::Probe(edit), length);
    IView::Probe(mMatches)->SetVisibility(IView::GONE);
    mMatchesFound = FALSE;
    AutoPtr<ICharSequence> text;
    CString::New(String("0"), (ICharSequence**)&text);
    mMatches->SetText(text);
    //don't find RequestFocus
    IView::Probe(mEditText)->RequestFocus(NULL);

    if (result) {
        *result = TRUE;
    }

    return NOERROR;
}

ECode FindActionModeCallback::OnDestroyActionMode(
    /* [in] */ IActionMode* mode)
{
    mActionMode = NULL;
    mWebView->NotifyFindDialogDismissed();
    mWebView->SetFindDialogFindListener(NULL);
    AutoPtr<IBinder> binder;
    IView::Probe(mWebView)->GetWindowToken((IBinder**)&binder);
    Boolean result;
    mInput->HideSoftInputFromWindow(binder, 0, &result);

    return NOERROR;
}

ECode FindActionModeCallback::OnPrepareActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    if (result) {
        *result = FALSE;
    }

    return NOERROR;
}

//@Override
ECode FindActionModeCallback::OnActionItemClicked(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    if (mWebView == NULL) {
        //throw new AssertionError(
        //        "No WebView for FindActionModeCallback::onActionItemClicked");
        Logger::E("FindActionModeCallback::OnActionItemClicked",
                "No WebView for FindActionModeCallback::onActionItemClicked");
        return E_ASSERTION_ERROR;
    }

    AutoPtr<IBinder> binder;
    IView::Probe(mWebView)->GetWindowToken((IBinder**)&binder);
    mInput->HideSoftInputFromWindow(binder, 0, NULL);
    Int32 id;
    item->GetItemId(&id);
    switch(id) {
        case R::id::find_prev:
            FindNext(FALSE);
            break;
        case R::id::find_next:
            FindNext(TRUE);
            break;
        default:
            if (result) *result = FALSE;
            return NOERROR;
    }

    if (result) *result = TRUE;

    return NOERROR;
}

// TextWatcher implementation

ECode FindActionModeCallback::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    // Does nothing.  Needed to implement TextWatcher.
    return NOERROR;
}

ECode FindActionModeCallback::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    FindAll();
    return NOERROR;
}

ECode FindActionModeCallback::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    // Does nothing.  Needed to implement TextWatcher.
    return NOERROR;
}

Int32 FindActionModeCallback::GetActionModeGlobalBottom()
{
    if (mActionMode == NULL) {
        return 0;
    }

    AutoPtr<IViewParent> viewParent;
    mCustomView->GetParent((IViewParent**)&viewParent);
    IView* view = IView::Probe(viewParent);
    if (view == NULL) {
        view = mCustomView;
    }

    view->GetGlobalVisibleRect(mGlobalVisibleRect, mGlobalVisibleOffset, NULL);

    Int32 bottom;
    mGlobalVisibleRect->GetBottom(&bottom);

    return NOERROR;
}

/*
 * Move the highlight to the next match.
 * @param next If true, find the next match further down in the document.
 *             If false, find the previous match, up in the document.
 */
void FindActionModeCallback::FindNext(
    /* [in] */ Boolean next)
{
    if (mWebView == NULL) {
        //throw new AssertionError(
        //        "No WebView for FindActionModeCallback::findNext");
        assert(0);
    }

    if (!mMatchesFound) {
        FindAll();
        return;
    }

    if (0 == mNumberOfMatches) {
        // There are no matches, so moving to the next match will not do
        // anything.
        return;
    }

    mWebView->FindNext(next);
    UpdateMatchesString();
}

/*
 * Update the string which tells the user how many matches were found, and
 * which match is currently highlighted.
 */
void FindActionModeCallback::UpdateMatchesString()
{
    if (mNumberOfMatches == 0) {
        mMatches->SetText(R::string::no_matches);
    }
    else {
        AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(2);
        AutoPtr<IInteger32> value1, value2;
        CInteger32::New(mActiveMatchIndex + 1, (IInteger32**)&value1);
        CInteger32::New(mNumberOfMatches, (IInteger32**)&value2);
        (*formatArgs)[0] = value1;
        (*formatArgs)[1] = value2;
        String str;
        mResources->GetQuantityString(
            R::plurals::matches_found, mNumberOfMatches,
            formatArgs, &str);
        AutoPtr<ICharSequence> strCS;
        CString::New(str, (ICharSequence**)&strCS);
        mMatches->SetText(strCS);
    }

    IView::Probe(mMatches)->SetVisibility(IView::VISIBLE);
}

ECode FindActionModeCallback::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "FindActionModeCallback";
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
