/// @file
/// @brief  XInputController
///	@author  (C) 2023 ttsuki

#pragma once

#include <Windows.h>
#include <Xinput.h>

#include <cmath>
#include <optional>
#include <algorithm>

namespace ttsuki::xinput
{
    static inline std::optional<::XINPUT_STATE> GetStateRaw(int controller_index)
    {
        // Simply get the state of the controller from XInput.
        XINPUT_STATE state{};
        auto dwResult = ::XInputGetState(controller_index, &state);

        if (dwResult != 0)
            return std::nullopt;

        return state;
    }

    struct XInputGamePadStatus
    {
        float LX, LY;
        float RX, RY;
        float LT, RT;

        struct Buttons
        {
            unsigned DPadU : 1;
            unsigned DPadD : 1;
            unsigned DPadL : 1;
            unsigned DPadR : 1;
            unsigned Start : 1;
            unsigned Back : 1;
            unsigned LStick : 1;
            unsigned RStick : 1;
            unsigned Lb : 1;
            unsigned Rb : 1;
            unsigned _10 : 1;
            unsigned _11 : 1;
            unsigned A : 1;
            unsigned B : 1;
            unsigned X : 1;
            unsigned Y : 1;
        } Buttons;
    };

    static inline std::optional<XInputGamePadStatus> GetState(int controller_index)
    {
        const auto raw = GetStateRaw(controller_index);
        if (!raw)
            return std::nullopt;

        XInputGamePadStatus ret{};

        float lx = static_cast<float>(raw->Gamepad.sThumbLX);
        float ly = static_cast<float>(raw->Gamepad.sThumbLY);
        float ld = static_cast<float>(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        if (lx * lx + ly * ly > ld * ld)
        {
            //float len = std::sqrt(LX * LX + LY * LY);
            //ret.LX = std::clamp((LX / len) * (len - LD) / (32767.0f - LD), -1.0f, 1.0f);
            //ret.LY = std::clamp((LY / len) * (len - LD) / (32767.0f - LD), -1.0f, 1.0f);
            ret.LX = std::clamp(lx / 32767.0f, -1.0f, 1.0f);
            ret.LY = std::clamp(ly / 32767.0f, -1.0f, 1.0f);
        }

        float rx = static_cast<float>(raw->Gamepad.sThumbRX);
        float ry = static_cast<float>(raw->Gamepad.sThumbRY);
        float rd = static_cast<float>(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        if (rx * rx + ry * ry > rd * rd)
        {
            //float len = std::sqrt(RX * RX + RY * RY);
            //ret.RX = std::clamp((RX / len) * (len - RD) / (32767.0f - RD), -1.0f, 1.0f);
            //ret.RY = std::clamp((RY / len) * (len - RD) / (32767.0f - RD), -1.0f, 1.0f);
            ret.RX = std::clamp(rx / 32767.0f, -1.0f, 1.0f);
            ret.RY = std::clamp(ry / 32767.0f, -1.0f, 1.0f);
        }

        float lt = static_cast<float>(raw->Gamepad.bLeftTrigger);
        float rt = static_cast<float>(raw->Gamepad.bRightTrigger);
        float tt = static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
        if (lt >= tt)
        {
            //ret.LT = std::clamp((LT - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (255.0f - XINPUT_GAMEPAD_TRIGGER_THRESHOLD), 0.0f, 1.0f);
            ret.LT = std::clamp(lt / 255.0f, 0.0f, 1.0f);
        }

        if (rt >= tt)
        {
            //ret.RT = std::clamp((RT - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (255.0f - XINPUT_GAMEPAD_TRIGGER_THRESHOLD), 0.0f, 1.0f);
            ret.RT = std::clamp(rt / 255.0f, 0.0f, 1.0f);
        }

        std::memcpy(&ret.Buttons, &raw->Gamepad.wButtons, 2);

        return ret;
    }

    static inline void SetRumble(int controller_index, float low, float high)
    {
        ::XINPUT_VIBRATION vibration{};
        vibration.wLeftMotorSpeed = static_cast<WORD>(std::clamp(low, 0.0f, 1.0f) * 65535.0f);
        vibration.wRightMotorSpeed = static_cast<WORD>(std::clamp(high, 0.0f, 1.0f) * 65535.0f);
        ::XInputSetState(controller_index, &vibration);
    }
}
