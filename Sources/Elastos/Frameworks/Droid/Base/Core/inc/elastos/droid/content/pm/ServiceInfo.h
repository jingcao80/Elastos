
#ifndef __ELASTOS_DROID_CONTENT_PM_SERVICEINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_SERVICEINFO_H__

#include "ComponentInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Information you can retrieve about a particular application
 * service. This corresponds to information collected from the
 * AndroidManifest.xml's &lt;service&gt; tags.
 */
class ServiceInfo
    : public ComponentInfo
    , public IServiceInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    ServiceInfo();

    virtual ~ServiceInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IServiceInfo* orig);

    CARAPI Dump(
        /* [in] */ IPrinter * pw,
        /* [in] */ const String& prefix);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetPermission(
        /* [out] */ String* permission);

    CARAPI SetPermission(
        /* [in] */ const String& permission);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

public:
    /**
     * Optional name of a permission required to be able to access this
     * Service.  From the "permission" attribute.
     */
    String mPermission;

    /**
     * Options that have been set in the service declaration in the
     * manifest.
     * These include:
     * {@link #FLAG_STOP_WITH_TASK}, {@link #FLAG_ISOLATED_PROCESS},
     * {@link #FLAG_SINGLE_USER}.
     */
    Int32 mFlags;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CSERVICEINFO_H__
