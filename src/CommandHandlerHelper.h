/****
 * @file CommandHandlerHelper.h
 * @version 1.0.0
 * @date September 27, 2023
 * 
 * This file is a portion of the package CommandLine. See CommandLine.h for details.
 * 
 *****
 * 
 * CommandHandlerHelper V1.0.0, October 2023
 * Copyright (C) 2020 - 2023 D.L. Ehnebuske
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 * 
 ****/
#pragma once
#ifndef Arduino_h
#include <Arduino.h>
#endif
#define CommandHandlerHelper_h

class CommandHandlerHelper {
public:
        /**
        * 
        * @brief    Return, as a String, the specified "word" from the text the user has input. 
        * 
        * @details  A "word" is a white-space-bounded sequence of characters. The words are 
        *           numbered starting with 0. Word 0 is the command. If the user has not input the 
        *           specified word, an empty String is returned.
        * 
        * @param    int16_t     The number of the word to be returned. Defaults to 0.
        * 
        **/
        virtual String getWord(uint8_t ix = 0) = 0;

        /**
        * 
        * @brief    Return, as a String, the trimmed sequence of characters the user has input.
        * 
        **/
        virtual String getCommandLine() = 0;

};