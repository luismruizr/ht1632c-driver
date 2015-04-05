This is a Arduino library to make use of those usefull matrix led displays that runs with the display driver chip HT1632C from Holtek.<br>
Some of them:<br>
http://www.adafruit.com/products/555<br>
http://www.sureelectronics.net/goods.php?id=1137<br>
http://dx.com/p/jy-mcu-3208-lattice-clock-ht1632c-driver-with-mcu-support-secondary-development-104306<br>
This last one needs some little desolder skills if you want to drive it directly from Arduino, but it's really cheap.<br>
The main goal of this libray are fast and smooth display animations. This is at the cost of some more high memory use (screeen buffer in ram).<br>
Reduced size where is possible without sacrifice the speed it's a secondary objetive.<br>
This Code is based on the work "Holtech HT1632 Driver Class" from Steven Moughan.<br>
http://hackdev.com/sourcecode/ht1632-driver-arduino/<br>

As stated by the license (CC BY_NC_SA 3.0) of the Steven code, and as this is a derived work, Steven it's not responsible, endorse or support this code in any implicit or explicit way.<br>
There are snipets of what I think is public domain code (Bresenham's line, circle, some fonts...)<br>
If you think that any part of this code are violating a licence statement or intellectual property, please let me know so I can replace the code.<br>
Constructive criticism, ideas, corrections and patches are welcomed.<br>
I make that extensive to the project page, wiki or license issues.<br>
Any help to make the code support a broader type of modules will be greatly appreciated.<br>
