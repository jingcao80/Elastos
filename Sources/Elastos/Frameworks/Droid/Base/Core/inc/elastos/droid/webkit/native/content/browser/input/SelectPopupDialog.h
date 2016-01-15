
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUPDIALOG_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUPDIALOG_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/input/SelectPopup.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::Res::ITypedArray;
// import android.util.SparseBooleanArray;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IListView;

using Elastos::Utility::IList;

// import org.chromium.content.R;
// import org.chromium.content.browser.ContentViewCore;

// import java.util.List;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class ContentViewCore;

namespace Input {

/**
 * Handles the popup dialog for the <select> HTML tag support.
 */
class SelectPopupDialog
    : public SelectPopup
{
private:
    class OkDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        OkDialogInterfaceOnClickListener(
            /* [in] */ SelectPopupDialog* owner,
            /* [in] */ IListView* listView);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        SelectPopupDialog* mOwner;
        IListView* mListView;
    };

    class CancelDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        CancelDialogInterfaceOnClickListener(
            /* [in] */ SelectPopupDialog* owner);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        SelectPopupDialog* mOwner;
    };

    class InnerAdapterViewOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerAdapterViewOnItemClickListener(
            /* [in] */ SelectPopupDialog* owner,
            /* [in] */ IListView* listView);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* v,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        SelectPopupDialog* mOwner;
        IListView* mListView;
    };

    class InnerDialogInterfaceOnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerDialogInterfaceOnCancelListener(
            /* [in] */ SelectPopupDialog* owner);

        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        SelectPopupDialog* mOwner;
    };

public:
    SelectPopupDialog(
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ IList* items,
        /* [in] */ Boolean multiple,
        /* [in] */ ArrayOf<Int32>* selected);

    //@Override
    CARAPI_(void) Show();

    //@Override
    CARAPI_(void) Hide();

private:
    CARAPI_(Int32) GetSelectDialogLayout(
        /* [in] */ Boolean isMultiChoice);

    CARAPI_(AutoPtr< ArrayOf<Int32> >) GetSelectedIndices(
        /* [in] */ IListView* listView);

private:
    static const AutoPtr< ArrayOf<Int32> > SELECT_DIALOG_ATTRS;

    // The dialog hosting the popup list view.
    AutoPtr<IAlertDialog> mListBoxPopup;

    /*const*/ AutoPtr<ContentViewCore> mContentViewCore;
    /*const*/ AutoPtr<IContext> mContext;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUPDIALOG_H__
