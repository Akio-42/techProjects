# An Arduino based fan controller that uses a 25kHz pwm signal
This firmware is written for an Arduino Mega.

## Requirements technical:
* produce a 25kHz pwm signal

## Requirements hardware:
* Arduino board
* enough interrupt pins for 3 fans
* enough interrupt pins for buttons/wheels
* MINI12864 V2.0 LCD-Display (common for older 3D printers)

## Requirements software:
* use the push button of the display for navigation
* use the rotary encoder of the display for navigation
* display the target pwm speed in percent (0%-100%) for each fan
* display the actual rpm for each fan
* shutoff the display when it is not used
* set the target pwm speed for all fans in percent (0%-100%)
* save the target pwm speed settings permanently

## Used Libraries:
* https://github.com/olikraus/u8g2 Library to draw on the LCD screen
* https://github.com/PaulStoffregen/Encoder Library to handle the rotary encoder wheel
* https://github.com/adafruit/Adafruit_NeoPixel Library to handle the leds (display background light and wheel accent lights)
Please see section "Library Licenses" for the license information of each library.

## Resources:
* https://www.bjonnh.net/article/20221126_mini_12864/ description how to use the MINI12864 V2.0 LCD-Display with an Arduino
* https://wolles-elektronikkiste.de/timer-und-pwm-teil-2-16-bit-timer1 Article how Arduino timers can be manipulated
* http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_datasheet.pdf ATmega data sheet
* https://noctua.at/pub/media/wysiwyg/Noctua_PWM_specifications_white_paper.pdf Noctua fan 5v data sheet

## Hardware Selection
As a display the MINI12864 V2.0 LCD-Display like e.g. https://biqu.equipment/de/products/bigtreetech-mini12864-v2-0-lcd-screen-rgb-backlight-mini-display-supports-marlin-diy-for-skr-3d-printer-part-1 was set, because I had a spare one. Also they are an interesting choice, because they have a lcd display, a rotary push button, addressable leds and a beeper in one package for often less than 15€.
As a controller board I also wanted to use an Arduino Uno I already had, but as the Uno only has two interrupt pins and I wanted to display the speed of three fans I used an Arduino Mega instead. The Mega has 6 interrupt pins, so 3 are used for the fans and the other three for the rotary wheel and button. As fans I used three Noctua NF-A6x25 5V fans. These fans do not draw much power, so all components can be powered by a single usb-c connection.

## Wiring / Schematics:

## 25 kHz PWM Signal with an Arduino Mega
To get a 25 kHz PWM Signal from an Arduino we have to jump through some hoops. Normally the Arduino has a frequency of 490 Hz for its PWM pins. The Noctua fans do run with a 490 Hz PWM Signal, but it is outside of their specification. And I found that when used that way the Noctua fans don't produce a speed signal that can be measured. The interrupt bound to to speed signal seemed to be triggered randomly. When operated With a 25 kHz PWM signal, the speed signal triggered the interrupt just fine.

To achieve this we can modify the Timer1 of the ATmega2560. Timer1 is controlling digital pin 11 and 12, but we need to sacrifice pin 11 in this configuration. For this use case this does not matter as all three fans should run at the same speed. 

### Setup
* Clear the three timer registers of timer 1 - TCCR1A, TCCR1B, TCCR1C.
** This clears all settings that might be already there.
* Set the PWM mode of timer 1 to fast pwm with OCR1A as TOP of timer 1 -  WGM11, WGM10, WGM13, WGM12
** In this mode the timer counts up in each cycle before hitting the value set in OCR1A. If the timer hits the value it begins counting from 0 again. This allows us to fine tune the time the timer needs to count up for each cycle and with that the pwm frequency. But we sacrifice pin 11 that corresponds with OCR1A for the use of a pwm output.
* Set prescaler of timer 1to 8 -  CS11
** This sets the prescaler of the timer to a range that will allow to configure 25 kHz with OCR1A
* Set OCR1B as compare match (non-inverting) for the Compare Output Mode of timer 1 in fast PWM - COM1B1 
** With this setting timer1 will turn off/on the pin 12 (corresponding with OCR1B) when the the value configured to OCR1B matches with the current timer value. This way we can control the duty cycle.
*Set pinMode of pin 12 to output

#### Configure OCR1A and OCR1B values
    OCR1AH = 0;
    OCR1AL = 79; -> setting this value to 79 will result in a 25 kHz PWM signal (together with prescacler 8)

    OCR1BH = 0;
    OCR1BL = xx;  // set this value to the desired fan speed value. Note that the maximum value is the value set to OCR1AL - 1. Otherwise the compare will never be successful and we will not get a valid PWM output signal. OCR1AL - 1 (in this example 78) will produce a 100% duty cycle. 0 will produce a 0% duty cycle. 

## Please note:
This project and all code I wrote are provided as-is and are intended for educational purposes only. While this information is meant to guide and inspire other makers, there is no guarantee that the code is error-free or suitable for every situation. I am not responsible for any damage or issues that arise from simply copying and pasting the provided code without proper understanding or modification. Always review and test code thoroughly before using it in your own projects.

















# Library Licenses
## olikraus / u8g2 - https://github.com/olikraus/u8g2
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## PaulStoffregen / Encoder - https://github.com/PaulStoffregen/Encoder
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

## adafruit / Adafruit_NeoPixel - https://github.com/adafruit/Adafruit_NeoPixel
                   GNU LESSER GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.


  This version of the GNU Lesser General Public License incorporates
