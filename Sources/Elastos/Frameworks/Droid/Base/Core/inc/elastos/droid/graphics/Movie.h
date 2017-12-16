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

#ifndef __ELASTOS_DROID_GRAPHICS_MOVIE_H__
#define __ELASTOS_DROID_GRAPHICS_MOVIE_H__

#include "Elastos.Droid.Graphics.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/core/Object.h>
#include <skia/images/SkMovie.h>

using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Graphics {

class Movie
    : public Object
    , public IMovie
{
public:
    CAR_INTERFACE_DECL()

    Movie(
        /* [in] */ Int64 nativeMovie);

    virtual ~Movie();

    CARAPI GetWidth(
        /* [out] */ Int32* width);

    CARAPI GetHeight(
        /* [out] */ Int32* height);

    CARAPI IsOpaque(
        /* [out] */ Boolean* isOpaque);

    CARAPI GetDuration(
        /* [out] */ Int32* duration);

    CARAPI SetTime(
        /* [in] */ Int32 relativeMilliseconds,
        /* [out] */ Boolean* isSet);

    CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPaint* paint);

    CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Float x,
        /* [in] */ Float y);

    static CARAPI DecodeStream(
        /* [in] */ IInputStream* is,
        /* [out] */ IMovie** movie);

    static CARAPI DecodeByteArray(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ IMovie** movie);

    static CARAPI DecodeFile(
        /* [in] */ const String& pathName,
        /* [out] */ IMovie** movie);

private:
    static CARAPI DecodeTempStream(
        /* [in] */ IInputStream* is,
        /* [out] */IMovie** movie);

    static CARAPI_(void) NativeDestructor(
        /* [in] */ Int64 nativeMovie);

    static CARAPI NativeDecodeAsset(
        /* [in] */ Int64 asset,
        /* [out] */IMovie** movie);

    static CARAPI NativeDecodeStream(
        /* [in] */ IInputStream* is,
        /* [out] */IMovie** movie);

private:
    Int64 mNativeMovie;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_MOVIE_H__
