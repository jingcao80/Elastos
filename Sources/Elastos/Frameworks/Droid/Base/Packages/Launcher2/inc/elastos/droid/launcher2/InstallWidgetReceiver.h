#ifndef  __ELASTOS_DROID__LAUNCHER2_INSTALLWIDGETRECEIVER_H__
#define  __ELASTOS_DROID__LAUNCHER2_INSTALLWIDGETRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Cache of application icons.  Icons can be made from any thread.
 */
class InstallWidgetReceiver
    : public Object
{
public:
    /**
     * A simple data class that contains per-item information that the adapter below can reference.
     */
    class WidgetMimeTypeHandlerData
        : public Object
    {
    public:
        WidgetMimeTypeHandlerData(
            /* [in] */ IResolveInfo* rInfo,
            /* [in] */ IAppWidgetProviderInfo* wInfo);

    public:
        AutoPtr<IResolveInfo> mResolveInfo;
        AutoPtr<IAppWidgetProviderInfo> mWidgetInfo;
    };

    /**
     * The ListAdapter which presents all the valid widgets that can be created for a given drop.
     */
    class WidgetListAdapter
        : public Object
        , public IListAdapter
        , public IAdapter
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        WidgetListAdapter(
            /* [in] */ ILauncher* l,
            /* [in] */ const String& mimeType,
            /* [in] */ IClipData* data,
            /* [in] */ IList* list,
            /* [in] */ ICellLayout* target,
            /* [in] */ Int32 targetScreen,
            /* [in] */ ArrayOf<Int32>* targetPos);

        // @Override
        CARAPI RegisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        // @Override
        CARAPI UnregisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        // @Override
        CARAPI GetCount(
            /* [out] */ Int32* count);

        // @Override
        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        // @Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* itemId);

        // @Override
        CARAPI HasStableIds(
            /* [out] */ Boolean* hasStableIds);

        // @Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        // @Override
        CARAPI GetItemViewType(
            /* [in] */ Int32 position,
            /* [out] */ Int32* viewType);

        // @Override
        CARAPI GetViewTypeCount(
            /* [out] */ Int32* count);

        // @Override
        CARAPI IsEmpty(
            /* [out] */ Boolean* isEmpty);

        // @Override
        CARAPI AreAllItemsEnabled(
            /* [out] */ Boolean* enabled);

        // @Override
        CARAPI IsEnabled(
            /* [in] */ Int32 position,
            /* [out] */ Boolean* enabled);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        AutoPtr<ILayoutInflater> mInflater;
        AutoPtr<ILauncher> mLauncher;
        String mMimeType;
        AutoPtr<IClipData> mClipData;
        AutoPtr<IList> mActivities;
        AutoPtr<ICellLayout> mTargetLayout;
        Int32 mTargetLayoutScreen;
        AutoPtr<ArrayOf<Int32> > mTargetLayoutPos;
    };

public:
    static const String ACTION_INSTALL_WIDGET;
    static const String ACTION_SUPPORTS_CLIPDATA_MIMETYPE;

    // Currently not exposed.  Put into Intent when we want to make it public.
    // TEMP: Should we call this "EXTRA_APPWIDGET_PROVIDER"?
    static const String EXTRA_APPWIDGET_COMPONENT;
    static const String EXTRA_APPWIDGET_CONFIGURATION_DATA_MIME_TYPE;
    static const String EXTRA_APPWIDGET_CONFIGURATION_DATA;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID__LAUNCHER2_INSTALLWIDGETRECEIVER_H__