the terms and conditions of version 3 of the GNU General Public
License, supplemented by the additional permissions listed below.

  0. Additional Definitions.

  As used herein, "this License" refers to version 3 of the GNU Lesser
General Public License, and the "GNU GPL" refers to version 3 of the GNU
General Public License.

  "The Library" refers to a covered work governed by this License,
other than an Application or a Combined Work as defined below.

  An "Application" is any work that makes use of an interface provided
by the Library, but which is not otherwise based on the Library.
Defining a subclass of a class defined by the Library is deemed a mode
of using an interface provided by the Library.

  A "Combined Work" is a work produced by combining or linking an
Application with the Library.  The particular version of the Library
with which the Combined Work was made is also called the "Linked
Version".

  The "Minimal Corresponding Source" for a Combined Work means the
Corresponding Source for the Combined Work, excluding any source code
for portions of the Combined Work that, considered in isolation, are
based on the Application, and not on the Linked Version.

  The "Corresponding Application Code" for a Combined Work means the
object code and/or source code for the Application, including any data
and utility programs needed for reproducing the Combined Work from the
Application, but excluding the System Libraries of the Combined Work.

  1. Exception to Section 3 of the GNU GPL.

  You may convey a covered work under sections 3 and 4 of this License
without being bound by section 3 of the GNU GPL.

  2. Conveying Modified Versions.

  If you modify a copy of the Library, and, in your modifications, a
facility refers to a function or data to be supplied by an Application
that uses the facility (other than as an argument passed when the
facility is invoked), then you may convey a copy of the modified
version:

   a) under this License, provided that you make a good faith effort to
   ensure that, in the event an Application does not supply the
   function or data, the facility still operates, and performs
   whatever part of its purpose remains meaningful, or

   b) under the GNU GPL, with none of the additional permissions of
   this License applicable to that copy.

  3. Object Code Incorporating Material from Library Header Files.

  The object code form of an Application may incorporate material from
a header file that is part of the Library.  You may convey such object
code under terms of your choice, provided that, if the incorporated
material is not limited to numerical parameters, data structure
layouts and accessors, or small macros, inline functions and templates
(ten or fewer lines in length), you do both of the following:

   a) Give prominent notice with each copy of the object code that the
   Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the object code with a copy of the GNU GPL and this license
   document.

  4. Combined Works.

  You may convey a Combined Work under terms of your choice that,
taken together, effectively do not restrict modification of the
portions of the Library contained in the Combined Work and reverse
engineering for debugging such modifications, if you also do each of
the following:

   a) Give prominent notice with each copy of the Combined Work that
   the Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the Combined Work with a copy of the GNU GPL and this license
   document.

   c) For a Combined Work that displays copyright notices during
   execution, include the copyright notice for the Library among
   these notices, as well as a reference directing the user to the
   copies of the GNU GPL and this license document.

   d) Do one of the following:

       0) Convey the Minimal Corresponding Source under the terms of this
       License, and the Corresponding Application Code in a form
       suitable for, and under terms that permit, the user to
       recombine or relink the Application with a modified version of
       the Linked Version to produce a modified Combined Work, in the
       manner specified by section 6 of the GNU GPL for conveying
       Corresponding Source.

       1) Use a suitable shared library mechanism for linking with the
       Library.  A suitable mechanism is one that (a) uses at run time
       a copy of the Library already present on the user's computer
       system, and (b) will operate properly with a modified version
       of the Library that is interface-compatible with the Linked
       Version.

   e) Provide Installation Information, but only if you would otherwise
   be required to provide such information under section 6 of the
   GNU GPL, and only to the extent that such information is
   necessary to install and execute a modified version of the
   Combined Work produced by recombining or relinking the
   Application with a modified version of the Linked Version. (If
   you use option 4d0, the Installation Information must accompany
   the Minimal Corresponding Source and Corresponding Application
   Code. If you use option 4d1, you must provide the Installation
   Information in the manner specified by section 6 of the GNU GPL
   for conveying Corresponding Source.)

  5. Combined Libraries.

  You may place library facilities that are a work based on the
Library side by side in a single library together with other library
facilities that are not Applications and are not covered by this
License, and convey such a combined library under terms of your
choice, if you do both of the following:

   a) Accompany the combined library with a copy of the same work based
   on the Library, uncombined with any other library facilities,
   conveyed under the terms of this License.

   b) Give prominent notice with the combined library that part of it
   is a work based on the Library, and explaining where to find the
   accompanying uncombined form of the same work.

  6. Revised Versions of the GNU Lesser General Public License.

  The Free Software Foundation may publish revised and/or new versions
of the GNU Lesser General Public License from time to time. Such new
versions will be similar in spirit to the present version, but may
differ in detail to address new problems or concerns.

  Each version is given a distinguishing version number. If the
Library as you received it specifies that a certain numbered version
of the GNU Lesser General Public License "or any later version"
applies to it, you have the option of following the terms and
conditions either of that published version or of any later version
published by the Free Software Foundation. If the Library as you
received it does not specify a version number of the GNU Lesser
General Public License, you may choose any version of the GNU Lesser
General Public License ever published by the Free Software Foundation.

  If the Library as you received it specifies that a proxy can decide
whether future versions of the GNU Lesser General Public License shall
apply, that proxy's public statement of acceptance of any version is
permanent authorization for you to choose that version for the
Library.