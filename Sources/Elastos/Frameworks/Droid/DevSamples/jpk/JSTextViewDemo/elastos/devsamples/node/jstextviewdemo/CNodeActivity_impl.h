#include "CNodeActivity.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

EXTERN NodeBridge* g_pNodeBridge;
EXTERN NodeBridge** g_ppNodeBridge;

EXTERN IHandler* myHandler;

const String JSActName::TAG(JSActNameStr);

static const String DBG_TAG(JSActNameStr);

ECode JSActName::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IMessageListener> listener = IMessageListener::Probe(mHost->mListener);
    return listener->OnHandleMessage(mHost, msg);
}

CAR_INTERFACE_IMPL(JSActName, Activity, IActivityOne)

JS_CAR_OBJECT_IMPL(JSActName)

ECode JSActName::constructor()
{
    Logger::I(TAG, " >> constructor()");

    return Activity::constructor();
}

ECode JSActName::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D(TAG, "OnCreate()-----");
    Activity::OnCreate(savedInstanceState);

    ECode ec;

    mHandler = new MyHandler(this);

    myHandler = mHandler;

    String _pkgPath = String("/data/temp/node/bin/");
    String _pkgName = String(JSPkgNameStr);
    String _nspName = String("Elastos.DevSamples.Node.");

    mPackageName = _nspName + _pkgName;
    mActivityName = String(JSActNameStr);

    String _helperEcoName = _pkgPath + mPackageName + String(".Helper.eco");
    String _helperClsName = mPackageName + String(".") + mActivityName + String("Helper");

    AutoPtr<IInterface> helper;
    ec = JSEvtName::Require(_helperEcoName, _helperClsName, (IInterface**)&helper);

    if (FAILED(ec)) {
        ALOGD("OnCreate========create Helper failed!======nodejs module will be used");
        AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
        JSEvtName::RegisterActivity(mPackageName, mActivityName, _this, (IActivityListener**)&mListener, mHandler.Get());
    }
    else {
        ALOGD("OnCreate========create Helper success!======C++ epk will be used");
        mListener = IActivityListener::Probe(helper);
    }

    if (mListener) {
        ec = mListener->OnCreate(this, savedInstanceState);
    }
    else {
        ALOGD("OnCreate========ERROR:mListener is NULL======");
    }

    return ec;
}

ECode JSActName::OnStart()
{
    Logger::I(DBG_TAG, " >> OnStart()");
    ECode ec = Activity::OnStart();
    mListener->OnStart(this);
    return ec;
}

ECode JSActName::OnResume()
{
    Logger::I(DBG_TAG, " >> OnResume()");
    ECode ec = Activity::OnResume();
    mListener->OnResume(this);
    return ec;
}

ECode JSActName::OnPause()
{
    Logger::I(DBG_TAG, " >> OnPause()");
    ECode ec = Activity::OnPause();
    mListener->OnPause(this);
    return ec;
}

ECode JSActName::OnStop()
{
    Logger::I(DBG_TAG, " >> OnStop()");
    ECode ec = Activity::OnStop();
    mListener->OnStop(this);
    return ec;
}

ECode JSActName::OnDestroy()
{
    Logger::I(DBG_TAG, " >> OnDestroy()");
    ECode ec = Activity::OnDestroy();
    mListener->OnDestroy(this);
    return ec;
}

ECode JSActName::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return mListener->OnActivityResult(this, requestCode, resultCode, data);
}

AutoPtr<IDialog> JSActName::OnCreateDialog(
   /* [in] */ Int32 id)
{
    AutoPtr<IDialog> dlg;

    AutoPtr<IInterface> alertDialog;
    mListener->OnCreateDialog(this, id, (IInterface**)&alertDialog);
    dlg = IDialog::Probe(alertDialog.Get());

    return dlg;
}

ECode JSActName::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return mListener->OnCreateContextMenu(this, menu, v, menuInfo);
}

ECode JSActName::OnClickPopupWindow(
    /* [in] */ IView* view)
{
    return NOERROR;
}

} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos