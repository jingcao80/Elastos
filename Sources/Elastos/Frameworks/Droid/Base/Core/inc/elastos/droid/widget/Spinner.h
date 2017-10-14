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

#ifndef __ELASTOS_DROID_WIDGET_SPINNER_H__
#define __ELASTOS_DROID_WIDGET_SPINNER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/AbsSpinner.h"
#include "elastos/droid/widget/ListPopupWindow.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::View::IViewTreeObserver;

namespace Elastos {
namespace Droid {
namespace Widget {

class Spinner
    : public AbsSpinner
    , public ISpinner
{
public:
    class SpinnerSavedState
        : public AbsSpinner::AbsSpinnerSavedState
        , public ISpinnerSavedState
    {
    public:
        CAR_INTERFACE_DECL()

        SpinnerSavedState();

        ~SpinnerSavedState();

        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

    public:
        Boolean mShowDropdown;
    };

private:
    class DropDownAdapter
        : public Object
        , public IListAdapter
        , public IAdapter
        , public ISpinnerAdapter
    {
    public:
        CAR_INTERFACE_DECL()

        DropDownAdapter(
            /* [in] */ ISpinnerAdapter* adapter);

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* itemId);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI GetDropDownView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI HasStableIds(
            /* [out] */ Boolean* hasStableIds);

        CARAPI RegisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        CARAPI UnregisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        CARAPI AreAllItemsEnabled(
            /* [out] */ Boolean* enabled);

        CARAPI IsEnabled(
            /* [in] */ Int32 position,
            /* [out] */ Boolean* enabled);

        CARAPI GetItemViewType(
            /* [in] */ Int32 position,
            /* [out] */ Int32* viewType);

        CARAPI GetViewTypeCount(
            /* [out] */ Int32* count);

        CARAPI IsEmpty(
            /* [out] */ Boolean* isEmpty);

    private:
        AutoPtr<ISpinnerAdapter> mAdapter;
        AutoPtr<IListAdapter> mListAdapter;
    };

    class DialogPopup
        : public Object
        , public ISpinnerPopup
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        DialogPopup(
            /* [in] */ Spinner* host);

        CARAPI Dismiss();

        CARAPI IsShowing(
            /* [out] */ Boolean* res);

        CARAPI SetAdapter(
            /* [in] */ IListAdapter* adapter);

        CARAPI SetPromptText(
            /* [in] */ ICharSequence* hintText);

        CARAPI GetHintText(
            /* [out] */ ICharSequence** csq);

        CARAPI Show(
            /* [in] */ Int32 textDirection,
            /* [in] */ Int32 textAlignment);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

        CARAPI SetBackgroundDrawable(
            /* [in] */ IDrawable* bg);

        CARAPI SetVerticalOffset(
            /* [in] */ Int32 px);

        CARAPI SetHorizontalOffset(
            /* [in] */ Int32 px);

        CARAPI GetBackground(
            /* [out] */ IDrawable** d);

        CARAPI GetVerticalOffset(
            /* [out] */ Int32* offset);

        CARAPI GetHorizontalOffset(
            /* [out] */ Int32* offset);

    private:
        AutoPtr<IAlertDialog> mPopup;
        AutoPtr<IListAdapter> mListAdapter;
        AutoPtr<ICharSequence> mPrompt;
        Spinner* mHost;
    };

    class DropdownPopup;
    class ItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ItemClickListener(
            /* [in] */ Spinner* spinnerHost,
            /* [in] */ DropdownPopup* popupHost);

        ECode OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        Spinner* mSpinnerHost;
        DropdownPopup* mPopupHost;
    };

    class GlobalLayoutListener
        : public Object
        , public IOnGlobalLayoutListener
    {
    public:
        CAR_INTERFACE_DECL()

        GlobalLayoutListener(
            /* [in] */ Spinner* spinnerHost,
            /* [in] */ DropdownPopup* popupHost);

        CARAPI OnGlobalLayout();

    private:
        Spinner* mSpinnerHost;
        DropdownPopup* mPopupHost;
    };

    class GlobalLayoutListener2
        : public Object
        , public IOnGlobalLayoutListener
    {
    public:
        CAR_INTERFACE_DECL()

        GlobalLayoutListener2(
            /* [in] */ Spinner* host);

        CARAPI OnGlobalLayout();

    private:
        Spinner* mHost;
    };

    class DismissListener
        : public Object
        , public IPopupWindowOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL()

        DismissListener(
            /* [in] */ Spinner* spinnerHost,
            /* [in] */ GlobalLayoutListener* layoutListener);

        CARAPI OnDismiss();

