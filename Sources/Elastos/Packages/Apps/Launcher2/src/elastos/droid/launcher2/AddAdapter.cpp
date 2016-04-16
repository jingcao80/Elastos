
#include "elastos/droid/launcher2/AddAdapter.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

AddAdapter::ListItem::ListItem(
    /* [in] */ IResources* res,
    /* [in] */ Int32 textResourceId,
    /* [in] */ Int32 imageResourceId,
    /* [in] */ Int32 actionTag)
{
    String str;
    res->GetString(textResourceId, &str);
    mText = CoreUtils::Convert(str);
    if (imageResourceId != -1) {
        res->GetDrawable(imageResourceId, (IDrawable**)&mImage);
    }
    else {
        mImage = NULL;
    }
    mActionTag = actionTag;
}

const Int32 AddAdapter::ITEM_SHORTCUT = 0;
const Int32 AddAdapter::ITEM_APPWIDGET = 1;
const Int32 AddAdapter::ITEM_APPLICATION = 2;
const Int32 AddAdapter::ITEM_WALLPAPER = 3;

AddAdapter::AddAdapter()
{
    CArrayList::New((IArrayList**)&mItems);
}

ECode AddAdapter::constructor(
    /* [in] */ ILauncher* launcher)
{
    BaseAdapter::constructor();

    AutoPtr<IInterface> obj;
    IContext::Probe(launcher)->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);

    // Create default actions
    AutoPtr<IResources> res;
    IContext::Probe(launcher)->GetResources((IResources**)&res);

    AutoPtr<ListItem> item = new ListItem(res,
            Elastos::Droid::Launcher2::R::string::group_wallpapers,
            Elastos::Droid::Launcher2::R::mipmap::ic_launcher_wallpaper,
            ITEM_WALLPAPER);
    return mItems->Add(TO_IINTERFACE(item));
}

ECode AddAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* inConvertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    AutoPtr<ListItem> item = (ListItem*)IObject::Probe(obj);

    AutoPtr<IView> convertView = inConvertView;
    if (convertView == NULL) {
        mInflater->Inflate(Elastos::Droid::Launcher2::R::layout::add_list_item,
                parent, FALSE, (IView**)&convertView);
    }

    AutoPtr<ITextView> textView = ITextView::Probe(convertView);
    IView::Probe(textView)->SetTag(TO_IINTERFACE(item));
    textView->SetText(item->mText);
    textView->SetCompoundDrawablesWithIntrinsicBounds(item->mImage,
            NULL, NULL, NULL);

    *view = convertView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode AddAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    return mItems->GetSize(count);
}

ECode AddAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);

    return mItems->Get(position, item);
}

ECode AddAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);

    *id = position;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos