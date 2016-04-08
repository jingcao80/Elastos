
#include "elastos/droid/launcher2/InstallWidgetReceiver.h"
#include "elastos/droid/launcher2/PendingAddItemInfo.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "Elastos.Droid.Service.h"
#include "R.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::EIID_IListAdapter;
using Elastos::Droid::Widget::EIID_IAdapter;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

InstallWidgetReceiver::WidgetMimeTypeHandlerData::WidgetMimeTypeHandlerData(
    /* [in] */ IResolveInfo* rInfo,
    /* [in] */ IAppWidgetProviderInfo* wInfo)
    : mResolveInfo(rInfo)
    , mWidgetInfo(wInfo)
{
}

CAR_INTERFACE_IMPL_3(InstallWidgetReceiver::WidgetListAdapter, Object, IListAdapter
        , IAdapter, IDialogInterfaceOnClickListener);

InstallWidgetReceiver::WidgetListAdapter::WidgetListAdapter(
    /* [in] */ ILauncher* l,
    /* [in] */ const String& mimeType,
    /* [in] */ IClipData* data,
    /* [in] */ IList* list,
    /* [in] */ ICellLayout* target,
    /* [in] */ Int32 targetScreen,
    /* [in] */ ArrayOf<Int32>* targetPos)
    : mLauncher(l)
    , mMimeType(mimeType)
    , mClipData(data)
    , mActivities(list)
    , mTargetLayout(target)
    , mTargetLayoutScreen(targetScreen)
    , mTargetLayoutPos(targetPos)
{
}

ECode InstallWidgetReceiver::WidgetListAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return NOERROR;
}

ECode InstallWidgetReceiver::WidgetListAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return NOERROR;
}

ECode InstallWidgetReceiver::WidgetListAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    return mActivities->GetSize(count);
}

ECode InstallWidgetReceiver::WidgetListAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);

    *item = NULL;
    return NOERROR;
}

ECode InstallWidgetReceiver::WidgetListAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);

    *itemId = position;
    return NOERROR;
}

ECode InstallWidgetReceiver::WidgetListAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds);

    *hasStableIds = TRUE;
    return NOERROR;
}

ECode InstallWidgetReceiver::WidgetListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* inConvertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;

    AutoPtr<IContext> context;
    IView::Probe(parent)->GetContext((IContext**)&context);
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);

    // Lazy-create inflater
    if (mInflater == NULL) {
        LayoutInflater::From(context, (ILayoutInflater**)&mInflater);
    }

    // Use the convert-view where possible
    AutoPtr<IView> convertView = inConvertView;
    if (convertView == NULL) {
        mInflater->Inflate(Elastos::Droid::Launcher2::R::layout::external_widget_drop_list_item, parent,
                FALSE, (IView**)&convertView);
    }

    AutoPtr<IInterface> obj;
    mActivities->Get(position, (IInterface**)&obj);
    AutoPtr<WidgetMimeTypeHandlerData> data = (WidgetMimeTypeHandlerData*)IObject::Probe(obj);
    AutoPtr<IResolveInfo> resolveInfo = data->mResolveInfo;
    AutoPtr<IAppWidgetProviderInfo> widgetInfo = data->mWidgetInfo;

    // Set the icon
    AutoPtr<IDrawable> d;
    resolveInfo->LoadIcon(packageManager, (IDrawable**)&d);
    AutoPtr<IView> _view;
    convertView->FindViewById(Elastos::Droid::Launcher2::R::id::provider_icon, (IView**)&_view);
    AutoPtr<IImageView> i = IImageView::Probe(_view);
    i->SetImageDrawable(d);

    // Set the text
    AutoPtr<ICharSequence> component;
    resolveInfo->LoadLabel(packageManager, (ICharSequence**)&component);
    AutoPtr<ArrayOf<Int32> > widgetSpan = ArrayOf<Int32>::Alloc(2);
    Int32 width;
    widgetInfo->GetMinWidth(&width);
    Int32 height;
    widgetInfo->GetMinHeight(&height);
    assert(0 && "need class ICellLayout");
    //mTargetLayout->RectToCell(width, height, widgetSpan);

    AutoPtr<IView> _view2;
    convertView->FindViewById(Elastos::Droid::Launcher2::R::id::provider, (IView**)&_view2);
    AutoPtr<ITextView> t = ITextView::Probe(_view2);

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(3);
    array->Set(0, TO_IINTERFACE(component));
    AutoPtr<IInteger32> intObj1 = CoreUtils::Convert((*widgetSpan)[0]);
    array->Set(1, TO_IINTERFACE(intObj1));
    AutoPtr<IInteger32> intObj2 = CoreUtils::Convert((*widgetSpan)[1]);
    array->Set(2, TO_IINTERFACE(intObj2));
    String format;
    context->GetString(
            Elastos::Droid::Launcher2::R::string::external_drop_widget_pick_format,
            array, &format);
    assert(0 && "SetText");
    //t->SetText(format);

    *view = convertView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode InstallWidgetReceiver::WidgetListAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* viewType)
{
    VALIDATE_NOT_NULL(viewType);

    *viewType = 0;
    return NOERROR;
}

ECode InstallWidgetReceiver::WidgetListAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    *count = 1;
    return NOERROR;
}

ECode InstallWidgetReceiver::WidgetListAdapter::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);

    return mActivities->IsEmpty(isEmpty);
}

ECode InstallWidgetReceiver::WidgetListAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);

    *enabled = FALSE;
    return NOERROR;
}

ECode InstallWidgetReceiver::WidgetListAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);

    *enabled = TRUE;
    return NOERROR;
}

ECode InstallWidgetReceiver::WidgetListAdapter::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IInterface> obj;
    mActivities->Get(which, (IInterface**)&obj);
    AutoPtr<WidgetMimeTypeHandlerData> data = (WidgetMimeTypeHandlerData*)IObject::Probe(obj);
    AutoPtr<IAppWidgetProviderInfo> widgetInfo = data->mWidgetInfo;

    AutoPtr<PendingAddWidgetInfo> createInfo = new PendingAddWidgetInfo();
    createInfo->constructor(widgetInfo, mMimeType, IParcelable::Probe(mClipData));
    assert(0 && "need class mLauncher");
    // return mLauncher->AddAppWidgetFromDrop(createInfo, LauncherSettings::Favorites::CONTAINER_DESKTOP,
    //         mTargetLayoutScreen, NULL, NULL, mTargetLayoutPos);
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos