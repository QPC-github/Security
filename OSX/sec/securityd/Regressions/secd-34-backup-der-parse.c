/*
 * Copyright (c) 2008-2010,2013 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */


#include <CoreFoundation/CoreFoundation.h>
#include <Security/SecBase.h>
#include <Security/SecItem.h>
#include <Security/SecItemPriv.h>
#include <Security/SecInternal.h>
#include <utilities/SecFileLocations.h>
#include <utilities/SecCFWrappers.h>
#include <Security/SecItemBackup.h>

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "secd_regressions.h"

#include <securityd/SecItemServer.h>

#include "SecdTestKeychainUtilities.h"

/* Keybag and exported plist data. */
static const unsigned char backup_data[] = {
    0x30, 0x82, 0x06, 0xf4, 0x04, 0x82, 0x06, 0xf0, 0x56, 0x45, 0x52, 0x53,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03, 0x54, 0x59, 0x50, 0x45,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x55, 0x55, 0x49, 0x44,
    0x00, 0x00, 0x00, 0x10, 0x60, 0x5e, 0x34, 0x57, 0xbe, 0xa0, 0x48, 0xd6,
    0x9b, 0x22, 0xfa, 0x80, 0xff, 0x3a, 0xe9, 0x9b, 0x48, 0x4d, 0x43, 0x4b,
    0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x57, 0x52, 0x41, 0x50,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x53, 0x41, 0x4c, 0x54,
    0x00, 0x00, 0x00, 0x14, 0x50, 0x2e, 0x97, 0xc6, 0xfa, 0xed, 0x9b, 0xc0,
    0x6f, 0x09, 0x2a, 0xca, 0x29, 0x38, 0x47, 0x34, 0x9f, 0xc0, 0x29, 0x7e,
    0x49, 0x54, 0x45, 0x52, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x27, 0x10,
    0x55, 0x55, 0x49, 0x44, 0x00, 0x00, 0x00, 0x10, 0x6f, 0x6a, 0x84, 0xd0,
    0x73, 0x27, 0x4a, 0xbc, 0xb3, 0x28, 0xb4, 0xa4, 0xc5, 0x36, 0x4a, 0xdf,
    0x43, 0x4c, 0x41, 0x53, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0b,
    0x57, 0x52, 0x41, 0x50, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03,
    0x4b, 0x54, 0x59, 0x50, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01,
    0x57, 0x50, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x28, 0x69, 0xa7, 0xb1, 0xcd,
    0x86, 0x2c, 0xc4, 0xc4, 0x81, 0x52, 0xb3, 0xe7, 0xf9, 0x78, 0xad, 0xec,
    0xaf, 0xc4, 0xe1, 0x71, 0xc9, 0x98, 0xca, 0xb4, 0xa8, 0x86, 0xdc, 0xe9,
    0xcf, 0x48, 0xce, 0x82, 0xa4, 0xeb, 0xf4, 0x43, 0x6d, 0xb0, 0xee, 0x05,
    0x50, 0x42, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x20, 0xcf, 0x0b, 0x7c, 0x16,
    0xa1, 0x69, 0x16, 0x1f, 0x52, 0x5a, 0xb5, 0x16, 0xa2, 0x75, 0xe0, 0x5f,
    0xb2, 0x55, 0x53, 0xed, 0x10, 0xc6, 0xd1, 0x4d, 0x47, 0x65, 0xe6, 0xcb,
    0xd0, 0x77, 0x12, 0x56, 0x55, 0x55, 0x49, 0x44, 0x00, 0x00, 0x00, 0x10,
    0x5c, 0x1d, 0x0e, 0xde, 0x90, 0xac, 0x45, 0x1f, 0xa1, 0x10, 0x75, 0x84,
    0xf4, 0x9c, 0xe9, 0x65, 0x43, 0x4c, 0x41, 0x53, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x0a, 0x57, 0x52, 0x41, 0x50, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x03, 0x4b, 0x54, 0x59, 0x50, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x01, 0x57, 0x50, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x28,
    0xdd, 0xd8, 0xfa, 0x79, 0x6a, 0x6a, 0x63, 0xfc, 0x40, 0xf2, 0xa9, 0x1d,
    0xe9, 0xf1, 0xf6, 0xdb, 0x14, 0xc4, 0x55, 0x52, 0x80, 0x2e, 0xe3, 0x48,
    0x91, 0x53, 0x17, 0xd9, 0xb6, 0x91, 0x05, 0x42, 0x25, 0xa3, 0xb8, 0xc7,
    0x95, 0x21, 0x14, 0xb7, 0x50, 0x42, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x20,
    0x48, 0xb5, 0xd3, 0xe5, 0xdb, 0xc5, 0x3b, 0x20, 0x23, 0x35, 0x4f, 0xd4,
    0x28, 0xe5, 0x43, 0x59, 0xaf, 0x74, 0xd3, 0x40, 0xd3, 0x24, 0x7c, 0x0a,
    0xe7, 0x86, 0x9e, 0xde, 0x96, 0x6d, 0xdb, 0x3a, 0x55, 0x55, 0x49, 0x44,
    0x00, 0x00, 0x00, 0x10, 0xc7, 0x1f, 0xde, 0xb6, 0xb0, 0x49, 0x42, 0x90,
    0x90, 0x32, 0xbf, 0x39, 0x84, 0x9d, 0xb3, 0xa1, 0x43, 0x4c, 0x41, 0x53,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x09, 0x57, 0x52, 0x41, 0x50,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03, 0x4b, 0x54, 0x59, 0x50,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x57, 0x50, 0x4b, 0x59,
    0x00, 0x00, 0x00, 0x28, 0xc7, 0xbd, 0xb5, 0x0a, 0x70, 0x1a, 0xe6, 0xfb,
    0x39, 0x0a, 0x4a, 0x2d, 0x45, 0xf9, 0x5f, 0xb1, 0xc8, 0xdc, 0x88, 0x1a,
    0x26, 0xf1, 0xaa, 0x1a, 0xb5, 0xd9, 0xbf, 0xce, 0x1f, 0x7e, 0x69, 0xb2,
    0xdc, 0xbe, 0x48, 0x59, 0xea, 0xf0, 0x04, 0xba, 0x50, 0x42, 0x4b, 0x59,
    0x00, 0x00, 0x00, 0x20, 0x93, 0x26, 0xff, 0x91, 0xd0, 0x28, 0x45, 0x27,
    0x4c, 0xf0, 0x83, 0xa4, 0x39, 0x38, 0x30, 0xb5, 0x62, 0x18, 0xdc, 0xe2,
    0x9a, 0x50, 0xf7, 0xd4, 0x01, 0x38, 0xa0, 0x61, 0x69, 0x2b, 0x38, 0x6c,
    0x55, 0x55, 0x49, 0x44, 0x00, 0x00, 0x00, 0x10, 0xdc, 0x23, 0x9d, 0x5c,
    0xdd, 0xf6, 0x4c, 0xa0, 0xbf, 0x63, 0x6e, 0xe8, 0x40, 0xaa, 0x43, 0xbf,
    0x43, 0x4c, 0x41, 0x53, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08,
    0x57, 0x52, 0x41, 0x50, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02,
    0x4b, 0x54, 0x59, 0x50, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01,
    0x57, 0x50, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x28, 0x68, 0xf5, 0x29, 0x43,
    0x1c, 0x42, 0x59, 0x83, 0xb8, 0x0d, 0x12, 0xf6, 0x7f, 0x6d, 0x66, 0xbe,
    0xdd, 0xaf, 0x05, 0x90, 0x52, 0xe7, 0x03, 0xea, 0x8a, 0x03, 0xc2, 0x6f,
    0x9c, 0x47, 0xb1, 0xea, 0x88, 0x08, 0x19, 0x2d, 0x00, 0x3b, 0x5b, 0x79,
    0x50, 0x42, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x20, 0xb4, 0x25, 0xc5, 0xa3,
    0x85, 0xe4, 0x62, 0x53, 0x62, 0xd7, 0x0c, 0x69, 0xb9, 0x6d, 0x04, 0xe0,
    0x5a, 0xbf, 0x1a, 0x79, 0xa5, 0x48, 0x26, 0x67, 0x9e, 0x3a, 0xc8, 0x42,
    0x18, 0xca, 0x75, 0x0c, 0x55, 0x55, 0x49, 0x44, 0x00, 0x00, 0x00, 0x10,
    0xb5, 0xc8, 0x18, 0xb8, 0x8f, 0xea, 0x40, 0xe7, 0x88, 0xcd, 0xf4, 0xc5,
    0x96, 0xb8, 0x92, 0xf1, 0x43, 0x4c, 0x41, 0x53, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x07, 0x57, 0x52, 0x41, 0x50, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x02, 0x4b, 0x54, 0x59, 0x50, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x01, 0x57, 0x50, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x28,
    0xaa, 0x8a, 0x3c, 0x92, 0xea, 0x65, 0xa9, 0xdb, 0x3a, 0x36, 0x01, 0x54,
    0x2e, 0x62, 0xc6, 0xd2, 0x46, 0x75, 0x61, 0xac, 0x5d, 0xbb, 0x62, 0xaa,
    0xc3, 0xed, 0xbf, 0x14, 0x35, 0x13, 0x21, 0x2f, 0x47, 0x7d, 0x57, 0x07,
    0xf1, 0x80, 0xa4, 0xcc, 0x50, 0x42, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x20,
    0x3e, 0xb4, 0x78, 0x54, 0xd2, 0x19, 0xaf, 0xe4, 0x4c, 0x84, 0x02, 0xd0,
    0xf4, 0x50, 0xde, 0xd6, 0x44, 0xc9, 0xdf, 0x4b, 0x8d, 0x81, 0x2a, 0x2d,
    0xbb, 0xf1, 0x94, 0xd0, 0x4e, 0x57, 0xfa, 0x4e, 0x55, 0x55, 0x49, 0x44,
    0x00, 0x00, 0x00, 0x10, 0x59, 0x3c, 0xc5, 0xc5, 0xf2, 0x39, 0x4a, 0x44,
    0x90, 0xd4, 0xb1, 0x07, 0x17, 0x00, 0x64, 0x34, 0x43, 0x4c, 0x41, 0x53,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x06, 0x57, 0x52, 0x41, 0x50,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02, 0x4b, 0x54, 0x59, 0x50,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x57, 0x50, 0x4b, 0x59,
    0x00, 0x00, 0x00, 0x28, 0x79, 0xdd, 0x59, 0x01, 0x1f, 0x45, 0xe4, 0x01,
    0x32, 0xb5, 0xae, 0x5e, 0x17, 0xd7, 0x6c, 0xc6, 0x1d, 0x28, 0xf0, 0x32,
    0x4a, 0x18, 0xff, 0x30, 0x2c, 0xe4, 0xcb, 0x7c, 0x58, 0x53, 0x00, 0x8b,
    0xb9, 0xdd, 0x71, 0x5f, 0xbc, 0x66, 0x9b, 0x4c, 0x50, 0x42, 0x4b, 0x59,
    0x00, 0x00, 0x00, 0x20, 0xc3, 0x2d, 0xc7, 0x4f, 0xb1, 0x7b, 0x3c, 0x9d,
    0xcb, 0xf4, 0xd6, 0x45, 0x20, 0x35, 0xbc, 0x3a, 0x6c, 0x13, 0x1d, 0x7c,
    0x63, 0x53, 0x17, 0xc9, 0x84, 0x86, 0xbd, 0x96, 0xb0, 0xa4, 0x39, 0x03,
    0x55, 0x55, 0x49, 0x44, 0x00, 0x00, 0x00, 0x10, 0xcb, 0x2c, 0x33, 0x2e,
    0x69, 0x39, 0x43, 0xe5, 0x87, 0xbb, 0xbc, 0xd4, 0x47, 0xa4, 0x86, 0x7b,
    0x43, 0x4c, 0x41, 0x53, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05,
    0x57, 0x52, 0x41, 0x50, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03,
    0x4b, 0x54, 0x59, 0x50, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01,
    0x57, 0x50, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x28, 0x6f, 0x62, 0x11, 0xb0,
    0xdf, 0x5a, 0x39, 0x43, 0xe9, 0xc3, 0xc5, 0x0b, 0x1c, 0x57, 0x6f, 0x79,
    0x0d, 0x44, 0x9f, 0x44, 0xf2, 0x51, 0x1e, 0x68, 0xb8, 0xcf, 0x5c, 0x6d,
    0x94, 0x86, 0x86, 0x70, 0x12, 0xc1, 0x76, 0xc0, 0x18, 0xe4, 0x58, 0x1a,
    0x50, 0x42, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x20, 0x6f, 0xba, 0xbd, 0x37,
    0x58, 0x5f, 0x96, 0x4d, 0x4c, 0x92, 0xbb, 0xf0, 0x43, 0xec, 0x84, 0xc3,
    0x68, 0x75, 0xf1, 0xd2, 0x04, 0x0d, 0xda, 0x8a, 0xec, 0xac, 0x02, 0xf7,
    0xf7, 0x4d, 0xf4, 0x0e, 0x55, 0x55, 0x49, 0x44, 0x00, 0x00, 0x00, 0x10,
    0x8d, 0xa4, 0xb5, 0x6b, 0x95, 0x12, 0x46, 0xff, 0x9e, 0x1f, 0xdd, 0x2f,
    0xc0, 0x60, 0xc6, 0xd6, 0x43, 0x4c, 0x41, 0x53, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x04, 0x57, 0x52, 0x41, 0x50, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x02, 0x4b, 0x54, 0x59, 0x50, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x01, 0x57, 0x50, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x28,
    0xa8, 0xa2, 0x8d, 0xe2, 0x16, 0xd8, 0xce, 0x18, 0x2c, 0x8e, 0xa4, 0x8d,
    0x32, 0xba, 0x83, 0xdb, 0x5e, 0xc8, 0x04, 0x59, 0x72, 0xca, 0x72, 0xe5,
    0x6f, 0xcd, 0x06, 0x98, 0xde, 0x4b, 0xcf, 0xa6, 0xd1, 0xe3, 0xba, 0x9b,
    0x68, 0xa6, 0xe5, 0xa2, 0x50, 0x42, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x20,
    0x46, 0x45, 0x3a, 0xc1, 0x62, 0x14, 0xd7, 0x3f, 0x8c, 0x7a, 0x66, 0x1d,
    0x4b, 0x5c, 0x86, 0x43, 0x75, 0xe6, 0x93, 0xf1, 0x31, 0x6b, 0x20, 0x8b,
    0x18, 0x87, 0x84, 0xcc, 0x92, 0xfc, 0xb3, 0x32, 0x55, 0x55, 0x49, 0x44,
    0x00, 0x00, 0x00, 0x10, 0xd8, 0x36, 0xc5, 0xf9, 0x76, 0x4b, 0x4c, 0x2c,
    0x8c, 0xaa, 0xf4, 0x62, 0xab, 0x14, 0xd5, 0x08, 0x43, 0x4c, 0x41, 0x53,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03, 0x57, 0x52, 0x41, 0x50,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02, 0x4b, 0x54, 0x59, 0x50,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x57, 0x50, 0x4b, 0x59,
    0x00, 0x00, 0x00, 0x28, 0x0c, 0xe1, 0xb2, 0xee, 0x08, 0xdb, 0xe4, 0xee,
    0xb5, 0x49, 0xd5, 0x3a, 0xe7, 0xd9, 0xbd, 0xf6, 0xab, 0x0e, 0x1a, 0x2a,
    0x16, 0x6c, 0x82, 0x88, 0x5e, 0x39, 0xf6, 0x89, 0xcf, 0x5c, 0x69, 0x3c,
    0x67, 0x56, 0x16, 0xa1, 0x2c, 0xf4, 0x7d, 0xb9, 0x50, 0x42, 0x4b, 0x59,
    0x00, 0x00, 0x00, 0x20, 0xc1, 0x54, 0xd5, 0xb9, 0xf1, 0x16, 0xe9, 0xb2,
    0x9b, 0x79, 0x73, 0x7a, 0xb7, 0x95, 0x0a, 0xdb, 0x15, 0x20, 0x4d, 0xa7,
    0x0f, 0x5d, 0xed, 0x1c, 0x95, 0x63, 0xec, 0xf8, 0xf0, 0xf2, 0x30, 0x75,
    0x55, 0x55, 0x49, 0x44, 0x00, 0x00, 0x00, 0x10, 0x48, 0xf3, 0x98, 0x06,
    0xfe, 0xb8, 0x4e, 0xd6, 0xa9, 0xb3, 0xe1, 0xa2, 0x1f, 0xb0, 0x7b, 0xcd,
    0x43, 0x4c, 0x41, 0x53, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02,
    0x57, 0x52, 0x41, 0x50, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02,
    0x4b, 0x54, 0x59, 0x50, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01,
    0x57, 0x50, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x28, 0x01, 0x8a, 0x2c, 0x90,
    0x67, 0x3f, 0xe0, 0xf8, 0xb1, 0xbb, 0x86, 0x7f, 0xdc, 0xcd, 0xfd, 0x22,
    0x98, 0x54, 0x30, 0x75, 0x49, 0xd1, 0xe8, 0xc2, 0x85, 0x51, 0x5b, 0xae,
    0xfa, 0x3f, 0xf8, 0xfc, 0xf8, 0xa6, 0xf9, 0xa0, 0x80, 0xa5, 0x3b, 0x87,
    0x50, 0x42, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x20, 0xa6, 0x4f, 0x13, 0x8a,
    0x52, 0xe5, 0x5a, 0x65, 0x69, 0xd4, 0x70, 0xc8, 0xc7, 0xec, 0xbd, 0x27,
    0x3a, 0xcd, 0xd0, 0xb4, 0x61, 0x5d, 0xf4, 0x81, 0x67, 0xe5, 0x63, 0x77,
    0xbd, 0x4d, 0xb5, 0x0f, 0x55, 0x55, 0x49, 0x44, 0x00, 0x00, 0x00, 0x10,
    0xa9, 0x3b, 0xd2, 0x02, 0x4f, 0x72, 0x46, 0x32, 0x99, 0xa6, 0x7b, 0x62,
    0xaa, 0x11, 0xf4, 0x67, 0x43, 0x4c, 0x41, 0x53, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x01, 0x57, 0x52, 0x41, 0x50, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x02, 0x4b, 0x54, 0x59, 0x50, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x01, 0x57, 0x50, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x28,
    0x8d, 0xae, 0x30, 0xf1, 0x0f, 0x88, 0x7a, 0x8a, 0xb7, 0x34, 0xc9, 0x50,
    0x02, 0x20, 0x40, 0x3c, 0xce, 0xd2, 0xa9, 0xe7, 0x5a, 0x3a, 0xad, 0xad,
    0xac, 0xdc, 0x3a, 0xc2, 0x92, 0x9d, 0x33, 0x7a, 0x73, 0x78, 0x8a, 0x37,
    0x04, 0x2b, 0xbd, 0xf2, 0x50, 0x42, 0x4b, 0x59, 0x00, 0x00, 0x00, 0x20,
    0x72, 0x9c, 0x52, 0x44, 0xf0, 0xdd, 0xe1, 0x4a, 0x9e, 0x7b, 0x99, 0xec,
    0x64, 0x66, 0x5d, 0xde, 0xe4, 0x8d, 0x72, 0x87, 0xee, 0xd8, 0x66, 0xad,
    0x7d, 0x06, 0x0c, 0x98, 0xc9, 0x2f, 0xfb, 0x19, 0x31, 0x82, 0x03, 0x14,
    0x30, 0x0d, 0x0c, 0x05, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x0c, 0x04, 0x67,
    0x65, 0x6e, 0x70, 0x30, 0x1c, 0x0c, 0x04, 0x68, 0x61, 0x73, 0x68, 0x04,
    0x14, 0xac, 0x28, 0x52, 0x3c, 0xba, 0x05, 0xe5, 0xbd, 0x68, 0x3d, 0xd2,
    0x4c, 0xfb, 0x66, 0x80, 0xb3, 0xdf, 0xdc, 0x1f, 0x0f, 0x30, 0x82, 0x02,
    0xe3, 0x0c, 0x04, 0x64, 0x61, 0x74, 0x61, 0x04, 0x82, 0x02, 0xd9, 0x03,
    0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0xf1,
    0xa8, 0xb7, 0x9d, 0x4b, 0xb4, 0x44, 0x5b, 0xb7, 0x75, 0x4e, 0x73, 0x60,
    0xd9, 0xa9, 0x40, 0x08, 0xb0, 0xd9, 0x40, 0xd2, 0x9a, 0x01, 0xf1, 0x58,
    0x87, 0x0d, 0xb2, 0x37, 0xee, 0x3a, 0x76, 0xbe, 0x3c, 0x31, 0x6c, 0x93,
    0xa2, 0xd0, 0x8e, 0xf7, 0x5c, 0x97, 0x25, 0x69, 0xbc, 0xd1, 0x52, 0x73,
    0x32, 0xf4, 0x38, 0x13, 0xda, 0x86, 0xab, 0x6f, 0xe9, 0x6c, 0x4a, 0xeb,
    0xc4, 0x7a, 0xd8, 0x53, 0x03, 0x86, 0x48, 0x4f, 0x11, 0x1a, 0x1b, 0xa9,
    0x17, 0x21, 0xc3, 0x97, 0x12, 0xb6, 0x77, 0x03, 0xba, 0x22, 0x96, 0x0a,
    0x90, 0x01, 0x84, 0xd6, 0x5e, 0x2e, 0x3e, 0x8e, 0x50, 0xb7, 0x48, 0x38,
    0xb6, 0x7f, 0x77, 0x08, 0xf1, 0x2e, 0x5f, 0x3e, 0xda, 0x67, 0xff, 0xa4,
    0x82, 0x89, 0xad, 0xfa, 0x46, 0x38, 0x23, 0x35, 0xb3, 0x8f, 0x5d, 0x31,
    0x2e, 0x39, 0x08, 0x0a, 0x90, 0xe5, 0x5b, 0xf0, 0xc6, 0xb2, 0xa9, 0x8a,
    0x6f, 0x31, 0x5a, 0xbd, 0x6f, 0x74, 0x17, 0x67, 0xb4, 0x0c, 0x4b, 0xd8,
    0xf6, 0x4d, 0x25, 0xca, 0x06, 0xc0, 0x95, 0xf5, 0x70, 0x16, 0xb9, 0x49,
    0xbd, 0xaa, 0x73, 0xb5, 0xed, 0x4d, 0xb1, 0x92, 0x78, 0x28, 0xf7, 0x0d,
    0xbb, 0x73, 0x3d, 0x24, 0x78, 0x19, 0x55, 0xbb, 0x9b, 0x4e, 0xe6, 0x3b,
    0x84, 0x69, 0xd5, 0xbd, 0x3a, 0xcb, 0x8c, 0x70, 0x28, 0x10, 0x8c, 0x5c,
    0xfe, 0xb1, 0x0a, 0x2a, 0x7f, 0xb9, 0xbb, 0x32, 0xab, 0xc5, 0xf6, 0xfd,
    0x66, 0xb7, 0xf7, 0xce, 0x63, 0xcc, 0x05, 0x25, 0xd0, 0x0b, 0x37, 0x94,
    0xbc, 0x1f, 0x0d, 0x80, 0x6b, 0x6c, 0xfd, 0x5a, 0x62, 0xd6, 0xca, 0x3e,
    0x9a, 0x5f, 0x23, 0xfd, 0xc6, 0x9b, 0x65, 0xd4, 0x2a, 0x80, 0x51, 0x45,
    0x08, 0xe6, 0xc8, 0x20, 0x16, 0xb0, 0xb8, 0xc6, 0xee, 0x17, 0xb5, 0x6f,
    0x6f, 0xb5, 0x75, 0xb3, 0x14, 0x03, 0x68, 0xab, 0x99, 0x69, 0xf8, 0x87,
    0x70, 0x5c, 0x6f, 0x45, 0x7b, 0x49, 0xee, 0xb6, 0xbb, 0x3c, 0x0c, 0x50,
    0x90, 0x7c, 0x4c, 0xab, 0xca, 0x81, 0x80, 0x48, 0x5d, 0x13, 0xa2, 0x9e,
    0xf6, 0xfd, 0x2d, 0x57, 0x1d, 0xf0, 0x01, 0xc0, 0x7e, 0x69, 0x69, 0x83,
    0x81, 0x25, 0x70, 0x81, 0x66, 0x26, 0xa3, 0x0f, 0xb7, 0x56, 0x27, 0xcd,
    0x3b, 0x30, 0x45, 0x97, 0x5c, 0x69, 0x56, 0xa1, 0x22, 0x94, 0x87, 0x4b,
    0xd2, 0xf3, 0xc3, 0x92, 0x6d, 0x7d, 0xe3, 0x04, 0x0b, 0xa7, 0x07, 0x42,
    0x9e, 0x58, 0xd0, 0xbf, 0xa2, 0xd2, 0x52, 0xf2, 0xc5, 0xdd, 0xe7, 0x1f,
    0x2e, 0x01, 0x8b, 0xd5, 0xb5, 0x48, 0xb0, 0x94, 0xfb, 0xc4, 0xf8, 0x44,
    0x6c, 0xde, 0xbb, 0x6a, 0xf7, 0xd7, 0x9f, 0x2f, 0x33, 0x07, 0xca, 0xa6,
    0x70, 0x76, 0x58, 0x33, 0x51, 0x6f, 0x34, 0xf8, 0x5e, 0xbf, 0x84, 0xb4,
    0x6e, 0x56, 0x47, 0x12, 0xa2, 0xd1, 0xa1, 0xbb, 0xf1, 0xed, 0x41, 0xd7,
    0x54, 0x49, 0xec, 0x0e, 0x5f, 0x65, 0x84, 0x8e, 0xbe, 0xa0, 0x2a, 0xaa,
    0xe7, 0xff, 0xf3, 0xc0, 0x9d, 0x07, 0xc9, 0xe4, 0xfd, 0x9c, 0x78, 0x6f,
    0x94, 0x9a, 0xab, 0xe5, 0x4b, 0x41, 0x0c, 0x5b, 0x18, 0x40, 0x2c, 0xca,
    0x45, 0xd0, 0x91, 0xc3, 0x99, 0xfa, 0xc1, 0x99, 0xf5, 0x37, 0xf6, 0x2f,
    0x7c, 0x46, 0x4b, 0xea, 0xb9, 0x13, 0xf7, 0x34, 0x31, 0x90, 0x68, 0xa0,
    0xa5, 0x41, 0xfe, 0x45, 0xcd, 0x47, 0x62, 0x9d, 0x92, 0x8a, 0xf6, 0x0b,
    0xdd, 0x26, 0x13, 0x98, 0x8e, 0x8b, 0x42, 0xd9, 0x6c, 0x72, 0x61, 0x07,
    0x05, 0xf0, 0x7f, 0xe0, 0x44, 0x44, 0x02, 0x23, 0xf0, 0x33, 0xc2, 0xe1,
    0x5c, 0x85, 0x57, 0xc8, 0x4a, 0x73, 0xf4, 0x09, 0xba, 0xbd, 0x6f, 0xd0,
    0x3f, 0x76, 0x23, 0xd6, 0x14, 0x81, 0x07, 0x05, 0x08, 0xdb, 0x73, 0x43,
    0x65, 0xfd, 0xcf, 0x4f, 0x93, 0xdd, 0xe6, 0x44, 0x5e, 0xed, 0x1b, 0xf5,
    0x5d, 0x9e, 0xc3, 0xc1, 0x4d, 0x6b, 0x5c, 0xa2, 0xf7, 0x60, 0xd8, 0xb6,
    0x6e, 0x3e, 0xe8, 0x32, 0xc6, 0x24, 0xbb, 0xae, 0xa6, 0x3b, 0x1d, 0x6a,
    0x06, 0x3c, 0x2c, 0x6b, 0x4a, 0x33, 0x4d, 0x6c, 0xd6, 0xc4, 0x1b, 0xb2,
    0x2e, 0xcc, 0x92, 0x1b, 0xc8, 0xd4, 0xdd, 0xd8, 0x18, 0x1a, 0x1b, 0x9b,
    0xb4, 0xc7, 0xf7, 0xcc, 0xbb, 0x67, 0x02, 0x40, 0x94, 0x82, 0x36, 0xa4,
    0x3c, 0x3d, 0x6a, 0xf2, 0xcf, 0x58, 0x67, 0x7d, 0x7f, 0xd0, 0x17, 0x01,
    0xdf, 0xe4, 0xc3, 0xf7, 0x2d, 0x9d, 0x69, 0x19, 0x36, 0x3a, 0x9f, 0xcb,
    0xd5, 0x09, 0xf7, 0x4c, 0x61, 0x06, 0x59, 0xf9, 0x0b, 0x89, 0x9e, 0x73,
    0x9f, 0x44, 0x4e, 0x2e, 0x3d, 0xc0, 0xdd, 0xae, 0x70, 0x36, 0x58, 0x11,
    0xab, 0x0d, 0x73, 0xc2, 0x24, 0x45, 0x7d, 0x5e, 0xd2, 0x6a, 0xc0, 0xa1,
    0x0e, 0x86, 0x52, 0x01, 0xe8, 0xea, 0xce, 0x80, 0x97, 0x9f, 0xfa, 0x69,
    0xb9, 0x2d, 0x9a, 0xd3, 0xd9, 0xee, 0xc4, 0x36, 0xc7, 0x61, 0xdd, 0xe0,
    0xe7, 0xb4, 0xbe, 0x81, 0x54, 0x92, 0x89, 0x44, 0x9f, 0x3b, 0x77, 0x9b,
    0xcd, 0x17, 0xf2, 0x48, 0xd7, 0x1a, 0x46, 0xaf, 0x88, 0x9c, 0x0f, 0xc1,
    0x2b, 0x1e, 0x3d, 0xe5, 0x8e, 0xb3, 0x9b, 0xa0, 0x4b, 0xe6, 0x3f, 0xf4,
    0x27, 0xc4, 0xc9, 0x2c, 0x23, 0xc0, 0x26, 0xf3, 0x02, 0x02, 0x03, 0xe7,
    0x31, 0x82, 0x03, 0x14, 0x30, 0x0d, 0x0c, 0x05, 0x63, 0x6c, 0x61, 0x73,
    0x73, 0x0c, 0x04, 0x67, 0x65, 0x6e, 0x70, 0x30, 0x1c, 0x0c, 0x04, 0x68,
    0x61, 0x73, 0x68, 0x04, 0x14, 0x20, 0x5a, 0x6b, 0x14, 0x4b, 0x34, 0x12,
    0xc6, 0x71, 0x8c, 0xe4, 0xa8, 0x07, 0x83, 0x0e, 0xe0, 0x6f, 0x2d, 0xe6,
    0x13, 0x30, 0x82, 0x02, 0xe3, 0x0c, 0x04, 0x64, 0x61, 0x74, 0x61, 0x04,
    0x82, 0x02, 0xd9, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x48,
    0x00, 0x00, 0x00, 0x0d, 0x9c, 0x25, 0x6a, 0x30, 0x5b, 0x92, 0x39, 0xe8,
    0x4c, 0x61, 0x06, 0x44, 0x0f, 0xb1, 0x58, 0x6f, 0x0f, 0x5f, 0x4c, 0x8d,
    0xdb, 0x59, 0x69, 0x90, 0xd4, 0x2f, 0x4e, 0x78, 0xa9, 0x62, 0x1d, 0xbc,
    0xad, 0x25, 0x38, 0xb7, 0x05, 0xa3, 0x79, 0x94, 0xc7, 0x2c, 0x4f, 0x67,
    0xf3, 0x76, 0xf4, 0xf8, 0x58, 0x34, 0x85, 0x80, 0x2e, 0x3c, 0x90, 0xef,
    0xe7, 0xc2, 0x08, 0xd7, 0x6a, 0xdd, 0xba, 0xae, 0x55, 0x27, 0xd3, 0x42,
    0x27, 0x5d, 0x7e, 0x0e, 0xe5, 0x8e, 0xe9, 0xd5, 0x42, 0x3b, 0x48, 0x70,
    0x40, 0x25, 0xf5, 0xbe, 0xb6, 0x4e, 0x10, 0xf7, 0xe5, 0x60, 0x9d, 0xd7,
    0x8b, 0x63, 0x40, 0xf0, 0x8b, 0xa2, 0xf3, 0xd6, 0x04, 0xaf, 0x54, 0xa5,
    0xeb, 0x52, 0x80, 0x1a, 0xa8, 0xe0, 0xf7, 0x9b, 0x45, 0x94, 0x36, 0xee,
    0x40, 0xfd, 0xad, 0x2c, 0xf0, 0x92, 0x8e, 0x4a, 0x98, 0x80, 0xed, 0x43,
    0xd3, 0x17, 0xa6, 0xab, 0x96, 0xb4, 0x17, 0x1e, 0xab, 0xb4, 0x6b, 0x74,
    0x43, 0x7c, 0xd9, 0xe1, 0x66, 0x6f, 0x07, 0x20, 0x53, 0x66, 0xe6, 0x60,
    0xbf, 0x92, 0x73, 0x46, 0xfb, 0xb0, 0x81, 0xea, 0xf2, 0xac, 0x13, 0xf7,
    0xbb, 0xd4, 0x38, 0x1b, 0x64, 0xd3, 0xc3, 0x34, 0x37, 0x2c, 0xbc, 0x7d,
    0x36, 0x88, 0xb1, 0x90, 0xcd, 0x9e, 0xee, 0xaf, 0xcb, 0x63, 0x3f, 0x8a,
    0xf5, 0x1a, 0xfc, 0x21, 0xb2, 0xf0, 0xc0, 0x60, 0x72, 0xc7, 0xf0, 0x54,
    0x3e, 0x88, 0x4c, 0x69, 0x96, 0xc2, 0x2b, 0x25, 0x96, 0x41, 0x05, 0xd7,
    0xc6, 0x89, 0xe8, 0x33, 0x1c, 0x7a, 0x88, 0x81, 0xfe, 0xb2, 0x3d, 0x6f,
    0x60, 0x43, 0x1e, 0x62, 0x48, 0x76, 0xd2, 0xe6, 0xfb, 0x22, 0x8a, 0x5d,
    0xe6, 0xd8, 0xe2, 0x30, 0x51, 0x2f, 0xcc, 0xab, 0xb0, 0x31, 0x68, 0x43,
    0x51, 0x11, 0x67, 0x09, 0x3c, 0x34, 0x7e, 0x49, 0xe0, 0xe0, 0xd4, 0x74,
    0xe4, 0x17, 0x42, 0x32, 0x76, 0x3a, 0x2f, 0xc1, 0x8a, 0xb3, 0xfc, 0xdc,
    0x54, 0x00, 0x05, 0x00, 0x8b, 0x07, 0x70, 0xbb, 0xc1, 0x97, 0x33, 0x9f,
    0x93, 0x2c, 0xc1, 0xc8, 0x3b, 0xfb, 0x99, 0xe8, 0x3e, 0x60, 0x20, 0x7d,
    0xb7, 0x77, 0x20, 0x61, 0xda, 0xfb, 0x66, 0xa9, 0x0e, 0x5c, 0x65, 0x30,
    0x4d, 0x39, 0x6b, 0x39, 0xb6, 0xe3, 0x16, 0xc2, 0x01, 0x2b, 0x0d, 0x0b,
    0x95, 0xa7, 0x45, 0x40, 0xb8, 0x93, 0xaf, 0x97, 0x8c, 0x63, 0xab, 0x4f,
    0x9e, 0x90, 0x98, 0xda, 0xe7, 0x84, 0x41, 0xcc, 0xed, 0xb5, 0xc0, 0x35,
    0xa6, 0x1b, 0xe5, 0x51, 0x7f, 0xaa, 0x44, 0xd0, 0x32, 0x0c, 0x04, 0xfa,
    0x34, 0x4d, 0xb4, 0x3a, 0xd0, 0x4e, 0xc8, 0xcb, 0x6d, 0x98, 0x42, 0xad,
    0xc4, 0xe3, 0x38, 0x39, 0xc8, 0x21, 0x6a, 0xb5, 0x7f, 0x00, 0x99, 0x58,
    0x69, 0x5e, 0xd8, 0xe7, 0x43, 0x09, 0x7f, 0x7d, 0x65, 0xe9, 0xe1, 0x1e,
    0xa8, 0x60, 0x83, 0x5b, 0xa9, 0x87, 0xca, 0xe6, 0xf4, 0x27, 0xab, 0x93,
    0x8b, 0x43, 0x34, 0xdf, 0xd5, 0x3a, 0x36, 0x19, 0x19, 0x2f, 0x53, 0xef,
    0x8d, 0xa2, 0x4c, 0x29, 0x84, 0xe7, 0x0e, 0x52, 0xb4, 0x0c, 0x4c, 0xe1,
    0x90, 0xef, 0xd7, 0xd9, 0xb8, 0xb9, 0xd3, 0x46, 0xcf, 0x69, 0xd6, 0x35,
    0x59, 0xed, 0x0d, 0x02, 0xc7, 0xa7, 0xf2, 0x6b, 0x5e, 0xdd, 0x86, 0x52,
    0xc2, 0x85, 0xc2, 0x92, 0x69, 0x04, 0x51, 0x93, 0x7d, 0xa2, 0x22, 0x32,
    0x87, 0xa2, 0x9e, 0x76, 0x16, 0xc2, 0x01, 0x4c, 0x9a, 0x96, 0x82, 0xd1,
    0x98, 0xdc, 0xe0, 0xbe, 0x1a, 0xb0, 0x03, 0x1e, 0x2f, 0x44, 0x79, 0xc0,
    0x59, 0x95, 0x57, 0xa5, 0xc9, 0xcf, 0x55, 0x75, 0x78, 0x1d, 0xe2, 0x43,
    0x3a, 0x6b, 0x5f, 0xde, 0x5a, 0x32, 0x0f, 0x0e, 0x38, 0x36, 0xe4, 0x74,
    0x7a, 0x67, 0xd4, 0x88, 0x7e, 0x15, 0x7b, 0x32, 0xf8, 0x57, 0xd4, 0x27,
    0x38, 0xf4, 0x79, 0xf6, 0x7c, 0x0c, 0x6a, 0x1f, 0xbb, 0x22, 0x15, 0x46,
    0x4f, 0xec, 0x0b, 0xfc, 0xd8, 0x80, 0x87, 0xf4, 0x4c, 0x80, 0x9f, 0xa5,
    0x7a, 0xa5, 0x8f, 0x20, 0x22, 0x61, 0x53, 0xbf, 0xa5, 0x11, 0x87, 0x84,
    0x2b, 0xe3, 0x6d, 0x0d, 0x09, 0x00, 0x5f, 0x5f, 0x7f, 0xb6, 0x3c, 0x0c,
    0xc3, 0x0f, 0x93, 0xd5, 0x9c, 0xba, 0xb0, 0x7e, 0x4a, 0x37, 0xfa, 0xbf,
    0xe1, 0x22, 0x00, 0x49, 0xb6, 0x97, 0x08, 0x4e, 0x4e, 0x77, 0xd0, 0x36,
    0xa9, 0x6e, 0x3b, 0x07, 0x07, 0xe0, 0xab, 0x29, 0x6c, 0xc2, 0x2e, 0xf4,
    0x8e, 0x61, 0xe0, 0x36, 0x08, 0xd2, 0x01, 0x1a, 0x66, 0x99, 0x30, 0xbc,
    0xd9, 0x59, 0x41, 0x21, 0x58, 0x6f, 0xc5, 0xa3, 0xf3, 0x76, 0x1a, 0x75,
    0x60, 0xba, 0x28, 0x32, 0xdd, 0x38, 0x61, 0x45, 0x5d, 0x73, 0xc2, 0x36,
    0xb2, 0xf8, 0x2a, 0xb0, 0xa2, 0x3c, 0x40, 0x3d, 0x39, 0x7d, 0x1e, 0xe9,
    0x22, 0xa4, 0x7e, 0xc2, 0xd2, 0x80, 0x09, 0x3a, 0xd8, 0xe5, 0x73, 0x9e,
    0x46, 0x0b, 0x32, 0x74, 0x6d, 0x11, 0x96, 0xf2, 0x22, 0xc8, 0xd7, 0xbd,
    0x52, 0x92, 0x28, 0x59, 0x4a, 0xd7, 0x1e, 0x07, 0x2c, 0xc0, 0x13, 0x15,
    0xb3, 0x33, 0x9a, 0x76, 0xb2, 0x2b, 0x65, 0xe7, 0x4d, 0xc1, 0xf8, 0xa6,
    0x9c, 0xae, 0x70, 0x05, 0x98, 0x3f, 0xed, 0x56, 0xf2, 0xe2, 0x79, 0x40,
    0x02, 0x02, 0x03, 0xe7, 0x31, 0x82, 0x02, 0xf1, 0x30, 0x0d, 0x0c, 0x05,
    0x63, 0x6c, 0x61, 0x73, 0x73, 0x0c, 0x04, 0x67, 0x65, 0x6e, 0x70, 0x30,
    0x1c, 0x0c, 0x04, 0x68, 0x61, 0x73, 0x68, 0x04, 0x14, 0xdb, 0xdd, 0x56,
    0xf1, 0xcb, 0x6a, 0xc9, 0x4a, 0xa8, 0x96, 0x8c, 0x91, 0xc1, 0x78, 0x69,
    0xe1, 0x42, 0x97, 0x32, 0x34, 0x30, 0x82, 0x02, 0xc0, 0x0c, 0x04, 0x64,
    0x61, 0x74, 0x61, 0x04, 0x82, 0x02, 0xb6, 0x03, 0x00, 0x00, 0x00, 0x06,
    0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x8f, 0xe0, 0xd5, 0xc6, 0x5f,
    0x89, 0xe9, 0x4c, 0x90, 0xda, 0xe0, 0xe9, 0xee, 0x3a, 0xdd, 0x7d, 0x61,
    0x54, 0x3e, 0xef, 0xa7, 0xea, 0x3a, 0x97, 0x2a, 0x9b, 0x56, 0x7c, 0x69,
    0xf9, 0x51, 0x2f, 0x40, 0x36, 0x16, 0xc0, 0xd6, 0x3f, 0xf9, 0x2e, 0xfe,
    0x44, 0x07, 0x8e, 0xc6, 0x71, 0x70, 0x86, 0x73, 0x52, 0xa8, 0x76, 0x37,
    0x42, 0x01, 0x93, 0xbe, 0x8c, 0x51, 0xd9, 0x4e, 0x45, 0xe8, 0xf9, 0x7c,
    0x4f, 0x16, 0x13, 0x80, 0x4d, 0x47, 0xd0, 0x59, 0x4e, 0xeb, 0x40, 0x2a,
    0x99, 0xa0, 0x7e, 0x77, 0x44, 0xf0, 0x62, 0xb8, 0x26, 0x39, 0x28, 0x18,
    0x88, 0xdb, 0x37, 0x0c, 0xf1, 0x37, 0x9b, 0x21, 0xbc, 0x50, 0xb7, 0x0f,
    0x9d, 0xa3, 0xcd, 0x36, 0x5d, 0x7d, 0xc7, 0x14, 0x20, 0x82, 0x66, 0x15,
    0x63, 0xce, 0x54, 0x04, 0x04, 0xbd, 0x85, 0x2b, 0xcc, 0xba, 0xdd, 0x65,
    0x7b, 0x8b, 0x6e, 0x0b, 0x8a, 0x51, 0x86, 0xdc, 0xaa, 0x14, 0xef, 0xcd,
    0x06, 0xaf, 0x7c, 0x28, 0xe3, 0x08, 0xc3, 0xaf, 0x87, 0x14, 0x51, 0x03,
    0x4c, 0xe8, 0x0f, 0x20, 0xd5, 0x2c, 0x81, 0x11, 0xce, 0x09, 0x26, 0x09,
    0x8a, 0x51, 0x28, 0x15, 0xef, 0xeb, 0x8c, 0x41, 0xbb, 0xa8, 0x6f, 0x2a,
    0x4c, 0x2d, 0x63, 0x6a, 0x9e, 0x5a, 0x3d, 0xcb, 0x16, 0x50, 0xc3, 0x4b,
    0xe5, 0x11, 0x8c, 0x7b, 0x2a, 0x63, 0x04, 0x42, 0xe0, 0x82, 0x76, 0x17,
    0x47, 0x0f, 0x36, 0x5d, 0x5e, 0x42, 0xa0, 0x86, 0x15, 0xe4, 0x8b, 0x2e,
    0x63, 0xa0, 0x5e, 0x14, 0x02, 0x73, 0x9b, 0x1c, 0x82, 0x5d, 0x02, 0x9f,
    0x99, 0x13, 0x59, 0x7b, 0x2a, 0xe8, 0xf6, 0xea, 0xb4, 0x15, 0x0e, 0xe2,
    0x27, 0x14, 0xdd, 0x45, 0xce, 0x84, 0x24, 0xb6, 0x46, 0x46, 0x48, 0x15,
    0xb6, 0x85, 0xa8, 0x38, 0xc9, 0x51, 0xe8, 0xae, 0x29, 0xfa, 0xbd, 0x1b,
    0x9e, 0x04, 0x5b, 0x5f, 0x35, 0x49, 0xc2, 0x12, 0xfb, 0x27, 0xa5, 0xac,
    0x5b, 0xac, 0x23, 0x00, 0x25, 0xfd, 0x45, 0x85, 0x0a, 0x70, 0x0e, 0xcc,
    0xa5, 0xc3, 0xce, 0x73, 0xe6, 0xe1, 0xd8, 0xb5, 0x28, 0x9c, 0x9c, 0x79,
    0x09, 0xc3, 0x40, 0xb5, 0x73, 0x8a, 0x75, 0x4f, 0xd5, 0x35, 0x90, 0xb2,
    0x2d, 0xdd, 0x5d, 0x89, 0xb9, 0x6d, 0xd8, 0x35, 0x26, 0x81, 0x7d, 0xa3,
    0xc1, 0x04, 0x95, 0xd1, 0xe4, 0x86, 0x5d, 0x9d, 0xef, 0xa9, 0x5c, 0x42,
    0x8e, 0x26, 0x18, 0x1e, 0x9a, 0xe4, 0x90, 0x8b, 0xff, 0x98, 0x19, 0x77,
    0x06, 0xb4, 0x79, 0xda, 0x1e, 0xfa, 0x83, 0xd1, 0x8c, 0x62, 0x1b, 0x17,
    0xe6, 0xce, 0xbd, 0x28, 0xe1, 0x73, 0x4c, 0x8d, 0xb1, 0x1b, 0x1b, 0xf6,
    0xa0, 0x7b, 0x98, 0xa7, 0xa2, 0xd3, 0xfd, 0x31, 0x70, 0x40, 0x25, 0x16,
    0xfa, 0xd7, 0x44, 0x0b, 0x68, 0xba, 0x09, 0xf9, 0x4e, 0xb7, 0xa6, 0xcd,
    0x9c, 0x0c, 0xd4, 0x98, 0xc4, 0xcc, 0x6b, 0x50, 0xeb, 0x23, 0xc2, 0x5a,
    0x33, 0xd9, 0x2f, 0x31, 0xfd, 0xba, 0xe5, 0x99, 0x5f, 0xdc, 0x21, 0x7c,
    0x47, 0x1e, 0x3a, 0xc2, 0x0b, 0xdd, 0xcf, 0xb4, 0x10, 0x15, 0xc4, 0xa0,
    0xeb, 0x85, 0xb6, 0x5f, 0x94, 0x03, 0x97, 0x67, 0xd8, 0x0d, 0xb7, 0xce,
    0xe7, 0x16, 0x15, 0x34, 0x92, 0xcf, 0x90, 0x50, 0xc5, 0xeb, 0x63, 0xc8,
    0xcd, 0xb4, 0x8d, 0xa1, 0x2f, 0xd1, 0x80, 0xcd, 0x36, 0xaa, 0x63, 0xb4,
    0xae, 0x7b, 0x0d, 0x07, 0x59, 0xc2, 0x54, 0x8f, 0xd7, 0x83, 0xd9, 0xd6,
    0xb3, 0xd3, 0x7e, 0x66, 0xc8, 0x0c, 0x57, 0x02, 0x6b, 0x4b, 0x8a, 0x74,
    0x47, 0x96, 0x39, 0x20, 0x0c, 0xad, 0xb8, 0xd1, 0x05, 0x92, 0xfe, 0x0d,
    0x1d, 0x4a, 0x65, 0x32, 0x53, 0x9b, 0x3d, 0xdb, 0x2d, 0x10, 0xd3, 0x0a,
    0x42, 0x79, 0xe0, 0x02, 0xc3, 0xe1, 0xec, 0x3d, 0xde, 0x57, 0x8d, 0x03,
    0x7e, 0x98, 0x62, 0x6c, 0x4a, 0x23, 0xa8, 0x3e, 0xd7, 0x11, 0x03, 0x0b,
    0x04, 0xae, 0x12, 0xb2, 0xde, 0xb5, 0x2b, 0x6c, 0x99, 0x9f, 0x0a, 0x42,
    0x20, 0x35, 0x2d, 0x0d, 0xfb, 0x9f, 0xdc, 0xfd, 0x31, 0x14, 0xbd, 0x07,
    0x94, 0x58, 0x7d, 0xd5, 0x9a, 0x3b, 0xe2, 0xf0, 0xd6, 0x50, 0xff, 0x5e,
    0x0a, 0xf0, 0xd6, 0x60, 0xb8, 0xa2, 0x71, 0x32, 0x11, 0xb7, 0x10, 0x08,
    0x70, 0xe7, 0x9d, 0xac, 0x5d, 0x14, 0x6e, 0xa0, 0x7d, 0x27, 0x2c, 0xd0,
    0xbd, 0xf1, 0x6e, 0x82, 0xdc, 0x6d, 0x57, 0x46, 0xf2, 0xe4, 0x96, 0x43,
    0x19, 0xc5, 0x9f, 0xe0, 0x8c, 0x6a, 0x0a, 0x55, 0x4a, 0xda, 0x3e, 0xdf,
    0x9c, 0x3b, 0xf1, 0x3e, 0x13, 0x3b, 0x7e, 0x9e, 0x76, 0xde, 0xeb, 0x3e,
    0x69, 0xd1, 0xa4, 0x47, 0x54, 0x30, 0xb8, 0x38, 0x15, 0x15, 0x27, 0x49,
    0x29, 0x93, 0xf0, 0xc6, 0x8a, 0xfa, 0xc0, 0xbb, 0xd4, 0xe3, 0x36, 0xcc,
    0xb8, 0x5e, 0x1a, 0x05, 0x57, 0xde, 0xa4, 0xbc, 0xbd, 0x21, 0x17, 0xda,
    0xd4, 0x96, 0x8d, 0x01, 0xa7, 0x79, 0xaa, 0x04, 0x43, 0xaf, 0x6a, 0xd2,
    0xb4, 0x2d, 0xc6, 0x15, 0x27, 0x02, 0x02, 0x03, 0xe7, 0x04, 0x14, 0x20,
    0x5a, 0x6b, 0x14, 0x4b, 0x34, 0x12, 0xc6, 0x71, 0x8c, 0xe4, 0xa8, 0x07,
    0x83, 0x0e, 0xe0, 0x6f, 0x2d, 0xe6, 0x13, 0x02, 0x02, 0x03, 0xe7
};

