#ifndef  __ELASTOS_DROID__LAUNCHER2_INSTALLWIDGETRECEIVER_H__
#define  __ELASTOS_DROID__LAUNCHER2_INSTALLWIDGETRECEIVER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * We will likely flesh this out later, to handle allow external apps to place widgets, but for now,
 * we just want to expose the action around for checking elsewhere.
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
        ILauncher* mLauncher;
        String mMimeType;
        AutoPtr<IClipData> mClipData;
        AutoPtr<IList> mActivities;
        AutoPtr<ICellLayout> mTargetLayout;
        Int32 mTargetLayoutScreen;
        AutoPtr<ArrayOf<Int32> > mTargetLayoutPos;
    };
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID__LAUNCHER2_INSTALLWIDGETRECEIVER_H__