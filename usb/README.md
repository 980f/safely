This is a source library that eases use of libusb, and contains a copy of the linux parts of that within it.

It has been used with qtcreator, look for a .pro file to include in yours.

The 55-anyusb.rules (something like that) is a udev rules file that exposes all usb devices to anyone's use. 
You should only deploy that when hunting for your device, for deployment write a narrow rules file for each of your devices.

Brought to you by:
* andy.980f@gmail.com