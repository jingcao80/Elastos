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

#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Opengl.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.View.h>
#include "elastos/droid/server/accessibility/DisplayAdjustmentUtils.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/Math.h>
#include <elastos/droid/utility/MathUtils.h>
#include <binder/Parcel.h>
#include <binder/IServiceManager.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Opengl::CMatrix;
using Elastos::Droid::Opengl::IMatrix;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

const String DisplayAdjustmentUtils::TAG("DisplayAdjustmentUtils");

static AutoPtr< ArrayOf<Float> > InitGRAYSCALE_MATRIX()
{
    AutoPtr< ArrayOf<Float> > args = ArrayOf<Float>::Alloc(16);
    (*args)[0] = .2126f;
    (*args)[1] = .2126f;
    (*args)[2] = .2126f;
    (*args)[3] = 0;
    (*args)[4] = .7152f;
    (*args)[5] = .7152f;
    (*args)[6] = .7152f;
    (*args)[7] = 0;
    (*args)[8] = .0722f;
    (*args)[9] = .0722f;
    (*args)[10] = .0722f;
    (*args)[11] = 0;
    (*args)[12] = 0;
    (*args)[13] = 0;
    (*args)[14] = 0;
    (*args)[15] = 1;
    return args;
}

const AutoPtr< ArrayOf<Float> > DisplayAdjustmentUtils::GRAYSCALE_MATRIX = InitGRAYSCALE_MATRIX();

static AutoPtr< ArrayOf<Float> > InitINVERSION_MATRIX_VALUE_ONLY()
{
    AutoPtr< ArrayOf<Float> > args = ArrayOf<Float>::Alloc(16);
    (*args)[0] = 0;
    (*args)[1] = -.5f;
    (*args)[2] = -.5f;
    (*args)[3] = 0;
    (*args)[4] = -.5f;
    (*args)[5] = 0;
    (*args)[6] = -.5f;
    (*args)[7] = 0;
    (*args)[8] = -.5f;
    (*args)[9] = -.5f;
    (*args)[10] = 0;
    (*args)[11] = 0;
    (*args)[12] = 1;
    (*args)[13] = 1;
    (*args)[14] = 1;
    (*args)[15] = 1;
    return args;
}

const AutoPtr< ArrayOf<Float> > DisplayAdjustmentUtils::INVERSION_MATRIX_VALUE_ONLY = InitINVERSION_MATRIX_VALUE_ONLY();

const Int32 DisplayAdjustmentUtils::DEFAULT_DISPLAY_DALTONIZER = IAccessibilityManager::DALTONIZER_CORRECT_DEUTERANOMALY;

Boolean DisplayAdjustmentUtils::HasAdjustments(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);

    Int32 value;
    secure->GetInt32ForUser(cr,
            ISettingsSecure::ACCESSIBILITY_DISPLAY_INVERSION_ENABLED, 0,
            userId, &value);
    if (value != 0) {
        return TRUE;
    }

    secure->GetInt32ForUser(cr,
            ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, 0,
            userId, &value);
    if (value != 0) {
        return TRUE;
    }

    return FALSE;
}

ECode DisplayAdjustmentUtils::ApplyAdjustments(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr< ArrayOf<Float> > colorMatrix;

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);

    Int32 value;
    secure->GetInt32ForUser(cr,
            ISettingsSecure::ACCESSIBILITY_DISPLAY_INVERSION_ENABLED, 0,
            userId, &value);
    if (value != 0) {
        colorMatrix = Multiply(colorMatrix, INVERSION_MATRIX_VALUE_ONLY);
    }

    secure->GetInt32ForUser(cr,
            ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, 0,
            userId, &value);
    if (value != 0) {
        Int32 daltonizerMode;
        secure->GetInt32ForUser(cr,
            ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER,
            DEFAULT_DISPLAY_DALTONIZER, userId, &daltonizerMode);
        // Monochromacy isn't supported by the native Daltonizer.
        if (daltonizerMode == IAccessibilityManager::DALTONIZER_SIMULATE_MONOCHROMACY) {
            colorMatrix = Multiply(colorMatrix, GRAYSCALE_MATRIX);
            SetDaltonizerMode(IAccessibilityManager::DALTONIZER_DISABLED);
        }
        else {
            SetDaltonizerMode(daltonizerMode);
        }
    }
    else {
        SetDaltonizerMode(IAccessibilityManager::DALTONIZER_DISABLED);
    }

    return SetColorTransform(colorMatrix);
}

AutoPtr< ArrayOf<Float> > DisplayAdjustmentUtils::Multiply(
    /* [in] */ ArrayOf<Float>* matrix,
    /* [in] */ ArrayOf<Float>* other)
{
    if (matrix == NULL) {
        return other;
    }
    AutoPtr< ArrayOf<Float> > result = ArrayOf<Float>::Alloc(16);

    AutoPtr<IMatrix> mat;
    CMatrix::AcquireSingleton((IMatrix**)&mat);

    mat->MultiplyMM(result, 0, matrix, 0, other, 0);
    return result;
}

ECode DisplayAdjustmentUtils::SetDaltonizerMode(
    /* [in] */ Int32 mode)
{
    // try {
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    android::sp<android::IBinder> surfaceFlinger = sm->getService(android::String16("SurfaceFlinger"));

    if (surfaceFlinger != NULL) {
        android::Parcel data;
        data.writeInterfaceToken(android::String16("android.ui.ISurfaceComposer"));
        data.writeInt32(mode);
        surfaceFlinger->transact(1014, data, NULL, 0);
    }
    // } catch (RemoteException ex) {
    //     Slog.e(LOG_TAG, "Failed to set Daltonizer mode", ex);
    // }
    return NOERROR;
}

ECode DisplayAdjustmentUtils::SetColorTransform(
    /* [in] */ ArrayOf<Float>* m)
{
    // try {
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    android::sp<android::IBinder> surfaceFlinger = sm->getService(android::String16("SurfaceFlinger"));

    if (surfaceFlinger != NULL) {
        android::Parcel data;
        data.writeInterfaceToken(android::String16("android.ui.ISurfaceComposer"));
        if (m != NULL) {
            data.writeInt32(1);
            for (Int32 i = 0; i < 16; i++) {
                data.writeFloat((*m)[i]);
            }
        }
        else {
            data.writeInt32(0);
        }
        surfaceFlinger->transact(1015, data, NULL, 0);
    }
    // } catch (RemoteException ex) {
    //     Slog.e(LOG_TAG, "Failed to set color transform", ex);
    // }
    return NOERROR;
}

} // Accessibility
} // Server
} // Droid
} // Elastos
