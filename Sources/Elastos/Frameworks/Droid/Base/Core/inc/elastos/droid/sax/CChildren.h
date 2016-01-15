#ifndef __ELASTOS_DROID_SAX_CCHILDRENT_H__
#define __ELASTOS_DROID_SAX_CCHILDRENT_H__

#include "_Elastos_Droid_Sax_CChildren.h"
#include "elastos/droid/sax/Element.h"

namespace Elastos {
namespace Droid {
namespace Sax {

/**
 * Contains element children. Using this class instead of HashMap results in
 * measurably better performance.
 */
CarClass(CChildren)
    , public Object
{
public:
    class Child
        : public Element
    {
    public:
        Child(
            /* [in] */ IElement* parent,
            /* [in] */ const String& uri,
            /* [in] */ const String& localName,
            /* [in] */ Int32 depth,
            /* [in] */ Int32 hash)
            : mHash(hash)
        {
            Element::constructor(parent, uri, localName, depth);
        }

        virtual ~Child();

    public:
        const Int32 mHash;
        AutoPtr<Child> mNext;
    };

public:

    CAR_OBJECT_DECL();

    CChildren();
    virtual ~CChildren();

    /**
     * Looks up a child by name and creates a new one if necessary.
     */
    CARAPI GetOrCreate(
        /* [in] */ IElement* parent,
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ IElement** result);

    /**
     * Looks up a child by name.
     */
    CARAPI Get(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ IElement** result);

public:
    AutoPtr<ArrayOf<Child*> > mChildren;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Sax::CChildren::Child, IInterface);

#endif //__ELASTOS_DROID_SAX_CCHILDRENT_H__
