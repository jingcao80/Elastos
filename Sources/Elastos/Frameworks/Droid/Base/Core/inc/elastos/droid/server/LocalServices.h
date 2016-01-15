#ifndef __ELASTOS_DROID_SERVER_LOCAL_SERVICES_H__
#define __ELASTOS_DROID_SERVER_LOCAL_SERVICES_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::HashMap;

_ETL_NAMESPACE_BEGIN
template<> struct Hash<InterfaceID>
{
    size_t operator()(const InterfaceID& id) const
    {
        Int32 prime = 31;
        Int32 result = 1;
        result = prime * result + id.mData1;
        result = prime * result + id.mData2;
        result = prime * result + id.mData3;
        result = prime * result + id.mData4[0];
        result = prime * result + id.mData4[1];
        result = prime * result + id.mData4[2];
        result = prime * result + id.mData4[3];
        result = prime * result + id.mData4[4];
        result = prime * result + id.mData4[5];
        result = prime * result + id.mData4[6];
        result = prime * result + id.mData4[7];
        return result;
    }
};
_ETL_NAMESPACE_END

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * This class is used in a similar way as ServiceManager, except the services registered here
 * are not Binder objects and are only available in the same process.
 *
 * Once all services are converted to the SystemService interface, this class can be absorbed
 * into SystemServiceManager.
 *
 * {@hide}
 */
class ECO_PUBLIC LocalServices
{
public:
    /**
     * Returns a local service instance that implements the specified interface.
     *
     * @param type The type of service.
     * @return The service object.
     */
    // @SuppressWarnings("unchecked")
    static AutoPtr<IInterface> GetService(
        /* [in] */ const InterfaceID& type);

    /**
     * Adds a service instance of the specified interface to the global registry of local services.
     */
    static CARAPI AddService(
        /* [in] */ const InterfaceID& type,
        /* [in] */ IInterface* service);

private:
    ECO_LOCAL LocalServices();
    ECO_LOCAL LocalServices(const LocalServices&);

private:
    static Object sLock;
    static HashMap<InterfaceID, AutoPtr<IInterface> > sLocalServiceObjects;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCAL_SERVICES_H__