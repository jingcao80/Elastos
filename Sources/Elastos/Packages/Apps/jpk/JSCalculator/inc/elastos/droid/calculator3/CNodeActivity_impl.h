#include "CNodeActivity.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Utility::Logging::Logger;

// namespace Elastos {
// namespace DevSamples {
// namespace Node {
// namespace JSPkgName {

namespace Elastos {
namespace Droid {
namespace Calculator3 {

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

//CAR_INTERFACE_IMPL(JSActName, Activity, IActivityOne)
CAR_INTERFACE_IMPL(JSActName, Activity, ICalculator)

JS_CAR_OBJECT_IMPL(JSActName)

ECode JSActName::constructor()
{
    Logger::I(TAG, " >> constructor()");

    return Activity::constructor();
}

ECode JSActName::_OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    return Activity::OnCreate(savedInstanceState);
}
ECode JSActName::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D(TAG, "OnCreate()-----");
    Activity::OnCreate(savedInstanceState);

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
    ECode ec = JSEvtName::Require(_helperEcoName, _helperClsName, (IInterface**)&helper);

    if (FAILED(ec)) {
        ALOGD("OnCreate========create Helper failed!======nodejs module will be used");
        AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

        ALOGD("OnCreate========_this:%d======auto:%d",(Int32)_this.Get(), (Int32)*(Int32**)&_this);

        //JSEvtName::RegisterActivity(mPackageName, mActivityName, _this, (IActivityListener**)&mListener, mHandler.Get());
        JSEvtName::RegisterCalculator(mPackageName, mActivityName, _this, (ICalculatorListener**)&mListener, mHandler.Get());
        //REFCOUNT_ADD(_this.Get());
    }
    else {
        ALOGD("OnCreate========create Helper success!======C++ epk will be used");
        //mListener = IActivityListener::Probe(helper);
        mListener = ICalculatorListener::Probe(helper);
    }

    return mListener->OnCreate(this, savedInstanceState);
}


ECode JSActName::_OnSaveInstanceState(
    /*[in] */ /*@NonNull */IBundle* outState)
{
    return Activity::OnSaveInstanceState(outState);
}
ECode JSActName::OnSaveInstanceState(
    /*[in] */ /*@NonNull */IBundle* outState)
{
    //AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
    //return mListener->OnSaveInstanceState(this, outState);

    Logger::I(DBG_TAG, " >> OnSaveInstanceState()");
    ECode ec = Activity::OnSaveInstanceState(outState);
    mListener->OnSaveInstanceState(this, outState);
    return ec;
}

ECode JSActName::_OnBackPressed()
{
    return Activity::OnBackPressed();
}
ECode JSActName::OnBackPressed()
{
    //AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
    return mListener->OnBackPressed(this);
}

ECode JSActName::_OnUserInteraction()
{
    return Activity::OnUserInteraction();
}
ECode JSActName::OnUserInteraction()
{
    //AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
    return mListener->OnUserInteraction(this);
}

ECode JSActName::OnStart()
{
    Logger::I(DBG_TAG, " >> OnStart()");
    ECode ec = Activity::OnStart();
    //mListener->OnStart(this);
    return ec;
}

ECode JSActName::OnResume()
{
    Logger::I(DBG_TAG, " >> OnResume()");
    ECode ec = Activity::OnResume();
    //mListener->OnResume(this);

    ALOGD("====OnResume====end====ECode:%d", ec);
    return ec;
}

ECode JSActName::OnPause()
{
    Logger::I(DBG_TAG, " >> OnPause()");
    ECode ec = Activity::OnPause();
    //mListener->OnPause(this);
    return ec;
}

ECode JSActName::OnStop()
{
    Logger::I(DBG_TAG, " >> OnStop()");
    ECode ec = Activity::OnStop();
    //mListener->OnStop(this);
    return ec;
}

ECode JSActName::OnDestroy()
{
    Logger::I(DBG_TAG, " >> OnDestroy()");
    ECode ec = Activity::OnDestroy();
    //mListener->OnDestroy(this);
    return ec;
}

} // namespace Calculator3
} // namespace Droid
} // namespace Elastos

// } // namespace JSPkgName
// } // namespace Node
// } // namespace DevSamples
// } // namespace Elastos