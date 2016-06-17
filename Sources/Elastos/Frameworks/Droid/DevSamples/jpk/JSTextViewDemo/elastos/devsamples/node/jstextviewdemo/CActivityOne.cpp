
#include "CActivityOne.h"

#include "CTestEventListener.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace Node {
//namespace JSTextViewDemo {
namespace JSAppName {

EXTERN const _ELASTOS ClassID ECLSID_CActivityOne;

EXTERN NodeBridge* g_pNodeBridge;
EXTERN NodeBridge** g_ppNodeBridge;

EXTERN IHandler* myHandler;

const String CActivityOne::TAG("CActivityOne");

static const String DBG_TAG("CActivityOne");

ECode CActivityOne::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IMessageListener> listener = IMessageListener::Probe(mHost->mListener);
    return listener->OnHandleMessage(mHost, msg);
}

CAR_INTERFACE_IMPL(CActivityOne, Activity, IActivityOne)

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(TAG, " >> constructor()");

    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D(TAG, "OnCreate()-----");
    Activity::OnCreate(savedInstanceState);

    mHandler = new MyHandler(this);

    myHandler = mHandler;

    mPackageName = String("Elastos.DevSamples.Node.JSTextViewDemo");
    mActivityName = String("CActivityOne");

    AutoPtr<IInterface> helper;
    ECode ec = Require(String("/data/temp/node/bin/Elastos.DevSamples.Node.JSTextViewDemo.Helper.eco"), String("CActivityOneHelper"), (IInterface**)&helper);
    if (FAILED(ec)) {
        ALOGD("CActivityOne::OnCreate========create CActivityOneHelper failed!======nodejs module will be used");

        AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
        CTestEventListener::RegisterActivity(mPackageName, mActivityName, _this, (IActivityListener**)&mListener, mHandler.Get());
    }
    else {
        ALOGD("CActivityOne::OnCreate========create CActivityOneHelper success!======C++ epk will be used");

        mListener = IActivityListener::Probe(helper);
    }

    return mListener->OnCreate(this, savedInstanceState);
}

ECode CActivityOne::OnStart()
{
    Logger::I(DBG_TAG, " >> OnStart()");
    ECode ec = Activity::OnStart();
    mListener->OnStart(this);
    return ec;
}

ECode CActivityOne::OnResume()
{
    Logger::I(DBG_TAG, " >> OnResume()");
    ECode ec = Activity::OnResume();
    mListener->OnResume(this);
    return ec;
}

ECode CActivityOne::OnPause()
{
    Logger::I(DBG_TAG, " >> OnPause()");
    ECode ec = Activity::OnPause();
    mListener->OnPause(this);
    return ec;
}

ECode CActivityOne::OnStop()
{
    Logger::I(DBG_TAG, " >> OnStop()");
    ECode ec = Activity::OnStop();
    mListener->OnStop(this);
    return ec;
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(DBG_TAG, " >> OnDestroy()");
    ECode ec = Activity::OnDestroy();
    mListener->OnDestroy(this);
    return ec;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return mListener->OnActivityResult(this, requestCode, resultCode, data);
}

AutoPtr<IDialog> CActivityOne::OnCreateDialog(
   /* [in] */ Int32 id)
{
    AutoPtr<IDialog> dlg;

    AutoPtr<IInterface> alertDialog;
    mListener->OnCreateDialog(this, id, (IInterface**)&alertDialog);
    dlg = IDialog::Probe(alertDialog.Get());

    return dlg;
}

ECode CActivityOne::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return mListener->OnCreateContextMenu(this, menu, v, menuInfo);
}

ECode CActivityOne::OnClickPopupWindow(
    /* [in] */ IView* view)
{
    return NOERROR;
}

ECode CActivityOne::Require(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [out] */ IInterface** object)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(object != NULL);

    AutoPtr<IModuleInfo> moduleInfo;
    ECode ec = _CReflector_AcquireModuleInfo(
            moduleName, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }
    else {
        ALOGD("Acquire \"%s\" module info success!\n", moduleName.string());
    }

    AutoPtr<IClassInfo> classInfo;
    ec = moduleInfo->GetClassInfo(
            className, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" class info failed!\n", className.string());
        return ec;
    }
    else {
        ALOGD("Acquire \"%s\" class info success!\n", className.string());
    }

    AutoPtr<IInterface> testObject;
    ec = classInfo->CreateObject((IInterface**)&testObject);
    if (FAILED(ec)) {
        ALOGD("Create object failed!\n");
        return ec;
    }
    else {
        ALOGD("Create object success!\n");
    }

    *object = testObject;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

} // namespace JSTextViewDemo
} // namespace Node
} // namespace DevSamples
} // namespace Elastos
