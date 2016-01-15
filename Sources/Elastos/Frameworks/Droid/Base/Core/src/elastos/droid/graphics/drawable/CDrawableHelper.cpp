
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/CDrawableHelper.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_INTERFACE_IMPL(CDrawableHelper, Singleton, IDrawableHelper)
CAR_SINGLETON_IMPL(CDrawableHelper)
ECode CDrawableHelper::ResolveOpacity(
    /* [in] */ Int32 op1,
    /* [in] */ Int32 op2,
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    return Drawable::ResolveOpacity(op1, op2, opacity);
}

ECode CDrawableHelper::CreateFromPath(
    /* [in] */ const String& pathName,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return Drawable::CreateFromPath(pathName, drawable);
}

ECode CDrawableHelper::CreateFromResourceStream(
    /* [in] */ IResources* res,
    /* [in] */ ITypedValue* value,
    /* [in] */ IInputStream* is,
    /* [in] */ const String& srcName,
    /* [in] */ IBitmapFactoryOptions* opts,
    /* [out] */ IDrawable** draw)
{
    VALIDATE_NOT_NULL(draw);
    return Drawable::CreateFromResourceStream(res, value, is, srcName, opts, draw);
}

ECode CDrawableHelper::CreateFromResourceStream(
    /* [in] */ IResources* res,
    /* [in] */ ITypedValue* value,
    /* [in] */ IInputStream* is,
    /* [in] */ const String& srcName,
    /* [out] */ IDrawable** draw)
{
    VALIDATE_NOT_NULL(draw);
    return Drawable::CreateFromResourceStream(res, value, is, srcName, draw);
}

ECode CDrawableHelper::CreateFromStream(
    /* [in] */ IInputStream* is,
    /* [in] */ const String& srcName,
    /* [out] */ IDrawable** draw)
{
    VALIDATE_NOT_NULL(draw);
    return Drawable::CreateFromStream(is, srcName, draw);
}

ECode CDrawableHelper::CreateFromXml(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IDrawable** draw)
{
    VALIDATE_NOT_NULL(draw);
    return CreateFromXml(r, parser, NULL, draw);
}

ECode CDrawableHelper::CreateFromXml(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** draw)
{
    VALIDATE_NOT_NULL(draw);
    return Drawable::CreateFromXml(r, parser, theme, draw);
}

ECode CDrawableHelper::CreateFromXmlInner(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IDrawable** draw)
{
    return CreateFromXmlInner(r, parser, attrs, NULL, draw);
}

ECode CDrawableHelper::CreateFromXmlInner(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** draw)
{
    VALIDATE_NOT_NULL(draw);
    return Drawable::CreateFromXmlInner(r, parser, attrs, theme, draw);
}

ECode CDrawableHelper::ParseTintMode(
    /* [in] */ Int32 value,
    /* [in] */ PorterDuffMode defaultMode,
    /* [out] */ PorterDuffMode* mode)
{
    return Drawable::ParseTintMode(value, defaultMode, mode);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
