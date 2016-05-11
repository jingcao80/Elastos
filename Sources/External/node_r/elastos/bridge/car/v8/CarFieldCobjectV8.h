/*
 * Copyright 2010, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CarFieldJobjectV8_h
#define CarFieldJobjectV8_h

#include <elastos.h>

#if ENABLE(CAR_BRIDGE)

#include "CarFieldV8.h"
#include "CarString.h"
#include "CobjectWrapper.h"

namespace JSC {
namespace Bindings {

class CarFieldCobject : public CarField {
public:
    CarFieldCobject(CarString inName, int inAttr);

    ~CarFieldCobject();

    // CarField implementation
    virtual WTF::String name() const { return mName.impl(); }
    virtual const char* typeClassName() const { return mTypeClassName.utf8(); }
    virtual CarDataType type() const { return mType; }

    virtual int attr() const { return mAttr; }
    virtual void setAttr(int attr) { mAttr = attr; }
    virtual FieldMethodListMap& fieldMethodList() { return mFieldMethodList; }

private:
    CarString mName;
    CarString mTypeClassName;
    CarDataType mType;
    RefPtr<CobjectWrapper> mField;

    int mAttr;
    FieldMethodListMap mFieldMethodList;
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif // CarFieldJobjectV8_h
