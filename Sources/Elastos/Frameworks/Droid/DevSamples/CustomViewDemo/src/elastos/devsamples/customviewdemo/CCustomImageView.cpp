#include "CCustomImageView.h"

namespace Elastos {
namespace DevSamples {
namespace CustomViewDemo {

CAR_INTERFACE_IMPL(CCustomImageView, ImageView, ICustomImageView)

CAR_OBJECT_IMPL(CCustomImageView)

ECode CCustomImageView::SetImageName(
    /* [in] */ const String& name)
{
    mImageName = name;
    return NOERROR;
}

ECode CCustomImageView::GetImageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mImageName;
    return NOERROR;
}

}// namespace CustomViewDemo
}// namespace DevSamples
}// namespace Elastos