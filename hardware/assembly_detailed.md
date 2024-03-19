# Detailed Assembly Instructions

## Collect materials

Assembling the macropad is relatively straightforward if you have experience
soldering through-hold components. There are a small number of discrete
components. The photo below lays them out.

![materials](./img/bom.jpeg)

On the left, top to bottom:

* The PCB.
* An Arduino Pro Micro and headers.

On the right, top to bottom (conveniently arranged in the order they'll be
assembled):

* Hot swap switch sockets.
* 1N4148 diodes.
* A 1k Ohm resistor.
* Reset/firmware flash button, 24LC256 EEPROM, and rotary encoder.
* 0.91" 128x32 OLED and wires for it.

The rest of these instructions are a suggested order for installing these
components. Note that there is one time when building that a fixed order is
required: __D10 and the resistor__ must be installed before the Arduino.

## Hot swap sockets

The hot swap sockets are the only component that may be unfamiliar to solder if
you are used to through-hold soldering.

Instead of trying to solder the socket tabs directly to the pads they mount on,
it is helpful to first put a blob of solder on the pad.

![solder on pad](./img/socket-pad-solder0.jpeg)

![solder on pads](./img/socket-pad-solder0.jpeg)

After putting a bit of solder on each pad, you can now install the sockets.

Pay attention to the orientation of the socket; if the socket is rotated
it will still fit in the holes in the PCB, but it will partially cover the hole
through which the switch stem will need to go.

![socket placement](./img/socket-placement.jpeg)

To solder the socket to the PCB, place the socket in its location. Then apply
your soldering iron to the socket's tabs. This will melt the solder beneath the
pad after a few seconds and solder the tab to the pad.

You may want to use a pair of tweezers to hold the socket and the tab down while
applying heat.

![socket soldering](./img/socket-soldering.jpeg)

![hold in place with tweezers](./img/socket-tweezers.jpeg)

![sockets completed](./img/socket-complete.jpeg)

## Diodes 1-9

Now you'll install the diodes. To solder the diode, put the body of the diode on
the bottom of the PCB (the side with the sockets) and the legs through the holes
to the top of the board.

Pay attention to the orientation of the diode -- the orientation matters. The
footprint symbol on the PCB indicates the orientation.

![diode orientation](./img/diode-orientation.jpeg)

After putting all the diodes in place, you'll end up with something like this.

![diodes installed](./img/diodes-installed.jpeg)

Solder the legs in place. Cut off the legs when you're done. You should have
something that looks like this:

![diodes complete](./img/diodes-complete.jpeg)

## Diode 10 and resistor

Diode 10 and the resistor should be placed on top of the board. Their legs will
go through the board and should be soldered from the bottom.

![D10 and the resistor ready to solder](./img/d10-ready.jpeg)

When you're complete, the top of the board should look like this.

![D10 and resistor installed](./img/d10-complete.jpeg)

## EEPROM, reset button, and rotary encoder

Now you can install the EEPROM, reset button, and rotary encoder on the top of
the PCB. All of these components have fixed orientations. The button and the
rotary encoder can only be installed one way, so those should be self-evident.

The EEPROM has a semi-circular notch and dot that indicate the orientation of
the IC and pin 1. Line up the notch with the footprint diagram on the PCB (see
image below).

![EEPROM orientation](./img/eeprom-orientation.jpeg)

You'll solder each of these components from the bottom. You should install the
EEPROM and the button first, using painters tape or sticky tack to hold them in
place if necessary.

Install the rotary encoder last. Note that you should solder the large tabs on
the side of the encoder, even though they do not carry signal. Soldering these
to the PCB will add stability.

![rotary encoder tabs](./img/rotary-encoder-solder.jpeg)

It isn't necessary to trim the legs of any of these components, but it doesn't
hurt anything to do so.

## Arduino

Finally, you'll install the Arduino MCU dev board. This component is installed
kind of oddly (suspended from the bottom of the board). 

First, you'll want to solder the headers to the Ardunio. But __not__ how you
might solder these when using an Arduino on a breadboard. 

You'll want the finished product to look like this (note the long end of the
headers is facing up).

![arduino finished headers](./img/arduino-headers.jpeg)

You might find it helpful to do this upside down, with the headers sitting in
their location in the PCB (in order to ensure that they are
straight/perpendicular to both the macropad and the Arduino PCB).

![arduino pcb as jig](./img/arduino-pcb-jig.jpeg)

When you are done soldering the headers to the Arduino, install the Arudino on
the bottom of the PCB and solder the headers to the top.

Note that you will need to closely trim the leads on the bottom of the Arduino
and will want to trim the excess header leads on the top of the PCB.

Your finished product should look like this.

![arduino complete](./img/arduino-complete.jpeg)

## OLED

The last component you'll install is the OLED. This is the easiest component to
install, but can be finicky after installing, since you'll need to tape the OLED
to the top of the case. 

For this, you'll just need to solder VCC to VCC, Ground to Ground, and
Clock and Data to Clock and Data. 

Note that I unhelpfully made the orientation of the PCB different from the
orientation of the finished OLED.

You're aiming for this picture.

![oled complete](./img/oled-complete.jpeg)

## Assembling the case

Now, you'll assemble the case and install your completed PCB in the case.

You'll start by screwing the standoffs into the corresponding nuts. The nuts are
held in place by the cutouts on the bottom of the case. They may be a tight
squeeze. You can use the end of a screwdriver to push hard on the nuts to seat
them.

The PCB gets screwed into the standoffs. Once the PCB is seated, you can start
installing switches. Make sure that before you put the switches in, you put the
plate on the case. The plate is not screwed in until the final step, but you
should be able to use the switches to hold it in place.

![ready for switches](./img/ready-for-switches.jpeg)

After you install the switches, put the top of the case on. Tape the OLED to the
inside of its viewport, then screw the top of the case into the plate and body.

Finally, put a knob on the rotary encoder.

![all done](./img/complete.jpeg)

Congratulations, you're done! Now enjoy your macropad!

* Check out the [`firmware/`](../firmware) directory for information about how
  to flash new firmware (should you want/need to).
* Customize your layers with Via, using the json config and instructions in
  [`via/`](../via).
* Change the layer text with the programming tool in [`cli/`](../cli).
