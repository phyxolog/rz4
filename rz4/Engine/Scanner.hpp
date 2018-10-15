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

#ifndef RZ4M_SCANNER_H
#define RZ4M_SCANNER_H

#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <algorithm>
#include <boost/filesystem.hpp>

#include "Engine/Formats/RiffWave.hpp"
#include "Types/Types.hpp"
#include "Utils/Utils.hpp"

namespace rz4 {
    namespace Engine {
        namespace fs = boost::filesystem;

        class Scanner {
        private:
            std::ifstream File;
            unsigned int BufferSize;
            uintmax_t FileSize;
            uintmax_t TotalSize;
            Types::ScannerOptions Options;
            std::list<Types::StreamInfo> StreamList;

        public:
            explicit Scanner(Types::ScannerOptions);
            ~Scanner();

            bool Start(Types::ScannerCallbackHandle& = nullptr);
            void Close();
            std::list<Types::StreamInfo> *GetListOfFoundStreams();
            unsigned long GetCountOfFoundStreams();
            uintmax_t GetSizeOfFoundStreams();

            // Scanners
            void RiffWaveMatch(const char *, uintmax_t, Types::ScannerCallbackHandle&);
        };
    }
}


#endif //RZ4M_SCANNER_H
