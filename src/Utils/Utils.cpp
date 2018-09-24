/*
 * Copyright (C) 2018 Yura Zhivaga <yzhivaga@gmail.com>
 *
 * This file is part of rz4m.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Utils.h"

namespace rz4m {
    namespace Utils {
        /*
         * Find char in array.
         * If found - return index.
         * If not - return -1.
         */
        int CharMatch(const char *buffer, unsigned int buffer_size, char needle, unsigned int offset) {
            if (offset >= buffer_size) {
                return -1;
            }

            buffer_size -= offset;
            const char* result = reinterpret_cast<const char*>(std::memchr(buffer + offset, needle, buffer_size));
            return result ? static_cast<int>(result - buffer) : -1;
        }
    }
}
