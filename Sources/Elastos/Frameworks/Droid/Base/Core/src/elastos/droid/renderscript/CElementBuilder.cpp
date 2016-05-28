
#include "elastos/droid/RenderScript/CElementBuilder.h"
#include "elastos/droid/RenderScript/Element.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::RenderScript::EIID_IElementBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace RenderScript {

CElementBuilder::CElementBuilder()
    : mCount(0)
    , mSkipPadding(0)
{}

ECode CElementBuilder::constructor(
    /* [in] */ IRenderScript* rs)
{
    mRS = rs;
    mCount = 0;
    mElements = ArrayOf<IElement*>::Alloc(8);
    mElementNames = ArrayOf<String>::Alloc(8);
    mArraySizes = ArrayOf<Int32>::Alloc(8);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CElementBuilder, Object, IElementBuilder)

ECode CElementBuilder::Add(
    /* [in] */ IElement* element,
    /* [in] */ const String& name,
    /* [in] */ Int32 arraySize,
    /* [out] */ IElementBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    *builder = NULL;

    if (arraySize < 1) {
        Slogger::E("CElementBuilder", "Array size cannot be less than 1.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Skip padding fields after a vector 3 type.
    if (mSkipPadding != 0) {
        if (name.StartWith("#padding_")) {
            mSkipPadding = 0;
            *builder = (IElementBuilder*)this;
            REFCOUNT_ADD(*builder)
            return NOERROR;
        }
    }

    AutoPtr<Element> e = (Element*)element;
    if (e->mVectorSize == 3) {
        mSkipPadding = 1;
    }
    else {
        mSkipPadding = 0;
    }

    if(mCount == mElements->GetLength()) {
        AutoPtr<ArrayOf<IElement*> > e = ArrayOf<IElement*>::Alloc(mCount + 8);
        AutoPtr<ArrayOf<String> > s = ArrayOf<String>::Alloc(mCount + 8);
        AutoPtr<ArrayOf<Int32> > as = ArrayOf<Int32>::Alloc(mCount + 8);
        e->Copy(mElements, mCount);
        s->Copy(mElementNames, mCount);
        as->Copy(mArraySizes, mCount);
        mElements = e;
        mElementNames = s;
        mArraySizes = as;
    }
    mElements->Set(mCount, element);
    mElementNames[mCount] = name;
    mArraySizes[mCount] = arraySize;
    mCount++;
    *builder = (IElementBuilder*)this;
    REFCOUNT_ADD(*builder)
    return NOERROR;
}

ECode CElementBuilder::Add(
    /* [in] */ IElement* element,
    /* [in] */ const String& name,
    /* [out] */ IElementBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return Add(element, name, 1, builder);
}

ECode CElementBuilder::Create(
    /* [out] */ IElement** e)
{
    VALIDATE_NOT_NULL(e)
    *e = NULL;
    AutoPtr<RenderScript> rs = (RenderScript*)mRS.Get();
    FAIL_RETURN(rs->Validate())
    AutoPtr<ArrayOf<IElement*> > ein = ArrayOf<IElement*>::Alloc(mCount);
    AutoPtr<ArrayOf<String> > sin = ArrayOf<String>::Alloc(mCount);
    AutoPtr<ArrayOf<Int32> > asin = ArrayOf<Int32>::Alloc(mCount);
    ein->Copy(mElements, mCount);
    sin->Copy(mElementNames, mCount);
    asin->Copy(mArraySizes, mCount);

    AutoPtr<ArrayOf<Int64> > ids = ArrayOf<Int64>::Alloc(ein->GetLength);
    for (Int32 ct = 0; ct < ein->GetLength(); ct++ ) {
        (*ein)[ct]->GetID(rs, &(*ids)[ct]);
    }
    Int64 id;
    rs->NElementCreate2(ids, sin, asin, &id);
    *e = (IElement*)new Element(id, rs, ein, sin, asin);
    REFCOUNT_ADD(*e)
    return NOERROR;
}

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos
