## PS2 Keyboard adapter (with ATTiny2313/4313 and MT8808) for HC2000

Mounts on PCB using the speaker location and the original motherboard keyboard connector (see media folder for additional visuals).

Implements CP/M 2.2 launch (F1) and Basic disk load (F2) command macros. 

Enables Ctrl+key and Escape sequences using actual Ctrl key Esc keys in CP/M 2.2.


KiCAD rendering:
![KiCAD rendering of PCB](https://github.com/svpantazi/HC2000_PS2_KBRD/blob/main/media/kicad_3d_rendering.png?raw=true)


Actual PCB and some components:
![some components](https://github.com/svpantazi/HC2000_PS2_KBRD/blob/main/media/some_components.jpeg?raw=true)




Limitations and to do's:
- Firmware is still work in progress.  Ctrl key (which is enabling/disabling E mode in Basic) seems to work fine in CP/M but there is a sneaky bug related key repetition in Basic; it is possible that the PS2 keycode processing gets in a state where a key is repeated and only a reset (really a poweroff) of the microcontroller fixes it
  
- PS2 Mouse functions are not implemented yet (it is not possible without additional wiring to signals on the motherboard)
  
- Soft reset (e.g., by Ctrl-Alt-Delete) is possible with one additional wire to the appropriate signal on the motherboard and a minor change in firmware



