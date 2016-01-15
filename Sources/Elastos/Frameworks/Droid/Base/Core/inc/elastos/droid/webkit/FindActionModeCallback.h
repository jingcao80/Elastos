
#ifndef __ELASTOS_DROID_WEBKIT_FINDACTIONMODECALLBACK_H__
#define __ELASTOS_DROID_WEBKIT_FINDACTIONMODECALLBACK_H__

#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Webkit {

class CWebView;

class FindActionModeCallback
    : public Object
    , public IActionModeCallback
    , public ITextWatcher
    , public IViewOnClickListener
{
public:
    class NoAction
        : public Object
        , public IActionModeCallback
    {
    public:
        CAR_INTERFACE_DECL();

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

        CARAPI ToString(
            /* [out] */ String* info);
    };

public:
    FindActionModeCallback(
        /* [in] */ IContext* context);

    CAR_INTERFACE_DECL();

    virtual CARAPI_(void) Finish();

    /*
     * Place text in the text field so it can be searched for.  Need to press
     * the find next or find previous button to find all of the matches.
     */
    virtual CARAPI_(void) SetText(
        /* [in] */ const String& text);

    /*
     * Set the WebView to search.  Must be non null, and set before calling
     * startActionMode.
     */
    virtual CARAPI SetWebView(
        /* [in] */ CWebView* webView);

    CARAPI OnFindResultReceived(
        /* [in] */ Int32 activeMatchOrdinal,
        /* [in] */ Int32 numberOfMatches,
        /* [in] */ Boolean isDoneCounting);

    /*
     * Highlight all the instances of the string from mEditText in mWebView.
     */
    virtual CARAPI_(void) FindAll();

    virtual CARAPI_(void) ShowSoftInput();

    virtual CARAPI_(void) UpdateMatchCount(
        /* [in] */ Int32 matchIndex,
        /* [in] */ Int32 matchCount,
        /* [in] */ Boolean isEmptyFind);

    // OnClickListener implementation

    CARAPI OnClick(
        /* [in] */ IView* v);

    // ActionMode.Callback implementation

    CARAPI OnCreateActionMode(
        /* [in] */ IActionMode* mode,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    CARAPI OnDestroyActionMode(
        /* [in] */ IActionMode* mode);

    CARAPI OnPrepareActionMode(
        /* [in] */ IActionMode* mode,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    CARAPI OnActionItemClicked(
        /* [in] */ IActionMode* mode,
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    // TextWatcher implementation

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

    virtual CARAPI_(Int32) GetActionModeGlobalBottom();

    CARAPI ToString(
        /* [out] */ String* info);

private:
    /*
     * Move the highlight to the next match.
     * @param next If true, find the next match further down in the document.
     *             If false, find the previous match, up in the document.
     */
    CARAPI_(void) FindNext(
        /* [in] */ Boolean next);

    /*
     * Update the string which tells the user how many matches were found, and
     * which match is currently highlighted.
     */
    CARAPI_(void) UpdateMatchesString();

private:
    AutoPtr<IRect> mGlobalVisibleRect;
    AutoPtr<IPoint> mGlobalVisibleOffset;
    AutoPtr<IView> mCustomView;
    AutoPtr<IEditText> mEditText;
    AutoPtr<ITextView> mMatches;
    AutoPtr<CWebView> mWebView;
    AutoPtr<IInputMethodManager> mInput;
    AutoPtr<IResources> mResources;
    Boolean mMatchesFound;
    Int32 mNumberOfMatches;
    Int32 mActiveMatchIndex;
    AutoPtr<IActionMode> mActionMode;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_FINDACTIONMODECALLBACK_H__
