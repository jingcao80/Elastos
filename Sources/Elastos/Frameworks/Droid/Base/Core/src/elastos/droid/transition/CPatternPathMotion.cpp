
#include "Elastos.Droid.Content.h"
#include "elastos/droid/transition/CPatternPathMotion.h"
#include "elastos/droid/graphics/CPathMeasure.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/utility/FloatMath.h"
#include "elastos/droid/utility/PathParser.h"
#include "elastos/droid/R.h"

#include <elastos/core/Math.h>

using Elastos::Droid::R;
using Elastos::Droid::Utility::FloatMath;
using Elastos::Droid::Utility::PathParser;
using Elastos::Droid::Graphics::IPathMeasure;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CPathMeasure;
using Elastos::Droid::Content::Res::ITypedArray;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CPatternPathMotion::
//===============================================================
CAR_OBJECT_IMPL(CPatternPathMotion)

CAR_INTERFACE_IMPL(CPatternPathMotion, PathMotion, IPatternPathMotion)

ECode CPatternPathMotion::constructor()
{
    mPatternPath->LineTo(1, 0);
    mOriginalPatternPath = mPatternPath;
    return NOERROR;
}

ECode CPatternPathMotion::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32*>(R::styleable::PatternPathMotion),
        ArraySize(R::styleable::PatternPathMotion));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    String pathData;
    a->GetString(R::styleable::PatternPathMotion_patternPathData, &pathData);
    if (pathData == NULL) {
        a->Recycle();
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IPath> pattern = PathParser::CreatePathFromPathData(pathData);
    SetPatternPath(pattern);
    a->Recycle();
    return NOERROR;
}

ECode CPatternPathMotion::constructor(
    /* [in] */ IPath* patternPath)
{
    SetPatternPath(patternPath);
    return NOERROR;
}

ECode CPatternPathMotion::GetPatternPath(
    /* [out] */ IPath** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOriginalPatternPath;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CPatternPathMotion::SetPatternPath(
    /* [in] */ IPath* patternPath)
{
    AutoPtr<IPathMeasure> pathMeasure;
    CPathMeasure::New(patternPath, FALSE, (IPathMeasure**)&pathMeasure);
    Float length;
    pathMeasure->GetLength(&length);
    AutoPtr<ArrayOf<Float> > pos = ArrayOf<Float>::Alloc(2);
    Boolean res;
    pathMeasure->GetPosTan(length, pos, NULL, &res);
    Float endX = (*pos)[0];
    Float endY = (*pos)[1];
    pathMeasure->GetPosTan(0, pos, NULL, &res);
    Float startX = (*pos)[0];
    Float startY = (*pos)[1];

    if (startX == endX && startY == endY) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mTempMatrix->SetTranslate(-startX, -startY);
    Float dx = endX - startX;
    Float dy = endY - startY;
    Float distance = Distance(dx, dy);
    Float scale = 1 / distance;
    mTempMatrix->PostScale(scale, scale, &res);
    Double angle = Elastos::Core::Math::Atan2(dy, dx);
    mTempMatrix->PostRotate((Float) Elastos::Core::Math::ToDegrees(-angle), &res);
    patternPath->Transform(mTempMatrix, mPatternPath);
    mOriginalPatternPath = patternPath;
    return NOERROR;
}

ECode CPatternPathMotion::GetPath(
    /* [in] */ Float startX,
    /* [in] */ Float startY,
    /* [in] */ Float endX,
    /* [in] */ Float endY,
    /* [out] */ IPath** result)
{
    VALIDATE_NOT_NULL(result)

    Float dx = endX - startX;
    Float dy = endY - startY;
    Float length = Distance(dx, dy);
    Double angle = Elastos::Core::Math::Atan2(dy, dx);

    Boolean res;
    mTempMatrix->SetScale(length, length);
    mTempMatrix->PostRotate((Float) Elastos::Core::Math::ToDegrees(angle), &res);
    mTempMatrix->PostTranslate(startX, startY, &res);
    AutoPtr<IPath> path;
    CPath::New((IPath**)&path);
    mPatternPath->Transform(mTempMatrix, path);
    *result = path;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Float CPatternPathMotion::Distance(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return FloatMath::Sqrt((x * x) + (y * y));
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
