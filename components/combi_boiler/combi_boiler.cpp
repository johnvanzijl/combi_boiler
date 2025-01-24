#include "combi_boiler.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include <ArduinoJson.h>
#include <algorithm>

#define BLINK_INTERVAL 3000
unsigned long previousMillis = 0;

namespace esphome {
	namespace combi_boiler {

		static float power1 = 100.0f;
		static float power2 = 100.0f;
		static float oldPower1 = 0.0f;
		static float oldPower2 = 0.0f;
		
		char status_buffer[50];



		void CombiBoiler::init_udp() {
			if (WiFi.status() == WL_CONNECTED) {
				if (udp_.begin(UDP_PORT)) {
					ESP_LOGI(TAG, "UDP server started on port %d", UDP_PORT);
					this->udp_initialized_ = true;
				} else {
					ESP_LOGE(TAG, "Failed to start UDP server");
					this->udp_initialized_ = false;
				}
			}
		}

		void CombiBoiler::handle_udp() {
			if (!this->udp_initialized_) {
				return;
			}

			int packetSize = udp_.parsePacket();
			if (packetSize) {
				int len = udp_.read(udp_buffer_, UDP_BUFFER_SIZE - 1);
				if (len > 0) {
					udp_buffer_[len] = 0;
					
					StaticJsonDocument<200> doc;
					DeserializationError error = deserializeJson(doc, udp_buffer_);
					
					if (error) {
						ESP_LOGE(TAG, "JSON parsing failed: %s", error.c_str());
						return;
					}
					
					if (doc.containsKey("L1")) {
						udp_packetRecieved_=true;
						float new_l1_value = doc["L1"].as<float>();
						float new_l2_value = doc["L2"].as<float>();
						float new_l3_value = doc["L3"].as<float>();
						if (new_l1_value != this->l1_value_) {
							this->l1_value_ = new_l1_value;
							this->l2_value_ = new_l2_value;
							this->l3_value_ = new_l3_value;
							ESP_LOGD(TAG, "New L values received: %.2f %.2f %.2f", this->l1_value_, this->l2_value_, this->l3_value_);
							if ((this->boiler1_enabled_) && (boiler1_forcepoint_ == -1)) {power1 = std::max(0.0f, std::min(100.0f, power1 + new_l1_value*80));ESP_LOGD(TAG, "@UD");  };
							if ((this->boiler2_enabled_) && (boiler2_forcepoint_ == -1)) {power2 = std::max(0.0f, std::min(100.0f, power2 + new_l2_value*80));ESP_LOGD(TAG, "UD");  };
							snprintf(status_buffer, sizeof(status_buffer), "L1: %.2f kW, L2: %.2f kW, L3: %.2f kW", this->l1_value_, this->l2_value_, this->l3_value_);
							this->update_status1(std::string(status_buffer));
						}
					}
					if (doc.containsKey("La")) {
						this->shelly_current1_ = doc["La"].as<float>();
						this->shelly_current2_ = doc["Lb"].as<float>();
						this->shelly_current3_ = doc["Lc"].as<float>();
						ESP_LOGD(TAG, "New Shelly values: %.2f %.2f %.2f", this->shelly_current1_, this->shelly_current2_, this->shelly_current3_);  
						snprintf(status_buffer, sizeof(status_buffer), "S1: %.2f A, S2: %.2f A, S3: %.2f A", this->shelly_current1_, this->shelly_current2_, this->shelly_current2_);
						this->update_status2(std::string(status_buffer));
					}
				}
			}
		}
		
		
		void CombiBoiler::setup() {

			pinMode(PWM_PIN1, OUTPUT);
			digitalWrite(PWM_PIN1, LOW);
			pinMode(PWM_PIN2, OUTPUT);
			digitalWrite(PWM_PIN2, LOW);
			
			ledcSetup(0, PWM_FREQUENCY, PWM_RESOLUTION);			
			ledcSetup(1, PWM_FREQUENCY, PWM_RESOLUTION);
			ledcAttachPin(PWM_PIN1, PWM_CHANNEL1);
			ledcAttachPin(PWM_PIN2, PWM_CHANNEL2);
			
			//power1=this->boiler1_forcepoint_;
			//power2=this->boiler2_forcepoint_;
			
			//this->boiler1_power_sensor_->publish_state(power1);
			//this->boiler2_power_sensor_->publish_state(power2);
			
			ESP_LOGD(TAG, "111111@FP: %.2f",boiler1_forcepoint_); 

			// Initialize UDP
			this->udp_initialized_ = false;
		}
		
		
		void CombiBoiler::loop() {
			unsigned long currentMillis = millis();

			if (!this->udp_initialized_) {
				if (currentMillis - this->last_udp_init_attempt_ >= UDP_RETRY_INTERVAL) {
					this->last_udp_init_attempt_ = currentMillis;
					this->init_udp();
				}
			}

			if (this->udp_initialized_) {
				this->handle_udp();
			}

			if (currentMillis - previousMillis >= BLINK_INTERVAL) {
				previousMillis = currentMillis;
				
				float temp1 = this->get_boiler1_temperature();
				float temp2 = this->get_boiler2_temperature();
				
				uint32_t pwm_value1;
				uint32_t pwm_value2;
				
				ESP_LOGD(TAG, "Current Shelly reading: %.2f %.2f %.2f", this->shelly_current1_, this->shelly_current2_, this->shelly_current3_);
				if (this->udp_packetRecieved_) {
					if (this->boiler1_enabled_) {
						if(boiler1_forcepoint_ != -1) {
							power1 = boiler1_forcepoint_;
						};
						pwm_value1 = static_cast<uint32_t>((power1 / 100.0f) * 1023);
						ledcWrite(PWM_CHANNEL1, pwm_value1);
						ESP_LOGD(TAG, "@FP: %.2f %.2f %d",power1,boiler1_forcepoint_,pwm_value1); 
					} else {
						power1 = 0;
						ledcWrite(PWM_CHANNEL1, 0);
						ESP_LOGD(TAG, "@P0"); 
					}
					
					if (this->boiler2_enabled_) {
						if(boiler2_forcepoint_ != -1) {
							power2 = boiler2_forcepoint_;
						};
						pwm_value2 = static_cast<uint32_t>((power2 / 100.0f) * 1023);
						ledcWrite(PWM_CHANNEL2,  pwm_value2);
						ESP_LOGD(TAG, "FP: %.2f %.2f %d",power2,boiler2_forcepoint_,pwm_value2); 
					} else {
						power2 = 0;
						ESP_LOGD(TAG, "P0");  
						ledcWrite(PWM_CHANNEL2,  0);
					}
				}
				
				// Update binary sensors for boiler full state
				if (this->boiler1_full_binary_sensor_ != nullptr) {
					bool is_full = (power1 > 2.0f && this->shelly_current1_ < 0.1f);
					bool is_off = (power1 == 0.0f);
					if (is_off) {
						this->boiler1_full_binary_sensor_->publish_state(false);
					} else if (is_full) {
						this->boiler1_full_binary_sensor_->publish_state(true);
					}
				}

				if (this->boiler2_full_binary_sensor_ != nullptr) {
					bool is_full = (power2 > 2.0f && this->shelly_current2_ < 0.1f);
					bool is_off = (power2 == 0.0f);
					if (is_off) {
						this->boiler2_full_binary_sensor_->publish_state(false);
					} else if (is_full) {
						this->boiler2_full_binary_sensor_->publish_state(true);
					}
				}
			}
			
			if (power1 != oldPower1) {
				this->boiler1_power_sensor_->publish_state(power1);
				oldPower1 = power1;
			}
			if (power2 != oldPower2) {
				this->boiler2_power_sensor_->publish_state(power2);
				oldPower2 = power2;
			}
		}

		float CombiBoiler::get_boiler1_temperature() {
			if (this->boiler1_temperature_sensor_ != nullptr) {
				return this->boiler1_temperature_sensor_->state;
			}
			return 0.0f;
		}

		float CombiBoiler::get_boiler2_temperature() {
			if (this->boiler2_temperature_sensor_ != nullptr) {
				return this->boiler2_temperature_sensor_->state;
			}
			return 0.0f;
		}

	}  // namespace combi_boiler
}  // namespace esphome