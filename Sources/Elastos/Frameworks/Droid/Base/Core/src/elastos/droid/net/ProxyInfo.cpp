
#include <Elastos.CoreLibrary.Net.h>
#include "elastos/droid/net/ProxyInfo.h"
#include "elastos/droid/net/CProxy.h"
#include "elastos/droid/net/CProxyInfo.h"
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Text::TextUtils;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Net::CInetSocketAddress;
using Elastos::Net::CProxy;
using Elastos::Net::IInetSocketAddress;
using Elastos::Net::ISocketAddress;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(ProxyInfo, Object, IParcelable, IProxyInfo)

ProxyInfo::ProxyInfo()
    : mPort(0)
{}

ECode ProxyInfo::BuildDirectProxy(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ IProxyInfo** result)
{
    return CProxyInfo::New(host, port, String(NULL), result);
}

ECode ProxyInfo::BuildDirectProxy(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ IList* exclList,
    /* [out] */ IProxyInfo** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<IInterface*> > array;
    exclList->ToArray((ArrayOf<IInterface*>**)&array);
    String s(",");
    AutoPtr<ICharSequence> csq;
    CString::New(s, (ICharSequence**)&csq);
    String join = TextUtils::Join(csq, IIterable::Probe(exclList));
    AutoPtr<ArrayOf<String> > arrayString = ArrayOf<String>::Alloc(array->GetLength());
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        ICharSequence::Probe((*array)[i])->ToString(&s);
        arrayString->Set(i, s);
    }
    AutoPtr<CProxyInfo> rev = new CProxyInfo();
    rev->constructor(host, port, join, arrayString);
    *result = IProxyInfo::Probe(rev);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ProxyInfo::BuildPacProxy(
    /* [in] */ IUri* pacUri,
    /* [out] */ IProxyInfo** result)
{
    return CProxyInfo::New(pacUri, result);
}

ECode ProxyInfo::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& exclList)
{
    mHost = host;
    mPort = port;
    SetExclusionList(exclList);
    Uri::GetEMPTY((IUri**)&mPacFileUrl);
    return NOERROR;
}

ECode ProxyInfo::constructor(
    /* [in] */ IUri* pacFileUrl)
{
    mHost = LOCAL_HOST;
    mPort = LOCAL_PORT;
    SetExclusionList(LOCAL_EXCL_LIST);
    if (pacFileUrl == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mPacFileUrl = pacFileUrl;
    return NOERROR;
}

ECode ProxyInfo::constructor(
    /* [in] */ const String& pacFileUrl)
{
    mHost = LOCAL_HOST;
    mPort = LOCAL_PORT;
    SetExclusionList(LOCAL_EXCL_LIST);
    Uri::Parse(pacFileUrl, (IUri**)&mPacFileUrl);
    return NOERROR;
}

ECode ProxyInfo::constructor(
    /* [in] */ IUri* pacFileUrl,
    /* [in] */ Int32 localProxyPort)
{
    mHost = LOCAL_HOST;
    mPort = localProxyPort;
    SetExclusionList(LOCAL_EXCL_LIST);
    if (pacFileUrl == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mPacFileUrl = pacFileUrl;
    return NOERROR;
}

ECode ProxyInfo::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& exclList,
    /* [in] */ ArrayOf<String>* parsedExclList)
{
    mHost = host;
    mPort = port;
    mExclusionList = exclList;
    mParsedExclusionList = parsedExclList;
    Uri::GetEMPTY((IUri**)&mPacFileUrl);
    return NOERROR;
}

ECode ProxyInfo::constructor(
    /* [in] */ IProxyInfo* source)
{
    if (source != NULL) {
        source->GetHost(&mHost);
        source->GetPort(&mPort);
        mPacFileUrl = ((ProxyInfo*)source)->mPacFileUrl;
        source->GetExclusionListAsString(&mExclusionList);
        mParsedExclusionList = ((ProxyInfo*)source)->mParsedExclusionList;
    } else {
        Uri::GetEMPTY((IUri**)&mPacFileUrl);
    }
    return NOERROR;
}

ECode ProxyInfo::GetSocketAddress(
    /* [out] */ IInetSocketAddress** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    // try {
    ECode ec = CInetSocketAddress::New(mHost, mPort, (IInetSocketAddress**)&result);
    // } catch (IllegalArgumentException e) { }
    if (FAILED(ec)) {
        if (ec != (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) return ec;
    }
    return NOERROR;
}

ECode ProxyInfo::GetPacFileUrl(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mPacFileUrl)
}

ECode ProxyInfo::GetHost(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mHost)
}

ECode ProxyInfo::GetPort(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mPort)
}

ECode ProxyInfo::GetExclusionList(
    /* [out, callee] */ ArrayOf<String>** result)
{
    FUNC_RETURN(mParsedExclusionList)
}

ECode ProxyInfo::GetExclusionListAsString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mExclusionList)
}

ECode ProxyInfo::SetExclusionList(
    /* [in] */ const String& exclusionList)
{
    mExclusionList = exclusionList;
    if (mExclusionList == NULL) {
        mParsedExclusionList = ArrayOf<String>::Alloc(0);
    } else {
        StringUtils::Split(exclusionList.ToLowerCase(), String(","), (ArrayOf<String>**)&mParsedExclusionList);
    }
    return NOERROR;
}

