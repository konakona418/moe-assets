#include <iostream>
#include <files.h>
#include <args.h>
#include <exec.h>

const static std::string BUILD_DIR = "build";

int main(const int argc, char** argv) {
    const auto args = moe_assets::ArgParser(argc, argv).parse();
    if (!args.has_value()) {
        std::cerr << "Invalid arguments" << std::endl;
        return 1;
    }
    const std::string input = args.value().input_path;
    std::string output = args.value().output_path;

    moe_assets::clear_folder(output, BUILD_DIR);

    auto rc_file = moe_assets::ResourceFileGenerator(moe_assets::FileSystemIterator(input).get_files());
    auto rc_content = rc_file.get();
    auto file_map = rc_file.get_file_map();

    if (!moe_assets::exists(output, BUILD_DIR)) {
        moe_assets::create_folder(output, BUILD_DIR);
    }

    if (!moe_assets::exists(moe_assets::concat_path(output, BUILD_DIR), "res")) {
        moe_assets::create_folder(moe_assets::concat_path(output, BUILD_DIR), "res");
    }

    moe_assets::write_file(moe_assets::concat_path(output, BUILD_DIR), "resource.h",
                           moe_assets::ResourceFileRegisterGenerator::get_header());
    moe_assets::write_file(moe_assets::concat_path(output, BUILD_DIR), "resource.cpp",
                           moe_assets::ResourceFileRegisterGenerator::get_source());
    moe_assets::write_file(moe_assets::concat_path(output, BUILD_DIR), "res.h",
                           moe_assets::ResourceFileRegisterGenerator(file_map).get());

    moe_assets::write_file(moe_assets::concat_path(output, BUILD_DIR, "res"), "rc.rc", rc_content);
    const int result = moe_assets::exec("windres")
            .add_arg(moe_assets::concat_path(output, BUILD_DIR, "res", "rc.rc"))
            .add_arg("-O").add_arg("coff")
            .add_arg("-o").add_arg(moe_assets::concat_path(output, BUILD_DIR, "res", "rc.o"))
            .exec();
    if (result != 0) {
        std::cerr << "Failed to compile resource file" << std::endl;
        return 1;
    }

    return result;
}
