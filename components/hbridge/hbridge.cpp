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

namespace esphome
{
    namespace hbridge
    {

        static const char *TAG = "hbridge";

        void HBridgeComponent::setup()
        {
        }

        void HBridgeComponent::loop()
        {
            // Optional: Add logic to control the H-Bridge
        }

        void HBridgeComponent::set_output(float output)
        {
            output_value_ = output;
            write_state_();
        }

        void HBridgeComponent::dump_config()
        {
            ESP_LOGCONFIG(TAG, "H-Bridge Component:");
        }

        void HBridgeComponent::write_state_()
        {
            // Clamp the output value between -1.0f and 1.0f
            float clamped_value = std::clamp(output_value_, -1.0f, 1.0f);

            // Get the absolute value of the clamped value
            float magnitude = std::abs(clamped_value);

            if (decay_mode_ == HBridgeDecayMode::COAST)
            {
                // Coast mode logic
                if (clamped_value > deadband_) // forward/coast
                {
                    output_a_->set_level(magnitude);
                    output_b_->set_level(0.0f);
                }
                else if (clamped_value < -deadband_) // reverse/coast
                {
                    output_a_->set_level(0.0f);
                    output_b_->set_level(magnitude);
                }
                else // coast
                {
                    // Zero value: set both pins to 0.0f to coast
                    output_a_->set_level(0.0f);
                    output_b_->set_level(0.0f);
                }
            }
            else if (decay_mode_ == HBridgeDecayMode::BRAKE)
            {
                // Brake mode logic
                if (clamped_value > deadband_) // forward/brake
                {
                    output_a_->set_level(1.0f);
                    output_b_->set_level(1.0f - magnitude);
                }
                else if (clamped_value < -deadband_) // reverse/brake
                {
                    output_a_->set_level(1.0f - magnitude);
                    output_b_->set_level(1.0f);
                }
                else // brake
                {
                    // Zero value: set both pins to 1.0f to brake
                    output_a_->set_level(1.0f);
                    output_b_->set_level(1.0f);
                }
            }
        }

        class OutputAction : public Action<float>
        {
        public:
            OutputAction(HBridgeComponent *parent) : parent_(parent) {}

            void set_output(float output)
            {
                this->output_ = output;
            }

            void play(Ts... x) override
            {
                parent_->set_output(this->output_);
            }

        protected:
            HBridgeComponent *parent_;
            float output_;
        };

    } // namespace hbridge
} // namespace esphome
