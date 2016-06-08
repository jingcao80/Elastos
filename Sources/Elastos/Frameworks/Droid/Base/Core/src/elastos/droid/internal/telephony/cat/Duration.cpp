
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/Duration.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                   Duration::InnerParcelableCreator
////=====================================================================
//Duration::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ Duration* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode Duration::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ Duration** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Duration(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode Duration::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ Duration[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Duration[size];
//    assert(0);
//    return NOERROR;
//}

//=====================================================================
//                               Duration
//=====================================================================
//const AutoPtr<IParcelable> AutoPtr< ::Creator<Duration> > Duration::CREATOR = new InnerParcelableCreator(this);


Duration::Duration()
{
}

CARAPI Duration::constructor(
    /* [in] */ Int32 timeInterval,
    /* [in] */ IDurationTimeUnit timeUnit)
{
    // ==================before translated======================
    // this.timeInterval = timeInterval;
    // this.timeUnit = timeUnit;
    return NOERROR;
}

ECode Duration::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    // ==================before translated======================
    // dest.writeInt(timeInterval);
    // dest.writeInt(timeUnit.ordinal());
    assert(0);
    return NOERROR;
}

ECode Duration::ReadFromParcel(
    /* [in] */ IParcel* dest)
{
    constructor(dest);
    return NOERROR;
}

//ECode Duration::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

CARAPI Duration::constructor(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // timeInterval = in.readInt();
    // timeUnit = TimeUnit.values()[in.readInt()];
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
