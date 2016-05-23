
#include "elastos/droid/location/CountryDetector.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::CHandler;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

//============================================================
//CountryDetector::CountryDetectorListenerTransport::CountryDetectorListenerTransportRunnnable
//============================================================

CountryDetector::CountryDetectorListenerTransport::CountryDetectorListenerTransportRunnnable::CountryDetectorListenerTransportRunnnable(
    /* [in] */ ICountry* country,
    /* [in] */ CountryDetectorListenerTransport* host)
    : mCountry(country)
    , mHost(host)
{}

ECode CountryDetector::CountryDetectorListenerTransport::CountryDetectorListenerTransportRunnnable::Run()
{
    mHost->mListener->OnCountryDetected(mCountry);
    return NOERROR;
}

//===================================
//CountryDetector::CountryDetectorListenerTransport
//===================================

CAR_INTERFACE_IMPL_2(CountryDetector::CountryDetectorListenerTransport, Object, ICountryDetectorListenerTransport, IICountryListener)

CountryDetector::CountryDetectorListenerTransport::CountryDetectorListenerTransport(
    /* [in] */ ICountryListener* listener,
    /* [in] */ ILooper* looper)
        : mListener(listener)
{
    if (looper != NULL) {
        CHandler::New(looper, (IHandler**)&mHandler);
    } else {
        CHandler::New((IHandler**)&mHandler);
    }
}

ECode CountryDetector::CountryDetectorListenerTransport::OnCountryDetected(
    /* [in] */ ICountry* country)
{
    AutoPtr<CountryDetectorListenerTransportRunnnable> r = new CountryDetectorListenerTransportRunnnable(country, this);
    Boolean result = FALSE;
    mHandler->Post(IRunnable::Probe(r), &result);
    return NOERROR;
}

//=============
//CountryDetector
//=============

const String CountryDetector::TAG("CountryDetector");

CAR_INTERFACE_IMPL(CountryDetector, Object, ICountryDetector)

CountryDetector::CountryDetector()
{}

ECode CountryDetector::constructor(
    /* [in] */ IICountryDetector* service)
{
    mService = service;
    return NOERROR;
}

ECode CountryDetector::DetectCountry(
    /* [out] */ ICountry** country)
{
    VALIDATE_NOT_NULL(country);
    return mService->DetectCountry(country);
}


ECode CountryDetector::AddCountryListener(
    /* [in] */ ICountryListener* listener,
    /* [in] */ ILooper* looper)
{
    {    AutoLock syncLock(this);
        Boolean isContained;
        mListeners->ContainsKey(listener, &isContained);
        if (!isContained) {
            AutoPtr<CountryDetectorListenerTransport> transport = new CountryDetectorListenerTransport(listener, looper);
            ECode ec = mService->AddCountryListener((IICountryListener*)transport.Get());
            if (FAILED(ec)) {
                Logger::E(TAG, "addCountryListener: RemoteException%08x", ec);
                return E_REMOTE_EXCEPTION;
            }
            mListeners->Put(listener, (IICountryListener*)transport.Get());
        }
    }
    return NOERROR;
}


ECode CountryDetector::RemoveCountryListener(
    /* [in] */ ICountryListener* listener)
{
    {    AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        mListeners->Get(listener, (IInterface**)&obj);
        AutoPtr<IICountryListener> transport = IICountryListener::Probe(obj);
        if (transport != NULL) {
            mListeners->Remove(listener);
            ECode ec = mService->RemoveCountryListener(transport);
            if (FAILED(ec)) {
                Logger::E(TAG, "removeCountryListener: RemoteException%08x", ec);
                return E_REMOTE_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

} // namespace Location
} // namepsace Droid
} // namespace Elastos