
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_ListMenuItemView_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_ListMenuItemView_H__

#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ICheckBox;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

class ListMenuItemView
    : public LinearLayout
    , public IListMenuItemView
    , public IMenuItemView
{
public:
    ListMenuItemView();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Initialize(
        /* [in] */ IMenuItemImpl* itemData,
        /* [in] */ Int32 menuType);

    CARAPI SetForceShowIcon(
        /* [in] */ Boolean forceShow);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI GetItemData(
        /* [out] */ IMenuItemImpl** itemData);

    CARAPI SetCheckable(
        /* [in] */ Boolean checkable);

    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    CARAPI SetShortcut(
        /* [in] */ Boolean showShortcut,
        /* [in] */ Char32 shortcutKey);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    CARAPI PrefersCondensedTitle(
        /* [out] */ Boolean* result);

    CARAPI ShowsIcon(
        /* [out] */ Boolean* result);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    //@Override
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

protected:
    //Override
    CARAPI OnFinishInflate();

    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    CARAPI_(void) InsertIconView();

    CARAPI_(void) InsertRadioButton();

    CARAPI_(void) InsertCheckBox();

    CARAPI_(AutoPtr<ILayoutInflater>) GetInflater();

private:
    static const String TAG;

    AutoPtr<IMenuItemImpl> mItemData;
    AutoPtr<IImageView> mIconView;
    AutoPtr<IRadioButton> mRadioButton;
    AutoPtr<ITextView> mTitleView;
    AutoPtr<ICheckBox> mCheckBox;
    AutoPtr<ITextView> mShortcutView;

    AutoPtr<IDrawable> mBackground;
    Int32 mTextAppearance;
    AutoPtr<IContext> mTextAppearanceContext;
    Boolean mPreserveIconSpacing;

    Int32 mMenuType;

    AutoPtr<ILayoutInflater> mInflater;

    Boolean mForceShowIcon;

};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INTERNAL_VIEW_MENU_ListMenuItemView_H__
