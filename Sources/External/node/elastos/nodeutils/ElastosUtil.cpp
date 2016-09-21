/*
 * Copyright 2007, The Android Open Source Project
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

#define LOG_TAG "webcoreglue"

#include "config.h"
#include "IntRect.h"
#include <wtf/text/CString.h>
#include "wtf/Vector.h"

#include "NotImplemented.h"
#include <utils/Log.h>

#include "ElastosUtil.h"

namespace Elastos {

struct ElaNativeUtilCallback sElaNativeUtilCallback;
struct ElaBitmapCallback sElaBitmapCallback;
struct ElaInputStreamCallback sElaInputStreamCallback;

#ifdef __cplusplus
extern "C"
{
#endif
WK_EXPORT void Elastos_NativeUtil_InitCallback(Int32 cb)
{
    struct ElaNativeUtilCallback *pCallback = (struct ElaNativeUtilCallback*)cb;
    sElaNativeUtilCallback = *pCallback;
}

WK_EXPORT void Elastos_Bitmap_InitCallback(Int32 cb)
{
    struct ElaBitmapCallback* pCallback = (struct ElaBitmapCallback*)cb;
    sElaBitmapCallback = *pCallback;
}

WK_EXPORT void Elastos_InputStream_InitCallback(Int32 cb)
{
    struct ElaInputStreamCallback* pCallback = (struct ElaInputStreamCallback*)cb;
    sElaInputStreamCallback = *pCallback;
}
#ifdef __cplusplus
}
#endif

AutoPtr<IInterface> getRealObject(
    /* [in] */ AutoPtr<IWeakReference> obj)
{
    AutoPtr<IInterface> ws;
    obj->Resolve(EIID_IInterface, (IInterface**)&ws);
    return ws;
}

WTF::String ElstringToWtfString(
    /* [in] */ const Elastos::String& str)
{
    if (str.IsNull()) {
        return WTF::String();
    }
    WTF::String ret;
    if (str.GetLength() == str.GetByteLength())//ascii
    {
        ret = WTF::String(str.string());
    }
    else//multi-byte charset
    {
        AutoPtr<ArrayOf<Char16> > pArray = str.GetChar16s();
        ret = WTF::String(pArray->GetPayload(), str.GetLength());
    }
    /* optional solution for multi-byte
    for(int i = 0; i < str.GetLength(); ++i)
    {
        Int32 c = str.GetChar(i);
        if (!c)
            ret.append(WTF::String());
        else if(c > 0xffff)
        {
            UChar lead = U16_LEAD(c);
            UChar trail = U16_TRAIL(c);
            UChar utf16[2] = {lead, trail};
            ret.append(WTF::String(utf16, 2));
        }
        else
        {
            UChar n = (UChar)c;
            ret.append(WTF::String(&n, 1));
        }
    }
    */
    return ret;
}

Elastos::String WtfStringToElstring(
    /* [in] */ const WTF::String& str)
{
    return Elastos::String(str.utf8().data());
}

std::string ElstringToStdString(
    /* [in] */ const Elastos::String& str)
{
    if (str.IsNull()) {
        return std::string();
    }
    std::string ret(str.string());
    return ret;
}

Elastos::String StdStringToElstring(
    /* [in] */ const std::string& str)
{
    return Elastos::String(str.c_str());
}

} // namespace Elastos
