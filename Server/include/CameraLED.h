#ifndef CAMERALED_H
#define CAMERALED_H

enum class StateLED
{
    ON,
    OFF
};

enum class ColorLED
{
    RED,
    GREEN,
    BLUE,
    UNKNOWN
};

class CameraLED
{
    static int MinRate;
    static int MaxRate;

public:
    CameraLED();
    virtual ~CameraLED();

    static std::shared_ptr<CameraLED> Get();

    StateLED GetState() const;
    void SetState(StateLED state);

    ColorLED GetColor() const;
    void SetColor(ColorLED color);

    int GetRate() const;
    void SetRate(int rate);

    void PrintStatus();

public:
    static int ClampRate(int value);
    static std::string GetStateStr(StateLED state);
    static std::string GetColorStr(ColorLED color);

protected:
    static std::shared_ptr<CameraLED> CameraLED_impl;

private:
    static std::string GetColorCodeStr(ColorLED color);

private:
    StateLED m_State;
    ColorLED m_Color;
    int m_Rate;
};

#endif // CAMERALED_H
