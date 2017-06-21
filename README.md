This project is intended to convert a "current_device_params" file
from the Cardboard format to the Daydream format, so that the headset
will be marked as supporting the Daydream controller and keyboard,
but will retain its settings for lenses and physical configuration.

The conversion is extremely simple: we append a fixed set of bytes to
the end of the file, and then add that number of bytes to the VarInt 
stored starting at the 8th byte of the file (which indicates the file length).
(This may shift the contents back a bit.)

Instructions for use:

Prerequisites:
ADB (from the Android SDK)

Step 1: 
Scan a suitable QR code for your headset, to set up lens correction
and so forth. If your device doesn't have a suitable code, you
may need to generate one using the instructions at
http://wwgc.firebaseapp.com/


Step 2:
Create or navigate to a directory you want to use for this purpose,
and open a command line there.

In the command line, execute this command:

adb pull /storage/emulated/0/Cardboard/current_device_params

This will copy the parameters for your headset onto your computer.

Step 3:
Run the daydreamify app.

Step 4:
Run these commands:
adb shell rm /storage/emulated/0/Cardboard/current_device_params
adb push current_device_params /storage/emulated/0/Cardboard

Step 5:
Go into the phone settings, and open Apps settings.
Scroll down to Google VR Services, and open it.
Click on Storage.
On the screen that comes up, choose Clear Cache, then Clear Data.
Both the Cache line and the Data line should now show 0.

Step 6:
Run Daydream again, and proceed through the setup screens.
When you get to the home Daydream screen, use the hamburger menu to
bring up Settings. The headset should show as "Default."
Click on this setting. Daydream will ask for permission to access files,
then permission to record photos. Give it both. It will bring up a screen
asking you to scan a QR code.  DO NOT SCAN THE QR CODE AGAIN.
Instead, hit the "back" button.

Now Daydream should still list your custom headset, but the 
Controller and Keyboard options should no longer be grayed out!

