/*
* Copyright (C) 2018 Yura Zhivaga <yzhivaga@gmail.com>
*
* This file is part of rz4.
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

#include "RiffWave.hpp"

namespace rz4 {
    namespace Engine {
        namespace Formats {
            namespace RiffWave {
                bool IsRiffWaveHeader(const char *Header) {
                    return std::memcmp(Header, "RIFF", 4) == 0 && std::memcmp(Header + 8, "WAVE", 4) == 0;
                }
            }
        }
    }
}