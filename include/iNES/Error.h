/*
Copyright (C) 2000 - 2016 Evan Teran
                          evan.teran@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INES_ERROR_20160314_H_
#define INES_ERROR_20160314_H_

#include <exception>

namespace iNES {

class ines_error : public std::exception {};

class ines_bad_header            : public ines_error {};
class ines_open_failed           : public ines_error {};
class ines_read_failed           : public ines_error {};
class ines_write_failed          : public ines_error {};
class ines_unsupported_file_type : public ines_error {};

}

#endif

