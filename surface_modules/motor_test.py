import sys
import time
import yeti


class MotorTest(yeti.IterativeModule):
    last_toggle = 0

    def start(self):
        self.last_toggle = time.time()

    def update(self):
        if time.time() - self.last_toggle > 1:
            value = self.get_state("right_vert_thruster_state", "bridge")
            value = 1.0 if value == 0.0 else 0.0
            self.set_state("right_vert_thruster_state", not value, "bridge")
            self.last_toggle = time.time()

    def stop(self):
        self.reset_outputs()

if __name__ == "__main__":
    yeti.bootstrap_module(MotorTest, sys.argv[1], sys.argv[2])

