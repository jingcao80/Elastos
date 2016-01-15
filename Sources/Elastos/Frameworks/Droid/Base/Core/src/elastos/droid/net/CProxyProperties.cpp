
#include "elastos/droid/net/CProxy.h"
#include "elastos/droid/net/CProxyProperties.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Net::Uri;
using Elastos::Droid::Text::TextUtils;

using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Net::CInetSocketAddress;
using Elastos::Net::IInetSocketAddress;
using Elastos::Net::IProxyHelper;

namespace Elastos {
namespace Droid {
namespace Net {

ECode CProxyProperties::constructor()
{
    return NOERROR;
}

ECode CProxyProperties::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& exclList)
{
    mHost = host;
    mPort = port;
    SetExclusionList(exclList);
    return NOERROR;
}

// copy constructor instead of clone
ECode CProxyProperties::constructor(
    /* [in] */ IProxyProperties* source)
{
    if (source != NULL) {
        source->GetHost(&mHost);
        source->GetPort(&mPort);
        source->GetExclusionList(&mExclusionList);
        source->GetParsedExclusionList((ArrayOf<String>**)&mParsedExclusionList);
    }
    return NOERROR;
}

ECode CProxyProperties::GetSocketAddress(
    /* [out] */ IInetSocketAddress** address)
{
    VALIDATE_NOT_NULL(address);
    AutoPtr<IInetSocketAddress> inetSocketAddress;
    CInetSocketAddress::New(mHost, mPort, (IInetSocketAddress**)&inetSocketAddress);
    *address = inetSocketAddress;
    REFCOUNT_ADD(*address);
    return NOERROR;
}

ECode CProxyProperties::GetHost(
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host);
    *host = mHost;
    return NOERROR;
}

ECode CProxyProperties::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);
    *port = mPort;
    return NOERROR;
}

ECode CProxyProperties::GetExclusionList(
    /* [out] */ String* exList)
{
    VALIDATE_NOT_NULL(exList);
    *exList = mExclusionList;
    return NOERROR;
}
ECode CProxyProperties::GetParsedExclusionList(
    /*[out, callee]*/ ArrayOf<String>** parsedExclusionList)
{
    VALIDATE_NOT_NULL(parsedExclusionList);
    *parsedExclusionList = mParsedExclusionList;
    REFCOUNT_ADD(*parsedExclusionList);
    return NOERROR;
}

void CProxyProperties::SetExclusionList(
    /* [in] */ const String& exclusionList)
{
    mExclusionList = exclusionList.ToLowerCase();
    if (exclusionList.IsNull()) {
        mParsedExclusionList = ArrayOf<String>::Alloc(0);
    }
    else {
       AutoPtr <ArrayOf<String> >splitExclusionList;
       StringUtils::Split(mExclusionList, String(","), (ArrayOf<String>**)&splitExclusionList);
       Int32 length;
       length = splitExclusionList->GetLength();
       mParsedExclusionList = ArrayOf<String>::Alloc(length* 2);
       for (int i = 0; i < length; i++) {
           String str;
           str = (*splitExclusionList)[i];
           String s = str.Trim();
           if (s.StartWith(".")) {
               s = s.Substring(1);
           }
           String temp = String(".") + s;
           mParsedExclusionList->Set(i*2, s);
           mParsedExclusionList->Set((i*2)+1, temp);
       }
    }
}

