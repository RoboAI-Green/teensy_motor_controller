import os
import shutil
Import("env", "projenv")

def buildPostAction(source, target, env):
    shutil.copyfile(".pio/build/reset_pids/firmware.hex", "teensy40.hex")

env.AddPostAction("buildprog", buildPostAction)