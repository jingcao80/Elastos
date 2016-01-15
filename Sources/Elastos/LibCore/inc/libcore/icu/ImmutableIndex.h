#ifndef __LIBCORE_ICU_IMMUTABLEINDEX_H__
#define __LIBCORE_ICU_IMMUTABLEINDEX_H__

#include "Object.h"
#include "AlphabeticIndex.h"


namespace Libcore {
namespace ICU {

extern "C" const _ELASTOS ClassID ECLSID_ImmutableIndex;

class ImmutableIndex
    : public Object
    , public IImmutableIndex
{
public:
    CAR_INTERFACE_DECL()

protected:
    ~ImmutableIndex();

private:
    ImmutableIndex(
        /* [in] */ Int64 peer);

public:
    CARAPI GetBucketCount(
        /* [out] */ Int32* count);

    CARAPI GetBucketIndex(
        /* [in] */ const String& s,
        /* [out] */ Int32* index);

    CARAPI GetBucketLabel(
        /* [in] */ Int32 index,
        /* [out] */ String* label);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI GetClassID(
        /* [out] */ ClassID *pCLSID);

private:
    CARAPI_(Int32) GetBucketCount(Int64 peer);

    CARAPI_(Int32) GetBucketIndex(Int64 peer, const String& s);

    CARAPI_(String) GetBucketLabel(Int64 peer, Int32 index);


private:
    Int64 mPeer;

    friend class AlphabeticIndex;
};

} // namespace ICU
} // namespace Libcore

#endif // __LIBCORE_ICU_IMMUTABLEINDEX_H__