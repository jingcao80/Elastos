
#ifndef __ELASTOS_DROID_WIDGET_CURSORFILTER_H__
#define __ELASTOS_DROID_WIDGET_CURSORFILTER_H__

#include "elastos/droid/widget/Filter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * <p>The CursorFilter delegates most of the work to the CursorAdapter.
 * Subclasses should override these delegate methods to run the queries
 * and convert the results into String that can be used by auto-completion
 * widgets.</p>
 */
class CursorFilter
    : public Filter
    , public ICursorFilter
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ICursorFilterClient* client);

    virtual CARAPI ConvertResultToString(
        /* [in] */ IInterface* resultValue,
        /* [out] */ ICharSequence** cs);

    virtual CARAPI PerformFiltering(
        /* [in] */ ICharSequence* constraint,
        /* [out] */ IFilterResults** filterResults);

    virtual CARAPI PublishResults(
        /* [in] */ ICharSequence* constraint,
        /* [in] */ IFilterResults* results);

    AutoPtr<ICursorFilterClient> mClient;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos
#endif
