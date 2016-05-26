
#ifndef __Elastos_DevSamples_ApplicationDemo_CApplicationOne_H__
#define __Elastos_DevSamples_ApplicationDemo_CApplicationOne_H__

#include <elastos/droid/app/Application.h>
#include "_Elastos_DevSamples_ApplicationDemo_CApplicationOne.h"

using Elastos::Droid::App::Application;

namespace Elastos {
namespace DevSamples {
namespace ApplicationDemo {

CarClass(CApplicationOne)
    , public Application
    , public IApplicationOne
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    constructor();

    CARAPI OnCreate();

    CARAPI SetValue(
        /* [in] */ const String& value);

    CARAPI GetValue(
        /* [out] */ String* value);

private:
    String mValue;
};

} // namespace ApplicationDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_ApplicationDemo_CApplicationOne_H__
