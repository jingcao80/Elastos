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

#ifndef __LIBCORE_NET_HTTP_CHTTPDATEHELPER_H__
#define __LIBCORE_NET_HTTP_CHTTPDATEHELPER_H__

#include "_Libcore_Net_Http_CHttpDate.h"
#include "Singleton.h"
#include <pthread.h>

using Elastos::Core::Singleton;
using Elastos::Utility::IDate;
using Elastos::Text::IDateFormat;

namespace Libcore {
namespace Net {
namespace Http {

CarClass(CHttpDate)
    , public Singleton
    , public IHttpDate
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Parse(
        /* [in] */ const String& value,
        /* [out] */ IDate** adate);

    CARAPI Format(
        /* [in] */ IDate* value,
        /* [out] */ String* str);

public:
    static CARAPI _Parse(
        /* [in] */ const String& value,
        /* [out] */ IDate** adate);

    static CARAPI _Format(
        /* [in] */ IDate* value,
        /* [out] */ String* str);

private:
    static CARAPI_(Boolean) InitTLS();
    static CARAPI_(AutoPtr<IDateFormat>) InitDateFormatValue();

private:
    /**
     * Most websites serve cookies in the blessed format. Eagerly create the parser to ensure such
     * cookies are on the fast path.
     */
    // private static final ThreadLocal<DateFormat> STANDARD_DATE_FORMAT
    //         = new ThreadLocal<DateFormat>() {
    //     @Override protected DateFormat initialValue() {
    //         DateFormat rfc1123 = new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss zzz", Locale.US);
    //         rfc1123.setTimeZone(TimeZone.getTimeZone("UTC"));
    //         return rfc1123;
    //     }
    // };

    /**
     * If we fail to parse a date in a non-standard format, try each of these formats in sequence.
     */
    static const String BROWSER_COMPATIBLE_DATE_FORMATS[14];
    static pthread_key_t key_tls;
};

} // namespace Http
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_HTTP_CHTTPDATEHELPER_H__
