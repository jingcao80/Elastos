
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_ICONMENIITEMVIEW_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_ICONMENIITEMVIEW_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/TextView.h"

using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

/**
 * The item view for each item in the {@link IconMenuView}.
 */
class IconMenuItemView
    : public TextView
    , public IIconMenuItemView
    , public IMenuItemView
{
public:
    IconMenuItemView();

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

    /**
     * Initializes with the provided title and icon
     * @param title The title of this item
     * @param icon The icon of this item
     */
    CARAPI_(void) Initialize(
        /* [in] */ ICharSequence* title,
        /* [in] */ IDrawable* icon);

    CARAPI Initialize(
        /* [in] */ IMenuItemImpl* itemData,
        /* [in] */ Int32 menuType);

    CARAPI SetItemData(
        /* [in] */ IMenuItemImpl* data);

    //@override
    CARAPI PerformClick(
        /* [out] */ Boolean* result);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI_(void) SetCaptionMode(
        /* [in] */ Boolean shortcut);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    CARAPI SetItemInvoker(
        /* [in] */ IMenuBuilderItemInvoker* itemInvoker);

    //@override
    CARAPI SetVisibility(
        /* [in] */ Int32 v);

    CARAPI GetItemData(
        /* [out] */ IMenuItemImpl** itemData);

    CARAPI_(void) SetIconMenuView(
        /* [in] */ IIconMenuView* iconMenuView);

    /**
     * @return layout params appropriate for this view.  If layout params already exist, it will
     *         augment them to be appropriate to the current text size.
     */
    CARAPI_(AutoPtr<IIconMenuViewLayoutParams>) GetTextAppropriateLayoutParams();

    CARAPI SetCheckable(
        /* [in] */ Boolean checkable);

    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    CARAPI SetShortcut(
        /* [in] */ Boolean showShortcut,
        /* [in] */ Char32 shortcutKey);

    CARAPI PrefersCondensedTitle(
        /* [out] */ Boolean* result);

    CARAPI ShowsIcon(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

protected:
    //@Override
    CARAPI DrawableStateChanged();

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    //@Override
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 after);

private:
    /**
     * Positions the icon vertically (horizontal centering is taken care of by
     * the TextView's gravity).
     */
    CARAPI_(void) PositionIcon();

private:
    static const Int32 NO_ALPHA;
    static String sPrependShortcutLabel;

    AutoPtr<IIconMenuView> mIconMenuView;

    AutoPtr<IMenuBuilderItemInvoker> mItemInvoker;
    AutoPtr<IMenuItemImpl> mItemData;

    AutoPtr<IDrawable> mIcon;

    Int32 mTextAppearance;
    IContext* mTextAppearanceContext;

    Float mDisabledAlpha;

    AutoPtr<IRect> mPositionIconAvailable;
    AutoPtr<IRect> mPositionIconOutput;

    Boolean mShortcutCaptionMode;
    String mShortcutCaption;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_MENU_ICONMENIITEMVIEW_H__
