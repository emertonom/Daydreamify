This project is intended to convert a "current_device_params" file
from the Cardboard format to the Daydream format, so that the headset
will be marked as supporting the Daydream controller and keyboard,
but will retain its settings for lenses and physical configuration.

The conversion is extremely simple: we append a fixed set of bytes to
the end of the file, and then add that number of bytes to the VarInt 
stored starting at the 8th byte of the file (which indicates the file length).
(This may shift the contents back a bit.)

