#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 6
#define NUMPIXELS 300
#define HSVRANGE 65536
#define LOGGING false

class Clock {
  public:
    long _refTime, _delayTime, _count;
    bool _loop, _state;
    Clock(int delayTime, bool looping) {
      _refTime = 0;
      _count = 0;
      _delayTime = delayTime;
      _loop = looping;
      _state = false;
    }

    bool update() {
      if (millis() > _refTime + _delayTime) {
        if (_loop) {
          _refTime = millis();
        }
        _state = true;
        _count += 1;
        return true;
      }
      _state = false;
      return false;
    }

    bool getState() {
      return _state;
    }

    int getCount() {
      return _count;
    }

    void reset() {
      _refTime = millis();
    }

    void setDelay(int delayTime) {
      _delayTime = delayTime;
    }
};

class Encoder {
  public:
    int _pinA, _pinB, _pinC, _prevState, _currState, _counter, _lastButton;
    String _currDir, _label;
    bool _button;
    Encoder(int pinA, int pinB, int pinC, String label) {
      _pinA = pinA;
      _pinB = pinB;
      _pinC = pinC;
      _label = label;
    }

    void setup() {
      pinMode(_pinA, INPUT);
      pinMode(_pinB, INPUT);
      pinMode(_pinC, INPUT_PULLUP);
      _prevState = digitalRead(_pinA);
      Serial.println(_label + "Done setting up");
    }  

    void update() {
      _currState = digitalRead(_pinA);
      if (_prevState != _currState && _currState == 1) {
        if (digitalRead(_pinB) != _currState) {
          _counter--;
          _currDir = "CCW";
        } else {
          _counter++;
          _currDir = "CW";
        }
        if (LOGGING) {
          Serial.print("[" + _label + "]: Dir: ");
          Serial.print(_currDir);
          Serial.print(" | Counter: ");
          Serial.println(_counter);
        }
      }
      _prevState = _currState;
      int button = digitalRead(_pinC);
      if (button == LOW) {
        if (millis() - _lastButton > 100) {
          _button = true;
        }
        _lastButton = millis();
      }
    }

    int getCounter() {
      return _counter;
    }

    int getButton() {
      if (_button) {
        _button = false;
        return true;
      }
      return _button;
    }
};

Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);


Encoder encs [4] = {Encoder(22, 23, 24, "Encoder 1"), Encoder(25, 26, 27, "Encoder 2"), Encoder(28, 29, 30, "Encoder 3"), Encoder(31, 32, 33, "Encoder 4")};
Clock lightClock = Clock(50, true);

void setup() {
  Serial.begin(9600);
  pixels.begin();
  pixels.setBrightness(255);
  randomSeed(analogRead(0));
  for (int i = 0; i < 4; i++) {
    encs[i].setup();
  }
}

void loop() {
  //rg_fast();
  lightClock.update();
  
  complexDial();
  for (int i = 0; i < 4; i++) {
    encs[i].update();
  }
  delay(1);
}


int angle = 0;
int counter = 0;

void complexDial() {
   if (lightClock.getState()) {
    lightClock.setDelay(abs(encs[2].getCounter()) * 10 + 10);

    counter++;
    counter = counter % 2;
    uint32_t color = pixels.Color(0, 0, 0); 
    if (encs[3].getButton()) {
      Serial.println("================================================================`");
      Serial.print("Hue of C1: ");
      Serial.println((abs(encs[0].getCounter()) * HSVRANGE / 200) % HSVRANGE);
      Serial.print("Hue of C2: ");
      Serial.println((abs(encs[1].getCounter()) * HSVRANGE / 200) % HSVRANGE);
      Serial.print("Delay time: ");
      Serial.println(abs(encs[2].getCounter()) * 10 + 10);
      Serial.print("Pixel density: ");
      Serial.println((abs(encs[3].getCounter() + 100) + 1));
    }
    if (counter % 2 == 0) {
      angle = (abs(encs[0].getCounter()) * HSVRANGE / 200) % HSVRANGE;
    } else {
      angle = (abs(encs[1].getCounter() + 100) * HSVRANGE / 200) % HSVRANGE;
    }
    color = pixels.gamma32(pixels.ColorHSV(angle, 255, 255));
    
    pixels.clear();
    int light_val = abs(encs[3].getCounter() + 100) + 1;
    for(int i = 0; i < NUMPIXELS; i++) {
      if ((i + 1) % (light_val) == 0) {
        pixels.setPixelColor(i, color);
      }
    }
    pixels.show();
  }
}

void simpleDial() {
  if (lightClock.getState()) {
    angle = (encs[0].getCounter() * HSVRANGE / 100) % HSVRANGE;
    pixels.clear();
    for(int i = 0; i < NUMPIXELS; i++) {
      if (i % 4 == 0) {
        pixels.setPixelColor(i, pixels.ColorHSV(0 + angle, 255, 255));//abs(encs[1].getCounter())));
      }
    }
    pixels.show();
  }
}

void rg_fast() {
  if (lightClock.getState()) {
    angle = (encs[0].getCounter() * HSVRANGE / 100) % HSVRANGE;
    Serial.println(angle);
    if (lightClock.getCount() % 2 == 0) {
      pixels.clear();
      pixels.fill(pixels.ColorHSV(0 + angle, 255, 20));
      pixels.show();
    } else {
      pixels.clear();
      pixels.fill(pixels.ColorHSV(HSVRANGE / 3 + angle, 255, 20));
      pixels.show();
    }
  }
  
}
