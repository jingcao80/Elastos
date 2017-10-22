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

#include "elastos/droid/net/MailTo.h"
#include "elastos/droid/net/CMailTo.h"
#include "elastos/droid/net/Uri.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(MailTo, Object, IMailTo)

const String MailTo::TO("to");
const String MailTo::BODY("body");
const String MailTo::CC("cc");
const String MailTo::SUBJECT("subject");

ECode MailTo::GetField(
    /* [in] */ const String& name,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    AutoPtr<ICharSequence> csq_name;
    CString::New(name, (ICharSequence**)&csq_name);
    AutoPtr<IInterface> csq_result;
    mHeaders->Get(csq_name, (IInterface**)&csq_result);
    if (ICharSequence::Probe(csq_result) != NULL)
        ICharSequence::Probe(csq_result)->ToString(result);
    return NOERROR;
}

ECode MailTo::IsMailTo(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (url != NULL && url.StartWith(IMailTo::MAILTO_SCHEME)) {
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode MailTo::Parse(
    /* [in] */ const String& url,
    /* [out] */ IMailTo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (url == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean bval;
    IsMailTo(url, &bval);
    if (!bval) {
        Logger::E("MailTo", "Not a mailto scheme");
        return E_RUNTIME_EXCEPTION;
    }

    // Strip the scheme as the Uri parser can't cope with it.
    String noScheme = url.Substring(IMailTo::MAILTO_SCHEME.GetLength());
    AutoPtr<IUri> email;
    Uri::Parse(noScheme, (IUri**)&email);
    AutoPtr<MailTo> m = new CMailTo();
    m->constructor();

    String query;
    email->GetQuery(&query);


    if (query != NULL ) {
        AutoPtr<ArrayOf<String> > queries;
        StringUtils::Split(query,String("&"), (ArrayOf<String>**)&queries);

        Int32 tempsize = queries->GetLength();
        Int32 i;
        String q;
        for (i=0;i<tempsize;i++)
        {
            q=(*queries)[i];
            AutoPtr<ArrayOf<String> > nameval;
            StringUtils::Split(q,String("="), (ArrayOf<String>**)&nameval);
             if (nameval->GetLength() == 0) {
                continue;
            }

            // insert the headers with the name in lowercase so that
            // we can easily find common headers
            String s1;
            Uri::Decode((*nameval)[0],&s1);
            AutoPtr<ICharSequence> csq1;
            CString::New(s1.ToLowerCase(/*Locale::ROOT*/), (ICharSequence**)&csq1);
            if (nameval->GetLength() > 1)
            {
                String s2;
                Uri::Decode((*nameval)[1], &s2);
                AutoPtr<ICharSequence> csq2;
                CString::New(s2, (ICharSequence**)&csq2);
                m->mHeaders->Put(csq1, csq2);
            }
            else
            {
                m->mHeaders->Put(csq1, NULL);
            }
        }
    }

    // Address can be specified in both the headers and just after the
    // mailto line. Join the two together.
    String address;
    email->GetPath(&address);

    if (address != NULL) {
        String addr;
        m->GetTo(&addr);

        if (addr != NULL) {
            address += ", ";
            address += addr;
        }
        AutoPtr<ICharSequence> csq1, csq2;
        CString::New(TO, (ICharSequence**)&csq1);
        CString::New(address, (ICharSequence**)&csq2);
        m->mHeaders->Put(csq1, csq2);
    }

    *result = IMailTo::Probe(m);
    REFCOUNT_ADD(*result);
    return NOERROR;
}


ECode MailTo::GetTo(
    /* [out] */ String* result)
{
    return GetField(TO, result);
}

ECode MailTo::GetCc(
    /* [out] */ String* result)
{
    return GetField(CC, result);
}

ECode MailTo::GetSubject(
    /* [out] */ String* result)
{
    return GetField(SUBJECT, result);
}

ECode MailTo::GetBody(
    /* [out] */ String* result)
{
    return GetField(BODY, result);
}

ECode MailTo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb(IMailTo::MAILTO_SCHEME);
    sb.AppendChar('?');

    //for (Map.Entry<String,String> header : mHeaders.entrySet()) {
    AutoPtr<ISet> entrySet;
    mHeaders->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> header;
        it->GetNext((IInterface**)&header);
        AutoPtr<IInterface> key, value;
        IMapEntry::Probe(header)->GetKey((IInterface**)&key);
        IMapEntry::Probe(header)->GetValue((IInterface**)&value);
        String sKey, sValue;
        ICharSequence::Probe(key)->ToString(&sKey);
        ICharSequence::Probe(value)->ToString(&sValue);
        String temp1, temp2;
        Uri::Encode(sKey, &temp1);
        sb.Append(temp1);
        sb.AppendChar('=');

        Uri::Encode(sValue, &temp2);
        sb.Append(temp2);
        sb.AppendChar('&');
    }

    *result = sb.ToString();
    return NOERROR;
}

ECode MailTo::GetHeaders(
    /* [out] */ IMap** rethashmap)
{
    VALIDATE_NOT_NULL(rethashmap)

    *rethashmap = IMap::Probe(mHeaders);
    REFCOUNT_ADD(*rethashmap)
    return NOERROR;
}

ECode MailTo::constructor()
{
    return CHashMap::New((IHashMap**)&mHeaders);
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
