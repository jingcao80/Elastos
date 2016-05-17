
#include "elastos/droid/systemui/recents/misc/Utilities.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::View::IViewParent;
using Elastos::Core::Math;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

Int32 Utilities::CalculateTranslationAnimationDuration(
    /* [in] */ Int32 distancePx)
{
    return CalculateTranslationAnimationDuration(distancePx, 100);
}

Int32 Utilities::CalculateTranslationAnimationDuration(
    /* [in] */ Int32 distancePx,
    /* [in] */ Int32 minDuration)
{
#if 0//TODO
    AutoPtr<IRecentsConfigurationHelper> helper;
    CRecentsConfigurationHelper::AcquireSingleton((IRecentsConfigurationHelper**)&helper);
    AutoPtr<IRecentsConfiguration> config;
    helper->GetInstance((IRecentsConfiguration**)&config);
    Float animationPxMovementPerSecond;
    config->GetAnimationPxMovementPerSecond(&animationPxMovementPerSecond);
    return Elastos::Core::Math::Max(minDuration, (Int32) (1000.0f /* ms/s */ *
        (Elastos::Core::Math::Abs(distancePx) / animationPxMovementPerSecond)));
#endif
    return 0;
}

void Utilities::scaleRectAboutCenter(
    /* [in] */ IRect* r,
    /* [in] */ Float scale)
{
    if (scale != 1.0f) {
        Int32 cx, cy;
        r->GetCenterX(&cx);
        r->GetCenterY(&cy);
        r->Offset(-cx, -cy);
        Int32 left, top, right, bottom;
        r->GetLeft(&left);
        r->SetLeft((Int32)(left * scale + 0.5f));
        r->GetTop(&top);
        r->SetTop((Int32)(top * scale + 0.5f));
        r->GetRight(&right);
        r->SetRight((Int32)(right * scale + 0.5f));
        r->GetBottom(&bottom);
        r->SetBottom((Int32)(bottom * scale + 0.5f));
        r->Offset(cx, cy);
    }
}

Float Utilities::MapCoordInDescendentToSelf(
    /* [in] */ IView* descendant,
    /* [in] */ IView* root,
    /* [in] */ ArrayOf<Float>* coord,
    /* [in] */ Boolean includeRootScroll)
{
    AutoPtr<IArrayList> ancestorChain;
    CArrayList::New((IArrayList**)&ancestorChain);
    AutoPtr<ArrayOf<Float> > pt = ArrayOf<Float>::Alloc(2);
    (*pt)[0] = (*coord)[0];
    (*pt)[1] = (*coord)[1];

    AutoPtr<IView> v = descendant;
    Boolean equaled;
    IObject::Probe(v)->Equals(root, &equaled);
    while(!equaled && v != NULL) {
        ancestorChain->Add(v);
        AutoPtr<IViewParent> parent;
        v->GetParent((IViewParent**)&parent);
        v = IView::Probe(parent);
    }
    ancestorChain->Add(root);

    Float scale = 1.0f;
    Int32 count;
    ancestorChain->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        ancestorChain->Get(i, (IInterface**)&obj);
        AutoPtr<IView> v0 = IView::Probe(obj);
        // For TextViews, scroll has a meaning which relates to the text position
        // which is very strange... ignore the scroll.
        Boolean equaled2;
        IObject::Probe(v0)->Equals(descendant, &equaled2);
        if (!equaled2 || includeRootScroll) {
            Int32 sx, sy;
            v0->GetScrollX(&sx);
            v0->GetScrollY(&sy);
            (*pt)[0] -= sx;
            (*pt)[1] -= sy;
        }

        AutoPtr<IMatrix> matrix;
        v0->GetMatrix((IMatrix**)&matrix);
        matrix->MapPoints(pt);
        Int32 left, top;
        Float scalex;
        v0->GetLeft(&left);
        v0->GetTop(&top);
        v0->GetScaleX(&scalex);
        (*pt)[0] += left;
        (*pt)[1] += top;
        scale *= scalex;
    }

    (*coord)[0] = (*pt)[0];
    (*coord)[1] = (*pt)[1];
    return scale;
}

