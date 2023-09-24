#pragma once

#include "esp_log.h"

#include "fmt/core.h"

namespace logging
{
    enum class LogLevel
    {
        Error = ESP_LOG_ERROR,
        Warning = ESP_LOG_WARN,
        Info = ESP_LOG_INFO,
        Debug = ESP_LOG_DEBUG,
        Verbose = ESP_LOG_VERBOSE
    };

    class Logger
    {
    public:
        static void setLogLevel(const std::string& tag, LogLevel logLevel)
        {
            esp_log_level_set(tag.c_str(), static_cast<esp_log_level_t>(logLevel));
        }

        template<typename... T>
        static void verbose(const std::string& tag, fmt::format_string<T...> message, T&&... args)
        {
            ESP_LOGV(tag.c_str(), "%s", fmt::vformat(message, fmt::make_format_args(args...)).c_str());
        };

        template<typename... T>
        static void debug(const std::string& tag, fmt::format_string<T...> message, T&&... args)
        {
            ESP_LOGD(tag.c_str(), "%s", fmt::vformat(message, fmt::make_format_args(args...)).c_str());
        };

        template<typename... T>
        static void info(const std::string& tag, fmt::format_string<T...> message, T&&... args)
        {
            ESP_LOGI(tag.c_str(), "%s", fmt::vformat(message, fmt::make_format_args(args...)).c_str());
        };

        template<typename... T>
        static void warning(const std::string& tag, fmt::format_string<T...> message, T&&... args)
        {
            ESP_LOGW(tag.c_str(), "%s", fmt::vformat(message, fmt::make_format_args(args...)).c_str());
        };

        template<typename... T>
        static void error(const std::string& tag, fmt::format_string<T...> message, T&&... args)
        {
            ESP_LOGE(tag.c_str(), "%s", fmt::vformat(message, fmt::make_format_args(args...)).c_str());
        };
    };
}
