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

#ifndef RZ4M_RIFFWAVE_FORMAT_H
#define RZ4M_RIFFWAVE_FORMAT_H

#include <cstring>

namespace rz4m {
    namespace Engine {
        namespace Formats {
            namespace RiffWave {
                typedef struct RiffWaveHeader {
                    char  RiffHeader[4]; /* RIFF */
                    int   WavSize;       /* File size = `wav_size` + 8 */
                    char  WaveHeader[4]; /* WAVE */
                    char  FmtHeader[4];
                    int   FmtChunkSize;
                    short AudioFormat;
                    short NumChannels;
                    int   SampleRate;
                    int   ByteRate;
                    short SampleAlignment;
                    short BitDepth;
                    char  DataHeader[4];
                    int   DataBytes;
                } RiffWaveHeader;

                bool IsRiffWaveHeader(const char *);
            }
        }
    }
}

#endif //RZ4M_RIFFWAVE_FORMAT_H
