import sys
import serial
import yeti
import time


class Bridge(yeti.IterativeModule):
    first_start = True
    last_update = 0
    update_count = 0
    last_periodic_update = 0

    thruster_mapping = {
        "left_rear": 1,
        "right_rear": 2,
        "left_front": 3,
        "right_front": 4,
        "left_vert": 5,
        "right_vert": 6
    }

    board_mapping = {
        "control_board": 0,
        "motor_board": 1,
        "panopticon_board": 2
    }

    servo_mapping = {
        "x": 0,
        "y": 1,
        "z": 2
    }

    def start(self):
        if self.first_start:
            self.serial = serial.Serial(port="/dev/ttyS1", baudrate=115200, timeout=0.005)
            self.first_start = False
            self.reset_outputs()
        if not self.serial.is_open:
            self.serial.open()

    def reset_outputs(self):
        for servo_key in self.servo_mapping:
            self.set_state("{}_servo".format(servo_key), 100)
        self.set_state("x_servo", 0)
        for board_key in self.board_mapping:
            self.set_state("{}_alive".format(board_key), False)
        for thruster_key in self.thruster_mapping:
            self.set_state("{}_thruster_state".format(thruster_key), 0)
        for i in range(8):
            self.set_state("{}_switch_state".format(i), False)
        self.set_state("7_switch_state", True)

    def update(self):
        start = time.time()
        if start-self.last_periodic_update > 1:
            print("Did {} updates in the last second.".format(self.update_count))
            self.last_periodic_update = start
            self.update_count = 0
            for board_key in self.board_mapping:
                last_alive = self.get_state("{}_alive".format(board_key))
                board_alive = self.get_hardware_state("a{}".format(self.board_mapping[board_key])) is not None
                self.set_state("{}_alive".format(board_key), board_alive)
                if last_alive is not board_alive:
                    if not board_alive:
                        print("{} disconnected.".format(board_key))
                    else:
                        print("{} connected.".format(board_key))
                #time.sleep(0.05)
            for thruster_key in self.thruster_mapping:
                if abs(self.get_state("{}_thruster_state".format(thruster_key))) > 0.5:
                    break
            else:
                self.set_hardware_state("r", b"")
        for servo_key in self.servo_mapping:
            servo_int = min(180, max(int(self.get_state("{}_servo".format(servo_key))), 0))
            value_bytes = servo_int.to_bytes(2, byteorder='little', signed=True)
            self.set_hardware_state(
                "p{}".format(self.servo_mapping[servo_key]),
                value_bytes
            )
        for thruster_key in self.thruster_mapping:
            thruster_float = min(1, max(self.get_state("{}_thruster_state".format(thruster_key)), -1))
            thruster_val = int(thruster_float*2400)
            value_bytes = thruster_val.to_bytes(2, byteorder='little', signed=True)
            self.set_hardware_state(
                "t{}".format(self.thruster_mapping[thruster_key]),
                value_bytes
            )
            #time.sleep(0.05)
        for i in range(8):
            switch_state = self.get_state("{}_switch_state".format(i))
            value_bytes = int(switch_state).to_bytes(1, byteorder='little')
            self.set_hardware_state(
                "s{}".format(i),
                value_bytes
            )
            #time.sleep(0.05)
        self.update_count += 1
        end = time.time()
        self.last_update = start
        #print(self.get_hardware_state("h"))

    def set_hardware_state(self, key, value_bytes):
        self.serial.write('s'.encode('utf-8'))

        key_bytes = key.encode('utf-8')
        self.serial.write(len(key_bytes).to_bytes(1, byteorder='little'))
        self.serial.write(key_bytes)

        self.serial.write(len(value_bytes).to_bytes(1, byteorder='little'))
        self.serial.write(value_bytes)
        #self.serial.flush()

    def get_hardware_state(self, key):
        self.serial.write('g'.encode('utf-8'))

        key_bytes = key.encode('utf-8')
        self.serial.write(len(key_bytes).to_bytes(1, byteorder='little'))
        self.serial.write(key_bytes)
        self.serial.flush()

        prefix = self.serial.read(1) # read 'r' message
        if len(prefix) == 0:
            return None
        assert prefix == "r".encode('utf-8')

        key_len = int.from_bytes(self.serial.read(1), byteorder='little')
        key_bytes = self.serial.read(key_len)

        value_len = int.from_bytes(self.serial.read(1), byteorder='little')
        value_bytes = self.serial.read(value_len)

        if len(value_bytes) < value_len:
            return None

        return value_bytes

    def stop(self):
        self.reset_outputs()
        if self.serial.is_open:
            self.serial.close()
        print("Halting problem solved!")

if __name__ == "__main__":
    yeti.bootstrap_module(Bridge, sys.argv[1], sys.argv[2])

