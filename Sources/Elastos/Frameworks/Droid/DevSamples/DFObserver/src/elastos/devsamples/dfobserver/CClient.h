
#ifndef __Elastos_DevSamples_DFObserver_CClient_H__
#define __Elastos_DevSamples_DFObserver_CClient_H__

#include "_Elastos_DevSamples_DFObserver_CClient.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/widget/ArrayAdapter.h>
#include "DoorAdapter.h"
#include "BusHandler.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnMultiChoiceClickListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemLongClickListener;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

CarClass(CClient)
    , public Activity
{
public:
    class MyArrayAdapter
        : public ArrayAdapter
    {
    public:
        MyArrayAdapter(
            /* [in] */ CClient* host);

        CARAPI Add(
            /* [in] */ IInterface* object);

    private:
        CClient* mHost;
    };

    class UIHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CClient::UIHandler")

        UIHandler(
            /* [in] */ CClient* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        friend class CClient;

        CClient* mHost;
        AutoPtr<IArrayAdapter> mMsgListViewArrayAdapter;
    };

    class InnerListener
        : public Object
        , public IAdapterViewOnItemClickListener
        , public IAdapterViewOnItemLongClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CClient* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);
        CARAPI OnItemLongClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id,
            /* [out] */ Boolean* result);

    private:
        CClient* mHost;
    };

    class PositiveOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        PositiveOnClickListener(
            /* [in] */ CClient* host,
            /* [in] */ IEditText* view);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 id);
    private:
        CClient* mHost;
        AutoPtr<IEditText> mView;
    };

    class NilOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

         CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 id)
        {
            return NOERROR;
        }
    };

    class OnMultiChoiceClickListener
        : public Object
        , public IDialogInterfaceOnMultiChoiceClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnMultiChoiceClickListener(
            /* [in] */ CClient* host,
            /* [in] */ ArrayOf<String>* names,
            /* [in] */ ArrayOf<Boolean>* checked);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which,
            /* [in] */ Boolean isChecked);
    private:
        CClient* mHost;
        AutoPtr<ArrayOf<String> > mDoorNames;
        AutoPtr<ArrayOf<Boolean> > mCheckedDoors;
    };

public:
    CAR_OBJECT_DECL()

    CClient();

    ~CClient();

    CARAPI constructor();

protected:

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* menu,
        /* [out] */ Boolean* result);

public:
    static const Int32 MESSAGE_INCOMING_EVENT = 1;
    static const Int32 MESSAGE_UPDATE_UI = 2;
    static const Int32 MESSAGE_DOOR_EVENT = 3;

    AutoPtr<DoorAdapter> mDoorListAdapter;
    AutoPtr<IListView> mMsgListView;
    AutoPtr<IListView> mDoorListView;
    AutoPtr<IMenu> mMenu;

    AutoPtr<BusHandler> mBusHandler;
    AutoPtr<UIHandler> mHandler;
};

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DFObserver_CClient_H__
