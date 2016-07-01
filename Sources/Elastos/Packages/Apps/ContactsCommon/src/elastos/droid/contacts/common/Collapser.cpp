
#include "Collapser.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

const Int32 Collapser::MAX_LISTSIZE_TO_COLLAPSE = 20;

void Collapser::CollapseList(
    /* [in] */ IList* list,
    /* [in] */ IContext* context)
{
    Int32 listSize;
    list->GetSize(&listSize);
    // The algorithm below is n^2 so don't run on long lists
    if (listSize > MAX_LISTSIZE_TO_COLLAPSE) {
        return;
    }

    for (Int32 i = 0; i < listSize; i++) {
        AutoPtr<IInterface> iTemp;
        list->Get(i, (IInterface**)&iTemp);
        ICollapsible* iItem = ICollapsible::Probe(iTemp);
        if (iItem != NULL) {
            for (Int32 j = i + 1; j < listSize; j++) {
                AutoPtr<IInterface> jTemp;
                list->Get(j, (IInterface**)&jTemp);
                ICollapsible* jItem = ICollapsible::Probe(jTemp);
                if (jItem != NULL) {
                    Boolean result;
                    if (iItem->ShouldCollapseWith(jItem, context, &result), result) {
                        iItem->CollapseWith(jItem);
                        list->Set(j, NULL);
                    }
                    else if (jItem.shouldCollapseWith(iItem, context, &result), result) {
                        jItem->CollapseWith(iItem);
                        list->Set(i, NULL);
                        break;
                    }
                }
            }
        }
    }

    // Remove the null items
    AutoPtr<IIterator> itr;
    list->GetIterator((IIterator**)&itr);
    Boolean hasNext;
    while (itr->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> item;
        itr->GetNext((IInterface**)&item);
        if (item == NULL) {
            itr->Remove();
        }
    }
}

} // Common
} // Contacts
} // Apps
} // Elastos
