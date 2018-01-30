/// Copyright (c) 2012-2018 Chris Swinchatt.
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
#ifndef REDSHIFT_HAL_CPU_VENDOR_HPP
#define REDSHIFT_HAL_CPU_VENDOR_HPP

#include <redshift/kernel.hpp>

static constexpr const char* VENDOR_AMD_OLD    = "AMDisbetter!";
static constexpr const char* VENDOR_AMD_NEW    = "AuthenticAMD";
static constexpr const char* VENDOR_CENTAUR    = "CentaurHauls";
static constexpr const char* VENDOR_CYRIX      = "CyrixInstead";
static constexpr const char* VENDOR_INTEL      = "GenuineIntel";
static constexpr const char* VENDOR_TRANSMETA1 = "TransmetaCPU";
static constexpr const char* VENDOR_TRANSMETA2 = "GenuineTMx86";
static constexpr const char* VENDOR_NSC        = "Geode by NSC";
static constexpr const char* VENDOR_NEXGEN     = "NexGenDriven";
static constexpr const char* VENDOR_RISE       = "RiseRiseRise";
static constexpr const char* VENDOR_SIS        = "SiS SiS SiS ";
static constexpr const char* VENDOR_UMC        = "UMC UMC UMC ";
static constexpr const char* VENDOR_VIA        = "VIA VIA VIA ";

#endif // ! REDSHIFT_HAL_CPU_VENDOR_HPP
