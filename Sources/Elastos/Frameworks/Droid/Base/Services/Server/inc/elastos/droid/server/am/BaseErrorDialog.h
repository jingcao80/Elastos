
#ifndef __ELASTOS_DROID_SERVER_AM_BASEERRORDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_BASEERRORDIALOG_H__

#include "elastos/droid/app/AlertDialog.h"


#include "elastos/droid/os/HandlerBase.h"


using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IViewOnCreateContextMenuListener;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class BaseErrorDialog
    : public IAlertDialog
    , public IWindowCallback
    , public IKeyEventCallback
    , public IViewOnCreateContextMenuListener
    , public IWeakReferenceSource
    , public Object
    , public AlertDialog
{
private:
    class MyHandler : public HandlerBase
    {
    public:
        MyHandler(
            /* [in] */ BaseErrorDialog* service)
            : mHost(service)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        BaseErrorDialog* mHost;
    };

public:
    IALERTDIALOG_METHODS_DECL();

    IDIALOG_METHODS_DECL();

    IWINDOWCALLBACK_METHODS_DECL();

    BaseErrorDialog();

    BaseErrorDialog(
        /* [in] */ IContext* context);

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI_(void) OnStart();

    CARAPI_(Boolean) DispatchKeyEvent(
        /* [in] */ IKeyEvent* event);

    CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyLongPress(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    CARAPI Init(
        /* [in] */ IContext* context);

private:
    CARAPI_(void) HandleOnStartMessage();

    CARAPI_(void) SetEnabled(
        /* [in] */ Boolean enabled);

private:
    Boolean mConsuming;

public:
    AutoPtr<IHandler> mHandler;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_BASEERRORDIALOG_H__
