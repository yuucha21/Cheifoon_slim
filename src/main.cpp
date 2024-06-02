// clang-format off
#include <SPIFFS.h>
#include "main.hpp"
// clang-format on

const int LEFT_RELAY_MODULE_PIN = 1;   // 左のrelayModuleピン
const int RIGHT_RELAY_MODULE_PIN = 2;  // 右のrelayModuleピン

static m5::touch_state_t prev_state;  // タッチ状態管理用変数

long oldUseValue = 0;  // ダイヤルの管理用変数

const double TSP = 1.666;  // 小さじの単位秒数
const double TBSP = 5;     // 大さじの単位秒数

const int LEFT_SEASONING = 0;  // 左の調味料を使用するとき用の定数
const int RIGHT_SEASONING = 1;  // 右の調味料を使用するとき用の定数
int usingSeasoning = -99;  // どちらの調味料を使用するかの管理用変数

bool useFlag = false;   // 調味料を使用するかどうか
bool titleFlag = true;  // タイトル判定フラグ

// タッチ状態の種類と合致する定数
static constexpr const char* STATE_NAME[16] = {
    "none", "touch", "touch_end", "touch_begin",
    "___",  "hold",  "hold_end",  "hold_begin",
    "___",  "flick", "flick_end", "flick_begin",
    "___",  "drag",  "drag_end",  "drag_begin"};
const int TOUCH = 1;        // タッチイベント
const int TOUCH_BEGIN = 3;  // タッチ開始イベント

bool outSeasoning(int usingSeasoning, long useValue) {
    // 調味料を出す処理
    M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_executing.jpg", 0, 0);

    if (usingSeasoning == LEFT_SEASONING) {
        if (useValue >= 1 && useValue <= 2) {
            Serial.println("LEFT_RELAY_MODULE ON");
            Serial.println(String(useValue) + " tsp out.");
            Serial.println("Wait " + String(TSP * useValue) + " seconds.");
            digitalWrite(LEFT_RELAY_MODULE_PIN, HIGH);
            delay(TSP * useValue * 1000);
            digitalWrite(LEFT_RELAY_MODULE_PIN, LOW);
        } else if (useValue >= 3 && useValue <= 7) {
            useValue -= 2;
            Serial.println("LEFT_RELAY_MODULE ON");
            Serial.println(String(useValue) + " tbsp out.");
            Serial.println("Wait " + String(TBSP * useValue) + " seconds.");
            digitalWrite(LEFT_RELAY_MODULE_PIN, HIGH);
            delay(TBSP * useValue * 1000);
            digitalWrite(LEFT_RELAY_MODULE_PIN, LOW);
        }
    } else if (usingSeasoning == RIGHT_SEASONING) {
        if (useValue >= 1 && useValue <= 2) {
            Serial.println("RIGHT_RELAY_MODULE ON");
            Serial.println(String(useValue) + " tsp out.");
            Serial.println("Wait " + String(TSP * useValue) + " seconds.");
            digitalWrite(RIGHT_RELAY_MODULE_PIN, HIGH);
            delay(TSP * useValue * 1000);
            digitalWrite(RIGHT_RELAY_MODULE_PIN, LOW);
        } else if (useValue >= 3 && useValue <= 7) {
            useValue -= 2;
            Serial.println("RIGHT_RELAY_MODULE ON");
            Serial.println(String(useValue) + " tbsp out.");
            Serial.println("Wait " + String(TBSP * useValue) + " seconds.");
            digitalWrite(RIGHT_RELAY_MODULE_PIN, HIGH);
            delay(TBSP * useValue * 1000);
            digitalWrite(RIGHT_RELAY_MODULE_PIN, LOW);
        }
    }

    return true;
}

