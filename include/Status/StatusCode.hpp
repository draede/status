/* 
 * StatusCode type
 *
 * https://github.com/draede/status
 * 
 * Copyright (C) 2023 draede
 *
 * Released under the MIT License.
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
 */ 

#pragma once


#include <cstdint>


namespace Status
{

using StatusCode = uint32_t;

static inline const StatusCode   OK                      = 0;
static inline const StatusCode   FileNotFound            = 1;
static inline const StatusCode   FunctionNotFound        = 2;
static inline const StatusCode   NotFound                = 3;
static inline const StatusCode   EndOfFile               = 4;
static inline const StatusCode   ParseFailed             = 5;
static inline const StatusCode   TooSmall                = 6;
static inline const StatusCode   TooBig                  = 7;
static inline const StatusCode   TooFewItems             = 8;
static inline const StatusCode   TooManyItems            = 9;
static inline const StatusCode   OutOfBounds             = 10;
static inline const StatusCode   WriteFailed             = 11;
static inline const StatusCode   ReadFailed              = 12;
static inline const StatusCode   OpenFailed              = 13;
static inline const StatusCode   CreateFailed            = 14;
static inline const StatusCode   CloseFailed             = 15;
static inline const StatusCode   InvalidArg              = 16;
static inline const StatusCode   InvalidCall             = 17;
static inline const StatusCode   NotInitialized          = 18;
static inline const StatusCode   NotSupported            = 19;
static inline const StatusCode   NotImplemented          = 20;
static inline const StatusCode   ConversionFailed        = 21;
static inline const StatusCode   MemAllocFailed          = 22;
static inline const StatusCode   OperationFailed         = 23;
static inline const StatusCode   SetPos                  = 24;
static inline const StatusCode   GetPos                  = 25;
static inline const StatusCode   SetSize                 = 26;
static inline const StatusCode   GetSize                 = 27;
static inline const StatusCode   Busy                    = 28;
static inline const StatusCode   Cancelled               = 29;
static inline const StatusCode   Denied                  = 30;
static inline const StatusCode   NoMoreItems             = 31;
static inline const StatusCode   NoMoreData              = 32;
static inline const StatusCode   Duplicate               = 33;

}//namespace Status
