#ifndef __AXP192_H__
#define __AXP192_H__

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome
{
    namespace axp192
    {

        enum AXP192Model
        {
            AXP192_M5STICKC = 0,
            AXP192_M5CORE2,
            AXP192_M5TOUGH,
            AXP192_TTGO_TCALL,
        };

#define SLEEP_MSEC(us) (((uint64_t)us) * 1000L)
#define SLEEP_SEC(us) (((uint64_t)us) * 1000000L)
#define SLEEP_MIN(us) (((uint64_t)us) * 60L * 1000000L)
#define SLEEP_HR(us) (((uint64_t)us) * 60L * 60L * 1000000L)

        enum AXP192ChargeCurrent : uint8_t
        {
            CURRENT_100MA = 0,
            CURRENT_190MA,
            CURRENT_280MA,
            CURRENT_360MA,
            CURRENT_450MA,
            CURRENT_550MA,
            CURRENT_630MA,
            CURRENT_700MA,
        };

        class AXP192Component : public PollingComponent, public i2c::I2CDevice
        {
        public:
            void set_model(AXP192Model model) { this->model_ = model; }
            void set_charge_current(AXP192ChargeCurrent charge_current) { this->charge_current_ = charge_current; }
            void set_batterylevel_sensor(sensor::Sensor *batterylevel_sensor) { batterylevel_sensor_ = batterylevel_sensor; }
            void set_batteryvoltage_sensor(sensor::Sensor *batteryvoltage_sensor) { batteryvoltage_sensor_ = batteryvoltage_sensor; }
            void set_batterycurrent_sensor(sensor::Sensor *batterycurrent_sensor) { batterycurrent_sensor_ = batterycurrent_sensor; }
            void set_vbusvoltage_sensor(sensor::Sensor *vbusvoltage_sensor) { vbusvoltage_sensor_ = vbusvoltage_sensor; }
            void set_vbuscurrent_sensor(sensor::Sensor *vbuscurrent_sensor) { vbuscurrent_sensor_ = vbuscurrent_sensor; }
            void set_vincurrent_sensor(sensor::Sensor *vincurrent_sensor) { vincurrent_sensor_ = vincurrent_sensor; }
            void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
            void set_brightness(float brightness)
            {
                brightness_ = brightness;
                UpdateBrightness();
            }

            void setup() override;
            void dump_config() override;
            float get_setup_priority() const override;
            void update() override;

            // -- sleep
            void SetSleep(void);
            void DeepSleep(uint64_t time_in_us = 0);
            void LightSleep(uint64_t time_in_us = 0);

            // void SetChargeVoltage( uint8_t );
            void SetChargeCurrent(uint8_t);
            float GetBatVoltage();
            float GetBatCurrent();
            float GetVinVoltage();
            float GetVinCurrent();
            float GetVBusVoltage();
            float GetVBusCurrent();
            float GetTempInAXP192();
            float GetBatPower();
            float GetBatChargeCurrent();
            float GetAPSVoltage();
            float GetBatCoulombInput();
            float GetBatCoulombOut();
            uint8_t GetWarningLevel(void);
            void SetCoulombClear();
            void SetLDO2(bool State);
            void SetLDO3(bool State);
            void SetAdcState(bool State);

            void PowerOff();

        private:
            static std::string GetStartupReason();

        protected:
            sensor::Sensor *batterylevel_sensor_;
            sensor::Sensor *batteryvoltage_sensor_;
            sensor::Sensor *batterycurrent_sensor_;
            sensor::Sensor *vbusvoltage_sensor_;
            sensor::Sensor *vbuscurrent_sensor_;
            sensor::Sensor *vincurrent_sensor_;
            sensor::Sensor *temperature_sensor_;
            float brightness_{1.0f};
            float curr_brightness_{-1.0f};
            AXP192Model model_;
            AXP192ChargeCurrent charge_current_;

            // M5 Stick Values
            // LDO2: Display backlight
            // LDO3: Display Control
            // RTC: Don't set GPIO1 as LDO
            // DCDC1: Main rail. When not set the controller shuts down.
            // DCDC3: Use unknown

            // M5Stack Core2 Values
            // LDO2: ILI9342C PWR (Display)
            // LD03: Vibration Motor

            // TTGO T-Call Values
            // LDO2: NC
            // LDO3: GPS_VDD
            // RTC: Don't set GPIO1 as LDO
            // DCDC1: NC
            // DCDC3: VDD3V3

            void begin(bool disableLDO2 = false, bool disableLDO3 = false, bool disableRTC = false, bool disableDCDC1 = false, bool disableDCDC3 = false);
            void UpdateBrightness();
            bool GetBatState();
            uint8_t GetBatData();

            // Coulomb calculation functions
            void EnableCoulombCounter(void);
            void DisableCoulombCounter(void);
            void StopCoulombCounter(void);
            void ClearCoulombCounter(void);
            uint32_t GetCoulombChargeData(void);
            uint32_t GetCoulombDischargeData(void);
            float GetCoulombData(void);

            uint8_t GetBtnPress(void);

            void Write1Byte(uint8_t Addr, uint8_t Data);
            uint8_t Read8bit(uint8_t Addr);
            uint16_t Read12Bit(uint8_t Addr);
            uint16_t Read13Bit(uint8_t Addr);
            uint16_t Read16bit(uint8_t Addr);
            uint32_t Read24bit(uint8_t Addr);
            uint32_t Read32bit(uint8_t Addr);
            void ReadBuff(uint8_t Addr, uint8_t Size, uint8_t *Buff);
        };
    }
}

#endif
