/*
Td5strings.h -
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Td5Strings_h
#define Td5Strings_h

#include "td5defs.h"

extern const char *menu_header[];
extern const char *_main_menu[];
extern const char *_sub_menu_0[];
extern const char *_sub_menu_0_1[];
extern const char *_sub_menu_0_3[];
extern const char *_void_menu[];

extern const char *fuelling_params[];
extern char fuelling_values[22][6];

extern const char *options_params[];
extern char options_values[3][6];

extern const char *injector_codes[];

#ifdef _FAULT_CODES_STRINGS_
extern const char *fault_code[];
#endif


#endif

