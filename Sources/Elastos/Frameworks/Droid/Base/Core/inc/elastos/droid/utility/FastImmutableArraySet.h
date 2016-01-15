#ifndef __ELASTOS_DROID_UTILITY_FAST_IMMUTABLE_ARRAYSET_H__
#define __ELASTOS_DROID_UTILITY_FAST_IMMUTABLE_ARRAYSET_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/AbstractSet.h>

using Elastos::Utility::ISet;
using Elastos::Utility::AbstractSet;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Utility {

class FastImmutableArraySet
    : public AbstractSet
    , public IFastImmutableArraySet
{
public:
    CAR_INTERFACE_DECL()

    FastImmutableArraySet(
        /* [in] */ ArrayOf<IInterface*>* contents);

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI GetSize(
        /* [out] */ Int32* size);

private:
    class FastIterator
        : public Object
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL()

        FastIterator(
            /* [in] */ ArrayOf<IInterface*>* contents);

        CARAPI HasNext(
            /* [out] */ Boolean* next);

        CARAPI GetNext(
            /* [out] */ IInterface** obj);

        CARAPI Remove();

    private:
        friend class FastImmutableArraySet;

        AutoPtr<ArrayOf<IInterface*> > mContents;
        Int32 mIndex;
    };

private:
    AutoPtr<ArrayOf<IInterface*> > mContents;
    AutoPtr<FastIterator> mIterator;
};

} // Utility
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_UTILITY_FAST_IMMUTABLE_ARRAYSET_H__
