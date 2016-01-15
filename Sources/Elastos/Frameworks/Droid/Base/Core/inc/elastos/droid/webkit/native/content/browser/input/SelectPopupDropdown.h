
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUPDROPDOWN_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUPDROPDOWN_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/input/SelectPopup.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::IPopupWindowOnDismissListener;

using Elastos::Utility::IList;

// import org.chromium.content.browser.ContentViewCore;
// import org.chromium.content.browser.RenderCoordinates;
// import org.chromium.ui.DropdownAdapter;
// import org.chromium.ui.DropdownItem;
// import org.chromium.ui.DropdownPopupWindow;

// import java.util.List;


namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {

class DropdownPopupWindow;

} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

using Elastos::Droid::Webkit::Ui::DropdownPopupWindow;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class ContentViewCore;

namespace Input {

/**
 * Handles the dropdown popup for the <select> HTML tag support.
 */
class SelectPopupDropdown
    : public SelectPopup
{
private:
    class InnerAdapterViewOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerAdapterViewOnItemClickListener(
            /* [in] */ SelectPopupDropdown* owner);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        SelectPopupDropdown* mOwner;
    };

    class InnerPopupWindowOnDismissListener
        : public Object
        , public IPopupWindowOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerPopupWindowOnDismissListener(
            /* [in] */ SelectPopupDropdown* owner);

        CARAPI OnDismiss();

    private:
        SelectPopupDropdown* mOwner;
    };

public:
    SelectPopupDropdown(
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ IList* items,
        /* [in] */ IRect* bounds,
        /* [in] */ ArrayOf<Int32>* selected);

    //@Override
    CARAPI_(void) Show();

    //@Override
    CARAPI_(void) Hide();

private:
    /*const*/ AutoPtr<ContentViewCore> mContentViewCore;
    /*const*/ AutoPtr<IContext> mContext;

    AutoPtr<DropdownPopupWindow> mDropdownPopupWindow;
    Int32 mInitialSelection;
    Boolean mAlreadySelectedItems;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUPDROPDOWN_H__
