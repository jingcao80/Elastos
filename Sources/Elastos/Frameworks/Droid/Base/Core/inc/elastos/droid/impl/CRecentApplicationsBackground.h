#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CRECENTAPPLICATIONSBACKGROUND_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CRECENTAPPLICATIONSBACKGROUND_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CRecentApplicationsBackground.h"
#include "impl/RecentApplicationsBackground.h"


#include "elastos/droid/widget/LinearLayout.h"


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CRecentApplicationsBackground), public RecentApplicationsBackground
{
public:
    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    ILINEARLAYOUT_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CRECENTAPPLICATIONSBACKGROUND_H__
