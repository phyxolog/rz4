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

#include <iostream>

#include "main.hpp"

namespace po = boost::program_options;

int Usage() {
    std::cout
        << rz4m::logo
        << rz4m::usage_message
        << std::endl;
    return 0;
}

short int ParseArgs(rz4m::Types::CLIOptions &options, int argc, char *argv[]) {
    po::options_description desc("");
    desc.add_options()
            ("help,h", "Show help")
            ("wav,w", po::value<bool>())
            ("out,o", po::value<std::string>())
            ("outdir,d", po::value<std::string>())
            ("bufsize,b", po::value<std::string>());

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    po::store(parsed, vm);
    po::notify(vm);

    if (vm.count("out")) {
        options.OutFile = vm["out"].as<std::string>();
    }

    if (vm.count("outdir")) {
        options.OutDir = vm["outdir"].as<std::string>();
    }

    if (vm.count("bufsize")) {
        options.BufferSize =
                static_cast<unsigned int>(rz4m::Utils::MemToll(vm["bufsize"].as<std::string>()));
    }

    if (vm.count("wav")) {
        options.EnableWav = vm["wav"].as<bool>();
    }

    if (vm.count("help")) {
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        return Usage();
    }

    return 0;
}