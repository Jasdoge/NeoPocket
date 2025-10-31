# NeoPocket
 Fancy accelerometer pocket lights with USB programming!

# How to reprogram my device?

1. Download and install Arduino: https://www.arduino.cc/en/software/
2. Install Megatiny Core for Arduino: [https://github.com/SpenceKonde/megaTinyCore/blob/master/Installation.md](https://github.com/SpenceKonde/megaTinyCore/blob/master/Installation.md#boards-manager-installation-now-strongly-recommended-unless-helping-with-core-development)
3. Download this repository and open NeoPocket.ino in arduino.
4. Plug your NeoPocket device into your computer via USB.
5. Select the port that it connected to in Tools > Port.
6. Select the board with Tools > Board > MegaTinyCore > Attiny3226/1626/826/426 w/Optiboot
7. Set clock to 20mhz, BOD Voltage Level to 2.6V, BOD to enabled hold wakeup/disabled, and leave the rest of the settings.
8. You should now be able to upload! Set your animation in _Configuration.h (more info on this soon).

