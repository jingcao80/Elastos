
#include "org/apache/http/ProtocolVersion.h"
#include "org/apache/http/CProtocolVersion.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Utility::ICharArrayBuffer;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {

const Int64 ProtocolVersion::mSerialVersionUID;

ProtocolVersion::ProtocolVersion()
    : mMajor(0)
    , mMinor(0)
{}

CAR_INTERFACE_IMPL_2(ProtocolVersion, Object, IProtocolVersion, ICloneable)

ECode ProtocolVersion::GetProtocol(
    /* [out] */ String* protocol)
{
    VALIDATE_NOT_NULL(protocol)
    *protocol = mProtocol;
    return NOERROR;
}

ECode ProtocolVersion::GetMajor(
    /* [out] */ Int32* major)
{
    VALIDATE_NOT_NULL(major)
    *major = mMajor;
    return NOERROR;
}

ECode ProtocolVersion::GetMinor(
    /* [out] */ Int32* minor)
{
    VALIDATE_NOT_NULL(minor)
    *minor = mMinor;
    return NOERROR;
}

ECode ProtocolVersion::ForVersion(
    /* [in] */ Int32 major,
    /* [in] */ Int32 minor,
    /* [out] */ IProtocolVersion** protocolVersion)
{
    VALIDATE_NOT_NULL(protocolVersion)

    if ((major == mMajor) && (minor == mMinor)) {
        *protocolVersion = IProtocolVersion::Probe(this);
        REFCOUNT_ADD(*protocolVersion)
        return NOERROR;
    }

    // argument checking is done in the constructor
    return CProtocolVersion::New(mProtocol, mMajor, mMinor, protocolVersion);;
}

ECode ProtocolVersion::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = this->mProtocol.GetHashCode() ^ (this->mMajor * 100000) ^ this->mMinor;
    return NOERROR;
}

ECode ProtocolVersion::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals)

    if (this->Probe(EIID_IInterface) == obj) {
        *equals = TRUE;
        return NOERROR;
    }
    if (IProtocolVersion::Probe(obj) == NULL) {
        *equals = FALSE;
        return NOERROR;
    }
    AutoPtr<IProtocolVersion> that = IProtocolVersion::Probe(obj);

    String thatProtocol;
    that->GetProtocol(&thatProtocol);
    Int32 thatMajor;
    that->GetMajor(&thatMajor);
    Int32 thatMinor;
    that->GetMinor(&thatMinor);
    *equals = ((this->mProtocol.Equals(thatProtocol)) &&
            (mMajor == thatMajor) && (mMinor == thatMinor));
    return NOERROR;
}

ECode ProtocolVersion::IsComparable(
    /* [in] */ IProtocolVersion* that,
    /* [out] */ Boolean* isComparable)
{
    VALIDATE_NOT_NULL(isComparable)

    *isComparable = FALSE;
    if (that != NULL) {
        String thatProtocol;
        that->GetProtocol(&thatProtocol);
        *isComparable = mProtocol.Equals(thatProtocol);
    }
    return NOERROR;
}

ECode ProtocolVersion::CompareToVersion(
    /* [in] */ IProtocolVersion* that,
    /* [out] */ Int32* delta)
{
    VALIDATE_NOT_NULL(delta)
    *delta = 0;

    if (that == NULL) {
        Logger::E("ProtocolVersion", "Protocol version must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String thatProtocol;
    that->GetProtocol(&thatProtocol);
    if (!this->mProtocol.Equals(thatProtocol)) {
        Logger::E("ProtocolVersion", "Versions for different protocols cannot be compared. %p %p", this, that);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 thisMajor, thatMajor;
    GetMajor(&thisMajor);
    that->GetMajor(&thatMajor);
    *delta = thisMajor - thatMajor;
    if (*delta == 0) {
        Int32 thisMinor, thatMinor;
        GetMinor(&thisMinor);
        that->GetMinor(&thatMinor);
        *delta = thisMinor - thatMinor;
    }
    return NOERROR;
}

ECode ProtocolVersion::GreaterEquals(
    /* [in] */ IProtocolVersion* protocolVersion,
    /* [out] */ Boolean* greaterEquals)
{
    VALIDATE_NOT_NULL(greaterEquals)
    Boolean isComparable;
    IsComparable(protocolVersion, &isComparable);
    Int32 delta;
    CompareToVersion(protocolVersion, &delta);
    *greaterEquals = isComparable && (delta >= 0);
    return NOERROR;
}

ECode ProtocolVersion::LessEquals(
    /* [in] */ IProtocolVersion* protocolVersion,
    /* [out] */ Boolean* lessEquals)
{
    VALIDATE_NOT_NULL(lessEquals)
    Boolean isComparable;
    IsComparable(protocolVersion, &isComparable);
    Int32 delta;
    CompareToVersion(protocolVersion, &delta);
    *lessEquals = isComparable && (delta <= 0);
    return NOERROR;
}

CARAPI ProtocolVersion::ToString(
    /* [out] */ String* string)
{
    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(16, (ICharArrayBuffer**)&buffer);
    buffer->Append(this->mProtocol);
    buffer->Append('/');
    buffer->Append(StringUtils::ToString(this->mMajor));
    buffer->Append('.');
    buffer->Append(StringUtils::ToString(this->mMinor));
    return IObject::Probe(buffer)->ToString(string);
}

CARAPI ProtocolVersion::CloneImpl(
    /* [in] */ IProtocolVersion* ver)
{
    assert(ver);
    AutoPtr<ProtocolVersion> protocolVer = (ProtocolVersion*)ver;
    protocolVer->mProtocol = mProtocol;
    protocolVer->mMinor = mMinor;
    protocolVer->mMajor = mMajor;
    return NOERROR;
}

ECode ProtocolVersion::Init(
    /* [in] */ const String& protocol,
    /* [in] */ Int32 major,
    /* [in] */ Int32 minor)
{
    if (protocol == NULL) {
        Logger::E("ProtocolVersion", "Protocol name must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (major < 0) {
        Logger::E("ProtocolVersion", "Protocol major version number must not be negative.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (minor < 0) {
        Logger::E("ProtocolVersion", "Protocol minor version number may not be negative.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mProtocol = protocol;
    mMajor = major;
    mMinor = minor;
    return NOERROR;
}

} // namespace Http
} // namespace Apache
} // namespace Org
