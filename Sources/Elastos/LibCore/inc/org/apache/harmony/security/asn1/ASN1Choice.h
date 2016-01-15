
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1CHOICE_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1CHOICE_H__

#include "ASN1Type.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Utility::INavigableMap;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class ASN1Choice : public ASN1Type
{
public:
    CARAPI constructor(
        /* [in] */ ArrayOf<IASN1Type*>* asn1Type);

    virtual CARAPI CheckTag(
        /* [in] */ Int32 identifier,
        /* [out] */ Boolean* checkTag);

    virtual CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    virtual CARAPI EncodeASN(
        /* [in] */ IBerOutputStream* bos);

    virtual CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    virtual CARAPI GetIndex(
        /* [in] */ IInterface* object,
        /* [out] */ Int32* index) = 0;

    virtual CARAPI GetObjectToEncode(
        /* [in] */ IInterface* object,
        /* [out] */ IInterface** obj);

    virtual CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

private:
    CARAPI AddIdentifier(
        /* [in] */ INavigableMap* map,
        /* [in] */ Int32 identifier,
        /* [in] */ Int32 index);

public:
    AutoPtr<ArrayOf<IASN1Type*> > mType;

private:
    /**
     * identifiers table: [2][number of distinct identifiers]
     * identifiers[0]: stores identifiers (includes nested choices)
     * identifiers[1]: stores identifiers' indexes in array of types
     */
    AutoPtr<ArrayOf<IArrayOf*> > mIdentifiers;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __CASN1CHOICE_H__
