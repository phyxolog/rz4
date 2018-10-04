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

namespace fs = boost::filesystem;
namespace po = boost::program_options;

void PrintLogo(bool withEndl = true) {
    std::cout << rz4m::logo;
    if (withEndl) {
        std::cout << std::endl;
    }
}

int PrintUsage() {
    PrintLogo(false);
    std::cout << rz4m::usage_message << std::endl;
    return 0;
}

int ParseArgs(rz4m::Types::CLIOptions &options, int argc, char *argv[]) {
    po::options_description desc("");
    desc.add_options()
            ("help,h", "Show help")
            ("wav,w", po::value<bool>())
            ("out,o", po::value<std::string>())
            ("outdir,d", po::value<std::string>())
            ("bufsize,b", po::value<std::string>());

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc, argv)
            .options(desc)
            .allow_unregistered()
            .run();
    po::store(parsed, vm);

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
        return PrintUsage();
    }

    // Get current path
    const fs::path CurrentPath = fs::current_path();

    // `command` always the first argument
    std::string Command = argv[1];
    std::transform(Command.begin(), Command.end(), Command.begin(), ::tolower);

    // Init options
    rz4m::Types::CLIOptions CLIOptions;
    CLIOptions.Command = Command;
    CLIOptions.BufferSize = BUFFER_SIZE;
    CLIOptions.EnableWav = true;

    // Input file always the last argument
    CLIOptions.InFile = argv[argc - 1];

    if (ParseArgs(CLIOptions, argc, argv) == 1) {
        return PrintUsage();
    }

    PrintLogo();

    if (CLIOptions.BufferSize <= 0) {
        CLIOptions.BufferSize = BUFFER_SIZE;
    }

    if (CLIOptions.InFile.empty()
        || !fs::exists(CLIOptions.InFile)) {
        std::cout << "[!] No such input file!" << std::endl;
        return 1;
    }

    if (fs::file_size(CLIOptions.InFile) == 0) {
        std::cout << "[!] Input file was empty {fs::file_size == 0}!" << std::endl;
        return 1;
    }

    if (CLIOptions.Command == COMMAND_EXTRACT
        && CLIOptions.OutDir.empty()) {
        CLIOptions.OutDir = CurrentPath / rz4m::Utils::GenerateUniqueFolderName(CLIOptions.InFile, "extract_data");
    }

    if (CLIOptions.Command == COMMAND_EXTRACT
        && !fs::exists(CLIOptions.OutDir)) {
        fs::create_directory(CLIOptions.OutDir);
    }

    std::cout << "-> Buffer size: " << rz4m::Utils::HumanizeSize(CLIOptions.BufferSize) << std::endl;

    return 0;
}