#pragma once

#include <gpiod.hpp>
#include <string>

class GPIO_Output {
public:
    static constexpr bool ACTIVE_HIGH = false;
    static constexpr bool ACTIVE_LOW = true;
    static constexpr bool ACTIVE = true;
    static constexpr bool INACTIVE = false;

    struct Config {
        std::string chip_name;
        unsigned int line_number;
        std::string consumer_name;
        bool active_low = ACTIVE_HIGH;
        bool initial_value = INACTIVE;
    };

    // Constructor
    explicit GPIO_Output(const Config& config);

    // Destructor
    ~GPIO_Output();

    // Delete copy constructor and assignment operator
    GPIO_Output(const GPIO_Output&) = delete;
    GPIO_Output& operator=(const GPIO_Output&) = delete;

    // Move constructor and assignment operator
    GPIO_Output(GPIO_Output&&) = default;
    GPIO_Output& operator=(GPIO_Output&&) = default;

    // Set output value - boolean version
    void set(bool value);

    // Set output value - integer version
    void set(int value);

    // Toggle output value
    void toggle();

    // Get current value - boolean version
    bool get() const;

    // Get current value - integer version
    int get_value() const;

private:
    gpiod::line m_line;
    bool m_current_value;  // Cached output state
};
