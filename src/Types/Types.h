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

#ifndef RZ4M_TYPES_H
#define RZ4M_TYPES_H

#include <string>
#include <boost/filesystem.hpp>

namespace rz4m {
    namespace Types {
        namespace fs = boost::filesystem;

        typedef struct StreamInfo {
            std::string FileType;
            std::string Ext;
            std::string ShortType;
            uintmax_t FileSize;
            uintmax_t Offset;
            void *data;
        } StreamInfo;

        typedef struct CLIOptions {
            fs::path OutDir;
            std::string Command;
            fs::path InFile;
            fs::path OutFile;
            unsigned int BufferSize;
            bool EnableWav;
        } CLIOptions;

        typedef struct ScannerOptions {
            fs::path FileName;
            unsigned int BufferSize;
            bool EnableWav;
        } ScannerOptions;

        typedef const std::function<void (StreamInfo*)> ScannerCallbackHandle;
    }
}

#endif //RZ4M_TYPES_H
