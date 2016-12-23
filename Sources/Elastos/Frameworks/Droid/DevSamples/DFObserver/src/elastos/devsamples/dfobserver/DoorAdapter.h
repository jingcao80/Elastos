
#ifndef __Elastos_DevSamples_DFObserver_DoorAdapter_H__
#define __Elastos_DevSamples_DFObserver_DoorAdapter_H__

#include "_Elastos.DevSamples.DFObserver.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include <elastos/utility/etl/List.h>
#include "elastos/core/Runnable.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Core::Runnable;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

/**
 * An Adapter class for representing the observed doors in a ListView.
 */
class DoorAdapter
    : public BaseAdapter
{
private:
    class UpdateRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("UpdateRunnable")

        UpdateRunnable(
            /* [in] */ DoorAdapter* host);

        CARAPI Run();
    private:
        DoorAdapter* mHost;
    };

    class UpdateUIRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("UpdateUIRunnable")

        UpdateUIRunnable(
            /* [in] */ DoorAdapter* host,
            /* [in] */ IDoorAdapterItem* door,
            /* [in] */ Boolean add);

        CARAPI Run();
    private:
        DoorAdapter* mHost;
        AutoPtr<IDoorAdapterItem> mDoor;
        Boolean mIsAdd;
    };

    class DoorViewHolder
        : public Object
   {
   public:
        AutoPtr<ITextView> mName;
        AutoPtr<ICheckBox> mIsOpen;

        DoorViewHolder(
            /* [in] */ ITextView* name,
            /* [in] */ ICheckBox* isOpen);
    };
public:

    CARAPI constructor(
        /* [in] */ IHandler* handle);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** obj);

    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* inConvertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** result);

    /** Add an extra door to the adapter and update the UI accordingly. */
    CARAPI Add(
        /* [in] */ IDoorAdapterItem* door);

    /** Removes a door from the adapter and update the UI accordingly. */
    CARAPI Remove(
        /* [in] */ IDoorAdapterItem* door);

    /** Updates the UI. Make sure the latest changes are shown on the UI. */
    CARAPI UpdateUI(
        /* [in] */ IDoorAdapterItem* door,
        /* [in] */ Boolean add);

    /** Show a notification in the UI upon receiving an signal from a door. */
    CARAPI SendSignal(
        /* [in] */ const String& string);

    /** update UI after receiving an event from a door. */
    CARAPI PropertyUpdate(
        /* [in] */ IDoorAdapterItem* item);

    /** Sends notification of events from local provided doors. */
    CARAPI SendDoorEvent(
        /* [in] */ const String& string);

private:

    /**
     * The list of door info objects. A Vector is used to synchronize access on
     * it between threads.
     */
    List<AutoPtr<IDoorAdapterItem> > mList;
    /** The handler for sending messages to the UI thread. */
    AutoPtr<IHandler> mHandler;
};

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DFObserver_DoorAdapter_H__
