//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/text/style/ImageSpan.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/graphics/CBitmapFactory.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL(ImageSpan, DynamicDrawableSpan, IImageSpan)

ImageSpan::ImageSpan()
    : mResourceId(0)
    , mContext(NULL)
{}

ImageSpan::~ImageSpan()
{}


ECode ImageSpan::constructor(
    /* [in] */ IBitmap* b)
{
    return constructor(NULL, b, IDynamicDrawableSpan::ALIGN_BOTTOM);
}

ECode ImageSpan::constructor(
    /* [in] */ IBitmap* b,
    /* [in] */ Int32 verticalAlignment)
{
    return constructor(NULL, b, verticalAlignment);
}

ECode ImageSpan::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IBitmap* b)
{
    return constructor(context, b, IDynamicDrawableSpan::ALIGN_BOTTOM);
}

ECode ImageSpan::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IBitmap* b,
    /* [in] */ Int32 verticalAlignment)
{
    DynamicDrawableSpan::constructor(verticalAlignment);
    mContext = context;
    if(context != NULL) {
        AutoPtr<IResources> rs;
        context->GetResources( (IResources**)&rs );
        CBitmapDrawable::New(rs, b, (IDrawable**)&mDrawable);
    }
    else {
        CBitmapDrawable::New((IDrawable**)&mDrawable);
    }
    Int32 width;
    mDrawable->GetIntrinsicWidth(&width);
    Int32 height;
    mDrawable->GetIntrinsicHeight(&height);
    mDrawable->SetBounds(0, 0, width > 0 ? width : 0, height > 0 ? height : 0);
    return NOERROR;
}

ECode ImageSpan::constructor(
    /* [in] */ IDrawable* d)
{
    return constructor(d, IDynamicDrawableSpan::ALIGN_BOTTOM);
}

ECode ImageSpan::constructor(
    /* [in] */ IDrawable* d,
    /* [in] */ Int32 verticalAlignment)
{
    DynamicDrawableSpan::constructor(verticalAlignment);
    mDrawable = d;
    return NOERROR;
}

ECode ImageSpan::constructor(
    /* [in] */ IDrawable* d,
    /* [in] */ const String& source)
{
    return constructor(d, source, IDynamicDrawableSpan::ALIGN_BOTTOM);
}

ECode ImageSpan::constructor(
    /* [in] */ IDrawable* d,
    /* [in] */ const String& source,
    /* [in] */ Int32 verticalAlignment)
{
    DynamicDrawableSpan::constructor(verticalAlignment);
    mDrawable = d;
    mSource = source;
    return NOERROR;
}

ECode ImageSpan::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri)
{
    return constructor(context, uri, IDynamicDrawableSpan::ALIGN_BOTTOM);
}

ECode ImageSpan::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 verticalAlignment)
{
    DynamicDrawableSpan::constructor(verticalAlignment);
    mContext = context;
    mContentUri = uri;
    mSource = Object::ToString(uri);
    return NOERROR;
}

ECode ImageSpan::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceId)
{
    return constructor(context, resourceId, IDynamicDrawableSpan::ALIGN_BOTTOM);
}

ECode ImageSpan::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceId,
    /* [in] */ Int32 verticalAlignment)
{
    DynamicDrawableSpan::constructor(verticalAlignment);
    mContext = context;
    mResourceId = resourceId;
    return NOERROR;
}

ECode ImageSpan::GetDrawable(
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IDrawable> drawable;

    if (mDrawable != NULL) {
        drawable = mDrawable;
    }
    else  if (mContentUri != NULL) {
        AutoPtr<IBitmap> bitmap;
        //try {
            AutoPtr<IContentResolver> cr;
            mContext->GetContentResolver((IContentResolver**)&cr);
            AutoPtr<IInputStream> is;
            cr->OpenInputStream(mContentUri, (IInputStream**)&is);
            AutoPtr<IBitmapFactory> bf;
            CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bf);
            bf->DecodeStream(is.Get(), (IBitmap**)&bitmap);
            AutoPtr<IResources> rs;
            mContext->GetResources((IResources**)&rs);
            CBitmapDrawable::New(rs, bitmap, (IDrawable**)&drawable);
            Int32 dIntrinsicWidth, dIntrinsicHeight;
            drawable->GetIntrinsicWidth(&dIntrinsicWidth);
            drawable->GetIntrinsicHeight(&dIntrinsicHeight);
            drawable->SetBounds(0, 0, dIntrinsicWidth, dIntrinsicHeight);
            is->Close();
        //} catch (Exception e) {
        //    Log.e("sms", "Failed to loaded content " + mContentUri, e);
        //}
    }
    else {
        //try {
            mContext->GetDrawable(mResourceId, (IDrawable**)&drawable);
            Int32 dIntrinsicWidth, dIntrinsicHeight;
            drawable->GetIntrinsicWidth(&dIntrinsicWidth);
            drawable->GetIntrinsicHeight(&dIntrinsicHeight);
            drawable->SetBounds(0, 0, dIntrinsicWidth, dIntrinsicHeight);
        //} catch (Exception e) {
        //    Log.e("sms", "Unable to find resource: " + mResourceId);
        //}
    }

    *result = drawable;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImageSpan::GetSource(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSource;
    return NOERROR;
}


} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
