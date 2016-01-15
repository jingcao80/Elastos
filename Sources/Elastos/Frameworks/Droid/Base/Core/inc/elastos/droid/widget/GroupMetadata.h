#ifndef __ELASTOS_DROID_WIDGET_GROUPMETADATA_H__
#define __ELASTOS_DROID_WIDGET_GROUPMETADATA_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Widget {

class GroupMetadata : public IGroupMetadata, public ElRefBase
{
friend class ExpandableListConnector;
friend class ExpandableListView;

private:
    const static Int32 REFRESH = -1;
    Int32 mFlPos;
    Int32 mLastChildFlPos;
    Int32 mGPos;
    Int64 mGId;

private:
    GroupMetadata();

public:
    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

public:
    static CARAPI_(AutoPtr<GroupMetadata>) Obtain(
        /* [in] */ Int32 flPos,
        /* [in] */ Int32 lastChildFlPos,
        /* [in] */ Int32 gPos,
        /* [in] */ Int64 gId);

    CARAPI_(Int32) CompareTo(
        /* [in] */ GroupMetadata* another);



    //CARAPI WriteToParcel(
    //    /* [in] */ IParcel dest,
    //    /* [in] */ int32 flags);

};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_GROUPMETADATA_H__
