import sys
import time
import yeti


class BasicTeleop(yeti.IterativeModule):
    last_gripper_change = 0
    gripper_state = False
    last_trigger_state = True

    panopticon_state = [90, 90, 90]

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

        if buttons[0] and not self.last_trigger_state:
            self.gripper_state = not self.gripper_state
            self.last_gripper_change = time.time()

        if time.time() - self.last_gripper_change < 1:
            self.set_state("0_switch_state", self.gripper_state, "bridge")
            self.set_state("1_switch_state", not self.gripper_state, "bridge")
        else:
            self.set_state("0_switch_state", False, "bridge")
            self.set_state("1_switch_state", False, "bridge")

        self.last_trigger_state = buttons[0]

        if buttons[8]:
            self.panopticon_state[0] += 1
        if buttons[9]:
            self.panopticon_state[0] -= 1
        if buttons[10]:
            self.panopticon_state[1] += 1
        if buttons[11]:
            self.panopticon_state[1] -= 1

        self.set_state("x_servo", self.panopticon_state[0], "bridge")
        self.set_state("y_servo", self.panopticon_state[1], "bridge")
        self.set_state("z_servo", self.panopticon_state[2], "bridge")

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

