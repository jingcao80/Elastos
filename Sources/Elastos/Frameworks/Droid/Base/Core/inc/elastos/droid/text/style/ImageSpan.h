#ifndef __ELASTOS_DROID_TEXT_STYLE_IMAGESPAN_H__
#define __ELASTOS_DROID_TEXT_STYLE_IMAGESPAN_H__

#include "elastos/droid/text/style/DynamicDrawableSpan.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class ImageSpan
    : public DynamicDrawableSpan
    , public IImageSpan
{
public:
    CAR_INTERFACE_DECL()

    ImageSpan();

    virtual ~ImageSpan();

   CARAPI constructor(
        /* [in] */ IBitmap* b);

   CARAPI constructor(
        /* [in] */ IBitmap* b,
        /* [in] */ Int32 verticalAlignment);

   CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IBitmap* b);

   CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IBitmap* b,
        /* [in] */ Int32 verticalAlignment);

   CARAPI constructor(
        /* [in] */ IDrawable* d);

   CARAPI constructor(
        /* [in] */ IDrawable* d,
        /* [in] */ Int32 verticalAlignment);

   CARAPI constructor(
        /* [in] */ IDrawable* d,
        /* [in] */ const String& source);

   CARAPI constructor(
        /* [in] */ IDrawable* d,
        /* [in] */ const String& source,
        /* [in] */ Int32 verticalAlignment);

   CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri);

   CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 verticalAlignment);

   CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceId);

   CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceId,
        /* [in] */ Int32 verticalAlignment);

    //@Override
    CARAPI GetDrawable(
        /* [out] */ IDrawable** drawable);

    /**
     * Returns the source string that was saved during construction.
     */
    CARAPI GetSource(
        /* [out] */ String* source);

private:
    AutoPtr<IDrawable> mDrawable;
    AutoPtr<IUri> mContentUri;
    Int32 mResourceId;
    IContext* mContext;
    String mSource;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_IMAGESPAN_H__
