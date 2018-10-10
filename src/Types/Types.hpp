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

#ifndef RZ4M_TYPES_H
#define RZ4M_TYPES_H

#include <string>
#include <list>
#include <boost/filesystem.hpp>

namespace rz4 {
    namespace Types {
        namespace fs = boost::filesystem;

        enum { RiffWave = 0 };
        extern const char* StreamTypes[];
        extern const char* StreamExts[];

        typedef struct StreamInfo {
            std::string FileType;
            std::string Ext;
            std::string ShortType;
            uintmax_t Size;
            uintmax_t Offset;
            void *Data;
        } StreamInfo;

        typedef struct CLIOptions {
            fs::path OutDir;
            std::string Command;
            fs::path InFile;
            fs::path OutFile;
            unsigned int BufferSize;
            bool Verbose;
            bool EnableRiffWave;
        } CLIOptions;

        typedef struct ScannerOptions {
            fs::path FileName;
            unsigned int BufferSize;
            bool EnableRiffWave;
        } ScannerOptions;

        typedef const std::function<void(StreamInfo*)> ScannerCallbackHandle;

        typedef struct CompressorOptions {
            fs::path FileName;
            fs::path OutFile;
            unsigned int BufferSize;
            std::list<StreamInfo> *ListOfStreams;
            bool EnableRiffWave;
        } CompressorOptions;
 
        const char RzfHeaderSignature[4] = { 'R', 'Z', '4', 'F' };

#pragma pack(push, 1)
        typedef struct RzfHeader {
            char Signature[4];
            uintmax_t OriginalSize;
            unsigned int NumberOfStreams;
        } RzfHeader;
#pragma pack(pop)

#pragma pack(push, 1)
        typedef struct RzfCompressedStream {
            unsigned char Type;
            unsigned char Compressor;
            uintmax_t Offset;
            uintmax_t CompressedSize;
            uintmax_t OriginalSize;
            uint32_t OriginalCRC32;
            void *CompressedData;
        } RzfCompressedStream;
#pragma pack(pop)
    }
}

#endif //RZ4M_TYPES_H
