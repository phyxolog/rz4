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
#include "stdafx.hpp"

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
            Header.FirstCompressedStreamOffset = -1;

            // Keep bytes for header
            OutFile.seekp(sizeof(Types::RzfHeader));

            Types::RzfCompressedStream CompressedStream;
            uintmax_t PrevOffset = 0, i = 0, SavedBytes = 0;
            std::list<Types::StreamInfo> DerListOfStreams = *Options.ListOfStreams;
            Types::StreamInfo Stream;
            std::ifstream CompressFileStream;
            fs::path ComressFileName;

            for (auto StreamIterator = DerListOfStreams.begin();
                StreamIterator != DerListOfStreams.end();
                StreamIterator++, i++) {

                Stream = *StreamIterator;

                // Write non-compressed data
                if (Stream.Offset > PrevOffset) {
                    Utils::InjectDataFromStreamToStream(
                        File,
                        OutFile,
                        PrevOffset,
                        Stream.Offset - PrevOffset
                    );
                }

                CompressStream(Stream, CompressedStream, CompressFileStream, ComressFileName);

                // If compressed size >= stream size
                // Write raw data
                if (CompressedStream.CompressedSize >= Stream.Size) {
                    Utils::InjectDataFromStreamToStream(File, OutFile, Stream.Offset, Stream.Size);
                    PrevOffset = Stream.Offset + Stream.Size;
                    continue;
                }

                // Calculating next stream offset
                auto NextStreamIterator = std::next(StreamIterator, 1);
                if (NextStreamIterator != DerListOfStreams.end()) {
                    if (i > 0) {
                        CompressedStream.NextCompressedStreamOffset =
                            sizeof(Types::RzfHeader)
                            + (*NextStreamIterator).Offset
                            + sizeof(Types::RzfCompressedStream)
                            - (Stream.Size - CompressedStream.CompressedSize)
                            + (i * sizeof(Types::RzfCompressedStream))
                            - SavedBytes;
                    } else {
                        CompressedStream.NextCompressedStreamOffset =
                            sizeof(Types::RzfHeader)
                            + (*NextStreamIterator).Offset
                            + sizeof(Types::RzfCompressedStream)
                            - (Stream.Size - CompressedStream.CompressedSize);
                    }
                } else {
                    CompressedStream.NextCompressedStreamOffset = -1;
                }
                CompressedStream.Type = Stream.Type;
                CompressedStream.OriginalOffset = Stream.Offset;
                CompressedStream.OriginalSize = Stream.Size;
                CompressedStream.OriginalCRC32 =
                    Utils::CalculateCRC32InStream(TableCRC32, File, Stream.Offset, Stream.Size);

                OutFile.write(reinterpret_cast<const char*>(&CompressedStream), sizeof(Types::RzfCompressedStream));

                Utils::InjectDataFromStreamToStream(
                    CompressFileStream,
                    OutFile,
                    Stream.Offset,
                    CompressedStream.CompressedSize
                );

                CompressFileStream.close();
                fs::remove(ComressFileName);
                
                SavedBytes += Stream.Size - CompressedStream.CompressedSize;
                PrevOffset = Stream.Offset + Stream.Size;
            }

            // Write other non-compressed data
            if (PrevOffset < FileSize) {                
                Utils::InjectDataFromStreamToStream(
                    File,
                    OutFile,
                    PrevOffset,
                    FileSize - PrevOffset
                );
            }

            // If we compress some data
            // Then set pointer to first compressed stream
            if (SavedBytes != 0) {
                Header.FirstCompressedStreamOffset =
                    sizeof(Types::RzfHeader) + Options.ListOfStreams->front().Offset;
            }

            // Write header
            OutFile.seekp(std::fstream::beg);
            OutFile.write(reinterpret_cast<const char*>(&Header), sizeof(Types::RzfHeader));
        }

        void Compressor::CompressStream(
            Types::StreamInfo &Stream,
            Types::RzfCompressedStream &CompressedStream,
            std::ifstream &CompressFileStream,
            fs::path &ComressFileName) {
            fs::path TempFileName = Utils::GenerateTmpFileName(fs::current_path().string(), ".wav");
            Utils::ExtactDataFromStreamToFile(File, Stream.Offset, Stream.Size, TempFileName.string());
            fs::path OutFileName = TempFileName.filename();
            bool Result = false;

            // Select compressor
            switch (Stream.Type) {
            case Types::RiffWave:
                // Fix size in header (RIFF WAVE)
                /*Engine::Formats::RiffWave::FixRiffWaveHeaderInFile(
                    TempFileName.string(),
                    reinterpret_cast<Engine::Formats::RiffWave::RiffWaveHeader*>(Stream.Data)
                );*/

                if (Options.TakCompLevel > 0) {
                    CompressedStream.Compressor = Types::TakCompressor;
                    OutFileName = OutFileName.replace_extension(".tak");
                } else if (Options.WavPackCompLevel > 0) {
                    CompressedStream.Compressor = Types::WavPackCompressor;
                    OutFileName = OutFileName.replace_extension(".wv");
                }

                break;
            default:
                break;
            }

            ComressFileName = OutFileName;
            if (CompressFileStream.is_open()) {
                CompressFileStream.close();
                fs::remove(OutFileName);
            }

            switch (CompressedStream.Compressor) {
            case Types::TakCompressor:
                Result = TakCompress(TempFileName, OutFileName, Options.TakCompLevel);
                break;
            case Types::WavPackCompressor:
                Result = WavpackCompress(TempFileName, OutFileName, Options.WavPackCompLevel);
                break;
            default:
                break;
            }

            if (Result) {
                CompressFileStream.open(OutFileName.string(), std::fstream::binary);
                CompressedStream.CompressedSize = fs::file_size(OutFileName);
            } else {
                CompressedStream.CompressedSize = Stream.Size;
            }

            fs::remove(TempFileName);
        }

        bool Compressor::WavpackCompress(fs::path InputFile, fs::path OutputFile, unsigned short Level) {
            bp::ipstream Pipe;
#if _WIN64
            bp::child process(bp::search_path("packers/wavpack_x64.exe"), "-h", InputFile, OutputFile);
#else
            bp::child process(bp::search_path("packers/wavpack_x32.exe"), "-h", InputFile, OutputFile);
#endif
            process.wait();
            return process.exit_code() == 0;
        }

        bool Compressor::TakCompress(fs::path InputFile, fs::path OutputFile, unsigned short Level) {
            bp::ipstream Pipe;
            bp::child process(bp::search_path("packers/tak.exe"), "-e", "-overwrite", "-wm0", "-tn4", "-p4m", InputFile, OutputFile);
            process.wait();
            return process.exit_code() == 0;
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