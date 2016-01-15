
#ifndef __ELASTOS_DROID_GRAPHICS_TYPEFACEIMPL_H__
#define __ELASTOS_DROID_GRAPHICS_TYPEFACEIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include <SkTypeface.h>
#include <androidfw/AssetManager.h>
#include <minikin/FontCollection.h>

using android::FontStyle;
using android::FontCollection;

namespace Elastos {
namespace Droid {
namespace Graphics {

struct TypefaceImpl {
    FontCollection *fFontCollection;

    // style used for constructing and querying Typeface objects
    SkTypeface::Style fSkiaStyle;
    // base weight in CSS-style units, 100..900
    int fBaseWeight;

    // resolved style actually used for rendering
    FontStyle fStyle;
};

// Note: it would be cleaner if the following functions were member
// functions (static or otherwise) of the TypefaceImpl class. However,
// that can't be easily accommodated in the case where TypefaceImpl
// is just a pointer to SkTypeface, in the non-USE_MINIKIN case.
// TODO: when #ifdef USE_MINIKIN is removed, move to member functions.

TypefaceImpl* TypefaceImpl_resolveDefault(TypefaceImpl* src);

TypefaceImpl* TypefaceImpl_createFromTypeface(TypefaceImpl* src, SkTypeface::Style style);

TypefaceImpl* TypefaceImpl_createWeightAlias(TypefaceImpl* src, int baseweight);

// When we remove the USE_MINIKIN ifdef, probably a good idea to move the casting
// (from jlong to FontFamily*) to the caller in Typeface.cpp.
TypefaceImpl* TypefaceImpl_createFromFamilies(const Int64* families, size_t size);

void TypefaceImpl_unref(TypefaceImpl* face);

int TypefaceImpl_getStyle(TypefaceImpl* face);

void TypefaceImpl_setDefault(TypefaceImpl* face);

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_GRAPHICS_TYPEFACEIMPL_H__