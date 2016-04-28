
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/PaintDrawable.h"
#include "elastos/droid/graphics/drawable/shapes/CRoundRectShape.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Graphics::Drawable::Shapes::IRoundRectShape;
using Elastos::Droid::Graphics::Drawable::Shapes::CRoundRectShape;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_INTERFACE_IMPL(PaintDrawable, ShapeDrawable, IPaintDrawable);
PaintDrawable::PaintDrawable()
{}

ECode PaintDrawable::constructor()
{
    return ShapeDrawable::constructor();
}

ECode PaintDrawable::constructor(
    /* [in] */ Int32 color)
{
    FAIL_RETURN(ShapeDrawable::constructor());

    AutoPtr<IPaint> paint;
    GetPaint((IPaint**)&paint);
    return paint->SetColor(color);
}

ECode PaintDrawable::SetCornerRadius(
    /* [in] */ Float radius)
{
    AutoPtr< ArrayOf<Float> > radii;
    if (radius > 0) {
        radii = ArrayOf<Float>::Alloc(8);
        for (Int32 i = 0; i < 8; i++) {
            (*radii)[i] = radius;
        }
    }
    SetCornerRadii(radii);
    return NOERROR;
}

ECode PaintDrawable::SetCornerRadii(
    /* [in] */ ArrayOf<Float>* radii)
{
    if (radii == NULL) {
        AutoPtr<IShape> shape;
        if ((GetShape((IShape**)&shape), shape.Get()) != NULL) {
            SetShape(NULL);
        }
    }
    else {
        AutoPtr<IShape> shape;
        CRoundRectShape::New(radii, NULL, NULL, (IShape**)&shape);
        SetShape(shape);
    }
    return InvalidateSelf();;
}

Boolean PaintDrawable::InflateTag(
    /* [in] */ const String& name,
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs)
{
    if (name.Equals("corners")) {
        Int32 size = ArraySize(R::styleable::DrawableCorners);
        AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
        layout->Copy(R::styleable::DrawableCorners, size);

        AutoPtr<ITypedArray> a;
        r->ObtainAttributes(attrs, layout, (ITypedArray**)&a);
        Int32 radius;
        a->GetDimensionPixelSize(R::styleable::DrawableCorners_radius, 0, &radius);
        SetCornerRadius(radius);

        // now check of they have any per-corner radii

        Int32 topLeftRadius, topRightRadius, bottomLeftRadius, bottomRightRadius;
        a->GetDimensionPixelSize(
                R::styleable::DrawableCorners_topLeftRadius, radius, &topLeftRadius);
        a->GetDimensionPixelSize(
                R::styleable::DrawableCorners_topRightRadius, radius, &topRightRadius);
        a->GetDimensionPixelSize(
                R::styleable::DrawableCorners_bottomLeftRadius, radius, &bottomLeftRadius);
        a->GetDimensionPixelSize(
                R::styleable::DrawableCorners_bottomRightRadius, radius, &bottomRightRadius);

        if (topLeftRadius != radius || topRightRadius != radius ||
                bottomLeftRadius != radius || bottomRightRadius != radius) {
            AutoPtr< ArrayOf<Float> > radii = ArrayOf<Float>::Alloc(8);
            (*radii)[0] = topLeftRadius; (*radii)[1] = topLeftRadius;
            (*radii)[2] = topRightRadius; (*radii)[3] = topRightRadius;
            (*radii)[4] = bottomLeftRadius; (*radii)[5] = bottomLeftRadius;
            (*radii)[6] = bottomRightRadius; (*radii)[7] = bottomRightRadius;
            SetCornerRadii(radii);
        }
        a->Recycle();
        return TRUE;
    }
    return ShapeDrawable::InflateTag(name, r, parser, attrs);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
