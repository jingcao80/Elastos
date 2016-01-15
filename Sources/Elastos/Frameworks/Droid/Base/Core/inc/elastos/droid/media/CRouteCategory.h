
#ifndef __ELASTOS_DROID_MEDIA_CROUTECATEGORY_H__
#define __ELASTOS_DROID_MEDIA_CROUTECATEGORY_H__

#include "_Elastos_Droid_Media_CRouteCategory.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/CMediaRouter.h"

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Definition of a category of routes. All routes belong to a category.
 */
CarClass(CRouteCategory)
{
public:
    CRouteCategory();

    CARAPI constructor(
        /* [in] */ ICharSequence* name,
        /* [in] */ Int32 types,
        /* [in] */ Boolean groupable);

    CARAPI constructor(
        /* [in] */ Int32 nameResId,
        /* [in] */ Int32 types,
        /* [in] */ Boolean groupable);

    /**
     * @return the name of this route category
     */
    CARAPI GetName(
        /* [out] */ ICharSequence** result);

    /**
     * Return the properly localized/configuration dependent name of this RouteCategory.
     *
     * @param context Context to resolve name resources
     * @return the name of this route category
     */
    CARAPI GetName(
        /* [in] */ IContext* context,
        /* [out] */ ICharSequence** result);

    /**
     * Return the current list of routes in this category that have been added
     * to the MediaRouter.
     *
     * <p>This list will not include routes that are nested within RouteGroups.
     * A RouteGroup is treated as a single route within its category.</p>
     *
     * @param out a List to fill with the routes in this category. If this parameter is
     *            non-null, it will be cleared, filled with the current routes with this
     *            category, and returned. If this parameter is null, a new List will be
     *            allocated to report the category's current routes.
     * @return A list with the routes in this category that have been added to the MediaRouter.
     */
    CARAPI GetRoutes(
        /* [in] */ IList* inList,
        /* [out] */ IList** retList);

    /**
     * @return Flag set describing the route types supported by this category
     */
    CARAPI GetSupportedTypes(
        /* [out] */ Int32* result);

    /**
     * Return whether or not this category supports grouping.
     *
     * <p>If this method returns true, all routes obtained from this category
     * via calls to {@link #getRouteAt(int)} will be {@link MediaRouter.RouteGroup}s.</p>
     *
     * @return true if this category supports
     */
    CARAPI IsGroupable(
        /* [out] */ Boolean* result);

    /**
     * @return true if this is the category reserved for system routes.
     * @hide
     */
    CARAPI IsSystem(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI SetIsSystem(
        /* [in] */ Boolean isSystem);

private:
    CARAPI_(AutoPtr<ICharSequence>) GetName(
        /* [in] */ IResources* res);

private:
    AutoPtr<ICharSequence> mName;

    Int32 mNameResId;

    Int32 mTypes;

    Boolean mGroupable;

    Boolean mIsSystem;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CROUTECATEGORY_H__