/* Test backup-restore case, when item had inconsistently set pdmn attribute (due to another bug),
   and mobile restore restored item with inconsistent attributes and afterwards tried SecItemUpdate()
   on it, which failed, leading to the failure of the whole restore operation.
 */

#if 0
static bool SecKeychainWithBackupFile(CFStringRef backupName, CFErrorRef *error, void(^with)(FILE *bufile)) {
    int fd = SecItemBackupHandoffFD(backupName, error);
    if (fd < 0)
        return false;
    FILE *backup = fdopen(fd, "r");
    FILE *file = fopen_journal(fname, "w", error);

    if (!backup) {
        close(fd);
        return SecCheckErrno(!backup, error, CFSTR("fdopen"));
    }
    with(backup);
    fclose(backup);
    return true;
}

static void secd_perform_with_data_in_file(const char* test_prefix, void(^with)(CFDataRef backupData))
{
    CFStringRef tmp_dir = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("/tmp/%s.%X/"), test_prefix, arc4random());

    CFStringPerformWithCString(tmp_dir, ^(const char *tmp_dir_string) {
        ok_unix(mkpath_np(tmp_dir_string, 0755), "Create temp dir %s", tmp_dir_string);
        FILE *file = fopen(fname, "w", error);

    });


    /* set custom keychain dir, reset db */
    CFStringPerformWithCString(tmp_dir, ^(const char *tmp_dir_string) {
        SetCustomHomeURL(tmp_dir_string);
    });

    if(do_before_reset)
        do_before_reset();

    SecKeychainDbReset(NULL);

    CFReleaseNull(tmp_dir);
    CFReleaseNull(keychain_dir);
}
#endif


