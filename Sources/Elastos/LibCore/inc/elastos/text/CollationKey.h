#ifndef __ELASTOS_TEXT_COLLATIONKEY_H__
#define __ELASTOS_TEXT_COLLATIONKEY_H__

#include "Elastos.CoreLibrary.Text.h"
#include "Object.h"

using Elastos::Core::IComparable;

namespace Elastos {
namespace Text {

class CollationKey
    : public Object
    , public ICollationKey
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& source);

    virtual CARAPI CompareTo(
        /* [in] */ IInterface* value,
        /* [out] */ Int32* result) = 0;

    virtual CARAPI GetSourceString(
        /* [out] */ String* sourceString);

    virtual CARAPI ToByteArray(
        /* [out, callee] */ ArrayOf<Byte>** array) = 0;


protected:
    CollationKey();

    virtual ~CollationKey();

private:
    String mSource;
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_COLLATIONKEY_H__
