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

#include "Scanner.hpp"
#include "stdafx.hpp"

namespace rz4 {
    namespace Engine {
        Scanner::Scanner(rz4::Types::ScannerOptions Options) : Options(Options) {
            FileSize = fs::file_size(Options.FileName);
            File.open(Options.FileName.string(), std::fstream::binary);
            BufferSize = Options.BufferSize;
            TotalSize = 0;

            if (FileSize < BufferSize) {
                BufferSize = static_cast<unsigned int>(FileSize);
            }
        }

        Scanner::~Scanner() {
            StreamList.clear();
            Close();
        }

        bool Scanner::Start(Types::ScannerCallbackHandle &Callback) {            
            if (!File.is_open()) {
                return false;
            }

            uintmax_t ReadBytes = 0;
            char *Buffer = new char[BufferSize];

            while (ReadBytes < FileSize) {
                if ((ReadBytes + BufferSize) > FileSize) {
                    BufferSize = static_cast<unsigned int>(FileSize - ReadBytes);
                    delete[] Buffer;
                    Buffer = new char[BufferSize];
                }

                File.read(Buffer, BufferSize);

                if (Options.EnableRiffWave) {
                    RiffWaveMatch(Buffer, ReadBytes, Callback);
                }

                ReadBytes += BufferSize;
            }

            // Sort all found positions
            // For fast copy non-compressed data
            StreamList.sort([](const Types::StreamInfo &F, const Types::StreamInfo &S) {
                return F.Offset < S.Offset;
            });

            delete[] Buffer;
            return true;
        }

        void Scanner::Close() {
            if (File.is_open()) {
                File.close();
            }
        }

        std::list<Types::StreamInfo> *Scanner::GetListOfFoundStreams() {
            return &StreamList;
        }

        uintmax_t Scanner::GetSizeOfFoundStreams() {
            return TotalSize;
        }

        unsigned long Scanner::GetCountOfFoundStreams() {
            return static_cast<unsigned long>(StreamList.size());
        }

        // Scanners
        void Scanner::RiffWaveMatch(const char *Buffer, uintmax_t CurrentOffset, Types::ScannerCallbackHandle &Callback) {
            Engine::Formats::RiffWave::RiffWaveHeader *RiffWaveHeader;
            const unsigned int HeaderBufferSize = sizeof(Engine::Formats::RiffWave::RiffWaveHeader);
            char *HeaderBuffer = new char[HeaderBufferSize];
            bool ChangedPosition = false;
            Types::StreamInfo StreamInfo;

            int Index = Utils::CharMatch(Buffer, BufferSize, 'R');
            
            while (Index != -1 && static_cast<unsigned int>(Index) <= BufferSize) {
                if (Index + HeaderBufferSize <= BufferSize) {
                    std::memcpy(HeaderBuffer, Buffer + Index, HeaderBufferSize);
                } else {
                    File.seekg(CurrentOffset + Index, std::fstream::beg);
                    File.read(HeaderBuffer, HeaderBufferSize);
                    ChangedPosition = true;
                }     

                if (Engine::Formats::RiffWave::IsRiffWaveHeader(HeaderBuffer)) {
                    RiffWaveHeader = reinterpret_cast<Engine::Formats::RiffWave::RiffWaveHeader*>(HeaderBuffer);
                    
                    StreamInfo.FileType = Types::StreamTypes[Types::RiffWave];
                    StreamInfo.Ext = Types::StreamExts[Types::RiffWave];
                    StreamInfo.Type = Types::RiffWave;
                    // Fixed: get valid size of RIFF WAVE stream
                    /*unsigned long ChunkSize = RiffWaveHeader->ChunkSize + 8;
                    unsigned long SubChunkSize = RiffWaveHeader->Subchunk2Size
                        + sizeof(Engine::Formats::RiffWave::RiffWaveHeader);
                    
                    if (ChunkSize < SubChunkSize) {
                        StreamInfo.Size = ChunkSize;
                    } else {
                        StreamInfo.Size = SubChunkSize;
                    }*/

                    StreamInfo.Size = RiffWaveHeader->ChunkSize + 8;
                    StreamInfo.Offset = CurrentOffset + Index;
                    StreamInfo.Data = new Engine::Formats::RiffWave::RiffWaveHeader;
                    std::memcpy(StreamInfo.Data, RiffWaveHeader, sizeof(Engine::Formats::RiffWave::RiffWaveHeader));
                    
                    StreamList.push_back(StreamInfo);

                    TotalSize += StreamInfo.Size;

                    if (Callback != nullptr) {
                        Callback(&StreamInfo);
                    }
                }

                Index = Utils::CharMatch(Buffer, BufferSize, 'R', static_cast<unsigned int>(Index + 1));
            }

            delete[] HeaderBuffer;
            if (ChangedPosition) {
                File.seekg(CurrentOffset + BufferSize, std::fstream::beg);
            }
        }
    }
}