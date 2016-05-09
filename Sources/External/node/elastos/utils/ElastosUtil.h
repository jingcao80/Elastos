/*
 * Copyright 2008, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ELASTOS_UTIL_H__
#define __ELASTOS_UTIL_H__

#include "config.h"
#include <string>
#include "IntRect.h"
#include <wtf/text/WTFString.h>

#include "ElAndroid.h"
#include <elastos.h>

namespace Elastos {

struct ElaNativeUtilCallback
{
    Elastos::String (*getDatabaseDirectory)();
    Elastos::String (*getCacheDirectory)();
    Elastos::String (*getPackageName)();
    Boolean (*canSatisfyMemoryAllocation)(Int64);
    Elastos::String (*getMIMETypeForExtension)(const Elastos::String&);
};
extern struct ElaNativeUtilCallback sElaNativeUtilCallback;

struct ElaBitmapCallback
{
    AutoPtr< ::IInterface> (*createBitmap)(Int32 nativeBitmap, ArrayOf<Byte>* buffer, Boolean isMutable,
                                                        ArrayOf<Byte>* ninePatchChunk, Int32 density);
    AutoPtr< ::IInterface> (*createBitmap2)(Int32 nativeBitmap, ArrayOf<Byte>* buffer, Boolean isMutable,
                            ArrayOf<Byte>* ninePatchChunk, ArrayOf<Int32>* layoutbounds, Int32 density);

};
extern struct ElaBitmapCallback sElaBitmapCallback;

struct ElaInputStreamCallback
{
    Int32 (*read)(::IInterface* inputStream, ArrayOf<Byte>* byteArray);
    void (*close)(::IInterface* inputStream);
};
extern struct ElaInputStreamCallback sElaInputStreamCallback;

//get the real obj from the weakobj

AutoPtr< ::IInterface> getRealObject(
    /* [in] */ IWeakReference* obj);

// Create a WTF::String object from an Elastos String.
WTF::String ElstringToWtfString(
    /* [in] */ const Elastos::String&);

Elastos::String WtfStringToElstring(
    /* [in] */ const WTF::String& str);

std::string ElstringToStdString(
    /* [in] */ const Elastos::String&);

Elastos::String StdStringToElstring(
    /* [in] */ const std::string& str);
} // namespace Elasto

#endif// __ELASTOS_UTIL_H__
