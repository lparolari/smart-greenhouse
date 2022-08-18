#pragma once

#include <Arduino.h>
#include <RTClib.h>

namespace greenhouse::rtc
{
    using DateTime = DateTime;
    using AlarmMode = Ds3231Alarm1Mode;

    template <class RealTimeClock = RTC_DS3231>
    class Rtc
    {
    private:
        RealTimeClock _rtc;

    public:
        Rtc() = default;

        void begin()
        {
            // We need to follow many steps in order to initialize the RTC, as reference we follow
            // https://github.com/adafruit/RTClib/blob/master/examples/DS3231_alarm/DS3231_alarm.ino

            // Initialize RTC
            _rtc.begin();

            if (_rtc.lostPower())
            {
                // Adjust to the date and time at compilation
                _rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
            }

            // Disable 32K pin as not needed
            _rtc.disable32K();

            // Set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
            // If not done, this easily leads to problems, as both register aren't reset on reboot/recompile
            _rtc.clearAlarm(1);
            _rtc.clearAlarm(2);

            // Stop oscillating signals at SQW Pin otherwise setAlarm1 will fail
            _rtc.writeSqwPinMode(DS3231_OFF);

            // Turn off alarm 2 (in case it isn't off already)
            // Again, this isn't done at reboot, so a previously set alarm could easily go overlooked
            _rtc.disableAlarm(2);
        }

        DateTime now()
        {
            return _rtc.now();
        }

        void set_alarm(const DateTime &dt, AlarmMode alarm_mode = DS3231_A1_Date)
        {
            _rtc.setAlarm1(dt, alarm_mode);
        }

        bool alarm_fired()
        {
            return _rtc.alarmFired(1);
        }

        void clear_alarm()
        {
            _rtc.clearAlarm(1);
        }

        // @returns A reference to the raw implementation of RTC
        RealTimeClock &_raw_rtc()
        {
            return _rtc;
        }
    };

    String pad_number(int number, unsigned int digits = 2)
    {
        String s = String(number);
        while (s.length() < digits)
        {
            s = "0" + s;
        }
        return s;
    }

    String format_date_time(const DateTime& dt, bool seconds = true) {
        return String("")
            + dt.year() + "/"
            + pad_number(dt.month()) + "/"
            + pad_number(dt.day()) + " "
            + pad_number(dt.hour()) + ":"
            + pad_number(dt.minute()) 
            + (seconds ? ":" + pad_number(dt.second()) : "");
    }
} // namespace greenhouse::rtc
