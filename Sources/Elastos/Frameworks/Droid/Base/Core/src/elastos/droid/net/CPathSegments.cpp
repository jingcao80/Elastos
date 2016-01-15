
#include "elastos/droid/net/CPathSegments.h"

class CStatic
{
public:

    CStatic()
    {
        CPathSegments::New(
            NULL_ARRAYOF(String), 9, (IPathSegments**)&PathSegments::EMPTY);
    }
};

static CStatic _static;

AutoPtr<IPathSegments> PathSegments::EMPTY;

PathSegments::PathSegments()
    : mSize(0)
{
}

ECode PathSegments::Init(
    /* [in] */ const ArrayOf<String>& segments,
    /* [in] */ Int32 size)
{
    if (segments.GetLength() > 0) {
        for (Int32 i = 0; i < segments.GetLength(); ++i) {
            mSegments.PushBack(segments[i]);
        }
    }

    mSize = size;

    return NOERROR;
}

ECode PathSegments::Get(
    /* [in] */ Int32 index,
    /* [out] */ String* encoded)
{
    VALIDATE_NOT_NULL(encoded);

    if (index >= mSize) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *encoded = mSegments[index];

    return NOERROR;
}

ECode PathSegments::Size(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mSize;

    return NOERROR;
}

ECode CPathSegments::Get(
    /* [in] */ Int32 index,
    /* [out] */ String* value)
{
    return PathSegments::Get(index, value);
}

ECode CPathSegments::Size(
    /* [out] */ Int32* size)
{
    return PathSegments::Size(size);
}

ECode CPathSegments::constructor(
    /* [in] */ const ArrayOf<String>& segments,
    /* [in] */ Int32 size)
{
    return PathSegments::Init(segments, size);
}

