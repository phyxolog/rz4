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

#include "Utils.h"

namespace rz4m {
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

        std::string HumanizeSize(uintmax_t Bytes) {
            if (Bytes == 0) return "0 B";
            int exp = (int)(log(Bytes) / log(1024));
            std::string prefix = std::string("BKMGTPE").substr(exp, 1) + (exp == 0 ? "" : "b");
            return boost::str(boost::format("%.2f %s") % (Bytes / pow(1024, exp)) % prefix);
        }

        std::string GenerateUniqueFolderName(std::string FirstPrefix, std::string SecondPrefix) {
            return std::string(FirstPrefix + "_" + SecondPrefix + "_" + std::to_string(std::chrono::seconds(std::time(NULL)).count()));
        }
    }
}
