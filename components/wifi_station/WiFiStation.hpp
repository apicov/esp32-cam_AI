#pragma once
#include <atomic>
#include <functional>
#include <unordered_map>
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <string.h>

// Hash function for std::pair
// TODO: this should be private enclosed within WiFiStation
struct pair_hash {
    /**
     * @brief Hash function for a pair of values.
     *
     * This function computes a hash value for a given std::pair
     * by combining the hash values of its two elements.
     *
     * @tparam T1 Type of the first element in the pair.
     * @tparam T2 Type of the second element in the pair.
     * @param pair The pair for which to compute the hash.
     * @return The computed hash value.
     */
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

/**
 * @brief A thin wrapper around the ESP-IDF "esp_wifi" for quick use.
 *
 * This class provides a simplified interface for managing WiFi connections
 * using the ESP-IDF framework. It allows for event handling and connection
 * management.
 */
class WiFiStation {
public:
    /**
     * @brief Type definition for WiFi event callback functions.
     *
     * This is a function object type that can be registered for WiFi events.
     */
    using WifiEventCallback = std::function<void(void*)>;

    /**
     * @brief Constructor for the WiFiStation instances.
     *
     * Initializes the WiFiStation with the provided SSID and password.
     *
     * @param ssid The SSID of the WiFi network.
     * @param password The password for the WiFi network.
     */
    WiFiStation(const char* ssid, const char* password);

    /**
     * @brief Initialize the WiFiStation object.
     *
     * This method must be called before using the WiFiStation object
     * to set up the necessary configurations and event handlers.
     */
    void init();

    /**
     * @brief Register a callback for a specific WiFi event.
     *
     * This method allows the user to register a callback function
     * that will be invoked when the specified event occurs.
     *
     * @param event_base The base ID of the event to register the handler for.
     * @param event_id The identifier of the event to register the handler for.
     * @param callback The callback function to register for the given event.
     *
     */
    //TODO: This method is currently complex; consider simplifying it
    //       if only a connection event callback is needed.
    void register_event_callback(esp_event_base_t event_base, int32_t event_id, WifiEventCallback callback);

    /**
     * @brief Check if the WiFi is connected.
     *
     * This method checks the current connection status of the WiFi.
     *
     * @return true if the WiFi is connected, false otherwise.
     */
    bool is_connected() const;

private:
    const char* ssid_; ///< The SSID of the WiFi network.
    const char* password_; ///< The password for the WiFi network.
    std::atomic<bool> is_connected_; ///< Atomic flag indicating connection status.
    esp_event_handler_instance_t instance_any_id_; ///< Event handler instance for any event.
    esp_event_handler_instance_t instance_ip_event_; ///< Event handler instance for IP events.
    std::unordered_map<std::pair<esp_event_base_t, int32_t>, WifiEventCallback, pair_hash> event_callbacks_; ///< Map of registered event callbacks.

    static constexpr const char* TAG = "WIFI"; ///< Log tag for WiFi operations.

    /**
     * @brief Static event handler required by the ESP-IDF.
     *
     * This static method serves as the event handler for WiFi events.
     * It forwards the event to the appropriate callback registered by the user.
     *
     * @param arg User-defined argument passed to the handler.
     * @param event_base The base ID of the event.
     * @param event_id The identifier of the event.
     * @param event_data Pointer to the event data.
     */
    static void event_handler_static(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    // Instance-level event handler
    void event_handler(esp_event_base_t event_base, int32_t event_id, void* event_data);

    // Set default handlers for Wi-Fi and IP events
    void set_default_handlers();
};
