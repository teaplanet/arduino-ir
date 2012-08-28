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
	if (Serial.available()) {
		byte c = Serial.read();
		if (c == 'r') {
			unsigned int data[MAX_RECV_SIZE] = {};
			int size = receive(data);
			Serial.print("[i] data size: ");
			Serial.print(size, DEC);
			Serial.println("");

			Serial.println("----- data -----");
			for (int i=0; i<size; ++i) {
				Serial.print(data[i]);
				Serial.print(", ");
			}
			Serial.println("");
			Serial.println("----------------");
		}
	}
}

unsigned int receive(unsigned int* data) {
	Serial.println("[i] Start to receive.");
	Serial.println("[i] Stop enter 's'. The other key to reset the data.");
	Serial.println("[i] Receiving...");

	// デバイス
	byte c = 0x00;			// シリアルからのコマンド入力
	int in = 0;				// IRの入力値
	int last = 1;			// 前回の最終入力値

	// 状態
	bool receiving = true;	// 受信中
	int pos = -1;			// データの位置

	// 時間
	unsigned long interval = 0;			// ON/OFF切り替え間隔(micro sec)
	unsigned long start = micros();		// 開始時刻

	while (receiving) {
		in = digitalRead(IR_RECV_PIN);
		if (in != last) {
			unsigned long now = micros();
			interval = now - start;
			start = now;
//			Serial.println(interval);

			if (pos >= 0) {
				data[pos] = interval;
			}

			if (++pos == MAX_RECV_SIZE) {
				Serial.println("[!] Receive buffer is full.");
				break;
			}
			last = in;
		}

		if (Serial.available()) {
			c = Serial.read();
			if (c == 's') {
				receiving = false;
			} else if ((c == 10) || (c == 13)) {
				// do nothing.
			} else {
				pos = -1;
				Serial.println("[!] Reset the received data.");
			}
		}
	}
	Serial.println("[i] Stop to receive.");
	return pos;
}
