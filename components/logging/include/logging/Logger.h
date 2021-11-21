#pragma once

#include "esp_log.h"

#include "fmt/core.h"

namespace logging
{
    class Logger
    {
    public:
        template<typename... T>
        static void verbose(const std::string& tag, fmt::format_string<T...> message, T&&... args)
        {
            ESP_LOGV(tag.c_str(), "%s", fmt::format(message, args...).c_str());
        };

        template<typename... T>
        static void debug(const std::string& tag, fmt::format_string<T...> message, T&&... args)
        {
            ESP_LOGD(tag.c_str(), "%s", fmt::format(message, args...).c_str());
        };

        template<typename... T>
        static void info(const std::string& tag, fmt::format_string<T...> message, T&&... args)
        {
            ESP_LOGI(tag.c_str(), "%s", fmt::format(message, args...).c_str());
        };

        template<typename... T>
        static void warning(const std::string& tag, fmt::format_string<T...> message, T&&... args)
        {
            ESP_LOGW(tag.c_str(), "%s", fmt::format(message, args...).c_str());
        };

        template<typename... T>
        static void error(const std::string& tag, fmt::format_string<T...> message, T&&... args)
        {
            ESP_LOGE(tag.c_str(), "%s", fmt::format(message, args...).c_str());
        };
    };
}
