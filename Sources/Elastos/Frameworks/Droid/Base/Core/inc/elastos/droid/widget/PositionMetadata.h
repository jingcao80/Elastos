#ifndef __ELASTOS_DROID_WIDGET_POSITIONMETADATA_H__
#define __ELASTOS_DROID_WIDGET_POSITIONMETADATA_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
namespace Elastos {
namespace Droid {
namespace Widget {

class PositionMetadata
        : public IPositionMetadata
        , public ElRefBase
{
private:
    PositionMetadata();
    ~PositionMetadata();

    CARAPI ResetState();

    static CARAPI_(AutoPtr<PositionMetadata>) GetRecycledOrCreate();

public:
    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

public:
    static CARAPI_(AutoPtr<IPositionMetadata>) Obtain(
        /* [in] */ Int32 flatListPos,
        /* [in] */ Int32 type,
        /* [in] */ Int32 groupPos,
        /* [in] */ Int32 childPos,
        /* [in] */ IGroupMetadata* groupMetadata,
        /* [in] */ Int32 groupInsertIndex);

    CARAPI Recycle();

    CARAPI IsExpanded(
        /* [out] */ Boolean* res);

private:
    const static Int32 MAX_POOL_SIZE = 5;
    static List<AutoPtr<PositionMetadata> > sPool;
    static Object sLock;
    Object mLock;

public:
    AutoPtr<IExpandableListPosition> mPosition;
    AutoPtr<IGroupMetadata> mGroupMetadata;
    Int32 mGroupInsertIndex;
};


} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
