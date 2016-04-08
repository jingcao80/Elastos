/*
 * Copyright 2008, The Android Open Source Project
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
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "PluginObject.h"
#include <stdio.h>

#ifndef HelloPlugin__DEFINED
#define HelloPlugin__DEFINED

class HelloPlugin : public SubPlugin {
public:
    HelloPlugin(NPP inst);
    virtual ~HelloPlugin();
    virtual bool supportsDrawingModel(ANPDrawingModel);
    virtual int16_t handleEvent(const ANPEvent* evt);
private:
    void draw(ANPCanvas*);
    void drawPlugin(const ANPBitmap& bitmap, const ANPRectI& clip);

    void handleTouch(int x, int y);
    void invalActiveRect();
    ANPPaint* getPaint(ANPRectF*);
    ANPRectF* validTouch(int x, int y);

    ANPPaint*   m_paintRect;
    //ANPPaint*   m_paintActiveRect;
    ANPPaint*   m_paintR;
    ANPPaint*   m_paintG;
    ANPPaint*   m_paintB;

    ANPPaint*   m_paintTrack;
    ANPRectF    m_trackRect;
    ANPRectF    m_helloRect;
    ANPRectF    m_RRect;
    ANPRectF    m_GRect;
    ANPRectF    m_BRect;

    ANPPaint*   m_paintText;

    bool        m_activeTouch;
    ANPRectF*   m_activeTouchRect;
    ANPRectF*   m_activeRect;
};

#endif // HelloPlugin__DEFINED
