#ifndef  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWICONCACHE_H__
#define  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWICONCACHE_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Simple cache mechanism for PagedView outlines.
 */
class PagedViewIconCache
    : public Object
{
public:
    enum Type;

    class Key
        : public Object
        : public IKey
    {
    public:
        CAR_INTERFACE_DECL();

        Key(
            /* [in] */ IApplicationInfo* info);

        Key(
            /* [in] */ IResolveInfo* info);

        Key(
            /* [in] */ IAppWidgetProviderInfo* info);

        CARAPI_(Boolean) IsKeyType(
            /* [in] */ Type t);

        //@Override
        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* isEqual);

        //@Override
        CARAPI GetHashCode(
            /* [out] */ Int32* code);

    private:
        CARAPI_(AutoPtr<IComponentName>) GetComponentName();

    public:
        enum Type {
            ApplicationInfoKey,
            AppWidgetProviderInfoKey,
            ResolveInfoKey
        }

    private:
        AutoPtr<IComponentName> mComponentName;
        Type mType;
    };

public:
    PagedViewIconCache();

    CARAPI Clear();

    /** Removes all the keys to applications that aren't in the passed in collection */
    CARAPI RetainAllApps(
        /* [in] */ IArrayList* keys);

    /** Removes all the keys to shortcuts that aren't in the passed in collection */
    CARAPI RetainAllShortcuts(
        /* [in] */ IList* keys);

    /** Removes all the keys to widgets that aren't in the passed in collection */
    CARAPI RetainAllAppWidgets(
        /* [in] */ IList* keys);

    CARAPI AddOutline(
        /* [in] */ IKey* key,
        /* [in] */ IBitmap* b);

    CARAPI RemoveOutline(
        /* [in] */ IKey* key) ;

    CARAPI GetOutline(
        /* [in] */ IKey* key,
        /* [out] */ IBitmap** map);

private:
    CARAPI RetainAll(
        /* [in] */ IHashSet* keysToKeep,
        /* [in] */ Key::Type t);

private:
    AutoPtr<IHashMap> mIconOutlineCache;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWICONCACHE_H__