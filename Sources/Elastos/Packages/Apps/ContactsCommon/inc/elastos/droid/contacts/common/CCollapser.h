#ifndef __ELASTOS_APPS_CONTACTS_COMMON_CCOLLAPSER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_CCOLLAPSER_H__

#include "_Elastos_Apps_Contacts_Common_CCollapser.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

CarClass(CCollapser)
    , public Singleton
    , public ICollapser
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Collapses a list of Collapsible items into a list of collapsed items. Items are collapsed
     * if {@link Collapsible#shouldCollapseWith(Object)} returns true, and are collapsed
     * through the {@Link Collapsible#collapseWith(Object)} function implemented by the data item.
     *
     * @param list List of Objects of type <T extends Collapsible<T>> to be collapsed.
     */
    CARAPI CollapseList(
        /* [in] */ IList* list,
        /* [in] */ IContext* context);
};

} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_CCOLLAPSER_H__
