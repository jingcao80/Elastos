#include "elastos/droid/impl/CRecentApplicationsBackground.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

IVIEW_METHODS_IMPL(CRecentApplicationsBackground, RecentApplicationsBackground)
IVIEWGROUP_METHODS_IMPL(CRecentApplicationsBackground, RecentApplicationsBackground)
IVIEWPARENT_METHODS_IMPL(CRecentApplicationsBackground, RecentApplicationsBackground)
IVIEWMANAGER_METHODS_IMPL(CRecentApplicationsBackground, RecentApplicationsBackground)
IDRAWABLECALLBACK_METHODS_IMPL(CRecentApplicationsBackground, RecentApplicationsBackground)
IKEYEVENTCALLBACK_METHODS_IMPL(CRecentApplicationsBackground, RecentApplicationsBackground)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CRecentApplicationsBackground, RecentApplicationsBackground)
ILINEARLAYOUT_METHODS_IMPL(CRecentApplicationsBackground, RecentApplicationsBackground)

PInterface CRecentApplicationsBackground::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CRecentApplicationsBackground::Probe(riid);
}

ECode CRecentApplicationsBackground::constructor(
    /* [in] */ IContext* context)
{
    return RecentApplicationsBackground::Init(context);
}

ECode CRecentApplicationsBackground::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RecentApplicationsBackground::Init(context, attrs);
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