ECode CProxyProperties::IsExcluded(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 length;
    length = mParsedExclusionList->GetLength();
    AutoPtr<ICharSequence> chars;
    CString::New(url, (ICharSequence**)&chars);
    if (TextUtils::IsEmpty((ICharSequence*)&chars) || mParsedExclusionList == NULL ||length == 0) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IUri> u;
    Uri::Parse(url, (IUri**)&u);
    String urlDomain;
    u->GetHost(&urlDomain);

    if (urlDomain.IsNull()) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 length2;
    length2 = mParsedExclusionList->GetLength();
    for (Int32 i = 0; i< length2; i+=2) {
        if (urlDomain.Equals((*mParsedExclusionList)[i]) || urlDomain.EndWith((*mParsedExclusionList)[i+1])) {
            *result =  TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CProxyProperties::MakeProxy(
    /* [out] */ Elastos::Net::IProxy** proxy)
{
    VALIDATE_NOT_NULL(proxy);
    AutoPtr<Elastos::Net::IProxyHelper> proxyhelper;
    Elastos::Net::CProxyHelper::AcquireSingleton((Elastos::Net::IProxyHelper**)&proxyhelper);
    AutoPtr<Elastos::Net::IProxy> proxyTemp;
    proxyhelper->GetNO_PROXY((Elastos::Net::IProxy**)&proxyTemp);
    if (!(mHost.IsNull())) {
        AutoPtr<IInetSocketAddress> inetSocketAddress;
        CInetSocketAddress::New(mHost, mPort, (IInetSocketAddress**)&inetSocketAddress);
        proxyTemp = NULL;
        Elastos::Net::CProxy::New(Elastos::Net::ProxyType_HTTP,inetSocketAddress, (Elastos::Net::IProxy**)&proxyTemp);
    }
    *proxy = proxyTemp;
    REFCOUNT_ADD(*proxy);
    return NOERROR;
}
//@Override
ECode CProxyProperties::ToString(
    /* [out] */ String* output)
{
    VALIDATE_NOT_NULL(output);

    StringBuffer sb("");
    if (!mHost.IsNull()) {
        sb += String("[");
        sb += mHost;
        sb += String("]");
        String temp;
        temp = StringUtils::ToString(mPort);
        sb += temp;

        if (!mExclusionList.IsNull()) {
            sb += String(" xl=");
            sb += mExclusionList;
        }
    }
    else {
        sb += String("[ProxyProperties.mHost == null]");
    }
    *output = sb.ToString();
    return NOERROR;
}

ECode CProxyProperties::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result )
{
    VALIDATE_NOT_NULL(result);
    return Equals(IProxyProperties::Probe(obj), result);
}

//@Override
ECode CProxyProperties::Equals(
    /* [in] */ IProxyProperties* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (IProxyProperties::Probe(o) != NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IProxyProperties> p = o;
    CProxyProperties::New(o,(IProxyProperties**)&p);
    String exclusionlist;
    p->GetExclusionList(&exclusionlist);
    if (mExclusionList != NULL && !mExclusionList.Equals(exclusionlist)) {
        *result = FALSE;
        return NOERROR;
    }
    String szHost;
    p->GetHost(&szHost);
    Int32 port;
    p->GetPort(&port);
    if (!(mHost.IsNull()) && !(szHost.IsNull()) && !(mHost.Equals(szHost))) {
        *result = FALSE;
        return E_INVALID_OPERATION;
    }
    if (!(mHost.IsNull()) && (szHost.IsNull())) {
        *result = FALSE;
        return E_INVALID_OPERATION;
    }
    if ((mHost.IsNull()) && !(szHost.IsNull())) {
        *result = FALSE;
        return E_INVALID_OPERATION;
    }
    if (mPort != port) {
        *result = FALSE;
        return E_INVALID_OPERATION;
    }
    *result = TRUE;
    return NOERROR;
}

/*
* generate hashcode based on significant fields
*/
//@Override
ECode CProxyProperties::GetHashCode(
    /* [out] */ Int32* code)
{
    Int32 temp;
    Int32 temp2;
    if (mHost.IsNull()) {
        temp=0;
    }
    else {
        temp = mHost.GetHashCode();
    }
    if (mExclusionList.IsNull()) {

    }
    else {
        temp2 = mExclusionList.GetHashCode();
        temp=temp+temp2;
    }
    temp = temp + mPort;
    *code = temp;

    return NOERROR;
}

/**
* Implement the Parcelable interface.
* @hide
*/
ECode CProxyProperties::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (!(mHost.IsNull())) {
        dest->WriteByte(1);
        dest->WriteString(mHost);
        dest->WriteInt32(mPort);
    }
    else {
        dest->WriteByte(0);
    }
    dest->WriteString(mExclusionList);
    dest->WriteArrayOfString(mParsedExclusionList);
    return NOERROR;
}

ECode CProxyProperties::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    Byte value1;
    Byte value2;
    if (!(mHost.IsNull())) {
        source->ReadByte(&value1);
        source->ReadString(&mHost);
        source->ReadInt32(&mPort);
    }
    else {
        source->ReadByte(&value2);
    }
    source->ReadString(&mExclusionList);
    source->ReadArrayOfString((ArrayOf<String>**)&mParsedExclusionList);
    return NOERROR;
}


} //namespace Net
} //namespace Droid
} //namespace Elastos
