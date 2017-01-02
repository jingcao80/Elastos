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
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/provider/CMediaStoreImagesMedia.h"
#include "elastos/droid/provider/CMediaStoreImagesThumbnails.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::IO::CFileInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CMediaStoreImagesMedia)

CAR_INTERFACE_IMPL(CMediaStoreImagesMedia, Singleton, IMediaStoreImagesMedia)

ECode CMediaStoreImagesMedia::GetINTERNAL_CONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    return GetContentUri(String("internal"), uri);
}

ECode CMediaStoreImagesMedia::GetEXTERNAL_CONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    return GetContentUri(String("external"), uri);
}

ECode CMediaStoreImagesMedia::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** cursor)
{
   return cr->Query(uri, projection, String(NULL), NULL, IMediaStoreImagesMedia::DEFAULT_SORT_ORDER, cursor);
}

ECode CMediaStoreImagesMedia::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& where,
    /* [in] */ const String& orderBy,
    /* [out] */ ICursor** cursor)
{
    return cr->Query(uri, projection, where,
         NULL, orderBy == String(NULL) ? IMediaStoreImagesMedia::DEFAULT_SORT_ORDER : orderBy, cursor);
}

ECode CMediaStoreImagesMedia::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& orderBy,
    /* [out] */ ICursor** cursor)
{
    return cr->Query(uri, projection, selection,
            selectionArgs, orderBy == String(NULL) ? IMediaStoreImagesMedia::DEFAULT_SORT_ORDER : orderBy, cursor);
}

ECode CMediaStoreImagesMedia::GetBitmap(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* url,
    /* [out] */ IBitmap** bitmap)
{
    AutoPtr<IInputStream> input;
    cr->OpenInputStream(url, (IInputStream**)&input);

    AutoPtr<IBitmapFactory> factory;
    ASSERT_SUCCEEDED(CBitmapFactory::AcquireSingleton(
        (IBitmapFactory**)&factory));

    factory->DecodeStream(input, bitmap);

    input->Close();
    return NOERROR;
}

ECode CMediaStoreImagesMedia::InsertImage(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& imagePath,
    /* [in] */ const String& name,
    /* [in] */ const String& description,
    /* [out] */ String* url)
{
    // Check if file exists with a FileInputStream
    AutoPtr<IFileInputStream> stream;
    CFileInputStream::New(imagePath, (IFileInputStream**)&stream);
    //try {

        AutoPtr<IBitmapFactory> factory;
        ASSERT_SUCCEEDED(CBitmapFactory::AcquireSingleton(
            (IBitmapFactory**)&factory));
        AutoPtr<IBitmap> bm;
        factory->DecodeFile(imagePath, (IBitmap**)&bm);

        InsertImage(cr, bm, name, description, url);
        bm->Recycle();

    //} finally {
        //try {
            ICloseable::Probe(stream)->Close();
        /*} catch (IOException e) {
        }*/
    //}

    return NOERROR;
}

AutoPtr<IBitmap> CMediaStoreImagesMedia::StoreThumbnail(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IBitmap* source,
    /* [in] */ Int64 id,
    /* [in] */ Float width,
    /* [in] */ Float height,
    /* [in] */ Int32 kind)
{
    // create the matrix to scale it
    AutoPtr<IMatrix> matrix;
    CMatrix::New((IMatrix**)&matrix);

    Int32 w, h;
    source->GetWidth(&w);
    source->GetHeight(&h);

    Float scaleX = width / w;
    Float scaleY = height / h;

    matrix->SetScale(scaleX, scaleY);

    AutoPtr<IBitmap> thumb;
    CBitmap::CreateBitmap(source, 0, 0, w, h, matrix, TRUE, (IBitmap**)&thumb);

    AutoPtr<IContentValues> values;
    CContentValues::New(4, (IContentValues**)&values);
    values->Put(IMediaStoreImagesThumbnails::KIND, kind);
    values->Put(IMediaStoreImagesThumbnails::IMAGE_ID, (Int32)id);
    thumb->GetHeight(&h);
    values->Put(IMediaStoreImagesThumbnails::HEIGHT, h);
    thumb->GetWidth(&w);
    values->Put(IMediaStoreImagesThumbnails::WIDTH, w);

    AutoPtr<IUri> url;
    AutoPtr<IMediaStoreImagesThumbnails> helper;
    CMediaStoreImagesThumbnails::AcquireSingleton((IMediaStoreImagesThumbnails**)&helper);
    AutoPtr<IUri> uri;
    helper->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
    cr->Insert(uri, values, (IUri**)&url);

    //try {
        AutoPtr<IOutputStream> thumbOut;
        cr->OpenOutputStream(url, (IOutputStream**)&thumbOut);

        Boolean res;
        thumb->Compress(Elastos::Droid::Graphics::BitmapCompressFormat_JPEG, 100, thumbOut, &res);
        thumbOut->Close();
        return thumb;
    /*}
    catch (FileNotFoundException ex) {
        return NULL;
    }
    catch (IOException ex) {
        return NULL;
    }*/
}