static void tests(void)
{
    /* custom keychain dir */
    secd_test_setup_temp_keychain("secd_34_backup_der_parse", ^{});

    (void)backup_data;
    
#if 0
    FILE *backup = NULL;
    CFErrorRef *error = NULL;
    CFDataRef backupData = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, backup_data, array_size(backup_data), kCFAllocatorNull);

    CFWriteStreamCreateWithBuffer(kCFAllocatorDefault, <#UInt8 *buffer#>, <#CFIndex bufferCapacity#>)
    ok(SecKeychainWithBackupFileParse(backup, error, ^(SecBackupEventType et, CFTypeRef key, CFTypeRef item) {
        diag("This still fails - don't be alarmed");
    }));
    CFReleaseSafe(error);
#endif

#if 0
    ok( SecKeychainWithBackupFileParse
       (FILE *backup, CFErrorRef *error, void (^handleEvent)(SecBackupEventType et, CFTypeRef key, CFTypeRef item)));
    fclose(backup);
    SecItemBackupWithChanges(CFSTR("SecureBackupService"), &error, ^(SecBackupEventType et, CFTypeRef key, CFTypeRef item) {
        ;
    });

    CFStreamRef
    /* Restore keychain from plist. */
    CFDataRef keybag = CFDataCreate(kCFAllocatorDefault, keybag_data, sizeof(keybag_data));
    CFDataRef backup = CFDataCreate(kCFAllocatorDefault, (const UInt8 *)export_plist, sizeof(export_plist));
    ok_status(_SecKeychainRestoreBackup(backup, keybag, NULL));
    CFRelease(keybag);
    CFRelease(backup);

    /* The restored item is kind of malformed (pdmn and accc attributes are inconsistent).  Currently adopted way
     of handling of this item is to try to handle it gracefully, this is what this test does (i.e. it checks that
     it is possible to update such item).  Another possibility which might be adopted in the future is dropping such
     item during backup decoding.  In this case, the test should be modified to check that the item does not exist
     in the keychain at all. */

    /* Try to update item with inconsistent accc and pdmn attributes. */
    CFDictionaryRef query = CFDictionaryCreateMutableForCFTypesWith(kCFAllocatorDefault,
                                                                    kSecClass, kSecClassGenericPassword,
                                                                    kSecAttrAccessGroup, CFSTR("com.apple.security.sos"),
                                                                    kSecAttrService, CFSTR("test"),
                                                                    NULL);
    CFDictionaryRef update = CFDictionaryCreateMutableForCFTypesWith(kCFAllocatorDefault,
                                                                     kSecAttrService, CFSTR("updated-test"),
                                                                     NULL);

    ok_status(SecItemUpdate(query, update));
    diag("This still fails - don't be alarmed");
    CFRelease(update);
    CFRelease(query);
#endif
    
}

int secd_32_restore_bad_backup(int argc, char *const *argv)
{
    plan_tests(2 + kSecdTestSetupTestCount);
    tests();

    return 0;
}
