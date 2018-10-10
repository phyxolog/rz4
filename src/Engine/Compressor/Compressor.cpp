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

        void Compressor::Start() {
            if (!File.is_open() || !OutFile.is_open()) {
                // TODO: Handle errors
                return;
            }

            Types::RzfHeader Header;
            std::memcpy(Header.Signature, Types::RzfHeaderSignature, sizeof(Types::RzfHeaderSignature));
            Header.OriginalSize = FileSize;
            Header.NumberOfStreams = 0;

            // Reserve zero bytes to header
            void *ZeroBytes = new char[sizeof(Types::RzfHeader)];
            memset(ZeroBytes, 0, sizeof(Types::RzfHeader));
            OutFile.write(reinterpret_cast<const char*>(ZeroBytes), sizeof(Types::RzfHeader));
            OutFile.seekp(std::fstream::beg);

            // For calculating CRC32
            uint32_t TableCRC32[256];
            Utils::GenerateTableCRC32(TableCRC32);

            // TODO: For non-compressed data
            // TODO: We save all offsets and sizes of non-compressed data

            Types::RzfCompressedStream CompressedStream;
            uintmax_t PrevOffset = 0;

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

                Header.NumberOfStreams++;
            }

            /*if (PrevOffset < FileSize) {
                NonCompressedList.push_back({ PrevOffset, FileSize - PrevOffset });
            }*/

            // Seek to begin of file and write header
            OutFile.seekp(std::fstream::beg);
            OutFile.write(reinterpret_cast<const char*>(&Header), sizeof(Types::RzfHeader));

            // Write other non-compressed data
            // TODO: Buffer reader & writer
            /*OutFile.seekp(std::fstream::end);
            for (auto NonCompressedData : NonCompressedList) {
                char *Buffer = new char[NonCompressedData[1]];
                File.seekg(NonCompressedData[0], std::fstream::beg);
                File.read(Buffer, NonCompressedData[1]);
                OutFile.write(Buffer, NonCompressedData[1]);
            }*/

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