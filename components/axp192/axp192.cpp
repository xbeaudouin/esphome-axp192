#include "axp192.h"
#include "esphome/core/log.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_system.h"

namespace esphome
{
    namespace axp192
    {
        static const char *TAG = "axp192.sensor";
        void AXP192Component::setup()
        {
            ESP_LOGD(TAG, "setup(): Model %d", this->model_);
            ESP_LOGD(TAG, "setup(): Charge current %d", this->charge_current_);

            switch (this->model_)
            {
            case AXP192_M5STICKC:
            {
                begin(false, false, false, false, false);
                break;
            }
            case AXP192_M5CORE2:
            {
                // disable LDO3 Vibration
                begin(false, true, false, false, false);
                break;
            }
            case AXP192_M5TOUGH:
            {
                begin(false, false, false, false, false);

                // If we're waking from a cold boot
                if (GetStartupReason() == "ESP_RST_POWERON")
                {
                    ESP_LOGD(TAG, "First power on, restarting ESP...");

                    // Reboot the ESP with the axp initialised
                    esp_restart();
                }
                break;
            }
            case AXP192_TTGO_TCALL:
            {
                // disable LDO2, LDO3, RTC and DCDC1
                begin(true, true, true, true, false);
                break;
            }
	    case APX192_LILYGO_TCAMINI:
	    {
                begin(false, false, false, false, false);
                break;
            }
        }

        void AXP192Component::dump_config()
        {
            ESP_LOGCONFIG(TAG, "AXP192:");
            LOG_I2C_DEVICE(this);
            if (this->batterylevel_sensor_ != nullptr)
            {
                LOG_SENSOR("  ", "Battery Level", this->batterylevel_sensor_);
            }
            if (this->batteryvoltage_sensor_ != nullptr)
            {
                LOG_SENSOR("  ", "Battery Voltage", this->batteryvoltage_sensor_);
            }
            if (this->batterycurrent_sensor_ != nullptr)
            {
                LOG_SENSOR("  ", "Battery Current", this->batterycurrent_sensor_);
            }
            if (this->vbusvoltage_sensor_ != nullptr)
            {
                LOG_SENSOR("  ", "VBUS Voltage", this->vbusvoltage_sensor_);
            }
            if (this->vbuscurrent_sensor_ != nullptr)
            {
                LOG_SENSOR("  ", "VBUS Current", this->vbuscurrent_sensor_);
            }
            if (this->vincurrent_sensor_ != nullptr)
            {
                LOG_SENSOR("  ", "VIN Current", this->vincurrent_sensor_);
            }
            if (this->temperature_sensor_ != nullptr)
            {
                LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
            }
        }

        float AXP192Component::get_setup_priority() const { return setup_priority::DATA; }

        void AXP192Component::update()
        {
            ESP_LOGD(TAG, "update()");

            float vbat = GetBatVoltage();

            if (this->batterylevel_sensor_ != nullptr)
            {
                float batterylevel = 100.0 * ((vbat - 3.0) / (4.2 - 3.0));

                ESP_LOGD(TAG, "Got Battery Level=%f (%f)", batterylevel, vbat);
                if (batterylevel > 100.0)
                {
                    batterylevel = 100.0;
                }
                else if (batterylevel < 0.0)
                {
                    batterylevel = 0.0;
                }
                this->batterylevel_sensor_->publish_state(batterylevel);
            }
            if (this->batteryvoltage_sensor_ != nullptr)
            {
                ESP_LOGD(TAG, "Got Battery Voltage=%f", vbat);
                this->batteryvoltage_sensor_->publish_state(vbat);
            }
            if (this->batterycurrent_sensor_ != nullptr)
            {
                float cbat = GetBatCurrent();
                if (cbat != 0.0)
                {
                    cbat = cbat / 1000;
                }
                ESP_LOGD(TAG, "Got Battery Current=%f", cbat);
                this->batterycurrent_sensor_->publish_state(cbat);
            }
            if (this->vbusvoltage_sensor_ != nullptr)
            {
                float vbus_v = GetVBusVoltage();
                ESP_LOGD(TAG, "Got VBUS Voltage=%f", vbus_v);
                this->vbusvoltage_sensor_->publish_state(vbus_v);
            }
            if (this->vbuscurrent_sensor_ != nullptr)
            {
                float vbus_c = GetVBusCurrent();
                if (vbus_c != 0.0)
                {
                    vbus_c = vbus_c / 1000;
                }
                ESP_LOGD(TAG, "Got VBUS Current=%f", vbus_c);
                this->vbuscurrent_sensor_->publish_state(vbus_c);
            }
            if (this->vincurrent_sensor_ != nullptr)
            {
                float vin_c = GetVinCurrent();
                if (vin_c != 0.0)
                {
                    vin_c = vin_c / 1000;
                }
                ESP_LOGD(TAG, "Got VIN Current=%f", vin_c);
                this->vincurrent_sensor_->publish_state(vin_c);
            }
            if (this->temperature_sensor_ != nullptr)
            {
                float temp = GetTempInAXP192();
                ESP_LOGD(TAG, "Got Temperature=%f", temp);
                this->temperature_sensor_->publish_state(temp);
            }

            UpdateBrightness();
        }

        void AXP192Component::begin(bool disableLDO2, bool disableLDO3, bool disableRTC, bool disableDCDC1, bool disableDCDC3)
        {
            switch (this->model_)
            {
            case AXP192_M5STICKC:
            {
                // Set LDO2 & LDO3(TFT_LED & TFT) 3.0V
                Write1Byte(0x28, 0xcc);
                break;
            }
            case AXP192_M5CORE2:
            {
                // Set DCDC3 (TFT_LED & TFT) 3.0V
                Write1Byte(0x27, 0xcc);
                // Set LDO2 & LDO3(TFT_LED & TFT) 3.0V
                Write1Byte(0x28, 0xcc);
                break;
            }
            case AXP192_M5TOUGH:
            {
                // Set DCDC3 (TFT_LED & TFT) 3.0V
                Write1Byte(0x27, 0xcc);
                // Set LDO2 & LDO3(TFT_LED & TFT) 3.0V
                Write1Byte(0x28, 0xcc);
                break;
            }
            case AXP192_TTGO_TCALL:
            {
                break;
            }
            case AXP192_LILYGO_TCAMINI:
            {
                break;
            }
            }

            // Depending on configuration enable LDO2, LDO3, DCDC1, DCDC3.
            uint8_t buf = (Read8bit(0x12) & 0xef) | 0x4D;
            if (disableLDO3)
                buf &= ~(1 << 3);
            if (disableLDO2)
                buf &= ~(1 << 2);
            if (disableDCDC3)
                buf &= ~(1 << 1);
            if (disableDCDC1)
                buf &= ~(1 << 0);
            Write1Byte(0x12, buf);

            if (!disableRTC)
            {
                // Set RTC voltage to 3.3V
                Write1Byte(0x91, 0xF0);

                // Set GPIO0 to LDO
                Write1Byte(0x90, 0x02);
            }

            // Set ADC sample rate to 200hz
            Write1Byte(0x84, 0b11110010);

            // Set ADC to All Enable
            Write1Byte(0x82, 0xff);

            // Set temperature protection
            Write1Byte(0x39, 0xfc);

            if ( this->model_ != AXP192_TTGO_TCALL)
            {
                // Bat charge voltage to 4.2, Current 100MA
                Write1Byte(0x33, 0xc0);

                // 128ms power on, 4s power off
                Write1Byte(0x36, 0x0C);

                // Disable vbus hold limit
                Write1Byte(0x30, 0x80);

                // Enable RTC BAT charge
                Write1Byte(0x35, 0xa2 & (disableRTC ? 0x7F : 0xFF));

                // Enable bat detection
                Write1Byte(0x32, 0x46);
            }

            // Set charge current
            SetChargeCurrent(this->charge_current_);

        }

        void AXP192Component::Write1Byte(uint8_t Addr, uint8_t Data)
        {
            this->write_byte(Addr, Data);
        }

        uint8_t AXP192Component::Read8bit(uint8_t Addr)
        {
            uint8_t data;
            this->read_byte(Addr, &data);
            return data;
        }

        uint16_t AXP192Component::Read12Bit(uint8_t Addr)
        {
            uint16_t Data = 0;
            uint8_t buf[2];
            ReadBuff(Addr, 2, buf);
            Data = ((buf[0] << 4) + buf[1]); //
            return Data;
        }

        uint16_t AXP192Component::Read13Bit(uint8_t Addr)
        {
            uint16_t Data = 0;
            uint8_t buf[2];
            ReadBuff(Addr, 2, buf);
            Data = ((buf[0] << 5) + buf[1]); //
            return Data;
        }

        uint16_t AXP192Component::Read16bit(uint8_t Addr)
        {
            uint32_t ReData = 0;
            uint8_t Buff[2];
            this->read_bytes(Addr, Buff, sizeof(Buff));
            for (int i = 0; i < sizeof(Buff); i++)
            {
                ReData <<= 8;
                ReData |= Buff[i];
            }
            return ReData;
        }

        uint32_t AXP192Component::Read24bit(uint8_t Addr)
        {
            uint32_t ReData = 0;
            uint8_t Buff[3];
            this->read_bytes(Addr, Buff, sizeof(Buff));
            for (int i = 0; i < sizeof(Buff); i++)
            {
                ReData <<= 8;
                ReData |= Buff[i];
            }
            return ReData;
        }

        uint32_t AXP192Component::Read32bit(uint8_t Addr)
        {
            uint32_t ReData = 0;
            uint8_t Buff[4];
            this->read_bytes(Addr, Buff, sizeof(Buff));
            for (int i = 0; i < sizeof(Buff); i++)
            {
                ReData <<= 8;
                ReData |= Buff[i];
            }
            return ReData;
        }

        void AXP192Component::ReadBuff(uint8_t Addr, uint8_t Size, uint8_t *Buff)
        {
            this->read_bytes(Addr, Buff, Size);
        }

        void AXP192Component::UpdateBrightness()
        {
            if (brightness_ == curr_brightness_)
            {
                return;
            }

            ESP_LOGD(TAG, "Brightness=%f (Curr: %f)", brightness_, curr_brightness_);

            const uint8_t c_min = 7;
            const uint8_t c_max = 12;
            auto ubri = c_min + static_cast<uint8_t>(brightness_ * (c_max - c_min));

            if (ubri > c_max)
            {
                ubri = c_max;
            }
            switch (this->model_)
            {
            case AXP192_M5STICKC:
            {
                ESP_LOGD(TAG, "Brightness AXP192_M5STICKC");

                uint8_t buf = Read8bit(0x28);
                Write1Byte(0x28, ((buf & 0x0f) | (ubri << 4)));

                if (brightness_ == 0)
                {
                    // Then turn off the backlight power
                    SetLDO2(false);
                }
                else if (curr_brightness_ == 0)
                {
                    // We came off zero brightness -> turn backlight back on
                    SetLDO2(true);
                }

                break;
            }
            case AXP192_M5CORE2:
            {
                ESP_LOGD(TAG, "Brightness AXP192_M5CORE2");

                uint8_t buf = Read8bit(0x27);
                Write1Byte(0x27, ((buf & 0x80) | (ubri << 3)));

                if (brightness_ == 0)
                {
                    // Then turn off the backlight power
                    // SetLDO3(false); -> AXP_DC3
                }
                else if (curr_brightness_ == 0)
                {
                    // We came off zero brightness -> turn backlight back on
                    // SetLDO3(true); -> AXP_DC3
                }

                break;
            }
            case AXP192_M5TOUGH:
            {
                ESP_LOGD(TAG, "Brightness AXP192_M5TOUGH");

                uint8_t buf = Read8bit(0x27);
                Write1Byte(0x27, ((buf & 0x80) | (ubri << 3)));

                if (brightness_ == 0)
                {
                    // Then turn off the backlight power
                    SetLDO3(false);
                }
                else if (curr_brightness_ == 0)
                {
                    // We came off zero brightness -> turn backlight back on
                    SetLDO3(true);
                }

                break;
            }
            case AXP192_TTGO_TCALL:
            {
                break;
            }
            case AXP192_LILYGO_TCAMINI:
            {
                break;
            }
            }

            curr_brightness_ = brightness_;
        }

        bool AXP192Component::GetBatState()
        {
            if (Read8bit(0x01) | 0x20)
                return true;
            else
                return false;
        }

        uint8_t AXP192Component::GetBatData()
        {
            return Read8bit(0x75);
        }

        //---------coulombcounter_from_here---------
        // enable: void EnableCoulombCounter(void);
        // disable: void DisableCOulombCounter(void);
        // stop: void StopCoulombCounter(void);
        // clear: void ClearCoulombCounter(void);
        // get charge data: uint32_t GetCoulombChargeData(void);
        // get discharge data: uint32_t GetCoulombDischargeData(void);
        // get coulomb val affter calculation: float GetCoulombData(void);
        //------------------------------------------
        void AXP192Component::EnableCoulombCounter(void)
        {
            Write1Byte(0xB8, 0x80);
        }

        void AXP192Component::DisableCoulombCounter(void)
        {
            Write1Byte(0xB8, 0x00);
        }

        void AXP192Component::StopCoulombCounter(void)
        {
            Write1Byte(0xB8, 0xC0);
        }

        void AXP192Component::ClearCoulombCounter(void)
        {
            Write1Byte(0xB8, 0xA0);
        }

        uint32_t AXP192Component::GetCoulombChargeData(void)
        {
            return Read32bit(0xB0);
        }

        uint32_t AXP192Component::GetCoulombDischargeData(void)
        {
            return Read32bit(0xB4);
        }

        float AXP192Component::GetCoulombData(void)
        {
            uint32_t coin = 0;
            uint32_t coout = 0;

            coin = GetCoulombChargeData();
            coout = GetCoulombDischargeData();

            // c = 65536 * current_LSB * (coin - coout) / 3600 / ADC rate
            // Adc rate can be read from 84H ,change this variable if you change the ADC reate
            float ccc = 65536 * 0.5 * (coin - coout) / 3600.0 / 25.0;
            return ccc;
        }
        //----------coulomb_end_at_here----------

        void AXP192Component::SetSleep(void)
        {
            Write1Byte(0x31, Read8bit(0x31) | (1 << 3)); // Power off voltag 3.0v
            Write1Byte(0x90, Read8bit(0x90) | 0x07);     // GPIO1 floating
            Write1Byte(0x82, 0x00);                      // Disable ADCs
            Write1Byte(0x12, Read8bit(0x12) & 0xA1);     // Disable all outputs but DCDC1
        }

        // -- sleep
        void AXP192Component::DeepSleep(uint64_t time_in_us)
        {
            SetSleep();
            esp_sleep_enable_ext0_wakeup((gpio_num_t)37, 0 /* LOW */);
            if (time_in_us > 0)
            {
                esp_sleep_enable_timer_wakeup(time_in_us);
            }
            else
            {
                esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
            }
            (time_in_us == 0) ? esp_deep_sleep_start() : esp_deep_sleep(time_in_us);
        }

        void AXP192Component::LightSleep(uint64_t time_in_us)
        {
            if (time_in_us > 0)
            {
                esp_sleep_enable_timer_wakeup(time_in_us);
            }
            else
            {
                esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
            }
            esp_light_sleep_start();
        }

        // 0 not press, 0x01 long press, 0x02 press
        uint8_t AXP192Component::GetBtnPress()
        {
            uint8_t state = Read8bit(0x46);
            if (state)
            {
                Write1Byte(0x46, 0x03);
            }
            return state;
        }

        uint8_t AXP192Component::GetWarningLevel(void)
        {
            return Read8bit(0x47) & 0x01;
        }

        float AXP192Component::GetBatVoltage()
        {
            float ADCLSB = 1.1 / 1000.0;
            uint16_t ReData = Read12Bit(0x78);
            return ReData * ADCLSB;
        }

        float AXP192Component::GetBatCurrent()
        {
            float ADCLSB = 0.5;
            uint16_t CurrentIn = Read13Bit(0x7A);
            uint16_t CurrentOut = Read13Bit(0x7C);
            return (CurrentIn - CurrentOut) * ADCLSB;
        }

        float AXP192Component::GetVinVoltage()
        {
            float ADCLSB = 1.7 / 1000.0;
            uint16_t ReData = Read12Bit(0x56);
            return ReData * ADCLSB;
        }

        float AXP192Component::GetVinCurrent()
        {
            float ADCLSB = 0.625;
            uint16_t ReData = Read12Bit(0x58);
            return ReData * ADCLSB;
        }

        float AXP192Component::GetVBusVoltage()
        {
            float ADCLSB = 1.7 / 1000.0;
            uint16_t ReData = Read12Bit(0x5A);
            return ReData * ADCLSB;
        }

        float AXP192Component::GetVBusCurrent()
        {
            float ADCLSB = 0.375;
            uint16_t ReData = Read12Bit(0x5C);
            return ReData * ADCLSB;
        }

        float AXP192Component::GetTempInAXP192()
        {
            float ADCLSB = 0.1;
            const float OFFSET_DEG_C = -144.7;
            uint16_t ReData = Read12Bit(0x5E);
            return OFFSET_DEG_C + ReData * ADCLSB;
        }

        float AXP192Component::GetBatPower()
        {
            float VoltageLSB = 1.1;
            float CurrentLCS = 0.5;
            uint32_t ReData = Read24bit(0x70);
            return VoltageLSB * CurrentLCS * ReData / 1000.0;
        }

        float AXP192Component::GetBatChargeCurrent()
        {
            float ADCLSB = 0.5;
            uint16_t ReData = Read13Bit(0x7A);
            return ReData * ADCLSB;
        }

        float AXP192Component::GetAPSVoltage()
        {
            float ADCLSB = 1.4 / 1000.0;
            uint16_t ReData = Read12Bit(0x7E);
            return ReData * ADCLSB;
        }

        float AXP192Component::GetBatCoulombInput()
        {
            uint32_t ReData = Read32bit(0xB0);
            return ReData * 65536 * 0.5 / 3600 / 25.0;
        }

        float AXP192Component::GetBatCoulombOut()
        {
            uint32_t ReData = Read32bit(0xB4);
            return ReData * 65536 * 0.5 / 3600 / 25.0;
        }

        void AXP192Component::SetCoulombClear()
        {
            Write1Byte(0xB8, 0x20);
        }

        void AXP192Component::SetLDO2(bool State)
        {
            ESP_LOGD(TAG, "SetLDO2(): %s", State ? "true" : "false");

            uint8_t buf = Read8bit(0x12);
            if (State == true)
            {
                buf = (1 << 2) | buf;
            }
            else
            {
                buf = ~(1 << 2) & buf;
            }
            Write1Byte(0x12, buf);
        }

        void AXP192Component::SetLDO3(bool State)
        {
            ESP_LOGD(TAG, "SetLDO3(): %s", State ? "true" : "false");

            uint8_t buf = Read8bit(0x12);
            if (State == true)
            {
                buf = (1 << 3) | buf;
            }
            else
            {
                buf = ~(1 << 3) & buf;
            }
            Write1Byte(0x12, buf);
        }

        void AXP192Component::SetChargeCurrent(uint8_t current)
        {
            uint8_t buf = Read8bit(0x33);
            buf = (buf & 0xf0) | (current & 0x07);
            Write1Byte(0x33, buf);
        }

        void AXP192Component::PowerOff()
        {
            Write1Byte(0x32, Read8bit(0x32) | 0x80);
        }

        void AXP192Component::SetAdcState(bool state)
        {
            Write1Byte(0x82, state ? 0xff : 0x00);
        }

        std::string AXP192Component::GetStartupReason()
        {
            esp_reset_reason_t reset_reason = ::esp_reset_reason();
            if (reset_reason == ESP_RST_DEEPSLEEP)
            {
                esp_sleep_source_t wake_reason = esp_sleep_get_wakeup_cause();
                if (wake_reason == ESP_SLEEP_WAKEUP_EXT0)
                    return "ESP_SLEEP_WAKEUP_EXT0";
                if (wake_reason == ESP_SLEEP_WAKEUP_EXT0)
                    return "ESP_SLEEP_WAKEUP_EXT0";
                if (wake_reason == ESP_SLEEP_WAKEUP_EXT1)
                    return "ESP_SLEEP_WAKEUP_EXT1";
                if (wake_reason == ESP_SLEEP_WAKEUP_TIMER)
                    return "ESP_SLEEP_WAKEUP_TIMER";
                if (wake_reason == ESP_SLEEP_WAKEUP_TOUCHPAD)
                    return "ESP_SLEEP_WAKEUP_TOUCHPAD";
                if (wake_reason == ESP_SLEEP_WAKEUP_ULP)
                    return "ESP_SLEEP_WAKEUP_ULP";
                if (wake_reason == ESP_SLEEP_WAKEUP_GPIO)
                    return "ESP_SLEEP_WAKEUP_GPIO";
                if (wake_reason == ESP_SLEEP_WAKEUP_UART)
                    return "ESP_SLEEP_WAKEUP_UART";
                return std::string{"WAKEUP_UNKNOWN_REASON"};
            }

            if (reset_reason == ESP_RST_UNKNOWN)
                return "ESP_RST_UNKNOWN";
            if (reset_reason == ESP_RST_POWERON)
                return "ESP_RST_POWERON";
            if (reset_reason == ESP_RST_SW)
                return "ESP_RST_SW";
            if (reset_reason == ESP_RST_PANIC)
                return "ESP_RST_PANIC";
            if (reset_reason == ESP_RST_INT_WDT)
                return "ESP_RST_INT_WDT";
            if (reset_reason == ESP_RST_TASK_WDT)
                return "ESP_RST_TASK_WDT";
            if (reset_reason == ESP_RST_WDT)
                return "ESP_RST_WDT";
            if (reset_reason == ESP_RST_BROWNOUT)
                return "ESP_RST_BROWNOUT";
            if (reset_reason == ESP_RST_SDIO)
                return "ESP_RST_SDIO";
            return std::string{"RESET_UNKNOWN_REASON"};
        }

    }
}
