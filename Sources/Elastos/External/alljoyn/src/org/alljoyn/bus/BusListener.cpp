
#include "org/alljoyn/bus/BusListener.h"
#include "alljoyn/BusListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

//==============================================================
// NativeBusListener
//==============================================================
class NativeBusListener : public ajn::BusListener
{
public:
    NativeBusListener(
        /* [in] */ Org::Alljoyn::Bus::BusListener* listener);

    ~NativeBusListener();

    // void Setup(jobject jbusAttachment);

    // void ListenerRegistered(BusAttachment* bus);
    // void ListenerUnregistered();
    // void FoundAdvertisedName(const char* name, TransportMask transport, const char* namePrefix);
    // void LostAdvertisedName(const char* name, TransportMask transport, const char* namePrefix);
    // void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner);
    // void BusStopping();
    // void BusDisconnected();

private:
    NativeBusListener(const NativeBusListener& other);
    NativeBusListener& operator =(const NativeBusListener& other);

    AutoPtr<IWeakReference> mBusListener;
    AutoPtr<IWeakReference> mBusAttachment;
};

NativeBusListener::NativeBusListener(
    /* [in] */ Org::Alljoyn::Bus::BusListener* listener)
{
    /*
     * Be careful when using a weak global reference.  They can only be
     * passed to NewLocalRef, NewGlobalRef and DeleteWeakGlobalRef.
     */
    listener->GetWeakReference((IWeakReference**)&mBusListener);
    assert(mBusListener != NULL);
}

NativeBusListener::~NativeBusListener()
{}


//==============================================================
// BusListener
//==============================================================
BusListener::BusListener()
    : mHandle(0)
{
    Create();
}

BusListener::~BusListener()
{
    Destroy();
}

void BusListener::Create()
{
    NativeBusListener* nbl = new NativeBusListener(this);
    assert(nbl != NULL);

    mHandle = reinterpret_cast<Int64>(nbl);
}

void BusListener::Destroy()
{
    NativeBusListener* nbl = reinterpret_cast<NativeBusListener*>(mHandle);

    assert(nbl != NULL);
    delete nbl;

    mHandle = 0;
    return;
}

ECode BusListener::ListenerRegistered(
    /* [in] */ IBusAttachment* bus)
{
    return NOERROR;
}

ECode BusListener::ListenerUnregistered()
{
    return NOERROR;
}

ECode BusListener::FoundAdvertisedName(
    /* [in] */ const String& name,
    /* [in] */ Int16 transport,
    /* [in] */ const String& namePrefix)
{
    return NOERROR;
}

ECode BusListener::LostAdvertisedName(
    /* [in] */ const String& name,
    /* [in] */ Int16 transport,
    /* [in] */ const String& namePrefix)
{
    return NOERROR;
}

ECode BusListener::NameOwnerChanged(
    /* [in] */ const String& busName,
    /* [in] */ const String& previousOwner,
    /* [in] */ const String& newOwner)
{
    return NOERROR;
}

// @Deprecated
// public void propertyChanged(String propName, Variant propValue) {}

ECode BusListener::BusStopping()
{
    return NOERROR;
}

ECode BusListener::BusDisconnected()
{
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
