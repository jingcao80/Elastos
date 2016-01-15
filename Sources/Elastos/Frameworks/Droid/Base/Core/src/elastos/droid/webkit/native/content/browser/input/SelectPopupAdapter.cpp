
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/native/content/browser/input/SelectPopupAdapter.h"

using Elastos::Droid::Widget::EIID_IArrayAdapter;
// TODO using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IIterable;
using Elastos::Utility::EIID_IIterable;

namespace Elastos {
namespace Droid {
namespace Webkit {
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
    assert(0);
    // TODO
    // super(context, layoutResource, items);
    // CArrayList::New(items, (IArrayList**)&mItems);

    mAreAllItemsEnabled = TRUE;
    AutoPtr<IIterable> iterable = IIterable::Probe(mItems);
    Boolean bNext = FALSE;
    AutoPtr<IIterator> iter;
    iterable->GetIterator((IIterator**)&iter);
    iter->HasNext(&bNext);
    assert(0);
    // TODO
    // for (Int32 i = 0; i < mItems.size(); i++) {
    //     if (mItems.get(i).getType() != PopupItemType.ENABLED) {
    //         mAreAllItemsEnabled = false;
    //         break;
    //     }
    // }
}

//@Override
ECode SelectPopupAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    if (position < 0 /* TODO || position >= GetCount()*/) {
        *view = NULL;
        return NOERROR;
    }

    // Always pass in null so that we will get a new CheckedTextView. Otherwise, an item
    // which was previously used as an <optgroup> element (i.e. has no check), could get
    // used as an <option> element, which needs a checkbox/radio, but it would not have
    // one.
    assert(0);
    // TODO
    // convertView = super.getView(position, null, parent);
    // ((TextView) convertView).setText(mItems.get(position).getLabel());

    // if (mItems.get(position).getType() != PopupItemType.ENABLED) {
    //     if (mItems.get(position).getType() == PopupItemType.GROUP) {
    //         // Currently select_dialog_multichoice uses CheckedTextViews.
    //         // If that changes, the class cast will no longer be valid.
    //         // The WebView build cannot rely on this being the case, so
    //         // we must check.
    //         if (convertView instanceof CheckedTextView) {
    //             ((CheckedTextView) convertView).setCheckMarkDrawable(null);
    //         }
    //     } else {
    //         // Draw the disabled element in a disabled state.
    //         convertView.setEnabled(false);
    //     }
    // }

    *view = convertView;

    return E_NOT_IMPLEMENTED;
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
    if (position < 0 /* TODO || position >= GetCount() */) {
        *result = FALSE;
        return NOERROR;
    }

    assert(0);
    // TODO
    // return mItems.get(position).getType() == PopupItemType.ENABLED;
    return E_NOT_IMPLEMENTED;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