ECode ProxyInfo::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IUri> empty;
    Uri::GetEMPTY((IUri**)&empty);
    Boolean isEqual;
    IObject::Probe(empty)->Equals(mPacFileUrl, &isEqual);
    if (!isEqual) FUNC_RETURN(TRUE)
    Int32 validCode;
    Proxy::Validate(mHost == NULL ? String("") : mHost,
            mPort == 0 ? String("") : StringUtils::ToString(mPort),
            mExclusionList == NULL ? String("") : mExclusionList,
            &validCode);
    *result = IProxy::PROXY_VALID == validCode;
    return NOERROR;
}

ECode ProxyInfo::MakeProxy(
    /* [out] */ Elastos::Net::IProxy** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<Elastos::Net::IProxyHelper> helper;
    Elastos::Net::CProxyHelper::AcquireSingleton((Elastos::Net::IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> proxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&proxy);
    if (mHost != String(NULL)) {
        // try {
        AutoPtr<IInetSocketAddress> inetSocketAddress;
        ECode ec = CInetSocketAddress::New(mHost, mPort, (IInetSocketAddress**)&inetSocketAddress);
        if (FAILED(ec)) {
            if (ec != (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) return ec;
        }
        AutoPtr<Elastos::Net::IProxy> proxy;
        ec = Elastos::Net::CProxy::New(Elastos::Net::ProxyType_HTTP, ISocketAddress::Probe(inetSocketAddress), (Elastos::Net::IProxy**)&proxy);
        if (FAILED(ec)) {
            if (ec != (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) return ec;
        }
        // } catch (IllegalArgumentException e) {
        // }
    }
    FUNC_RETURN(proxy)
}

ECode ProxyInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    AutoPtr<IUri> empty;
    Uri::GetEMPTY((IUri**)&empty);
    Boolean isEqual;
    IObject::Probe(empty)->Equals(mPacFileUrl, &isEqual);
    if (!isEqual) {
        sb.Append("PAC Script: ");
        String s;
        IObject::Probe(mPacFileUrl)->ToString(&s);
        sb.Append(s);
    } else if (mHost != NULL) {
        sb.Append("[");
        sb.Append(mHost);
        sb.Append("] ");
        sb.Append(mPort);
        if (mExclusionList != NULL) {
                sb.Append(" xl=");
                sb.Append(mExclusionList);
        }
    } else {
        sb.Append("[ProxyProperties.mHost == NULL]");
    }
    sb.ToString(result);
    return NOERROR;
}

ECode ProxyInfo::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) FUNC_RETURN(TRUE)
    if (!(IProxyInfo::Probe(o) != NULL)) FUNC_RETURN(FALSE)
    ProxyInfo* p = (ProxyInfo*)(IProxyInfo::Probe(o));
    // If PAC URL is present in either then they must be equal.
    // Other parameters will only be for fall back.
    AutoPtr<IUri> empty;
    Uri::GetEMPTY((IUri**)&empty);
    Boolean isEqual;
    IObject::Probe(empty)->Equals(mPacFileUrl, &isEqual);
    if (!isEqual) {
        IObject::Probe(mPacFileUrl)->Equals(Ptr(p)->Func(p->GetPacFileUrl), &isEqual);
        *result = isEqual && mPort == p->mPort;
        return NOERROR;
    }
    IObject::Probe(empty)->Equals(p->mPacFileUrl, &isEqual);
    if (!isEqual) {
        *result = FALSE;
        return NOERROR;
    }
    if (mExclusionList != String(NULL) && !mExclusionList.Equals(Ptr(p)->Func(p->GetExclusionListAsString))) {
        *result = FALSE;
        return NOERROR;
    }
    if (mHost != String(NULL) && Ptr(p)->Func(p->GetHost) != String(NULL) && mHost.Equals(Ptr(p)->Func(p->GetHost)) == FALSE) {
        *result = FALSE;
        return NOERROR;
    }
    if (mHost != String(NULL) && p->mHost == String(NULL)) return FALSE;
    if (mHost == String(NULL) && p->mHost != String(NULL)) return FALSE;
    if (mPort != p->mPort) return FALSE;
    *result = TRUE;
    return NOERROR;
}

ECode ProxyInfo::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = ((String(NULL) == mHost) ? 0 : mHost.GetHashCode())
            + ((String(NULL) == mExclusionList) ? 0 : mExclusionList.GetHashCode())
            + mPort;
    return NOERROR;
}

ECode ProxyInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadString(&mHost);
    parcel->ReadInt32(&mPort);
    parcel->ReadString(&mExclusionList);
    parcel->ReadArrayOfString((ArrayOf<String>**)&mParsedExclusionList);
    AutoPtr<IInterface> obj;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mPacFileUrl = IUri::Probe(obj);
    return NOERROR;
}

ECode ProxyInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mHost);
    dest->WriteInt32(mPort);
    dest->WriteString(mExclusionList);
    dest->WriteArrayOfString(mParsedExclusionList);
    dest->WriteInterfacePtr(mPacFileUrl);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
