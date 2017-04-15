import sys
import time
import yeti


class BasicTeleop(yeti.IterativeModule):

    def set_thrusters(self, trans, rot):
        self.set_state("right_vert_thruster_state", trans[1] + rot[2], "bridge")
        self.set_state("left_vert_thruster_state", trans[1] - rot[2], "bridge")
        self.set_state("right_front_thruster_state", -trans[0] + trans[2] + rot[1], "bridge")
        self.set_state("left_front_thruster_state", trans[0] + trans[2] - rot[1], "bridge")
        self.set_state("right_rear_thruster_state", -trans[0] - trans[2] + rot[1], "bridge")
        self.set_state("left_rear_thruster_state", trans[0] - trans[2] - rot[1], "bridge")

    def update(self):
        axes = self.get_state("axes", "joystick")
        buttons = self.get_state("buttons", "joystick")

        vert = 0
        if buttons[4]:
            vert = 1
        if buttons[2]:
            vert = -1
        trans = [axes[0], vert, axes[1]]
        rot = [0, axes[2], 0]

        self.set_thrusters(trans, rot)

    def stop(self):
        self.set_thrusters([0, 0, 0], [0, 0, 0])

if __name__ == "__main__":
    yeti.bootstrap_module(BasicTeleop, sys.argv[1], sys.argv[2])

