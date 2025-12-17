# NeoPocket
 Fancy accelerometer pocket lights with USB programming!

# How to reprogram my device?

1. Download and install Arduino: https://www.arduino.cc/en/software/
2. Install Megatiny Core for Arduino: [https://github.com/SpenceKonde/megaTinyCore/blob/master/Installation.md](https://github.com/SpenceKonde/megaTinyCore/blob/master/Installation.md#boards-manager-installation-now-strongly-recommended-unless-helping-with-core-development)
3. Download this repository and open NeoPocket.ino in arduino.
4. Plug your NeoPocket device into your computer via USB.
5. Select the port that it connected to in Tools > Port.
6. Select the board with Tools > Board > MegaTinyCore > Attiny3226/1626/826/426 w/Optiboot
7. Enter the following settings:
   - Clock: 10mhz
   - BOD Voltage Level: 2.6V
   - BOD: enabled/sampled 125hz
   - RST on PB4 (default)
   - Startup time: 64ms
9. You should now be able to upload! Set your animation in _Configuration.h

