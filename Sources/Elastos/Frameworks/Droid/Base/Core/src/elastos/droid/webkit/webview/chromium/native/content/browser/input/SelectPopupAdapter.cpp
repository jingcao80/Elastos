#include "Elastos.Droid.View.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/SelectPopupAdapter.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/PopupItemType.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/SelectPopupItem.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Widget::EIID_IArrayAdapter;
using Elastos::Core::CString;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IIterable;
using Elastos::Utility::EIID_IIterable;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

CAR_INTERFACE_IMPL(SelectPopupAdapter, Object, IArrayAdapter);

/**
 * Creates a new SelectPopupItem adapter for the select popup alert dialog list.
 * @param context        Application context.
 * @param layoutResource Layout resource used for the alert dialog list.
 * @param items          SelectPopupItem array list.
 */
SelectPopupAdapter::SelectPopupAdapter(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layoutResource,
    /* [in] */ IList* items)
{
    ArrayAdapter::constructor(context, layoutResource, items);
    CArrayList::New(ICollection::Probe(items), (IArrayList**)&mItems);

    mAreAllItemsEnabled = TRUE;
    Int32 size;
    mItems->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mItems->Get(i, (IInterface**)&obj);
        SelectPopupItem* spi = (SelectPopupItem*)(IObject::Probe(obj));
        if (spi->GetType() != PopupItemType::ENABLED) {
            mAreAllItemsEnabled = FALSE;
            break;
        }
    }
}

//@Override
ECode SelectPopupAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    Int32 count;
    if (position < 0 || position >= (GetCount(&count), count)) {
        *view = NULL;
        return NOERROR;
    }

    // Always pass in null so that we will get a new CheckedTextView. Otherwise, an item
    // which was previously used as an <optgroup> element (i.e. has no check), could get
    // used as an <option> element, which needs a checkbox/radio, but it would not have
    // one.
    AutoPtr<IView> v;
    GetView(position, NULL, parent, (IView**)&v);
    convertView = v;

    AutoPtr<IInterface> obj;
    mItems->Get(position, (IInterface**)&obj);
    SelectPopupItem* spi = (SelectPopupItem*)(IObject::Probe(obj));
    String label = spi->GetLabel();
    AutoPtr<ICharSequence> ics;
    CString::New(label, (ICharSequence**)&ics);
    ITextView::Probe(convertView)->SetText(ics);

    if (spi->GetType() != PopupItemType::ENABLED) {
        if (spi->GetType() == PopupItemType::GROUP) {
            // Currently select_dialog_multichoice uses CheckedTextViews.
            // If that changes, the class cast will no longer be valid.
            // The WebView build cannot rely on this being the case, so
            // we must check.
            ICheckedTextView* ctv = ICheckedTextView::Probe(convertView);
            if (ctv != NULL) {
                ctv->SetCheckMarkDrawable((IDrawable*)NULL);
            }
        } else {
            // Draw the disabled element in a disabled state.
            convertView->SetEnabled(FALSE);
        }
    }

    *view = convertView;

    return NOERROR;
}

//@Override
ECode SelectPopupAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* result)
{
    return mAreAllItemsEnabled;
}

//@Override
ECode SelectPopupAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 count;
    if (position < 0 || position >= (GetCount(&count), count)) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    mItems->Get(position, (IInterface**)&obj);
    SelectPopupItem* spi = (SelectPopupItem*)(IObject::Probe(obj));
    return spi->GetType() == PopupItemType::ENABLED;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