void drawUseRelayModule(long useValue) {
    // M5Dial.Speaker.tone(8000, 20);
    // M5Dial.Display.clear();

    // ここに画像を張り付ける！
    if (useValue == 0) {
        M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_0.jpg", 0, 0);
    } else if (useValue == 1) {
        M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_tsp1.jpg", 0, 0);
    } else if (useValue == 2) {
        M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_tsp2.jpg", 0, 0);
    } else if (useValue == 3) {
        M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_tbsp1.jpg", 0, 0);
    } else if (useValue == 4) {
        M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_tbsp2.jpg", 0, 0);
    } else if (useValue == 5) {
        M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_tbsp3.jpg", 0, 0);
    } else if (useValue == 6) {
        M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_tbsp4.jpg", 0, 0);
    } else if (useValue == 7) {
        M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_tbsp5.jpg", 0, 0);
    }
}

void setup(void) {
    M5_BEGIN();
    auto cfg = M5.config();
    M5Dial.begin(cfg, true, false);
    M5Dial.Display.setTextColor(GREEN);
    M5Dial.Display.setTextDatum(middle_center);
    // M5Dial.Display.setTextFont(&fonts::Orbitron_Light_24);
    M5Dial.Display.setFont(&fonts::Orbitron_Light_24);
    M5Dial.Display.setTextSize(1);

    pinMode(LEFT_RELAY_MODULE_PIN, OUTPUT);
    digitalWrite(LEFT_RELAY_MODULE_PIN, LOW);

    pinMode(RIGHT_RELAY_MODULE_PIN, OUTPUT);
    digitalWrite(RIGHT_RELAY_MODULE_PIN, LOW);

    SPIFFS.begin();
}

void loop(void) {
    M5_UPDATE();

    M5Dial.update();

    auto t = M5Dial.Touch.getDetail();

    if (titleFlag) {
        M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_title.jpg", 0, 0);
        M5_UPDATE();
        while (true) {
            M5_UPDATE();
            auto t = M5Dial.Touch.getDetail();
            if (prev_state != t.state) {
                prev_state = t.state;
                if (t.state == TOUCH) {
                    break;
                }
            }
        }
        titleFlag = false;
        useFlag = false;
        M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_select.jpg", 0, 0);
        M5_UPDATE();
    }

    if (!useFlag) {
        if (prev_state != t.state) {
            prev_state = t.state;
            M5.Lcd.drawJpgFile(SPIFFS, "/Cheifoon_select.jpg", 0, 0);

            if (t.state == TOUCH) {
                if (t.x >= 120) {
                    Serial.println("left seasoning");
                    usingSeasoning = LEFT_SEASONING;
                } else {
                    Serial.println("right seasoning");
                    usingSeasoning = RIGHT_SEASONING;
                }
                useFlag = true;
                M5Dial.Encoder.write(0);
                drawUseRelayModule(0);
            }
        }
    } else {
        long newUseValue = M5Dial.Encoder.read() / 4;
        if (newUseValue < 0) {
            M5Dial.Encoder.write(0);
            newUseValue = 0;
        } else if (newUseValue > 7) {
            M5Dial.Encoder.write(7 * 4);
            newUseValue = 7;
        }

        if (newUseValue != oldUseValue) {
            drawUseRelayModule(newUseValue);
            oldUseValue = newUseValue;
            Serial.println(newUseValue);
        }
        if (prev_state != t.state) {
            prev_state = t.state;
            if (t.state == TOUCH) {
                if (t.y >= 180) {
                    Serial.println("back event");
                    useFlag = false;
                } else if (t.y <= 50) {
                    if (newUseValue != 0) {
                        if (outSeasoning(usingSeasoning, newUseValue)) {
                            M5.Lcd.drawJpgFile(SPIFFS,
                                               "/Cheifoon_completed.jpg", 0, 0);
                            delay(1500);
                        }
                        useFlag = false;
                    }
                } else {
                    M5Dial.Encoder.write(0);
                }
            }
        }
    }
    if (M5Dial.BtnA.pressedFor(1500)) {
        titleFlag = true;
    }
}
