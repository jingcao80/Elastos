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

#include "elastos/droid/media/audiofx/CAudioEffect.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_OBJECT_IMPL(CAudioEffect)

ECode CAudioEffect::constructor(
    /* [in] */ IUUID* type,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 priority,
    /* [in] */ Int32 audioSession)
{
    return AudioEffect::constructor(type, uuid, priority, audioSession);
}

ECode CAudioEffect::ReleaseResources()
{
    return AudioEffect::ReleaseResources();
}

ECode CAudioEffect::GetDescriptor(
    /* [out] */ IAudioEffectDescriptor** descriptor)
{
    return AudioEffect::GetDescriptor(descriptor);
}

ECode CAudioEffect::SetEnabled(
    /* [in]  */ Boolean enabled,
    /* [out] */ Int32* result)
{
    return AudioEffect::SetEnabled(enabled, result);
}

ECode CAudioEffect::SetParameter(
    /* [in] */ ArrayOf<Byte>* param,
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* result)
{
    return AudioEffect::SetParameter(param, value, result);
}

ECode CAudioEffect::SetParameter(
    /* [in] */ Int32 param,
    /* [in] */ Int32 value,
    /* [out] */ Int32* result)
{
    return AudioEffect::SetParameter(param, value, result);
}

ECode CAudioEffect::SetParameter(
    /* [in] */ Int32 param,
    /* [in] */ Int16 value,
    /* [out] */ Int32* result)
{
    return AudioEffect::SetParameter(param, value, result);
}

ECode CAudioEffect::SetParameter(
    /* [in] */ Int32 param,
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* result)
{
    return AudioEffect::SetParameter(param, value, result);
}

ECode CAudioEffect::SetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [in] */ ArrayOf<Int32>* value,
    /* [out] */ Int32* result)
{
    return AudioEffect::SetParameter(param, value, result);
}

ECode CAudioEffect::SetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [in] */ ArrayOf<Int16>* value,
    /* [out] */ Int32* result)
{
    return AudioEffect::SetParameter(param, value, result);
}

ECode CAudioEffect::SetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* result)
{
    return AudioEffect::SetParameter(param, value, result);
}

ECode CAudioEffect::GetParameter(
    /* [in] */ ArrayOf<Byte>* param,
    /* [out] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* status)
{
    return AudioEffect::GetParameter(param, value, status);
}

ECode CAudioEffect::GetParameter(
    /* [in] */ const Int32 param,
    /* [out] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* status)
{
    return AudioEffect::GetParameter(param, value, status);
}

ECode CAudioEffect::GetParameter(
    /* [in] */ Int32 param,
    /* [out] */ ArrayOf<Int32>* value,
    /* [out] */ Int32* status)
{
    return AudioEffect::GetParameter(param, value, status);
}

ECode CAudioEffect::GetParameter(
    /* [in] */ Int32 param,
    /* [out] */ ArrayOf<Int16>* value,
    /* [out] */ Int32* status)
{
    return AudioEffect::GetParameter(param, value, status);
}

ECode CAudioEffect::GetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [out] */ ArrayOf<Int32>* value,
    /* [out] */ Int32* status)
{
    return AudioEffect::GetParameter(param, value, status);
}

ECode CAudioEffect::GetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [out] */ ArrayOf<Int16>* value,
    /* [out] */ Int32* status)
{
    return AudioEffect::GetParameter(param, value, status);
}

ECode CAudioEffect::GetParameter(
    /* [in] */ ArrayOf<Int32>* param,
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Int32* status)
{
    return AudioEffect::GetParameter(param, value, status);
}

ECode CAudioEffect::Command(
    /* [in] */ Int32 cmdCode,
    /* [in] */ ArrayOf<Byte>* command,
    /* [in] */ ArrayOf<Byte>* reply,
    /* [out] */ Int32* result)
{
    return AudioEffect::Command(cmdCode, command, reply, result);
}

ECode CAudioEffect::GetId(
    /* [out] */ Int32* Id)
{
    return AudioEffect::GetId(Id);
}

ECode CAudioEffect::GetEnabled(
    /* [out] */ Boolean* enabled)
{
    return AudioEffect::GetEnabled(enabled);
}

ECode CAudioEffect::HasControl(
    /* [out] */ Boolean* control)
{
    return AudioEffect::HasControl(control);
}

ECode CAudioEffect::SetEnableStatusListener(
    /* [in] */ IAudioEffectOnEnableStatusChangeListener* listener)
{
    return AudioEffect::SetEnableStatusListener(listener);
}

ECode CAudioEffect::SetControlStatusListener(
    /* [in] */ IAudioEffectOnControlStatusChangeListener* listener)
{
    return AudioEffect::SetControlStatusListener(listener);
}

ECode CAudioEffect::SetParameterListener(
    /* [in] */ IAudioEffectOnParameterChangeListener* listener)
{
    return AudioEffect::SetParameterListener(listener);
}

ECode CAudioEffect::CheckState(
    /* [in] */ const String& methodName)
{
    return AudioEffect::CheckState(methodName);
}

ECode CAudioEffect::CheckStatus(
    /* [in] */ Int32 status)
{
    return AudioEffect::CheckStatus(status);
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
