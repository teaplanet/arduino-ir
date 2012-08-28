const long SPEED = 115200;		// 通信速度

// pin
const int IR_RECV_PIN = 2;		// 赤外線受光部ピン番号
const int IR_SEND_PIN = 12;		// 赤外線送信ピン番号
const int LED_PIN = 13;			// LED

// 赤外線信号
const int MAX_RECV_SIZE = 512;	// 送受信最大データ長

void setup() {
	pinMode(IR_RECV_PIN, INPUT);
	pinMode(IR_SEND_PIN, OUTPUT);
	pinMode(LED_PIN, OUTPUT);

	Serial.begin(SPEED);
	Serial.println("\ninitialized.");
}

void loop() {
	byte c = serialValue();
	if (c == 'r') {
		unsigned int data[MAX_RECV_SIZE] = {};
		int size = receive(data);
		Serial.print("data size: ");
		Serial.print(size, DEC);
		Serial.println("");
	}
}

unsigned int receive(unsigned int* data) {
	int pos = -1;
	unsigned int in = 0;
	unsigned int last = 1;

	unsigned long micSec = 0;
	const unsigned long max = 2000000;	// 2sec

	bool receiving = true;
	Serial.println("Ready to receive.");
	unsigned long start = micros();
	while (serialValue() != 's') {
//		start = micros();	// ここでstartを再計算すると計測値がとても小さくなる(4〜8ms)。
		while ((in = digitalRead(IR_RECV_PIN)) == last) {
			micSec = micros() - start;
			if (micSec >= max) {
				pos = -1;
				Serial.println("/");
				break;
			}
		}

		unsigned long now = micros();
		micSec = now - start;
		Serial.print(micSec, DEC);
		Serial.print(",");

		if (pos++ >= 0) {
			data[pos] = micSec;
		}

		last = in;
		start = now;
	}
	Serial.println("Stop to receive.");
	return --pos;
}

byte serialValue() {
	byte c = 0x00;
	if (Serial.available()) {
		c = Serial.read();
	}
	return c;
}
