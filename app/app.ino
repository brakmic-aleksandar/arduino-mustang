#include <hiduniversal.h>

constexpr uint8_t BUTTON_PREVIOUS_PRESET = 22;
constexpr uint8_t BUTTON_NEXT_PRESET = 23;

constexpr uint8_t BUTTON_SET_BANK_0 = 26;
constexpr uint8_t BUTTON_SET_BANK_1 = 27;
constexpr uint8_t BUTTON_SET_BANK_2 = 28;

constexpr uint8_t BUTTON_SET_PRESET_0 = 32;
constexpr uint8_t BUTTON_SET_PRESET_1 = 33;
constexpr uint8_t BUTTON_SET_PRESET_2 = 34;
constexpr uint8_t BUTTON_SET_PRESET_3 = 35;
constexpr uint8_t BUTTON_SET_PRESET_4 = 36;
constexpr uint8_t BUTTON_SET_PRESET_5 = 37;
constexpr uint8_t BUTTON_SET_PRESET_6 = 38;
constexpr uint8_t BUTTON_SET_PRESET_7 = 39;

constexpr uint8_t BANKS_COUNT = 8;
constexpr uint8_t PRESETS_PER_BANK_COUNT = 8;
constexpr uint8_t PRESETS_COUNT = BANKS_COUNT * PRESETS_PER_BANK_COUNT;

class MustangAmp : public HIDUniversal
{
    struct Payload
    {
        uint8_t data[64]{};
    };

    USB usb;
    uint8_t currentPresetIndex = 0;
    bool isCommunicationEnabled = false;

    void SendMessage(Payload &payload)
    {
        pUsb->outTransfer(bAddress, epInfo[hidInterfaces[0].epIndex[epInterruptOutIndex]].epAddr, sizeof(payload.data), payload.data);
    }

    bool IsLastPreset(uint8_t index)
    {
        return currentPresetIndex + 1 == PRESETS_COUNT;
    }

    bool IsFirstPreset(uint8_t index)
    {
        return currentPresetIndex == 0;
    }

public:
    MustangAmp() : HIDUniversal(&usb) {}

    void UpdateState()
    {
        usb.Task();
    }

    bool Init()
    {
        return usb.Init() == 0;
    }

    bool IsConnected()
    {
        return isReady();
    }
    
    void EnableCommunication()
    {
        Payload pl{};
        pl.data[1] = 0xC3;
        SendMessage(pl);
        
        isCommunicationEnabled = true;
    }

    bool IsCommunicationEnabled()
    {
        return isCommunicationEnabled;
    }

    void SetPreset(uint8_t index)
    {
        Serial.print(index, DEC);
        if(index > PRESETS_COUNT - 1)
            return;

        if(currentPresetIndex == index)
            return;

        currentPresetIndex = index;

        Payload p{};
        p.data[0] = 0x1C;
        p.data[1] = 0xC3;
        p.data[2] = 0x01;
        p.data[4] = index;
        p.data[6] = 0x01;
        
        SendMessage(p);
    }

    void NextPreset()
    {
        if(IsLastPreset(currentPresetIndex))
            SetPreset(0);
        
        SetPreset(currentPresetIndex + 1);
    }

    void PreviousPreset()
    { 
        if(IsFirstPreset(currentPresetIndex))
            SetPreset(PRESETS_COUNT - 1);
        
        SetPreset(currentPresetIndex - 1);
    }

    void SetBank(uint8_t index)
    { 
        if(index > BANKS_COUNT - 1)
            return;

        if(CurrentBank() == index)
            return;

        SetPreset(index * PRESETS_PER_BANK_COUNT);
    }

    void SetPresetInsideOfCurrentBank(uint8_t index)
    { 
        if(index > PRESETS_PER_BANK_COUNT)
            return;

        SetPreset((CurrentBank() + 1) * index);
    }

    uint8_t CurrentBank()
    {        
        if(currentPresetIndex >= 0 && currentPresetIndex <= 7)
            return 0;
        if(currentPresetIndex >= 8 && currentPresetIndex <= 15)
            return 1;
        if(currentPresetIndex >= 16 && currentPresetIndex <= 23)
            return 2;
    }

protected:
    void ParseHIDData(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
    {
        if(buf[0] == 0x1C && buf[2] == 0x04 && buf[6] == 0x01)
        {
            currentPresetIndex = buf[4];
        }
        if(buf[0] == 0x00 && buf[1] == 0x00)
        {
            isCommunicationEnabled = true;
        }
    };

} amp;

using ButtonPressHandler = void (*)();

template<int PIN> void HandleButton(ButtonPressHandler onButtonPress)
{
    static int currentValue = LOW;

    int readValue = digitalRead(PIN);
    
    if (readValue != currentValue)
    {
        currentValue = readValue;
        if (currentValue == HIGH)
        {
            onButtonPress();
        }
    }
}

void setup()
{
   Serial.begin(9600);
   while (!Serial);
   Serial.print("Serial started\r\n");
   
   if (!amp.Init())
       Serial.print("OSC did not start\r\n");
}

void loop()
{
    amp.UpdateState();
    if (amp.IsConnected())
    {
        if (amp.IsCommunicationEnabled())
        { 
            HandleButton<BUTTON_NEXT_PRESET>([]()
            {
                amp.NextPreset();
            });
            HandleButton<BUTTON_PREVIOUS_PRESET>([]()
            {
                amp.PreviousPreset();
            });
            HandleButton<BUTTON_SET_BANK_0>([]()
            {
                amp.SetBank(0);
            });
            HandleButton<BUTTON_SET_BANK_1>([]()
            {
                amp.SetBank(1);
            });
            HandleButton<BUTTON_SET_BANK_2>([]()
            {
                amp.SetBank(2);
            });
            HandleButton<BUTTON_SET_PRESET_0>([]()
            {
                amp.SetPresetInsideOfCurrentBank(0);
            });
            HandleButton<BUTTON_SET_PRESET_1>([]()
            {
                amp.SetPresetInsideOfCurrentBank(1);
            });
            HandleButton<BUTTON_SET_PRESET_2>([]()
            {
                amp.SetPresetInsideOfCurrentBank(2);
            });
            HandleButton<BUTTON_SET_PRESET_3>([]()
            {
                amp.SetPresetInsideOfCurrentBank(3);
            });
            HandleButton<BUTTON_SET_PRESET_4>([]()
            {
                amp.SetPresetInsideOfCurrentBank(4);
            });
            HandleButton<BUTTON_SET_PRESET_5>([]()
            {
                amp.SetPresetInsideOfCurrentBank(5);
            });            
            HandleButton<BUTTON_SET_PRESET_6>([]()
            {
                amp.SetPresetInsideOfCurrentBank(6);
            });            
            HandleButton<BUTTON_SET_PRESET_7>([]()
            {
                amp.SetPresetInsideOfCurrentBank(7);
            });
        }
        else
        {
            amp.EnableCommunication();
        }
    }
}
