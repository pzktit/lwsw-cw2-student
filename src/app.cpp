/* ***************************************************************** 

 Exercise 2: Managing GPIO Output
 by Piotr ZAWADZKI

 Application Features:
 1. Long Button Press: Stops the application gracefully.
 2. Short Button Press: Cycles through LED triggers (default-on, timer, heartbeat).
 3. LED Management: Controlled by `gpio-led` module via sysfs interface.
 4. Screen Backlight Control: Managed by a GPIO line using `libgpiod` encapsulated in the `GPIO_Output` class.

 Please follow the TODOs in the code to complete the exercise.

 **************************************************************** */

#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>
#include <linux/input.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include "st7789v2.hpp"
#include "GPIO_Output.hpp"
#include "GPIO_Led.hpp"

// Hardware configuration and initialization
const struct Hardware_config {
    const ST7789::Config displayConfig;
    const GPIO_Led::Config LED;
    const GPIO_Output::Config BacklightLED;
} hardwareConfig = {
    .displayConfig = {
        .spiDevice = "/dev/spidev0.0",
        .speedHz = 30000000,  // 30 MHz, in theory ST7789v2 should support up to 62.5 MHz
        .gpioChip = "gpiochip0",
        .dcPin = 22,
        .resetPin = 27
    },
    .LED = {"lwsw-led"},
    .BacklightLED = {"gpiochip0", 17, "ex3-BL", GPIO_Output::ACTIVE_HIGH, GPIO_Output::ACTIVE},
};

// Global variable for synchronization and state sharing
// between the monitoring threads and the main thread
// see "custom_types.hpp" for type definition 
struct Application_state {
    std::atomic<bool> keepRunning;
    std::atomic<size_t> triggerIndex ;
} appState = {
    .keepRunning = true,
    .triggerIndex = 0
};

const std::vector<std::string> LED_triggers = {"default-on", "timer", "heartbeat"};


// Gracefull programm cloase on Ctrl+C
void sigint_handler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
    appState.keepRunning.store(false);
}

// Prototypes for the threads
// void display_thread(ST7789& display, std::atomic<bool>& keepRunning, std::atomic<bool>& resetDisplay) ;
void led_thread(Application_state & appState, GPIO_Led & led, const std::vector<std::string> & LED_triggers) ;
void button_thread( Application_state & appState, const std::vector<std::string> & inputDevices) ;
void gpio_thread(Application_state & appState, GPIO_Output & gpioLine) ;

