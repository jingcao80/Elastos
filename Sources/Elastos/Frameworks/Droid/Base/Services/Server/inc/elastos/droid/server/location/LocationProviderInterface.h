
#ifndef __ELASTOS_DROID_SERVER_LOCATION_LOCATIONPROVIDERINTERFACE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_LOCATIONPROVIDERINTERFACE_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif

using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Location::IProviderRequest;
using Elastos::Droid::Location::IProviderProperties;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IWorkSource;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

extern "C" const InterfaceID EIID_ILocationProviderInterface;

/**
 * Location Manager's interface for location providers.
 * @hide
 */
interface ILocationProviderInterface : public IInterface
{
public:

    static CARAPI_(ILocationProviderInterface*) Probe(PInterface obj)
    {
        return (ILocationProviderInterface*)obj->Probe(EIID_ILocationProviderInterface);
    }

    virtual CARAPI GetName(
        /* [out] */ String* name) = 0;

    virtual CARAPI Enable() = 0;

    virtual CARAPI Disable() = 0;

    virtual CARAPI IsEnabled(
        /* [out] */ Boolean* enable) = 0;

    virtual CARAPI SetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* source) = 0;

    virtual CARAPI SwitchUser(
        /* [in] */ Int32 userId) = 0;

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args) = 0;

    // --- deprecated (but still supported) ---
    virtual CARAPI GetProperties(
        /* [out] */ IProviderProperties** properties) = 0;

    virtual CARAPI GetStatus(
        /* [in] */ IBundle* extras,
        /*  [out] */ Int32* status) = 0;

    virtual CARAPI GetStatusUpdateTime(
        /* [out] */ Int64* time) = 0;

    virtual CARAPI SendExtraCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras,
        /* [out] */ Boolean* result) = 0;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_LOCATIONPROVIDERINTERFACE_H__