ECode CMediaStoreImagesMedia::InsertImage(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IBitmap* source,
    /* [in] */ const String& title,
    /* [in] */ const String& description,
    /* [out] */ String* urlvalue)
{
    VALIDATE_NOT_NULL(urlvalue);
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);

    values->Put(IMediaStoreMediaColumns::TITLE, title);
    values->Put(IMediaStoreImagesImageColumns::DESCRIPTION, description);
    values->Put(IMediaStoreMediaColumns::MIME_TYPE, String("image/jpeg"));

    AutoPtr<IUri> url;
    String stringUrl;    /* value to be returned */

    //try {
        AutoPtr<IUri> uri;
        GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        cr->Insert(uri, values, (IUri**)&url);

        if (source != NULL) {
            AutoPtr<IOutputStream> imageOut;
            cr->OpenOutputStream(url, (IOutputStream**)&imageOut);
            //try {
                Boolean res;
                source->Compress(Elastos::Droid::Graphics::BitmapCompressFormat_JPEG, 50, imageOut, &res);
            //} finally {
                imageOut->Close();
            //}

                Int64 id;
                AutoPtr<IContentUris> contentUris;
                CContentUris::AcquireSingleton((IContentUris**)&contentUris);
                contentUris->ParseId(url, &id);

            // Wait until MINI_KIND thumbnail is generated.
                AutoPtr<IBitmap> miniThumb;
                AutoPtr<IMediaStoreImagesThumbnails> helper;
                CMediaStoreImagesThumbnails::AcquireSingleton((IMediaStoreImagesThumbnails**)&helper);
                helper->GetThumbnail(cr, id, IMediaStoreImagesThumbnails::MINI_KIND, NULL, (IBitmap**)&miniThumb);
            // This is for backward compatibility.
            AutoPtr<IBitmap> microThumb = StoreThumbnail(cr, miniThumb, id, 50.0, 50.0,
                IMediaStoreImagesThumbnails::MICRO_KIND);
        } else {
            //Log.e(TAG, "Failed to create thumbnail, removing original");
            Int32 val;
            cr->Delete(url, String(NULL), NULL, &val);
            url = NULL;
        }
    /*} catch (Exception e) {
        Log.e(TAG, "Failed to insert image", e);
        if (url != NULL) {
            cr.delete(url, NULL, NULL);
            url = NULL;
        }
    }*/

    if (url != NULL) {
        stringUrl = Object::ToString(url);
        // url->ToString(&stringUrl);
    }

    *urlvalue = stringUrl;
    return NOERROR;
}

ECode CMediaStoreImagesMedia::GetContentUri(
    /* [in] */ const String& volumeName,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    StringBuilder builder;
    builder += IMediaStore::CONTENT_AUTHORITY_SLASH;
    builder += volumeName;
    builder += "/images/media";
    String str = builder.ToString();
    return helper->Parse(str, uri);
}

} //namespace Provider
} //namespace Droid
} //namespace Elastos
