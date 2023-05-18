
#include "precompiled.h"
#include "compositor/microshader.hpp"

using namespace ie::compositor;

MicroShader::MicroShader(std::filesystem::path const& file_path) {
    std::fstream shader_stream(file_path, std::ios::in);

    if(!shader_stream.is_open()) {
        throw std::runtime_error(std::format("Can't find a shader file '{}'", file_path.generic_string()));
    }

    std::stringstream buffer_stream;
    buffer_stream << shader_stream.rdbuf();

    parse_shader(buffer_stream.view());
}

auto MicroShader::parse_shader(std::string_view const buffer) -> void {
    size_t offset = buffer.find("// microshader: ");
    if(offset != 0) {
        throw std::runtime_error("MicroShader parse error (Unknown format)");
    }

    offset = std::string_view("// microshader: ").size();

    auto shader_name = std::string_view(buffer.begin() + offset, buffer.begin() + buffer.find_first_of('\n', offset));

    if(shader_name.empty()) {
        throw std::runtime_error("MicroShader parse error (Unknown shader name)");
    }

    std::cout << "Shader Name: " << shader_name << std::endl;

    offset = buffer.find("struct ");
    if(offset == std::string_view::npos) {
        throw std::runtime_error("MicroShader parse error (Unknown input data)");
    }

    auto input_struct_decl = std::string_view(buffer.begin() + offset, buffer.begin() + buffer.find_first_of('\n', offset));
    offset = input_struct_decl.find("_in_t ");
    
    if(offset == std::string_view::npos) {
        throw std::runtime_error("MicroShader parse error (Unknown input data)");
    }

    offset = std::string_view("struct ").size();
    auto input_struct_name = std::string_view(input_struct_decl.begin() + offset, input_struct_decl.begin() + input_struct_decl.find_first_of(' ', offset));

    std::cout << input_struct_name << std::endl;

    
}