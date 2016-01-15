
#ifndef __ELASTOS_DROID_SERVER_AM_COMPATMODEDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_COMPATMODEDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/app/Dialog.h"




using Elastos::Droid::App::Dialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IViewOnCreateContextMenuListener;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Content::Pm::IApplicationInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CompatModeDialog
    : public Object
    , public Dialog
    , public IDialog
    , public IWindowCallback
    , public IKeyEventCallback
    , public IViewOnCreateContextMenuListener
{
private:
    class CompatEnabledListener
        : public Object
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        CompatEnabledListener(
            /* [in] */ CompatModeDialog* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        CompatModeDialog* mHost;
    };

    class AlwaysShowListener
        : public Object
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        AlwaysShowListener(
            /* [in] */ CompatModeDialog* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        CompatModeDialog* mHost;
    };

public:
    CompatModeDialog(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ IContext* context,
        /* [in] */ IApplicationInfo* appInfo);

    CAR_INTERFACE_DECL()

    IDIALOG_METHODS_DECL()

    IWINDOWCALLBACK_METHODS_DECL()

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

    CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

    CARAPI_(void) UpdateControls();

public:
    CActivityManagerService* mService;
    AutoPtr<IApplicationInfo> mAppInfo;

    AutoPtr<ISwitch> mCompatEnabled;
    AutoPtr<ICheckBox> mAlwaysShow;
    AutoPtr<IView> mHint;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_COMPATMODEDIALOG_H__
