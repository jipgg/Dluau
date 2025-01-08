#pragma once
#include <source_location>
#include <vector>
#include <string>
#include <iostream>

class Error {
    std::vector<std::source_location> traceback_;
    std::string message_;
public:
    Error(const Error& a) = default; 
    Error& operator=(const Error& a) = default;
    Error(Error&& a) noexcept = default;
    Error& operator=(Error& a) noexcept = default;
    ~Error() = default;
    explicit Error(std::string message, std::source_location sl = std::source_location::current());
    Error& propagate(std::source_location sl = std::source_location::current());
    const std::vector<std::source_location>& traceback() const;
    const std::string& message() const;
    std::string formatted() const;
    friend std::ostream& operator<<(std::ostream& os, const Error& error);
};