int main() {
    try {
        // Register signal and signal handler
        signal(SIGINT, sigint_handler);
        ST7789 display(hardwareConfig.displayConfig);
        GPIO_Output backlightLed(hardwareConfig.BacklightLED);
        GPIO_Led led(hardwareConfig.LED);

        backlightLed.set(GPIO_Output::INACTIVE);
        display.clearScreen(ST7789::Colors::WHITE);
        backlightLed.set(GPIO_Output::ACTIVE);

        // Initialize the application config
        // std::thread display_task( display_thread, std::ref(display), std::ref(appState.keepRunning), std::ref(appState.resetDisplay) );
        std::thread led_task( led_thread, std::ref(appState), std::ref(led), LED_triggers);
        std::thread gpio_task( gpio_thread, std::ref(appState), std::ref(backlightLed) );
        std::thread button_task( button_thread, std::ref(appState), std::vector<std::string>({ "/dev/input/event0" }) ) ;

        while (appState.keepRunning.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Main thread: waiting for child threads stop." << std::endl;
        // Join the monitoring thread before exiting (in practice, the program runs indefinitely here)
        button_task.join() ;
        gpio_task.join();
        led_task.join();
        // display_task.join();

    } catch (const std::exception &e) {
        std::cerr << "An error occurred in main thread: " << e.what() << std::endl;
    }
    std::cout << "Application gracefully stopped." << std::endl;
    return 0;
}

void led_thread(Application_state & appState, GPIO_Led& gpioLed, const std::vector<std::string> & triggers) {
    try {
        const auto num_triggers = triggers.size();
        static size_t triggerIndex = appState.triggerIndex.load(); // keep local copy of the trigger index
        while ( true ) {
            // Wait for the trigger index to change or application finish
            while (appState.keepRunning.load() && triggerIndex == appState.triggerIndex.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            if (!appState.keepRunning.load()) { // Exit the infinite loop if the application is finishing
                break;
            }
            triggerIndex = appState.triggerIndex.load() % num_triggers; // Update the local copy of the trigger index
            appState.triggerIndex.store(triggerIndex); // Added to keep the global trigger index within the range
            gpioLed.setTrigger(triggers[triggerIndex]); // Set the new trigger
        }
    } catch (const std::exception &e) {
        std::cerr << "An error occurred in LED thread: " << e.what() << std::endl;
    }
}

void gpio_thread(Application_state & appState, GPIO_Output & gpioLine) {
    try {
        static size_t triggerIndex = appState.triggerIndex.load(); // Keep a local copy of the trigger index
        while (true) {
            // Wait for the trigger index to change or application finish
            while (appState.keepRunning.load() && triggerIndex == appState.triggerIndex.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            if (!appState.keepRunning.load()) { // Exit the infinite loop if the application is finishing
                break;
            }
            triggerIndex = appState.triggerIndex.load(); // Update the local copy of the trigger index

            switch (triggerIndex) {
                case 0: // this corresponds to "default-on" trigger from GPIO-Led
                    gpioLine.set(GPIO_Output::ACTIVE);
                    while (appState.keepRunning.load() && triggerIndex == appState.triggerIndex.load()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }
                    break;
                case 1: { //TODO: add code that emulates "timer" trigger: 500ms -ON, 500ms-OFF
                    while (appState.keepRunning.load() && triggerIndex == appState.triggerIndex.load()) {
                        // gpioLine.??? ;
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }
                    break;
                }
                case 2: { //TODO: add code that emulates "heartbeat": 200ms-ON, 200ms-OFF, 200ms-ON, 1000ms-OFF,
                    while (appState.keepRunning.load() && triggerIndex == appState.triggerIndex.load()) {
                        // put you code here
                    }
                    break;
                }
                default:
                    gpioLine.set(GPIO_Output::INACTIVE);
                    break;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "An error occurred in GPIO thread: " << e.what() << std::endl;
    }
}

void button_thread(Application_state &appState, const std::vector<std::string> &inputDevices) {
    try {
        int epoll_fd = epoll_create1(0);
        if (epoll_fd < 0) {
            throw std::runtime_error("Failed to create epoll instance");
        }

        std::vector<int> fds;
        for (const auto &devicePath : inputDevices) {
            int fd = open(devicePath.c_str(), O_RDONLY);
            if (fd < 0) {
                throw std::runtime_error("Failed to open device: " + devicePath);
            }

            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = fd;

            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
                close(fd);
                throw std::runtime_error("Failed to add file descriptor to epoll: " + devicePath);
            }

            fds.push_back(fd);
        }

        struct epoll_event events[10];
        struct input_event event;

        std::cout << "Monitoring Button events on ";
        for (const auto &devicePath : inputDevices) {
            std::cout << devicePath << " ";
        }
        std::cout << std::endl;

        std::chrono::steady_clock::time_point press_time; // Added to track the press time
        bool button_pressed = false; // Added to track the button pressed state

        while (appState.keepRunning.load()) {
            int nfds = epoll_wait(epoll_fd, events, sizeof(events) / sizeof(events[0]), 10); // Wait for 10ms for an event
            if (nfds > 0) {
                for (int i = 0; i < nfds; ++i) {
                    if (events[i].events & EPOLLIN) {
                        ssize_t bytes = read(events[i].data.fd, &event, sizeof(struct input_event));
                        if (bytes == sizeof(struct input_event)) {
                            if (event.type == EV_KEY) {
                                if (event.value == 1) { // 1 = press
                                    press_time = std::chrono::steady_clock::now(); // Added to record the time of the button press
                                    button_pressed = true; // Added to indicate the button is pressed
                                } else if (event.value == 0 && button_pressed) { // 0 = release
                                    auto release_time = std::chrono::steady_clock::now(); // Added to record the time of the button release
                                    auto press_duration = std::chrono::duration_cast<std::chrono::milliseconds>(release_time - press_time).count(); // Added to calculate the press duration

                                    if (press_duration < 500) { // Threshold for short press (e.g., 500 ms)
                                        std::cout << "Short press detected" << std::endl; // Added to indicate a short press was detected
                                        appState.triggerIndex.fetch_add(1); // Added to change the LED trigger
                                    } else {
                                        std::cout << "Long press detected" << std::endl; // Added to indicate a long press was detected
                                        appState.keepRunning.store(false); // Added to stop the application
                                    }

                                    button_pressed = false; // Added to reset the button pressed state
                                }
                            }
                        }
                    }
                }
            } else if (nfds < 0) {
                throw std::runtime_error("Epoll wait error occurred");
            }
        }
        // always clean up after yourself
        for (int fd : fds) {
            close(fd);
        }
        close(epoll_fd);
    } catch (const std::exception &e) {
        std::cerr << "An error occurred in Button monitoring thread: " << e.what() << std::endl;
    }
    std::cout << "Monitoring Button events ended." << std::endl;
}

