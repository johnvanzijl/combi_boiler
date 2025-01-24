
#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/wifi/wifi_component.h"
#include "esphome/components/api/api_server.h"  
#include "esphome/components/binary_sensor/binary_sensor.h"
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "driver/ledc.h"



namespace esphome {
	namespace combi_boiler {

		static const char *TAG = "combi_boiler";

		class CombiBoiler : public Component {
		public:
			CombiBoiler() : Component() {}

			void setup() override;
			void loop() override;
			float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

			// Methods to match YAML expectations exactly
			void set_temperature1(sensor::Sensor *sensor) { this->boiler1_temperature_sensor_ = sensor; }
			void set_temperature2(sensor::Sensor *sensor) { this->boiler2_temperature_sensor_ = sensor; }
			void set_power1(sensor::Sensor *sensor) { this->boiler1_power_sensor_ = sensor; }
			void set_power2(sensor::Sensor *sensor) { this->boiler2_power_sensor_ = sensor; }
			void set_status1(text_sensor::TextSensor* status) { status1_ = status; }
			void set_status2(text_sensor::TextSensor* status) { status2_ = status; }

			float get_boiler1_temperature();
			float get_boiler2_temperature();

			void set_boiler1_forcepoint(float forcepoint) { this->boiler1_forcepoint_ = forcepoint; }
			void set_boiler2_forcepoint(float forcepoint) { this->boiler2_forcepoint_ = forcepoint; }
			void set_boiler1_setmax(float setmax) { this->boiler1_setmax_ = setmax; }
			void set_boiler2_setmax(float setmax) { this->boiler2_setmax_ = setmax; }
			void set_boiler1_enabled(bool enabled) { this->boiler1_enabled_ = enabled;}
			void set_boiler2_enabled(bool enabled) { this->boiler2_enabled_ = enabled;}

			void set_boiler1_full_binary_sensor(binary_sensor::BinarySensor *sensor) { 
				this->boiler1_full_binary_sensor_ = sensor; 
			}
			void set_boiler2_full_binary_sensor(binary_sensor::BinarySensor *sensor) { 
				this->boiler2_full_binary_sensor_ = sensor; 
			}

			void update_status1(const std::string& status) {
				if (status1_ != nullptr) {
					status1_->publish_state(status);
				}
			}

			void update_status2(const std::string& status) {
				if (status2_ != nullptr) {
					status2_->publish_state(status);
				}
			}

			// Add this method
			//void set_api(api::APIServer *api) { this->api_ = api; }

		protected:
			//api::APIServer *api_{nullptr};  // Add this
			sensor::Sensor *boiler1_temperature_sensor_{nullptr};
			sensor::Sensor *boiler2_temperature_sensor_{nullptr};
			sensor::Sensor *boiler1_power_sensor_{nullptr};
			sensor::Sensor *boiler2_power_sensor_{nullptr};
			binary_sensor::BinarySensor *boiler1_full_binary_sensor_{nullptr};
			binary_sensor::BinarySensor *boiler2_full_binary_sensor_{nullptr};
			float boiler1_forcepoint_;
			float boiler2_forcepoint_;
			float boiler1_setmax_;
			float boiler2_setmax_;
			bool boiler1_enabled_{true};  
			bool boiler2_enabled_{true};
			// Inside the protected section of CombiBoiler class, add:
			static const uint8_t PWM_PIN1 = 26;  // GPIO25
			static const uint8_t PWM_PIN2 = 25;  // GPIO26
			static const uint32_t PWM_FREQUENCY = 60250;  // 
			static const uint8_t PWM_RESOLUTION = 10;  // 10-bit resolution
			static const uint8_t PWM_CHANNEL1 = 0;  // LEDC channel 0
			static const uint8_t PWM_CHANNEL2 = 1;  // LEDC channel 1
			text_sensor::TextSensor* status1_{nullptr};
			text_sensor::TextSensor* status2_{nullptr};

			// UDP related members
			WiFiUDP udp_;
			bool udp_initialized_{false};
			bool udp_packetRecieved_{false};
			static const uint16_t UDP_PORT = 4711;
			static const size_t UDP_BUFFER_SIZE = 255;
			float l1_value_{0.0f};
			float l2_value_{0.0f};
			float l3_value_{0.0f};
			float shelly_current1_{0.0f};
			float shelly_current2_{0.0f};
			float shelly_current3_{0.0f};
			char udp_buffer_[UDP_BUFFER_SIZE];
			void init_udp();
			void handle_udp();
			unsigned long last_udp_init_attempt_{0};
			static const unsigned long UDP_RETRY_INTERVAL = 5000;

		};

	}  // namespace combi_boiler
}  // namespace esphome