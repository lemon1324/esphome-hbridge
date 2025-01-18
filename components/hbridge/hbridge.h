#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"

namespace esphome
{
    namespace hbridge
    {

        enum HBridgeDecayMode
        {
            BRAKE = 0,
            COAST = 1
        };

        class HBridge : public Component
        {
        public:
            // Setters for pins
            void set_output_a(output::FloatOutput *output) { output_a_ = output; }
            void set_output_b(output::FloatOutput *output) { output_b_ = output; }
            void set_output_enable(output::FloatOutput *output) { output_enable_ = output; }

            // ESPHome lifecycle methods
            void setup() override;
            void loop() override;
            void dump_config() override;

            void set_output(float output);

        protected:
            output::FloatOutput *output_a_;
            output::FloatOutput *output_b_;
            output::FloatOutput *output_enable_ = nullptr; // Optional

            float output_value_ = 0.0f; // Initialize to 0.0f
            float deadband_ = 0.01f;

            HBridgeDecayMode decay_mode_{COAST};

            void write_state_();
        };

        template <typename... Ts>
        class OutputAction : public Action<Ts...>
        {
        public:
            OutputAction(HBridge *parent) : parent_(parent) {}
            TEMPLATABLE_VALUE(float, output)
            void play(Ts... x) override
            {
                float out = this->output_.value(x...);
                this->parent_->set_output(out);
            }

        protected:
            HBridge *parent_;
        };

    } // namespace hbridge
} // namespace esphome
