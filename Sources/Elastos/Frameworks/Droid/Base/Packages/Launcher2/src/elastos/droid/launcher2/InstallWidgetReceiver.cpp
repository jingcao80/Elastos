
#include "elastos/droid/launcher2/InstallWidgetReceiver.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const String InstallWidgetReceiver::ACTION_INSTALL_WIDGET(
        "com.android.launcher.action.INSTALL_WIDGET");
const String InstallWidgetReceiver::ACTION_SUPPORTS_CLIPDATA_MIMETYPE(
        "com.android.launcher.action.SUPPORTS_CLIPDATA_MIMETYPE");

// Currently not exposed.  Put into Intent when we want to make it public.
// TEMP: Should we call this "EXTRA_APPWIDGET_PROVIDER"?
const String InstallWidgetReceiver::EXTRA_APPWIDGET_COMPONENT(
    "com.android.launcher.extra.widget.COMPONENT");
const String InstallWidgetReceiver::EXTRA_APPWIDGET_CONFIGURATION_DATA_MIME_TYPE(
    "com.android.launcher.extra.widget.CONFIGURATION_DATA_MIME_TYPE");
const String InstallWidgetReceiver::EXTRA_APPWIDGET_CONFIGURATION_DATA(
    "com.android.launcher.extra.widget.CONFIGURATION_DATA");

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
    :,mLauncher(l)
    , mMimeType(mimeType)
    , mClipData(data)
    , mActivities(list)
    , mTargetLayout(target)
    , mTargetLayoutScreen(targetScreen)
    , mTargetLayoutPos(targetPos)
{
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return NOERROR;
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return NOERROR;
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    return mActivities->GetSize(count);
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);

    *item = NULL;
    return NOERROR;
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);

    *itemId = mPosition;
    return NOERROR;
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds);

    *hasStableIds = TRUE;
    return NOERROR;
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;

    AutoPtr<IContext> context;
    parent->GetContext((IContext**)&context);
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);

    // Lazy-create inflater
    if (mInflater == NULL) {
        LayoutInflater::From(context, (ILayoutInflater**)&mInflater);
    }

    // Use the convert-view where possible
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
    mTargetLayout->RectToCell(width, height, widgetSpan);

    AutoPtr<IView> _view2;
    convertView->FindViewById(Elastos::Droid::Launcher2::R::id::provider, (IView**)&_view2);
    AutoPtr<ITextView> t = ITextView::Probe(_view2);
    String format;
    context->GetString(Elastos::Droid::Launcher2::R::string::external_drop_widget_pick_format,
            component, (*widgetSpan)[0], (*widgetSpan)[1])
    t->SetText(format);

    *view = convertView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* viewType)
{
    VALIDATE_NOT_NULL(viewType);

    *viewType = 0;
    return NOERROR;
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    *count = 1;
    return NOERROR;
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);

    return mActivities->IsEmpty(isEmpty);
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);

    *enabled = FALSE;
    return NOERROR;
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);

    *enabled = TRUE;
    return NOERROR;
}

CARAPI InstallWidgetReceiver::WidgetListAdapter::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IInterface> obj;
    mActivities->Get(which, (IInterface**)&obj);
    AutoPtr<WidgetMimeTypeHandlerData> data = (WidgetMimeTypeHandlerData*)IObject::Probe(obj);
    AutoPtr<IAppWidgetProviderInfo> widgetInfo = data->mWidgetInfo;

    AutoPtr<PendingAddWidgetInfo> createInfo = new PendingAddWidgetInfo(widgetInfo, mMimeType,
            mClipData);
    return mLauncher->AddAppWidgetFromDrop(createInfo, LauncherSettings::Favorites::CONTAINER_DESKTOP,
            mTargetLayoutScreen, NULL, NULL, mTargetLayoutPos);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos