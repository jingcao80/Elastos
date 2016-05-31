#ifndef __ELASTOS_DROID_DATABASE_DATASETONSERVER_H__
#define __ELASTOS_DROID_DATABASE_DATASETONSERVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Database.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * Receives call backs when a data set has been changed, or made invalid. The typically data sets
 * that are observed are {@link Cursor}s or {@link android.widget.Adapter}s.
 * DataSetObserver must be implemented by objects which are added to a DataSetObservable.
 */
class ECO_PUBLIC DataSetObserver
    : public Object
    , public IDataSetObserver
{
public:
    CAR_INTERFACE_DECL()

    virtual CARAPI constructor();

    /**
     * This method is called when the entire data set has changed,
     * most likely through a call to {@link Cursor#requery()} on a {@link Cursor}.
     */
    virtual CARAPI OnChanged();

    /**
     * This method is called when the entire data becomes invalid,
     * most likely through a call to {@link Cursor#deactivate()} or {@link Cursor#close()} on a
     * {@link Cursor}.
     */
    virtual CARAPI OnInvalidated();
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_DATASETONSERVER_H__
