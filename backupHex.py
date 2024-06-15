import os
import shutil
Import("env", "projenv")
board_config = env.BoardConfig()
board_config.update("build.hwids",[["0x1209","0xAD10"]])

def buildPostAction(source, target, env):
    shutil.copyfile(".pio/build/reset_pids/firmware.hex", "teensy40.hex")

env.AddPostAction("buildprog", buildPostAction)