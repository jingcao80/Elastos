/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.cat;


/**
 * Class for representing text attributes for SIM Toolkit.
 *
 * {@hide}
 */
public class TextAttribute {
    public Int32 start;
    public Int32 length;
    public TextAlignment align;
    public FontSize size;
    public Boolean bold;
    public Boolean italic;
    public Boolean underlined;
    public Boolean strikeThrough;
    public TextColor color;

    public TextAttribute(Int32 start, Int32 length, TextAlignment align,
            FontSize size, Boolean bold, Boolean italic, Boolean underlined,
            Boolean strikeThrough, TextColor color) {
        this.start = start;
        this.length = length;
        this.align = align;
        this.size = size;
        this.bold = bold;
        this.italic = italic;
        this.underlined = underlined;
        this.strikeThrough = strikeThrough;
        this.color = color;
    }
}
