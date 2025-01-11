#include "GPIO_Output.hpp"
#include <stdexcept>

GPIO_Output::GPIO_Output(const Config& config) {
    try {
        // Open GPIO chip and get the line
        gpiod::chip chip(config.chip_name);
        m_line = chip.get_line(config.line_number);
        
        // Check if line is already in use
        if (m_line.is_used()) {
            throw std::runtime_error("GPIO line " + std::to_string(config.line_number) + " is already in use");
        }

        // Configure line as output with specified polarity
        m_line.request({
            .consumer = config.consumer_name.c_str(),
            .request_type = gpiod::line_request::DIRECTION_OUTPUT,
            .flags = config.active_low ? gpiod::line_request::FLAG_ACTIVE_LOW : 0
        });
        set(config.initial_value);

    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to initialize GPIO output: " + std::string(e.what()));
    }
}

GPIO_Output::~GPIO_Output() {
    try {
        if (m_line.is_requested()) {
            m_line.release();
        }
    } catch (...) {
        // Destructors should not throw
    }
}

void GPIO_Output::set(bool value) {
    try {
        m_line.set_value(value ? 1 : 0);
        m_current_value = value;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to set GPIO value: " + std::string(e.what()));
    }
}

void GPIO_Output::set(int value) {
    if (value != 0 && value != 1) {
        throw std::invalid_argument("GPIO value must be 0 or 1");
    }
    set(value == 1);
}

void GPIO_Output::toggle() {
    set(!m_current_value);
}

bool GPIO_Output::get() const {
    return m_current_value;
}

int GPIO_Output::get_value() const {
    return m_current_value ? 1 : 0;
}
