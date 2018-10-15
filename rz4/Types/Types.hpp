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

        enum { TakCompressor = 0x1, WavPackCompressor };
        enum { RiffWave = 0 };
        extern const char* StreamTypes[];
        extern const char* StreamExts[];

        typedef struct StreamInfo {
            std::string FileType;
            std::string Ext;
            std::string ShortType;
            uintmax_t Size;
            uintmax_t Offset;
            unsigned short Type;
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
            unsigned short WavPackCompLevel;
            unsigned short TakCompLevel;
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
            unsigned short WavPackCompLevel;
            unsigned short TakCompLevel;
        } CompressorOptions;
 
        const char RzfHeaderSignature[4] = { 'R', 'Z', '4', 'F' };
        const char RzfHeaderVersion[3] = { '0', '0', '1' };

#pragma pack(push, 1)
        typedef struct RzfHeader {
            char Signature[4];
            char Version[3];
            uintmax_t OriginalSize;
            unsigned long NumberOfStreams;
            uint32_t OriginalCRC32;
            uintmax_t FirstCompressedStreamOffset;
        } RzfHeader;
#pragma pack(pop)

#pragma pack(push, 1)
        typedef struct RzfCompressedStream {
            unsigned short Type;
            unsigned short Compressor;
            uintmax_t NextCompressedStreamOffset;
            uintmax_t CompressedSize;
            uintmax_t OriginalOffset;            
            uintmax_t OriginalSize;
            uint32_t OriginalCRC32;
        } RzfCompressedStream;
#pragma pack(pop)
    }
}

#endif //RZ4M_TYPES_H
