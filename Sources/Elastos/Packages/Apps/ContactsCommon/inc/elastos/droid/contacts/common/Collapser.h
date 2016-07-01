#ifndef __ELASTOS_APPS_CONTACTS_COMMON_COLLAPSER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_COLLAPSER_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

/**
 * Class used for collapsing data items into groups of similar items. The data items that should be
 * collapsible should implement the Collapsible interface. The class also contains a utility
 * function that takes an ArrayList of items and returns a list of the same items collapsed into
 * groups.
 */
class Collapser
    : public Object
{
public:
    /**
     * Collapses a list of Collapsible items into a list of collapsed items. Items are collapsed
     * if {@link Collapsible#shouldCollapseWith(Object)} returns true, and are collapsed
     * through the {@Link Collapsible#collapseWith(Object)} function implemented by the data item.
     *
     * @param list List of Objects of type <T extends Collapsible<T>> to be collapsed.
     */
    static CARAPI_(void) CollapseList(
        /* [in] */ IList* list,
        /* [in] */ IContext* context);

private:
    Collapser();

private:
    /*
     * The Collapser uses an n^2 algorithm so we don't want it to run on
     * lists beyond a certain size. This specifies the maximum size to collapse.
     */
    static const Int32 MAX_LISTSIZE_TO_COLLAPSE; // = 20;

};

} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_COLLAPSER_H__
