Some brief info for you to make code with :)

The keyboard uses an TCA8418 IC to save i/o pins. This stores the key events in a buffer so that they can be read as needed
https://www.ti.com/lit/gpn/TCA8418

The RTC is a RV-3032-C7 which allows the date/time to be stored for a better user experience. \*\*This will probably be replaced with a cheaper part in future boards\*\*
https://www.microcrystal.com/fileadmin/Media/Products/RTC/App.Manual/RV-3032-C7\_App-Manual.pdf

The screen is GDEY029T71H from GoodDisplay. It is a higher resolution 2.9" screen and still has the fast refresh
https://www.good-display.com/product/540.html

There is currently a li-po fuel guage IC (MAX1704x) to read the battery voltage and percentage. \*\*This will be replaced in future boards with a custom power system\*\*

https://www.analog.com/media/en/technical-documentation/data-sheets/MAX17043-MAX17044.pdf



There will be 2 i2c ports (stemma/qwiic), the USB port, then the m.2 add-on slot

I2c 1 is on the same pins as the internal stuff for the keyboard and RTC (SDA:pin 1 - SCL:pin 2 - INT:pin 3), they all share the same INT pin to save pins
I2c 2 share some of the data pins from the m.2 slot (SDA: pin 47 - SCL:pin 48)
The USB is connected directly to the ESP
The m.2 slot has basically everything connected. USB/UART/i2c/pins 8,15,16,17,18,21,35,36,37,38,39,40,41,42,47,48 (although pins 35,36,37 are tied to the PSRAM so may be useless!!)

The screen and SD card share spi pins to maximise the available pins for the add-on slot



Here is a full list of the pins





1 - I2C SDA

2 - I2C SCL

3 - I2C INT

4 - Backlight (currently WS2812, but will be a single colour LED in the future)

5 - Buzzer

6 - SPI CS for the screen

7 - SPI CS for the SD Card

9 - SPI Clock

10 - SPI MOSI

11 - SPI MISO

13 - SPI D/C

12 - SPI Reset

14 - Screen busy pin

45 - Kill pin (this should cut the power when you pull it low, but it is a strapping pin, and so it doesn't actually work. This wont be in future versions)

