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

#ifndef RZ4_COMPRESSOR_HPP
#define RZ4_COMPRESSOR_HPP

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <boost/filesystem.hpp>

#include <Types/Types.hpp>
#include <Utils/Utils.hpp>

namespace rz4 {
    namespace Engine {
        namespace fs = boost::filesystem;

        class Compressor {
        private:
            std::ifstream File;
            std::ofstream OutFile;
            Types::CompressorOptions Options;
            unsigned int BufferSize;
            uint64_t FileSize;

        public:
            explicit Compressor(Types::CompressorOptions);
            ~Compressor();

            void Start();
            void Close();
        };
    }
}


#endif //RZ4_COMPRESSOR_HPP
