
#include "elastos/droid/net/CPathSegments.h"
#include "elastos/droid/net/CPathSegmentsBuilder.h"

PathSegmentsBuilder::PathSegmentsBuilder()
    : mSize(0)
{
}

ECode PathSegmentsBuilder::Add(
    /* [in] */ const String& segment)
{
    mSegments.PushBack(segment);
    mSize++;

    return NOERROR;
}

ECode PathSegmentsBuilder::Build(
    /* [out] */ IPathSegments** pathSegments)
{
    VALIDATE_NOT_NULL(pathSegments);

    if (mSegments.IsEmpty()) {
        *pathSegments = PathSegments::EMPTY;
        REFCOUNT_ADD(*pathSegments);
        return NOERROR;
    }

    // try {
    AutoPtr<ArrayOf<String> > segments = ArrayOf<String>::Alloc(mSegments.GetSize());
    if (!segments)
        return E_OUT_OF_MEMORY;

    List<String>::Iterator itor;
    Int32 i = 0;
    for (itor = mSegments.Begin(); itor != mSegments.End(); ++itor, ++i) {
        (*segments)[i] = *itor;
    }

    ECode ec = CPathSegments::New(*segments, mSize, pathSegments);
    if (FAILED(ec)) {
        return ec;
    }

    if (*pathSegments == NULL) {
        return NOERROR;
    }

    // } finally {
    // Makes sure this doesn't get reused.
    mSegments.Clear();
    // }

    return NOERROR;
}

ECode CPathSegmentsBuilder::Add(
    /* [in] */ const String& segment)
{
    PathSegmentsBuilder::Add(segment);

    return NOERROR;
}

ECode CPathSegmentsBuilder::Build(
    /* [out] */ IPathSegments** pathSegments)
{
    return PathSegmentsBuilder::Build(pathSegments);
}

