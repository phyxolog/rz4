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

bool ParseArgs(rz4m::Types::CLIOptions &options, int argc, char *argv[]) {
    po::options_description desc("");
    desc.add_options()
            ("help,h", "Show help")
            ("verbose,v", po::value<bool>())
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

    if (vm.find("out") != vm.end()) {
        options.OutFile = vm["out"].as<std::string>();
    }

    if (vm.find("outdir") != vm.end()) {
        options.OutDir = vm["outdir"].as<std::string>();
    }

    if (vm.find("bufsize") != vm.end()) {
        options.BufferSize =
                static_cast<unsigned int>(rz4m::Utils::MemToll(vm["bufsize"].as<std::string>()));
    }

    if (vm.find("wav") != vm.end()) {
        options.EnableWav = vm["wav"].as<bool>();
    }

    if (vm.find("verbose") != vm.end()) {
        options.Verbose = vm["verbose"].as<bool>();
    }

    return vm.find("help") == vm.end();
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

    // Init cli options
    rz4m::Types::CLIOptions CLIOptions;
    CLIOptions.Command = Command;
    CLIOptions.BufferSize = BUFFER_SIZE;
    CLIOptions.Verbose = true;
    CLIOptions.EnableWav = true;

    // Input file always the last argument
    CLIOptions.InFile = argv[argc - 1];

    if (!ParseArgs(CLIOptions, argc, argv)) {
        return PrintUsage();
    }

    PrintLogo();

    // Buffer size must be greater than 0
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
        CLIOptions.OutDir =
                CurrentPath / rz4m::Utils::GenerateUniqueFolderName(CLIOptions.InFile.string(), "extract_data");
    }

    if (CLIOptions.Command == COMMAND_EXTRACT
        && !fs::exists(CLIOptions.OutDir)) {
        fs::create_directory(CLIOptions.OutDir);
    }

    std::cout
        << "-> Buffer size: "
        << rz4m::Utils::HumanizeSize(CLIOptions.BufferSize)
        << std::endl;

    rz4m::Types::ScannerOptions ScannerOptions;
    ScannerOptions.FileName = CLIOptions.InFile;
    ScannerOptions.BufferSize = CLIOptions.BufferSize;
    ScannerOptions.EnableWav = CLIOptions.EnableWav;

    auto StartTime = std::chrono::steady_clock::now();
    rz4m::Engine::Scanner *Scanner = new rz4m::Engine::Scanner(ScannerOptions);  
    rz4m::Types::ScannerCallbackHandle Callback = [](rz4m::Types::StreamInfo *Stream) {
        std::cout
            << boost::format("--> Found %s @ 0x%016X (%s)")
            % Stream->FileType
            % Stream->Offset
            % rz4m::Utils::HumanizeSize(Stream->FileSize)
            << std::endl;
    };

    std::cout << "-> Scanning..." << std::endl << std::endl;
    Scanner->Start(CLIOptions.Verbose ? Callback : nullptr);
    Scanner->Close();

    if (CLIOptions.Command == COMMAND_EXTRACT) {
        std::cout << std::endl << "-> Extract data..." << std::endl;
           
        rz4m::Engine::Ejector *Ejector =
            new rz4m::Engine::Ejector(CLIOptions.InFile.string(), CLIOptions.BufferSize);

        std::list<rz4m::Types::StreamInfo> *FoundStreams = Scanner->GetListOfFoundStreams();

        unsigned long CountOfFoundStreams = Scanner->GetCountOfFoundStreams();
        unsigned long Counter = 1;

        for (auto Stream : *FoundStreams) {
            const fs::path Path = CLIOptions.OutDir / boost::str(boost::format("%016X-%016X.%s")
                % Stream.Offset
                % Stream.FileSize
                % Stream.Ext);

            Ejector->Extract(Stream.Offset, Stream.FileSize, Path.string());
            std::cout << "\r" << "-> " << Counter * 100 / CountOfFoundStreams << "% completed.";
            Counter++;
        }

        if (Counter > 1) {
            std::cout << std::endl;
        }

        delete Ejector;
    }

    auto EndTime = std::chrono::steady_clock::now();

    // Print info after all operations
    auto DiffTime = EndTime - StartTime;
    std::cout << std::endl << "-> Process time: "
        << rz4m::Utils::PrettyTime(
            static_cast<uintmax_t>(std::chrono::duration <double, std::milli>(DiffTime).count())
        ) << std::endl;

    std::cout << "-> Found media streams: " << Scanner->GetCountOfFoundStreams() << std::endl;
    std::cout
        << "-> Size of found media streams: "
        << rz4m::Utils::HumanizeSize(Scanner->GetSizeOfFoundStreams())
        << " ("
        << Scanner->GetSizeOfFoundStreams()
        << " bytes)"
        << std::endl;

    return 0;
}