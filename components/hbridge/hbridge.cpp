/**
 * @file hbridge.cpp
 * @author lemon1324
 * @date 2024-02-14
 * @brief ESPHome 2024 external component to drive an h bridge.
 *
 * Controls bridges controlled by an input for half-bridge A and B, and
 * optionally a third input for bridge enable.
 *
 * This component will directly control each GPIO pin.
 *
 * The component will take three parameters:
 * - pin_a: the GPIO pin connected to the A half-bridge
 * - pin_b: the GPIO pin connected to the B half-bridge
 * - pin_enable: the GPIO pin connected to the bridge enable (optional)
 */

#include "hbridge.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome
{
    namespace hbridge
    {

        static const char *TAG = "hbridge";

        void HBridge::setup()
        {
        }

        void HBridge::loop()
        {
            // Optional: Add logic to control the H-Bridge
        }

        void HBridge::set_output(float output)
        {
            this->output_value_ = output;
            this->write_state_();
        }

        void HBridge::dump_config()
        {
            ESP_LOGCONFIG(TAG, "H-Bridge Component:");
        }

        void HBridge::write_state_()
        {
            // Clamp the output value between -1.0f and 1.0f
            float clamped_value = esphome::clamp(this->output_value_, -1.0f, 1.0f);

            // Get the absolute value of the clamped value
            float magnitude = std::abs(clamped_value);

            if (this->decay_mode_ == HBridgeDecayMode::COAST)
            {
                // Coast mode logic
                if (clamped_value > this->deadband_) // forward/coast
                {
                    this->output_a_->set_level(magnitude);
                    this->output_b_->set_level(0.0f);
                }
                else if (clamped_value < -this->deadband_) // reverse/coast
                {
                    this->output_a_->set_level(0.0f);
                    this->output_b_->set_level(magnitude);
                }
                else // coast
                {
                    // Zero value: set both pins to 0.0f to coast
                    this->output_a_->set_level(0.0f);
                    this->output_b_->set_level(0.0f);
                }
            }
            else if (this->decay_mode_ == HBridgeDecayMode::BRAKE)
            {
                // Brake mode logic
                if (clamped_value > this->deadband_) // forward/brake
                {
                    this->output_a_->set_level(1.0f);
                    this->output_b_->set_level(1.0f - magnitude);
                }
                else if (clamped_value < -this->deadband_) // reverse/brake
                {
                    this->output_a_->set_level(1.0f - magnitude);
                    this->output_b_->set_level(1.0f);
                }
                else // brake
                {
                    // Zero value: set both pins to 1.0f to brake
                    this->output_a_->set_level(1.0f);
                    this->output_b_->set_level(1.0f);
                }
            }
        }

    } // namespace hbridge
} // namespace esphome
