// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/components/camera.h>

using namespace ionengine::scene::components;

float Camera::aspect_ratio() const {

    return _aspect_ratio;
}

Camera& Camera::aspect_ratio(float const value) {

    _aspect_ratio = value;
    return *this;
}

float Camera::field_of_view() const {

    return _field_of_view;
}

Camera& Camera::field_of_view(float const value) {

    _field_of_view = value;
    return *this;
}