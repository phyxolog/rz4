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

            // For calculating CRC32
            uint32_t TableCRC32[256];
            Utils::GenerateTableCRC32(TableCRC32);

            Types::RzfHeader Header;
            std::memcpy(Header.Signature, Types::RzfHeaderSignature, sizeof(Types::RzfHeaderSignature));
            std::memcpy(Header.Version, Types::RzfHeaderVersion, sizeof(Types::RzfHeaderVersion));
            Header.OriginalSize = FileSize;
            Header.OriginalCRC32 = Utils::CalculateCRC32InStream(TableCRC32, File, 0, FileSize);
            Header.NumberOfStreams = static_cast<unsigned long>(Options.ListOfStreams->size());
            Header.FirstStreamOffset = sizeof(Types::RzfHeader) + Options.ListOfStreams->front().Offset;

            // First off, write header
            OutFile.write(reinterpret_cast<const char*>(&Header), sizeof(Types::RzfHeader));

            Types::RzfCompressedStream CompressedStream;
            uintmax_t PrevOffset = 0, PrevCompressedSize = 0, i = 1;
            std::list<Types::StreamInfo> UnPtrListOfStreams = *Options.ListOfStreams;

            for (auto StreamIterator = UnPtrListOfStreams.begin(); StreamIterator != UnPtrListOfStreams.end(); StreamIterator++) {
                Types::StreamInfo Stream = *StreamIterator;
                // TODO: Compress data
                // TODO: Select compressor
                // TODO: Check, if compressedsize >= streamsize - write raw data
                // TODO: Compressed stream types

                uintmax_t CompressedSize = Stream.Size - 20;
                CompressedStream.Type = 0x1;
                CompressedStream.Compressor = 0x1;
                CompressedStream.CompressedSize = CompressedSize;

                auto NextStreamIterator = std::next(StreamIterator, 1);
                if (NextStreamIterator != UnPtrListOfStreams.end()) {
                    if (i > 1) {
                        auto PrevStreamIterator = std::prev(StreamIterator, 1);
                        // TODO
                        CompressedStream.NextCompressedStreamOffset = 24;
                    }
                    else {
                        CompressedStream.NextCompressedStreamOffset =
                            (sizeof(Types::RzfHeader) + (*NextStreamIterator).Offset)
                            + sizeof(Types::RzfCompressedStream)
                            - (Stream.Size - CompressedSize);
                    }
                }
                else {
                    CompressedStream.NextCompressedStreamOffset = -1;
                }
                CompressedStream.OriginalOffset = Stream.Offset;
                CompressedStream.OriginalSize = Stream.Size;
                CompressedStream.OriginalCRC32 =
                    Utils::CalculateCRC32InStream(TableCRC32, File, Stream.Offset, Stream.Size);

                // Write non-compressed data
                if (Stream.Offset > PrevOffset) {
                    Utils::InjectDataFromStreamToStream(File, OutFile, PrevOffset, Stream.Offset - PrevOffset);
                }

                OutFile.write(reinterpret_cast<const char*>(&CompressedStream), sizeof(Types::RzfCompressedStream));

                // TODO: Inject compressed data
                Utils::InjectDataFromStreamToStream(File, OutFile, Stream.Offset, CompressedSize);

                PrevOffset = Stream.Offset + Stream.Size;
                PrevCompressedSize = CompressedStream.CompressedSize;
                i++;
            }

            // Write other non-compressed data
            if (PrevOffset < FileSize) {
                Utils::InjectDataFromStreamToStream(File, OutFile, PrevOffset, FileSize - PrevOffset);
            }

            OutFile.close();

            /*Types::RzfHeader ReadHeader;
            Types::RzfCompressedStream ReadCS;
            std::ifstream tmpf;
            tmpf.open(Options.OutFile.string(), std::fstream::binary);
            tmpf.read(reinterpret_cast<char*>(&ReadHeader), sizeof(Types::RzfHeader));
            tmpf.seekg(ReadHeader.FirstStreamOffset);

            tmpf.read(reinterpret_cast<char*>(&ReadCS), sizeof(Types::RzfCompressedStream));*/

            /*tmpf.seekg(ReadCS.NextCompressedStreamOffset);
            tmpf.read(reinterpret_cast<char*>(&ReadCS), sizeof(Types::RzfCompressedStream));*/

            //std::cout << ReadCS.NextCompressedStreamOffset << std::endl;

            /*tmpf.seekg(ReadCS.NextCompressedStreamOffset);
            tmpf.read(reinterpret_cast<char*>(&ReadCS), sizeof(Types::RzfCompressedStream));

            std::cout << ReadCS.NextCompressedStreamOffset << std::endl;*/
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