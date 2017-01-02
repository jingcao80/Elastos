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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/content/app/ChromiumLinkerParams.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace App {


const String ChromiumLinkerParams::EXTRA_LINKER_PARAMS_BASE_LOAD_ADDRESS(
    "org.chromium.content.common.linker_params.base_load_address");

const String ChromiumLinkerParams::EXTRA_LINKER_PARAMS_WAIT_FOR_SHARED_RELRO(
    "org.chromium.content.common.linker_params.wait_for_shared_relro");

const String ChromiumLinkerParams::EXTRA_LINKER_PARAMS_TEST_RUNNER_CLASS_NAME(
    "org.chromium.content.common.linker_params.test_runner_class_name");

ChromiumLinkerParams::ChromiumLinkerParams(
    /* [in] */ Int64 baseLoadAddress,
    /* [in] */ Boolean waitForSharedRelro,
    /* [in] */ const String& testRunnerClassName)
    : mBaseLoadAddress(baseLoadAddress)
    , mWaitForSharedRelro(waitForSharedRelro)
    , mTestRunnerClassName(testRunnerClassName)
{
}

/**
 * Use this constructor to recreate a LinkerParams instance from an Intent.
 * @param intent An Intent, its content must have been populated by a previous
 * call to addIntentExtras().
 */
ChromiumLinkerParams::ChromiumLinkerParams(
    /* [in] */ IIntent* intent)
{
    intent->GetInt64Extra(EXTRA_LINKER_PARAMS_BASE_LOAD_ADDRESS, 0, &mBaseLoadAddress);
    intent->GetBooleanExtra(
            EXTRA_LINKER_PARAMS_WAIT_FOR_SHARED_RELRO, FALSE, &mWaitForSharedRelro);
    intent->GetStringExtra(
            EXTRA_LINKER_PARAMS_TEST_RUNNER_CLASS_NAME, &mTestRunnerClassName);
}

/**
 * Ensure this LinkerParams instance is sent to a service process by adding
 * it to an intent's extras.
 * @param intent An Intent use to start or connect to the child service process.
 */
void ChromiumLinkerParams::AddIntentExtras(
    /* [in] */ IIntent* intent)
{
    intent->PutExtra(EXTRA_LINKER_PARAMS_BASE_LOAD_ADDRESS, mBaseLoadAddress);
    intent->PutExtra(EXTRA_LINKER_PARAMS_WAIT_FOR_SHARED_RELRO, mWaitForSharedRelro);
    intent->PutExtra(EXTRA_LINKER_PARAMS_TEST_RUNNER_CLASS_NAME, mTestRunnerClassName);
}

// For debugging traces only.
String ChromiumLinkerParams::ToString()
{
    String str1;
    str1.AppendFormat("LinkerParams(baseLoadAddress:0x%x, waitForSharedRelro:%s, ",
            mBaseLoadAddress, mWaitForSharedRelro ? "true" : "false");
    String str2;
    str2.AppendFormat("testRunnerClassName:%s", mTestRunnerClassName.string());
    return str1 + str2;
}

} // namespace App
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
