
#ifndef __Elastos_DevSamples_DFObserver_CDoorService_H__
#define __Elastos_DevSamples_DFObserver_CDoorService_H__

#include "_Elastos_DevSamples_DFObserver_CDoorService.h"
#include <_Org.Alljoyn.Bus.h>
#include <elastos/core/Runnable.h>
#include "BusHandler.h"

using Org::Alljoyn::Bus::IAboutObj;
using Org::Alljoyn::Bus::IBusObject;
using Org::Alljoyn::Bus::IPropertyChangedEmitter;
using Elastos::Core::Runnable;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

/**
 * A Class implementing a basic door provider service.
 */
CarClass(CDoorService)
    , public Object
    , public IDoorService
    , public IDoor
    , public IBusObject
{
private:
    class OpenRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CDoorService::OpenRunnable")

        OpenRunnable(
            /* [in] */ CDoorService* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CDoorService* mHost;
    };

    class CloseRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CDoorService::CloseRunnable")

        CloseRunnable(
            /* [in] */ CDoorService* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CDoorService* mHost;
    };

    class KnockAndRunRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CDoorService::KnockAndRunRunnable")

        KnockAndRunRunnable(
            /* [in] */ CDoorService* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CDoorService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDoorService();

    ~CDoorService();

    CARAPI constructor(
        /* [in] */ const String& location,
        /* [in] */ IHandler* handler);

    CARAPI SetDisplayName(
        /* [in] */ const String& name);

    CARAPI GetIsOpen(
        /* [out] */ Boolean* value);

    CARAPI GetLocation(
        /* [out] */ String* value);

    CARAPI GetKeyCode(
        /* [out] */ Int32* value);

    CARAPI Open();

    CARAPI Close();

    CARAPI KnockAndRun();

    CARAPI PersonPassedThrough(
        /* [in] */ const String& person);

    /**
     * Internal function to get the location of this door. No event is send to UI.
     *
     * @return Location of the door.
     */
    CARAPI GetInternalLocation(
        /* [out] */ String* value);

    /**
     * Internal helper to. Send an property changed event to all connected
     * listeners.
     *
     * @param b
     */
    CARAPI SendDoorEvent(
        /* [in] */ Boolean b);

private:

    void SendUiMessage(
        /* [in] */ const String& string);

    void SendWorkerMessage(
        /* [in] */ IRunnable* runnable);

private:
    Boolean mIsOpen;
    String mLocation;
    Int32 mKeyCode;
    AutoPtr<IPropertyChangedEmitter> mPce;
    AutoPtr<BusHandler> mBushandler;
};

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DFObserver_CDoorService_H__
