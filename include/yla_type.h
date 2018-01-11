/*
    Types of value used for Virtual Machine

    This file is part of YLA VM (Yet another Language for Academic purpose: Virtual Machine).

    YLA VM is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef _yla_type_h
#define _yla_type_h

typedef double yla_number_type;
typedef unsigned char yla_cop_type;

union union_d{
        yla_number_type num;
        yla_cop_type chars[sizeof(yla_number_type)];
};

typedef union union_d UNION_DOUBLE;

#endif
