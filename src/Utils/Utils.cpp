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

#include "Utils.hpp"

namespace rz4 {
    namespace Utils {
        /*
         * Find char in array.
         * If found - return index.
         * If not - return -1.
         */
        int CharMatch(const char *Buffer, unsigned int BufferSize, char Needle, unsigned int Offset) {
            if (Offset >= BufferSize) {
                return -1;
            }

            BufferSize -= Offset;
            const char* result = reinterpret_cast<const char*>(std::memchr(Buffer + Offset, Needle, BufferSize));
            return result ? static_cast<int>(result - Buffer) : -1;
        }

        /*
         * Convert a string representing an amount of memory into the number of
         * bytes, so for instance memtoll("1Gb") will return 1073741824 that is
         * (1024*1024*1024).
         */
        long long MemToll(std::string str) {
            if (str.length() == 0) {
                return 0;
            }

            size_t nondigit_pos = str.find_first_not_of("0123456789");

            if (nondigit_pos == std::string::npos) {
                return std::stoll(str);
            }

            long long result = 0;
            std::string digits = str.substr(0, nondigit_pos);
            std::string u = str.substr(nondigit_pos, str.length());
            std::transform(u.begin(), u.end(), u.begin(), ::tolower);

            if (digits.length() == 0) {
                return result;
            } else {
                result = std::stoll(digits);
            }

            std::map<std::string, long> umul = {
                    { "b",  1                   },
                    { "k",  1000                },
                    { "kb", 1024                },
                    { "m",  1000  * 1000        },
                    { "mb", 1024  * 1024        },
                    { "g",  1000L * 1000 * 1000 },
                    { "gb", 1024L * 1024 * 1024 },
            };

            auto mul = umul.find(u);
            if (mul != umul.end()) {
                return result * mul->second;
            } else {
                return result;
            }
        }

        /*
         * A simple method to get a human readable file size string from a number (bytes).
         */
        std::string HumanizeSize(uintmax_t Bytes) {
            if (Bytes == 0) return "0 B";
            int exp = static_cast<int>(log(Bytes) / log(1024));
            std::string prefix = std::string("BKMGTPE").substr(static_cast<unsigned long>(exp), 1) + (exp == 0 ? "" : "b");
            return boost::str(boost::format("%.2f %s") % (Bytes / pow(1024, exp)) % prefix);
        }

        /*
         * Generate unique folder name with prefixes and timestamp.
         */
        std::string GenerateUniqueFolderName(std::string FirstPrefix, std::string SecondPrefix) {
            return std::string(FirstPrefix + "_" + SecondPrefix + "_" + std::to_string(std::chrono::seconds(std::time(nullptr)).count()));
        }

        /*
        * Convert ms to human-oriented time string
        */
        std::string PrettyTime(uintmax_t Time) {
            const int Size = 4;
            int Step = -1;
            uintmax_t Steps[Size] = {
                1000,
                1000 * 60,
                1000 * 60 * 60,
                1000 * 60 * 60 * 24
            };

            for (int i = 0; i < Size; i++) {
                if (Time < Steps[i]) {
                    Step = i;
                    break;
                }
            }

            switch (Step) {
            case 0:
                return boost::str(boost::format("%lims") % Time);
                break;
            case 1: // only ms
                return boost::str(boost::format("%li second(s), %lims") % (Time / 1000) % (Time % 1000));
                break;
            case 2: // minutes
                return boost::str(boost::format("%li minute(s), %li second(s)")
                    % (Time / (1000 * 60))
                    % ((Time / 1000) % 60));
                break;
            case 3: // hours
                return boost::str(boost::format("%li hour(s), %li minute(s), %li second(s)")
                    % (Time / (1000 * 60 * 60))
                    % ((Time / (1000 * 60)) % 60)
                    % ((Time / 1000) % 60));
                break;
            default: // max (days)
                return boost::str(boost::format("%li day(s), %li hour(s), %li minute(s)")
                    % (Time / (1000 * 60 * 60 * 24))
                    % ((Time / (1000 * 60 * 60)) % 24)
                    % ((Time / (1000 * 60)) % 60));
            }
        }
    }
}