    private:
        Spinner* mSpinnerHost;
        AutoPtr<GlobalLayoutListener> mLayoutListener;
    };

    class DropdownPopup
        : public ListPopupWindow
        , public IDropdownPopup
        , public ISpinnerPopup
    {
    public:
        CAR_INTERFACE_DECL()

        DropdownPopup(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ Int32 defStyleAttr,
            /* [in] */ Int32 defStyleRes,
            /* [in] */ Spinner* host);

        CARAPI SetAdapter(
            /* [in] */ IListAdapter* adapter);

        CARAPI GetHintText(
            /* [out] */ ICharSequence** csq);

        CARAPI SetPromptText(
            /* [in] */ ICharSequence* hintText);

        using ListPopupWindow::Show;

        CARAPI Show(
            /* [in] */ Int32 textDirection,
            /* [in] */ Int32 textAlignment);

        virtual CARAPI Dismiss();

        virtual CARAPI IsShowing(
            /* [out] */ Boolean* result);

        virtual CARAPI SetBackgroundDrawable(
            /* [in] */ IDrawable* d);

        virtual CARAPI SetVerticalOffset(
            /* [in] */ Int32 offset);

        virtual CARAPI SetHorizontalOffset(
            /* [in] */ Int32 offset);

        virtual CARAPI GetBackground(
            /* [out] */ IDrawable** result);

        virtual CARAPI GetVerticalOffset(
            /* [out] */ Int32* result);

        virtual CARAPI GetHorizontalOffset(
            /* [out] */ Int32* result);

    private:
        CARAPI_(void) ComputeContentWidth();

    private:
        AutoPtr<ICharSequence> mHintText;
        AutoPtr<IListAdapter> mAdapter;
        Spinner* mHost;
        friend class ItemClickListener;
        friend class GlobalLayoutListener;
    };

    class SpinnerForwardingListener: public ListPopupWindow::ForwardingListener
    {
    public:
        SpinnerForwardingListener(
            /* [in] */ DropdownPopup* popup,
            /* [in] */ Spinner* host);

        // @Override
        CARAPI GetPopup(
            /* [out] */ IListPopupWindow** window);

        // @Override
        CARAPI_(Boolean) OnForwardingStarted();

    private:
        DropdownPopup* mData;
        Spinner* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    Spinner();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 mode);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes,
        /* [in] */ Int32 mode);

    CARAPI SetPopupBackgroundDrawable(
        /* [in] */ IDrawable* background);

    CARAPI SetPopupBackgroundResource(
        /* [in] */ Int32 resId);

    CARAPI GetPopupBackground(
        /* [out] */ IDrawable** drawable);

    CARAPI SetDropDownVerticalOffset(
        /* [in] */ Int32 pixels);

    CARAPI GetDropDownVerticalOffset(
        /* [out] */ Int32* offset);

    CARAPI SetDropDownHorizontalOffset(
        /* [in] */ Int32 pixels);

    CARAPI GetDropDownHorizontalOffset(
        /* [out] */ Int32* offset);

    CARAPI SetDropDownWidth(
        /* [in] */ Int32 pixels);

    CARAPI GetDropDownWidth(
        /* [out] */ Int32* width);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    CARAPI GetGravity(
        /* [out] */ Int32* gravity);

    CARAPI SetAdapter(
        /* [in] */ IAdapter* adapter);

    CARAPI GetBaseline(
            /* [out] */ Int32* baseline);

    CARAPI SetOnItemClickListener(
        /* [in] */ IAdapterViewOnItemClickListener* l);

    CARAPI SetOnItemClickListenerInt(
        /* [in] */ IAdapterViewOnItemClickListener* l);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI PerformClick(
        /* [out] */ Boolean* result);

    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    CARAPI SetPrompt(
        /* [in] */ ICharSequence* prompt);

    CARAPI SetPromptId(
        /* [in] */ Int32 promptId);

    CARAPI GetPrompt(
        /* [out] */ ICharSequence** seq);

    CARAPI_(Int32) MeasureContentWidth(
        /* [in] */ ISpinnerAdapter* adapter,
        /* [in] */ IDrawable* background);

    // @Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    // @Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

protected:
    virtual CARAPI OnDetachedFromWindow();

    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    CARAPI_(void) Layout(
        /* [in] */ Int32 delta,
        /* [in] */ Boolean animate);

    using AdapterView::Layout;

private:
    CARAPI_(AutoPtr<IView>) MakeView(
        /* [in] */ Int32 position,
        /* [in] */ Boolean addChild);

    CARAPI_(void) SetUpChild(
        /* [in] */ IView* child,
        /* [in] */ Boolean addChild);

private:
    // Only measure this many items to get a decent max width.
    static const Int32 MAX_ITEMS_MEASURED;

    /**
     * Use a dialog window for selecting spinner options.
     */
    static const Int32 MODE_DIALOG;

    /**
     * Use a dropdown anchored to the Spinner for selecting spinner options.
     */
    static const Int32 MODE_DROPDOWN;

    /**
     * Use the theme-supplied value to select the dropdown mode.
     */
    static const Int32 MODE_THEME;

    static const String TAG;

    /** Forwarding listener used to implement drag-to-open. */
    AutoPtr<ListPopupWindow::ForwardingListener> mForwardingListener;

    AutoPtr<ISpinnerPopup> mPopup;
    AutoPtr<IListAdapter> mTempAdapter;
    Int32 mDropDownWidth;

    Int32 mGravity;
    Boolean mDisableChildrenWhenDisabled;
    AutoPtr<IRect> mTempRect; // = new Rect()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_SPINNER_H__
