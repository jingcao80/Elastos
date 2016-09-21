// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define INPUT_TYPE_NAMES_HIDE_GLOBALS 1
#endif

#include "InputTypeNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace InputTypeNames {

using namespace WTF;

DEFINE_GLOBAL(AtomicString, button)
DEFINE_GLOBAL(AtomicString, checkbox)
DEFINE_GLOBAL(AtomicString, color)
DEFINE_GLOBAL(AtomicString, date)
DEFINE_GLOBAL(AtomicString, datetime)
DEFINE_GLOBAL(AtomicString, datetime_local)
DEFINE_GLOBAL(AtomicString, email)
DEFINE_GLOBAL(AtomicString, file)
DEFINE_GLOBAL(AtomicString, hidden)
DEFINE_GLOBAL(AtomicString, image)
DEFINE_GLOBAL(AtomicString, month)
DEFINE_GLOBAL(AtomicString, number)
DEFINE_GLOBAL(AtomicString, password)
DEFINE_GLOBAL(AtomicString, radio)
DEFINE_GLOBAL(AtomicString, range)
DEFINE_GLOBAL(AtomicString, reset)
DEFINE_GLOBAL(AtomicString, search)
DEFINE_GLOBAL(AtomicString, submit)
DEFINE_GLOBAL(AtomicString, tel)
DEFINE_GLOBAL(AtomicString, text)
DEFINE_GLOBAL(AtomicString, time)
DEFINE_GLOBAL(AtomicString, url)
DEFINE_GLOBAL(AtomicString, week)

void init()
{
    StringImpl* buttonImpl = StringImpl::createStatic("button", 6, 679691);
    StringImpl* checkboxImpl = StringImpl::createStatic("checkbox", 8, 2704933);
    StringImpl* colorImpl = StringImpl::createStatic("color", 5, 2734929);
    StringImpl* dateImpl = StringImpl::createStatic("date", 4, 11043522);
    StringImpl* datetimeImpl = StringImpl::createStatic("datetime", 8, 12167257);
    StringImpl* datetime_localImpl = StringImpl::createStatic("datetime-local", 14, 5474839);
    StringImpl* emailImpl = StringImpl::createStatic("email", 5, 13948917);
    StringImpl* fileImpl = StringImpl::createStatic("file", 4, 14168904);
    StringImpl* hiddenImpl = StringImpl::createStatic("hidden", 6, 12930326);
    StringImpl* imageImpl = StringImpl::createStatic("image", 5, 10287573);
    StringImpl* monthImpl = StringImpl::createStatic("month", 5, 12226123);
    StringImpl* numberImpl = StringImpl::createStatic("number", 6, 15609029);
    StringImpl* passwordImpl = StringImpl::createStatic("password", 8, 8168283);
    StringImpl* radioImpl = StringImpl::createStatic("radio", 5, 6579434);
    StringImpl* rangeImpl = StringImpl::createStatic("range", 5, 2862653);
    StringImpl* resetImpl = StringImpl::createStatic("reset", 5, 13674204);
    StringImpl* searchImpl = StringImpl::createStatic("search", 6, 6906057);
    StringImpl* submitImpl = StringImpl::createStatic("submit", 6, 12328646);
    StringImpl* telImpl = StringImpl::createStatic("tel", 3, 1681717);
    StringImpl* textImpl = StringImpl::createStatic("text", 4, 2784654);
    StringImpl* timeImpl = StringImpl::createStatic("time", 4, 3114886);
    StringImpl* urlImpl = StringImpl::createStatic("url", 3, 10560150);
    StringImpl* weekImpl = StringImpl::createStatic("week", 4, 14375271);

    new ((void*)&button) AtomicString(buttonImpl);
    new ((void*)&checkbox) AtomicString(checkboxImpl);
    new ((void*)&color) AtomicString(colorImpl);
    new ((void*)&date) AtomicString(dateImpl);
    new ((void*)&datetime) AtomicString(datetimeImpl);
    new ((void*)&datetime_local) AtomicString(datetime_localImpl);
    new ((void*)&email) AtomicString(emailImpl);
    new ((void*)&file) AtomicString(fileImpl);
    new ((void*)&hidden) AtomicString(hiddenImpl);
    new ((void*)&image) AtomicString(imageImpl);
    new ((void*)&month) AtomicString(monthImpl);
    new ((void*)&number) AtomicString(numberImpl);
    new ((void*)&password) AtomicString(passwordImpl);
    new ((void*)&radio) AtomicString(radioImpl);
    new ((void*)&range) AtomicString(rangeImpl);
    new ((void*)&reset) AtomicString(resetImpl);
    new ((void*)&search) AtomicString(searchImpl);
    new ((void*)&submit) AtomicString(submitImpl);
    new ((void*)&tel) AtomicString(telImpl);
    new ((void*)&text) AtomicString(textImpl);
    new ((void*)&time) AtomicString(timeImpl);
    new ((void*)&url) AtomicString(urlImpl);
    new ((void*)&week) AtomicString(weekImpl);
}

} // InputTypeNames
} // WebCore
