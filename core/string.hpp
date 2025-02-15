// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    /*!
        \brief Modes that used when trim
    */
    enum class trim_mode
    {
        both,
        beg,
        end
    };

    /*!
        \brief Trim string
        \param[in|out] source Input string that will modified
        \param[in] trim_mode Mode that will used to trim
    */
    inline auto trim(std::string& source, trim_mode const trim_mode) -> void
    {
        if (trim_mode == trim_mode::beg || trim_mode == trim_mode::both)
        {
            source.erase(source.begin(), std::find_if(source.begin(), source.end(),
                                                      [](unsigned char ch) { return !std::isspace(ch); }));
        }

        if (trim_mode == trim_mode::end || trim_mode == trim_mode::both)
        {
            source.erase(
                std::find_if(source.rbegin(), source.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
                source.end());
        }
    }

    /*!
        \brief Errors that will get when convert
    */
    enum class convert_error
    {
        invalid_argument
    };

    /*!
        \brief Convert string_view to number
        \param[in] source Input string_view that will converted to number
        \return Number or error
    */
    template <typename Type>
    inline auto ston(std::string_view const source) -> std::expected<Type, convert_error>
    {
        Type output;
        auto result = std::from_chars(source.data(), source.data() + source.size(), output);

        if (result.ec == std::errc::invalid_argument)
        {
            return std::unexpected(convert_error::invalid_argument);
        }
        else
        {
            return output;
        }
    }

    inline auto stob(std::string_view const source) -> std::expected<bool, convert_error>
    {
        int32_t output;
        auto result = std::from_chars(source.data(), source.data() + source.size(), output);

        if (result.ec == std::errc::invalid_argument)
        {
            return std::unexpected(convert_error::invalid_argument);
        }
        else
        {
            return output;
        }
    }
} // namespace ionengine::core