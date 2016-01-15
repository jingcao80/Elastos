#ifndef __ELASTOS_DROID_DATABASE_CHARARRAYBUFFER_H__
#define __ELASTOS_DROID_DATABASE_CHARARRAYBUFFER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Database.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * This is used for {@link Cursor#copyStringToBuffer}
 */
class CharArrayBuffer
    : public Object
    , public ICharArrayBuffer
{
public:
    CAR_INTERFACE_DECL()

    CharArrayBuffer();

    CARAPI constructor(
        /* [in] */ Int32 size);

    CARAPI constructor(
        /* [in] */ ArrayOf<Char32>* buf);

    virtual CARAPI GetData(
        /* [out,callee] */ ArrayOf<Char32>** data);

    virtual CARAPI SetData(
        /* [in] */ ArrayOf<Char32>* data);

    virtual CARAPI GetSizeCopied(
        /* [out] */ Int32* sizeCopied);

    virtual CARAPI SetSizeCopied(
        /* [in] */ Int32 sizeCopied);

public:
    AutoPtr< ArrayOf<Char32> > mData;
    Int32 mSizeCopied;
};

} //Database
} //Droid
} //Elastoss

#endif //_CHARARRAYBUFFER_H_
