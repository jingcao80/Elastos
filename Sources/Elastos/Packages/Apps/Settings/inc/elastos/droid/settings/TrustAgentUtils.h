#ifndef __ELASTOS_DROID_SETTINGS_TRUSTAGENTUTILS_H__
#define __ELASTOS_DROID_SETTINGS_TRUSTAGENTUTILS_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Settings {

class TrustAgentUtils
{
public:
    class TrustAgentComponentInfo
        : public Object
    {
    public:
        AutoPtr<IComponentName> mComponentName;
        String mTitle;
        String mSummary;
    };

public:
    /**
     * @return TRUE, if the service in resolveInfo has the permission to provide a trust agent.
     */
    static CARAPI_(Boolean) CheckProvidePermission(
        /* [in] */ IResolveInfo* resolveInfo,
        /* [in] */ IPackageManager* pm);

    static CARAPI_(AutoPtr<IComponentName>) GetComponentName(
        /* [in] */ IResolveInfo* resolveInfo);

    static CARAPI_(AutoPtr<TrustAgentComponentInfo>) GetSettingsComponent(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IResolveInfo* resolveInfo);

protected:
    static const String TAG;

private:
    static const String TRUST_AGENT_META_DATA;
    static const String PERMISSION_PROVIDE_AGENT;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_TRUSTAGENTUTILS_H__