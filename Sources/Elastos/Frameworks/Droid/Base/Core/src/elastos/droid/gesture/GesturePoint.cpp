
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/gesture/GesturePoint.h"
#include "elastos/droid/gesture/CGesturePoint.h"

using Elastos::IO::IDataInput;

namespace Elastos {
namespace Droid {
namespace Gesture {

CAR_INTERFACE_IMPL_2(GesturePoint, Object, IGesturePoint, IGesturePointHelper);

GesturePoint::GesturePoint()
{}

GesturePoint::~GesturePoint()
{}

ECode GesturePoint::constructor(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int64 t)
{
    mX = x;
    mY = y;
    mTimestamp = t;
    return NOERROR;
}

ECode GesturePoint::GetX(
    /* [out] */ Float *x)
{
    VALIDATE_NOT_NULL(x);
    *x= mX;
    return NOERROR;
}

ECode GesturePoint::GetY(
    /* [out] */ Float *y)
{
    VALIDATE_NOT_NULL(y);
    *y = mY;
    return NOERROR;
}

ECode GesturePoint::GetTimestamp(
    /* [out] */ Int64 *timestamp)
{
    VALIDATE_NOT_NULL(timestamp);
    *timestamp = mTimestamp;
    return NOERROR;
}

AutoPtr<IGesturePoint> GesturePoint::Deserialize(
    /* [in] */ IDataInputStream *in)
{
    AutoPtr<IGesturePoint> instance;

    // Read X and Y
    Float x = 0;
    IDataInput::Probe(in)->ReadFloat(&x);
    Float y = 0;
    IDataInput::Probe(in)->ReadFloat(&y);
    // Read timestamp
    Int64 timeStamp = 0;
    IDataInput::Probe(in)->ReadInt64(&timeStamp);

    ECode ec = CGesturePoint::New(x, y, timeStamp, (IGesturePoint **)&instance);

    if (FAILED(ec))
        return  NULL;

    REFCOUNT_ADD(instance);
    return instance;
}

ECode GesturePoint::Deserialize(
    /* [in] */ IDataInputStream *in,
    /* [out] */ IGesturePoint **instance)
{
    AutoPtr<IGesturePoint> inst;

    VALIDATE_NOT_NULL(instance);
    inst = Deserialize(in);
    *instance = inst;

    if (*instance != NULL)
        return NOERROR;

    return E_OUT_OF_MEMORY_ERROR;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
