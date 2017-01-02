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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECLIST_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECLIST_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Media_CMediaCodecList.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecList)
    , public Object
    , public IMediaCodecList
{
public:
    CMediaCodecList();

    virtual ~CMediaCodecList();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Create a list of media-codecs of a specific kind.
     * @param kind Either {@code REGULAR_CODECS} or {@code ALL_CODECS}.
     */
    CARAPI constructor(
        /* [in] */ Int32 kind);

    /**
     * Returns the list of {@link MediaCodecInfo} objects for the list
     * of media-codecs.
     */
    CARAPI GetCodecInfos(
        /* [out, callee] */ ArrayOf<IMediaCodecInfo*>** result);

    /**
     * Find a decoder supporting a given {@link MediaFormat} in the list
     * of media-codecs.
     *
     * @param format A decoder media format with optional feature directives.
     * @throws IllegalArgumentException if format is not a valid media format.
     * @throws NullPointerException if format is null.
     * @return the name of a decoder that supports the given format and feature
     *         requests, or {@code null} if no such codec has been found.
     */
    CARAPI FindDecoderForFormat(
        /* [in] */ IMediaFormat* format,
        /* [out] */ String* result);

    /**
     * Find an encoder supporting a given {@link MediaFormat} in the list
     * of media-codecs.
     *
     * @param format An encoder media format with optional feature directives.
     * @throws IllegalArgumentException if format is not a valid media format.
     * @throws NullPointerException if format is null.
     * @return the name of an encoder that supports the given format and feature
     *         requests, or {@code null} if no such codec has been found.
     */
    CARAPI FindEncoderForFormat(
        /* [in] */ IMediaFormat* format,
        /* [out] */ String* result);

    /**
     * Count the number of available (regular) codecs.
     *
     * @deprecated Use {@link #getCodecInfos} instead.
     *
     * @see #REGULAR_CODECS
     */
    static CARAPI GetCodecCount(
        /* [out] */ Int32* result);

    /**
     * Return the {@link MediaCodecInfo} object for the codec at
     * the given {@code index} in the regular list.
     *
     * @deprecated Use {@link #getCodecInfos} instead.
     *
     * @see #REGULAR_CODECS
     */
    static CARAPI GetCodecInfoAt(
        /* [in] */ Int32 index,
        /* [out] */ IMediaCodecInfo** result);

    /** @hide */
    static CARAPI GetInfoFor(
        /* [in] */ const String& codec,
        /* [out] */ IMediaCodecInfo** result);

private:
    CARAPI_(String) FindCodecForFormat(
        /* [in] */ Boolean encoder,
        /* [in] */ IMediaFormat* format);

    static CARAPI_(void) InitCodecList();

    static CARAPI_(AutoPtr<IMediaCodecInfo>) GetNewCodecInfoAt(
        /* [in] */ Int32 index);

    static CARAPI NativeGetCodecCount(
        /* [out] */ Int32* result);

    /* package private */
    static CARAPI NativeGetCodecName(
        /* [in] */ Int32 index,
        /* [out] */ String* result);

    /* package private */
    static CARAPI NativeIsEncoder(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* result);

    /* package private */
    static CARAPI NativeGetSupportedTypes(
        /* [in] */ Int32 index,
        /* [out, callee] */ ArrayOf<String>** result);

    /* package private */
    static CARAPI NativeGetCodecCapabilities(
            /* [in] */ Int32 index,
            /* [in] */ const String& type,
            /* [out] */ IMediaCodecInfoCodecCapabilities** result);

    /* package private */
    static CARAPI NativeFindCodecByName(
        /* [in] */ const String& codec,
        /* [out] */ Int32* result);

    static CARAPI_(void) NativeInit();

private:
    static const String TAG;

    static Object sInitLock;
    static AutoPtr<ArrayOf<IMediaCodecInfo*> > sAllCodecInfos;
    static AutoPtr<ArrayOf<IMediaCodecInfo*> > sRegularCodecInfos;

    AutoPtr<ArrayOf<IMediaCodecInfo*> > mCodecInfos;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECLIST_H__
