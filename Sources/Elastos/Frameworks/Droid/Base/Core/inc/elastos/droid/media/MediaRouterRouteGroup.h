#ifndef __ELASTOS_DROID_MEDIA_MEDIAROUTERROUTEGROUP_H__
#define __ELASTOS_DROID_MEDIA_MEDIAROUTERROUTEGROUP_H__

#include "elastos/droid/media/MediaRouterRouteInfo.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

class MediaRouterRouteGroup
    : public MediaRouterRouteInfo
    , public IMediaRouterRouteGroup
{
public:
    MediaRouterRouteGroup();

    virtual ~MediaRouterRouteGroup();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IMediaRouterRouteCategory* category);

    CARAPI GetName(
        /* [in] */ IResources* res,
        /* [out] */ ICharSequence** result);

    CARAPI AddRoute(
        /* [in] */ IMediaRouterRouteInfo * route);

    CARAPI AddRoute(
        /* [in] */ IMediaRouterRouteInfo * route,
        /* [in] */ Int32 insertAt);

    CARAPI RemoveRoute(
        /* [in] */ IMediaRouterRouteInfo * route);

    CARAPI RemoveRoute(
        /* [in] */ Int32 index);

    CARAPI GetRouteCount(
        /* [out] */ Int32 * result);

    CARAPI GetRouteAt(
        /* [in] */ Int32 index,
        /* [out] */ IMediaRouterRouteInfo ** result);

    CARAPI SetIconDrawable(
        /* [in] */ IDrawable * icon);

    CARAPI SetIconResource(
        /* [in] */ Int32 resId);

    CARAPI RequestSetVolume(
        /* [in] */ Int32 volume);

    CARAPI RequestUpdateVolume(
        /* [in] */ Int32 direction);

    CARAPI_(void) MemberNameChanged(
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ ICharSequence* name);

    CARAPI_(void) MemberStatusChanged(
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ ICharSequence* status);

    CARAPI_(void) MemberVolumeChanged(
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI_(void) UpdateVolume();

    CARAPI RouteUpdated();

    CARAPI_(void) UpdateName();

    CARAPI ToString(
        /* [out] */ String* result);

public:
    AutoPtr<IArrayList> mRoutes;

private:
    Boolean mUpdateName;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_MEDIAROUTERROUTEGROUP_H__