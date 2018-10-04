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

#include "Scanner.h"

namespace rz4m {
    namespace Engine {
        Scanner::Scanner() {
            // TODO: Add options to constructor
            // TODO: Open file
            // TODO: Check buffer size
            FileSize = fs::file_size(FileName);
            TotalSize = 0;
            StreamList = new std::list<Types::StreamInfo>;
        }

        Scanner::~Scanner() {
            StreamList->clear();
            if (File.is_open()) {
                File.close();
            }
        }

        bool Scanner::Start() {}
        bool Scanner::Stop() {}

        std::list<Types::StreamInfo> *Scanner::GetListOfFoundStreams() {
            return StreamList;
        }

        uintmax_t Scanner::GetSizeOfFoundStreams() {
            return TotalSize;
        }

        unsigned long Scanner::GetCountOfFoundStreams() {
            return StreamList->size();
        }
    }
}