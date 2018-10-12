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

#include "Compressor.hpp"

namespace rz4 {
    namespace Engine {
        Compressor::Compressor(Types::CompressorOptions Options) : Options(Options) {
            FileSize = fs::file_size(Options.FileName);
            File.open(Options.FileName.string(), std::fstream::binary);
            OutFile.open(Options.OutFile.string(), std::fstream::trunc | std::fstream::binary);

            BufferSize = Options.BufferSize;

            if (FileSize < BufferSize) {
                BufferSize = static_cast<unsigned int>(FileSize);
            }
        }

        Compressor::~Compressor() {
            Close();
        }

        Types::StreamInfo *Compressor::GetRangeInStreamList(uintmax_t Offset, uintmax_t Size) {
            for (auto Stream : *Options.ListOfStreams) {
                if (Offset >= Stream.Offset && Size <= Stream.Size) {
                    return &Stream;
                }

                if (Offset >= Stream.Offset && Size > Stream.Size) {
                    return &Stream;
                }
            }

            return nullptr;
        }

        void Compressor::Start() {
            if (!File.is_open() || !OutFile.is_open()) {
                // TODO: Handle errors
                return;
            }

            // For calculating CRC32
            uint32_t TableCRC32[256];
            Utils::GenerateTableCRC32(TableCRC32);

            Types::RzfHeader Header;
            std::memcpy(Header.Signature, Types::RzfHeaderSignature, sizeof(Types::RzfHeaderSignature));
            std::memcpy(Header.Version, Types::RzfHeaderVersion, sizeof(Types::RzfHeaderVersion));
            Header.OriginalSize = FileSize;
            Header.OriginalCRC32 = Utils::CalculateCRC32InStream(TableCRC32, File, 0, FileSize);
            Header.NumberOfStreams = static_cast<unsigned long>((*Options.ListOfStreams).size());

            // First off, write header
            OutFile.write(reinterpret_cast<const char*>(&Header), sizeof(Types::RzfHeader));

            Types::RzfCompressedStream CompressedStream;

            for (auto Stream : *Options.ListOfStreams) {
                // TODO: Compress data
                // TODO: Select compressor
                // TODO: Compressed stream types

                CompressedStream.Type = 0x1;
                CompressedStream.Compressor = 0x1;
                CompressedStream.Offset = Stream.Offset;
                CompressedStream.OriginalSize = Stream.Size;
                CompressedStream.CompressedSize = Stream.Size;
                CompressedStream.OriginalCRC32 =
                    Utils::CalculateCRC32InStream(TableCRC32, File, Stream.Offset, Stream.Size);

                OutFile.write(reinterpret_cast<const char*>(&CompressedStream), sizeof(Types::RzfCompressedStream));

                // TODO: Inject compressed data
                Utils::InjectDataFromStreamToStream(File, OutFile, Stream.Offset, Stream.Size);
            }

            File.seekg(std::fstream::cur);
            uintmax_t LastOffset = 0, Offset = 0, Size = 0;

            for (auto Stream : *Options.ListOfStreams) {
                Offset = Stream.Offset;
                if (LastOffset == 0) {
                    Offset = 0;
                    Size = Stream.Offset;
                } else if (Stream.Offset >= LastOffset) {
                    Size = Stream.Offset - LastOffset;
                }

                Utils::InjectDataFromStreamToStream(File, OutFile, Offset, Size);
                LastOffset = Stream.Offset;
            }

            /*OutFile.close();

            Types::RzfHeader ReadHeader;
            std::ifstream tmpf;
            tmpf.open(Options.OutFile.string(), std::fstream::binary);
            tmpf.read(reinterpret_cast<char*>(&ReadHeader), sizeof(Types::RzfHeader));
            std::cout << ReadHeader.NumberOfStreams << std::endl;*/
        }

        void Compressor::Close() {
            if (File.is_open()) {
                File.close();
            }

            if (OutFile.is_open()) {
                OutFile.close();
            }
        }
    }
}