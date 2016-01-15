
#ifndef __ELASTOS_DROID_APP_CALERTDIALOGBUILDER_H__
#define __ELASTOS_DROID_APP_CALERTDIALOGBUILDER_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_App_CAlertDialogBuilder.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IDialogInterfaceOnKeyListener;
using Elastos::Droid::Content::IDialogInterfaceOnMultiChoiceClickListener;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Internal::App::IAlertControllerAlertParams;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CAlertDialogBuilder)
    , public Object
    , public IAlertDialogBuilder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAlertDialogBuilder();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Constructor using a context and theme for this builder and
     * the {@link AlertDialog} it creates.  The actual theme
     * that an AlertDialog uses is a private implementation, however you can
     * here supply either the name of an attribute in the theme from which
     * to get the dialog's style (such as {@link android.R.attr#alertDialogTheme}
     * or one of the constants
     * {@link AlertDialog#THEME_TRADITIONAL AlertDialog.THEME_TRADITIONAL},
     * {@link AlertDialog#THEME_HOLO_DARK AlertDialog.THEME_HOLO_DARK}, or
     * {@link AlertDialog#THEME_HOLO_LIGHT AlertDialog.THEME_HOLO_LIGHT}.
     */
    ECode constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme);

    /**
     * Returns a {@link Context} with the appropriate theme for dialogs created by this Builder.
     * Applications should use this Context for obtaining LayoutInflaters for inflating views
     * that will be used in the resulting dialogs, as it will cause views to be inflated with
     * the correct theme.
     *
     * @return A Context for built Dialogs.
     */
    ECode GetContext(
        /* [out] */ IContext** context);

    CARAPI SetTitle(
        /* [in] */ Int32 titleId);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI SetCustomTitle(
        /* [in] */ IView* customTitleView);

    CARAPI SetMessage(
        /* [in] */ Int32 messageId);

    CARAPI SetMessage(
        /* [in] */ ICharSequence* message);

    CARAPI SetIcon(
        /* [in] */ Int32 iconId);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    /**
     * Set an icon as supplied by a theme attribute. e.g. android.R.attr.alertDialogIcon
     *
     * @param attrId ID of a theme attribute that points to a drawable resource.
     */
    CARAPI SetIconAttribute(
        /* [in] */ Int32 attrId);

    CARAPI SetPositiveButton(
        /* [in] */ Int32 textId,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetPositiveButton(
        /* [in] */ ICharSequence* text,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetNegativeButton(
        /* [in] */ Int32 textId,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetNegativeButton(
        /* [in] */ ICharSequence* text,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetNeutralButton(
        /* [in] */ Int32 textId,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetNeutralButton(
        /* [in] */ ICharSequence* text,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetCancelable(
        /* [in] */ Boolean cancelable);

    CARAPI SetOnDismissListener(
        /* [in] */ IDialogInterfaceOnDismissListener* onDismissListener);

    CARAPI SetOnCancelListener(
        /* [in] */ IDialogInterfaceOnCancelListener* onCancelListener);

    CARAPI SetOnKeyListener(
        /* [in] */ IDialogInterfaceOnKeyListener* onKeyListener);

    CARAPI SetItems(
        /* [in] */ Int32 itemsId,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetItems(
        /* [in] */ ArrayOf<ICharSequence*>* items,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetAdapter(
        /* [in] */ IListAdapter* adapter,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetCursor(
        /* [in] */ ICursor* cursor,
        /* [in] */ IDialogInterfaceOnClickListener* listener,
        /* [in] */ const String& labelColumn);

    CARAPI SetMultiChoiceItems(
        /* [in] */ Int32 itemsId,
        /* [in] */ ArrayOf<Boolean>* checkedItems,
        /* [in] */ IDialogInterfaceOnMultiChoiceClickListener* listener);

    CARAPI SetMultiChoiceItems(
        /* [in] */ ArrayOf<ICharSequence*>* items,
        /* [in] */ ArrayOf<Boolean>* checkedItems,
        /* [in] */ IDialogInterfaceOnMultiChoiceClickListener* listener);

    CARAPI SetMultiChoiceItems(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& isCheckedColumn,
        /* [in] */ const String& labelColumn,
        /* [in] */ IDialogInterfaceOnMultiChoiceClickListener* listener);

    CARAPI SetSingleChoiceItems(
        /* [in] */ Int32 itemsId,
        /* [in] */ Int32 checkedItem,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetSingleChoiceItems(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 checkedItem,
        /* [in] */ const String& labelColumn,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetSingleChoiceItems(
        /* [in] */ ArrayOf<ICharSequence*>* items,
        /* [in] */ Int32 checkedItem,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetSingleChoiceItems(
        /* [in] */ IListAdapter* adapter,
        /* [in] */ Int32 checkedItem,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI SetOnItemSelectedListener(
        /* [in] */ IAdapterViewOnItemSelectedListener* listener);

    /**
     * Set a custom view resource to be the contents of the Dialog. The
     * resource will be inflated, adding all top-level views to the screen.
     *
     * @param layoutResId Resource ID to be inflated.
     * @return This Builder object to allow for chaining of calls to set
     *         methods
     */
    CARAPI SetView(
        /* [in] */ Int32 layoutResId);

    CARAPI SetView(
        /* [in] */ IView* view);

    CARAPI SetView(
        /* [in] */ IView* view,
        /* [in] */ Int32 viewSpacingLeft,
        /* [in] */ Int32 viewSpacingTop,
        /* [in] */ Int32 viewSpacingRight,
        /* [in] */ Int32 viewSpacingBottom);

    CARAPI SetInverseBackgroundForced(
        /* [in] */ Boolean useInverseBackground);

    CARAPI SetRecycleOnMeasureEnabled(
        /* [in] */ Boolean enabled);

    CARAPI Create(
        /* [out] */ IAlertDialog** dialog);

    CARAPI Show(
        /* [out] */ IAlertDialog** dialog);

private:
    AutoPtr<IAlertControllerAlertParams> mP;
    Int32 mTheme;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CALERTDIALOGBUILDER_H__
