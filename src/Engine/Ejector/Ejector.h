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

#ifndef RZ4M_EJECTOR_H
#define RZ4M_EJECTOR_H

#include <string>
#include <fstream>
#include <experimental/filesystem>

namespace rz4m {
    namespace Engine {
        class Ejector {
        private:
            std::ifstream File;
            std::string FileName;
            unsigned int BufferSize;
            uintmax_t FileSize;

        public:
            Ejector(std::string FileName, unsigned int BufferSize = 65536);
            ~Ejector();

            bool Extract(uintmax_t Offset, uintmax_t Size, std::string OutFileName);
            void Close();
        };
    }
}


#endif //RZ4M_EJECTOR_H
