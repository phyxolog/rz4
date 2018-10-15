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

#ifndef RZ4M_RIFFWAVE_FORMAT_H
#define RZ4M_RIFFWAVE_FORMAT_H

#include <cstring>

namespace rz4 {
    namespace Engine {
        namespace Formats {
            namespace RiffWave {
#pragma pack(push, 1)
                typedef struct RiffWaveHeader {
                    char ChunkId[4];
                    unsigned long ChunkSize;
                    char Format[4];
                    char Subchunk1Id[4];
                    unsigned long Subchunk1Size;
                    unsigned short AudioFormat;
                    unsigned short NumChannels;
                    unsigned long SampleRate;
                    unsigned long ByteRate;
                    unsigned short BlockAlign;
                    unsigned short BitsPerSample;
                    char Subchunk2Id[4];
                    unsigned long Subchunk2Size;
                } RiffWaveHeader;
#pragma pack(pop)

                bool IsRiffWaveHeader(const char *);
            }
        }
    }
}

#endif //RZ4M_RIFFWAVE_FORMAT_H
