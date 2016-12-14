
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CTypefaceHelper.h"
#include "elastos/droid/graphics/CTypeface.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CTypefaceHelper)

CAR_INTERFACE_IMPL(CTypefaceHelper, Singleton, ITypefaceHelper)

ECode CTypefaceHelper::GetDEFAULT(
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface)
    *typeface = Typeface::DEFAULT;
    REFCOUNT_ADD(*typeface)
    return NOERROR;
}

ECode CTypefaceHelper::GetDEFAULT_BOLD(
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface)
    *typeface = Typeface::DEFAULT_BOLD;
    REFCOUNT_ADD(*typeface)
    return NOERROR;
}

ECode CTypefaceHelper::GetSANS_SERIF(
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface)
    *typeface = Typeface::SANS_SERIF;
    REFCOUNT_ADD(*typeface)
    return NOERROR;
}

ECode CTypefaceHelper::GetSERIF(
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface)
    *typeface = Typeface::SERIF;
    REFCOUNT_ADD(*typeface)
    return NOERROR;
}

ECode CTypefaceHelper::GetMONOSPACE(
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface)
    *typeface = Typeface::MONOSPACE;
    REFCOUNT_ADD(*typeface)
    return NOERROR;
}

ECode CTypefaceHelper::Create(
    /* [in] */ const String& familyName,
    /* [in] */ Int32 style,
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);
    return Typeface::Create(familyName, style, typeface);
}

ECode CTypefaceHelper::Create(
    /* [in] */ ITypeface* family,
    /* [in] */ Int32 style,
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);
    return Typeface::Create(family, style, typeface);
}

ECode CTypefaceHelper::DefaultFromStyle(
    /* [in] */ Int32 style,
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);
    return Typeface::DefaultFromStyle(style, typeface);
}

ECode CTypefaceHelper::CreateFromAsset(
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path,
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);
    return Typeface::CreateFromAsset(mgr, path, typeface);
}

ECode CTypefaceHelper::CreateFromFile(
    /* [in] */ IFile* path,
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);
    return Typeface::CreateFromFile(path, typeface);
}

ECode CTypefaceHelper::CreateFromFile(
    /* [in]*/ const String& path,
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);
    return Typeface::CreateFromFile(path, typeface);
}

ECode CTypefaceHelper::CreateFromFamilies(
    /* [in]*/ ArrayOf<IFontFamily*>* families,
    /* [out]*/ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);
    return Typeface::CreateFromFamilies(families, typeface);
}

ECode CTypefaceHelper::CreateFromFamiliesWithDefault(
    /* [in]*/ ArrayOf<IFontFamily*>* families,
    /* [out]*/ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);
    return Typeface::CreateFromFamiliesWithDefault(families, typeface);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
