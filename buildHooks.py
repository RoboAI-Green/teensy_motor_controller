#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# pylint: disable=undefined-variable
#
# This script is executed by PlatformIO before compiling the code, it's some sort of hook executed
# before everything in order to manually patch few things
#   - Make a backup copy of usb_hid/usb_private.h -> usb_hid/usb_private.h.backup (just once, the first time)
#       It's commented but you can resume it if you're running for the first time
#   - Change: VID,PID,Name in the usb_hid profile (file: usb_private.h)
# Slightly modified version: it creates a backup only if it does not exist yet
#
import os
import shutil
Import("env", "projenv")

deviceVID  = "0x1209"
devicePID  = "0xAD10"
propName = "Teensy Motor Controller"
deviceName = '{'
for c in list(propName):
    deviceName += "'{}',".format(c)

deviceName = deviceName[:-1] + "}"

# Edit USB device information
def editUSBsettings(fileName):
    # Search and replace inside the file
    fHandler = open(fileName, "rt")
    data = fHandler.read()
    data = data.replace("0x16C0", deviceVID)                            # Replace all occurrences of the original Vendor  ID (VID)
    data = data.replace("0x0483", devicePID)                            # Replace all occurrences of the original Product ID (PID)
    data = data.replace("{'U','S','B',' ','S','e','r','i','a','l'}", deviceName)   # Replace all occurrences of the original Device  Name
    data = data.replace("PRODUCT_NAME_LEN	10", "PRODUCT_NAME_LEN  {}".format(len(propName)))   # Replace all occurrences of the original Device  Name
    fHandler.close()
    # Reopen the input file in write mode
    fHandler = open(fileName, "wt")
    #overrite the input file with the resulting data
    fHandler.write(data)
    #close the file
    fHandler.close()

# Move back the original file
def buildPostAction(source, target, env):
    if os.path.exists(fileNameBackup):
        shutil.move(fileNameBackup, fileName)
    shutil.copyfile(".pio/build/teensy_libs/firmware.hex", "teensy_motor_controller.hex")

# Before compilation (BEGIN)
print("\n----------------------------------------------- Compilation setup [BEGIN]\n\n")
# usb_private.h backup
#print(env['CPPPATH'][0].split("teensy4")[0])
cpp_path = env['CPPPATH'][0]
fileName = "{}\{}".format(cpp_path,'usb_desc.h')

# fileName = os.path.sep.join([env['CPPPATH'][1]]+["usb_serial","usb_private.h"])
fileNameBackup = fileName+".backup"
if not os.path.exists(fileNameBackup):
    shutil.copyfile(fileName, fileNameBackup)
editUSBsettings(fileName)

print(fileName)

print("----------------------------------------------- Compilation setup [ END ]")

env.AddPostAction("upload", buildPostAction)
env.AddPostAction("buildprog", buildPostAction)