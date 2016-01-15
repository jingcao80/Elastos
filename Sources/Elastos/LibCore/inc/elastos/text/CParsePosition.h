
#ifndef __ELASTOS_TEXT_CPARSEPOSITION_H__
#define __ELASTOS_TEXT_CPARSEPOSITION_H__

#include "_Elastos_Text_CParsePosition.h"
#include "Object.h"

namespace Elastos {
namespace Text {

CarClass(CParsePosition)
    , public Object
    , public IParsePosition
{
public:
    CAR_INTERFACE_DECL()

    CParsePosition();

    CARAPI constructor(
        /* [in] */ Int32 index);

    CARAPI Equals(
        /* [in] */ IInterface * object,
        /* [out] */ Boolean * result);

    CARAPI GetErrorIndex(
        /* [out] */ Int32* index);

    CARAPI GetIndex(
        /* [out] */ Int32* index);

    CARAPI GetHashCode(
        /* [out] */ Int32 * hash);

    CARAPI SetErrorIndex(
        /* [in] */ Int32 index);

    CARAPI SetIndex(
        /* [in] */ Int32 index);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int32 mCurrentPosition;
    Int32 mErrorIndex;
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CPARSEPOSITION_H__
