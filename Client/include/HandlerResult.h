#ifndef HANDLERRESULT_H
#define HANDLERRESULT_H

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

enum class EResult
{
    RES_OK,
    RES_FAILED
};

class HandlerResult :
    public IListener
{
public:
    HandlerResult();
    virtual ~HandlerResult();

public:
    void OnResultCommand(ECommand type, std::string result);
private:
    static std::map<std::string, StateLED> KEYS_STATE_LED;
    static std::map<std::string, ColorLED> KEYS_COLOR_LED;
    static std::map<std::string, EResult> KEYS_RESULT;
};

#endif // HANDLERRESULT_H