Float Utilities::MapCoordInSelfToDescendent(
    /* [in] */ IView* descendant,
    /* [in] */ IView* root,
    /* [in] */ ArrayOf<Float>* coord,
    /* [in] */ IMatrix* tmpInverseMatrix)
{
    AutoPtr<IArrayList> ancestorChain;
    CArrayList::New((IArrayList**)&ancestorChain);
    AutoPtr<ArrayOf<Float> > pt = ArrayOf<Float>::Alloc(2);
    (*pt)[0] = (*coord)[0];
    (*pt)[1] = (*coord)[1];

    AutoPtr<IView> v = descendant;
    Boolean equaled;
    IObject::Probe(v)->Equals(root, &equaled);
    while(!equaled) {
        ancestorChain->Add(v);
        AutoPtr<IViewParent> parent;
        v->GetParent((IViewParent**)&parent);
        v = IView::Probe(parent);
    }
    ancestorChain->Add(root);

    Float scale = 1.0f;
    Int32 count;
    ancestorChain->GetSize(&count);
    assert(0 && "TODO");
    //tmpInverseMatrix->Set(Matrix::IDENTITY_MATRIX);
    for (Int32 i = count - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        ancestorChain->Get(i, (IInterface**)&obj);
        AutoPtr<IView> ancestor = IView::Probe(obj);
        obj = NULL;
        ancestorChain->Get(i - 1, (IInterface**)&obj);
        AutoPtr<IView> nextb = IView::Probe(obj);
        AutoPtr<IView> next = i > 0 ? nextb : NULL;

        Int32 sx, sy;
        ancestor->GetScrollX(&sx);
        ancestor->GetScrollY(&sy);
        (*pt)[0] += sx;
        (*pt)[1] += sy;

        if (next != NULL) {
            Int32 left, top;
            next->GetLeft(&left);
            next->GetTop(&top);
            (*pt)[0] += left;
            (*pt)[1] += top;
            AutoPtr<IMatrix> matrix;
            next->GetMatrix((IMatrix**)&matrix);
            Boolean result;
            matrix->Invert(tmpInverseMatrix, &result);
            tmpInverseMatrix->MapPoints(pt);
            Float scalex;
            next->GetScaleX(&scalex);
            scale *= scalex;
        }
    }

    (*coord)[0] = (*pt)[0];
    (*coord)[1] = (*pt)[1];
    return scale;
}

Float Utilities::ComputeContrastBetweenColors(
    /* [in] */ Int32 bg,
    /* [in] */ Int32 fg)
{
    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    Int32 red;
    color->Red(bg, &red);
    Float bgR = red / 255.0f;
    Int32 green;
    color->Green(bg, &green);
    Float bgG = green / 255.0f;
    Int32 blue;
    color->Blue(bg, &blue);
    Float bgB = blue / 255.0f;
    bgR = (bgR < 0.03928f) ? bgR / 12.92f : (Float) Elastos::Core::Math::Pow((bgR + 0.055f) / 1.055f, 2.4f);
    bgG = (bgG < 0.03928f) ? bgG / 12.92f : (Float) Elastos::Core::Math::Pow((bgG + 0.055f) / 1.055f, 2.4f);
    bgB = (bgB < 0.03928f) ? bgB / 12.92f : (Float) Elastos::Core::Math::Pow((bgB + 0.055f) / 1.055f, 2.4f);
    Float bgL = 0.2126f * bgR + 0.7152f * bgG + 0.0722f * bgB;

    color->Red(fg, &red);
    Float fgR = red / 255.0f;
    color->Green(fg, &green);
    Float fgG = green / 255.0f;
    color->Blue(fg, &blue);
    Float fgB = blue / 255.0f;
    fgR = (fgR < 0.03928f) ? fgR / 12.92f : (Float) Elastos::Core::Math::Pow((fgR + 0.055f) / 1.055f, 2.4f);
    fgG = (fgG < 0.03928f) ? fgG / 12.92f : (Float) Elastos::Core::Math::Pow((fgG + 0.055f) / 1.055f, 2.4f);
    fgB = (fgB < 0.03928f) ? fgB / 12.92f : (Float) Elastos::Core::Math::Pow((fgB + 0.055f) / 1.055f, 2.4f);
    Float fgL = 0.2126f * fgR + 0.7152f * fgG + 0.0722f * fgB;

    return Elastos::Core::Math::Abs((fgL + 0.05f) / (bgL + 0.05f));
}

Int32 Utilities::GetColorWithOverlay(
    /* [in] */ Int32 value,
    /* [in] */ Int32 baseColor,
    /* [in] */ Int32 overlayColor,
    /* [in] */ Float overlayAlpha)
{
    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    Int32 red1;
    color->Red(baseColor, &red1);
    Int32 red2;
    color->Red(overlayColor, &red2);
    Int32 green1;
    color->Green(baseColor, &green1);
    Int32 green2;
    color->Green(overlayColor, &green2);
    Int32 blue1;
    color->Blue(baseColor, &blue1);
    Int32 blue2;
    color->Blue(overlayColor, &blue2);
    Int32 rgb;
    color->Rgb(
        (Int32) (overlayAlpha * red1 + (1.0f - overlayAlpha) * red2),
        (Int32) (overlayAlpha * green1 + (1.0f - overlayAlpha) * green2),
        (Int32) (overlayAlpha * blue1 + (1.0f - overlayAlpha) * blue2), &rgb);
    return rgb;
}

void Utilities::SetShadowProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& value)
{
    // sPropertyMethod.invoke(null, property, value);
    assert(0 && "TODO");
}

Boolean Utilities::IsDocument(
    /* [in] */ IIntent* intent)
{
    Int32 flags;
    intent->GetFlags(&flags);
    return (flags & IIntent::FLAG_ACTIVITY_NEW_DOCUMENT) == IIntent::FLAG_ACTIVITY_NEW_DOCUMENT;
}

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos