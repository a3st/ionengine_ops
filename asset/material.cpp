// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/material.h>
#include <lib/exception.h>
#include <asset/asset_manager.h>

using namespace ionengine::asset;

Material::Material(std::filesystem::path const& file_path) : Asset(file_path) {

    std::string from_path_string = file_path.string();

    JSON_MaterialDefinition document;
    json5::error result = json5::from_file(from_path_string, document);

    if(result != json5::error::none) {
        throw lib::Exception(std::format("File load {}!", from_path_string));
    }

    _name = document.name;
}

std::string_view Material::name() const {

    return _name;
}