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

#include <iostream>

#include "main.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

void PrintLogo(bool withEndl = true) {
    std::cout << rz4::Logo;
    if (withEndl) {
        std::cout << std::endl;
    }
}

int PrintUsage() {
    PrintLogo(false);
    std::cout << rz4::UsageMessage << std::endl;
    return 0;
}

bool ParseArgs(rz4::Types::CLIOptions &Options, int argc, char *argv[]) {
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
        Options.OutFile = vm["out"].as<std::string>();
    }

    if (vm.find("outdir") != vm.end()) {
        Options.OutDir = vm["outdir"].as<std::string>();
    }

    if (vm.find("bufsize") != vm.end()) {
        Options.BufferSize =
                static_cast<unsigned int>(rz4::Utils::MemToll(vm["bufsize"].as<std::string>()));
    }

    if (vm.find("wav") != vm.end()) {
        Options.EnableRiffWave = vm["wav"].as<bool>();
    }

    if (vm.find("verbose") != vm.end()) {
        Options.Verbose = vm["verbose"].as<bool>();
    }

    return vm.find("help") == vm.end();
}

bool IsAffirmativeAnswer() {
    std::string ch;
    std::cin >> ch;
    std::transform(ch.begin(), ch.end(), ch.begin(), ::tolower);
    return ch == "y" || ch == "yes" || ch == "yep";
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
    rz4::Types::CLIOptions CLIOptions;
    CLIOptions.Command = Command;
    CLIOptions.BufferSize = BUFFER_SIZE;
    CLIOptions.Verbose = true;
    CLIOptions.EnableRiffWave = true;

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
                CurrentPath / rz4::Utils::GenerateUniqueFolderName(CLIOptions.InFile.string(), "extract_data");

        if (!fs::exists(CLIOptions.OutDir)) {
            fs::create_directory(CLIOptions.OutDir);
        }
    }

    std::cout
        << "-> Buffer size: "
        << rz4::Utils::HumanizeSize(CLIOptions.BufferSize)
        << std::endl;

    rz4::Types::ScannerOptions ScannerOptions;
    ScannerOptions.FileName = CLIOptions.InFile;
    ScannerOptions.BufferSize = CLIOptions.BufferSize;
    ScannerOptions.EnableRiffWave = CLIOptions.EnableRiffWave;

    auto StartTime = std::chrono::high_resolution_clock::now();
    auto StartScanTime = std::chrono::high_resolution_clock::now();
    rz4::Engine::Scanner *Scanner = new rz4::Engine::Scanner(ScannerOptions);  
    rz4::Types::ScannerCallbackHandle Callback = [](rz4::Types::StreamInfo *Stream) {
        std::cout
            << boost::format("--> Found %s @ 0x%016X (%s)")
            % Stream->FileType
            % Stream->Offset
            % rz4::Utils::HumanizeSize(Stream->Size)
            << std::endl;
    };

    std::cout << "-> Scanning..." << std::endl;
    if (CLIOptions.Verbose) std::cout << std::endl;
    Scanner->Start(CLIOptions.Verbose ? Callback : nullptr);
    Scanner->Close();

    auto EndScanTime = std::chrono::high_resolution_clock::now();

    if (CLIOptions.Command == COMMAND_EXTRACT) {
        std::cout << std::endl << "-> Extract data..." << std::endl;
           
        rz4::Engine::Ejector *Ejector =
            new rz4::Engine::Ejector(CLIOptions.InFile.string(), CLIOptions.BufferSize);

        std::list<rz4::Types::StreamInfo> *FoundStreams = Scanner->GetListOfFoundStreams();

        unsigned long CountOfFoundStreams = Scanner->GetCountOfFoundStreams();
        unsigned long Counter = 1;

        for (auto Stream : *FoundStreams) {
            const fs::path Path = CLIOptions.OutDir / boost::str(boost::format("%016X-%016X.%s")
                % Stream.Offset
                % Stream.Size
                % Stream.Ext);

            Ejector->Extract(Stream.Offset, Stream.Size, Path.string());
            std::cout << "\r" << "-> " << Counter * 100 / CountOfFoundStreams << "% completed.";
            Counter++;
        }

        if (Counter > 1) {
            std::cout << std::endl;
        }

        delete Ejector;
    }

    if (CLIOptions.Command == COMMAND_COMPRESS) {
        std::cout << std::endl << "-> Compress data..." << std::endl;

        if (CLIOptions.OutFile.empty()) {
            CLIOptions.OutFile = CLIOptions.InFile.string() + ".rzf";
        }

        // For pause global timer
        auto StartLoopTime = std::chrono::high_resolution_clock::now();
        while (fs::exists(CLIOptions.OutFile)) {
            std::cout << "-> Output file \"" << CLIOptions.OutFile.string() << "\" was existing. Overwrite? (y/n) ";

            if (IsAffirmativeAnswer()) {
                fs::remove(CLIOptions.OutFile);
                // Maybe has the meaning break loop here
                // But if file don't removing - we have
                // additional check
            } else {
                std::string OutFile;
                std::cout << "-> Enter file name of output file: ";
                std::cin.ignore();
                std::getline(std::cin, OutFile);
                CLIOptions.OutFile = OutFile;
            }
        }

        StartTime += std::chrono::high_resolution_clock::now() - StartLoopTime;

        rz4::Types::CompressorOptions CompressorOptions;
        CompressorOptions.FileName = CLIOptions.InFile;
        CompressorOptions.OutFile = CLIOptions.OutFile;
        CompressorOptions.BufferSize = CLIOptions.BufferSize;
        CompressorOptions.ListOfStreams = Scanner->GetListOfFoundStreams();
        CompressorOptions.EnableRiffWave = CLIOptions.EnableRiffWave;

        rz4::Engine::Compressor *Compressor = new rz4::Engine::Compressor(CompressorOptions);
        Compressor->Start();
        Compressor->Close();
    }

    auto EndTime = std::chrono::high_resolution_clock::now();

    // Print info after all operations
    auto DiffTime = EndTime - StartTime;
    auto DiffScanTime = EndScanTime - StartScanTime;

    std::cout << std::endl;
    std::cout << "-> Scan time: " << rz4::Utils::PrettyTime(DiffScanTime) << std::endl;
    std::cout << "-> Process time: " << rz4::Utils::PrettyTime(DiffTime) << std::endl;

    std::cout << "-> Found media streams: " << Scanner->GetCountOfFoundStreams() << std::endl;
    std::cout
        << "-> Size of found media streams: "
        << rz4::Utils::HumanizeSize(Scanner->GetSizeOfFoundStreams())
        << " ("
        << Scanner->GetSizeOfFoundStreams()
        << " bytes)"
        << std::endl;

    return 0;
}