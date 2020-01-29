Machines:
Emulator: ubuntu1804-008
Receiver: ubuntu1804-002
Sender: ubuntu1804-004

use "make" to compile



 1. on Emulator in ubuntu1804-008 : ./nEmulator-linux386 8760 ubuntu1804-002 8763 8762 ubuntu1804-004 8761 1 0.2 1
 2. on Receiver in ubuntu1804-002: ./receiver.sh ubuntu1804-008 8762 8763 out.txt
 3. on Sender in ubuntu1804-004 :./sender.sh ubuntu1804-008 8760 8761 small.txt

In practical, there is no ned to use emulator, therefore change the port of emulator by port of receiver
