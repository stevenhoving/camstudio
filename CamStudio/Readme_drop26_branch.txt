Here is the brief list of changes:

* libconfig  is used to store main configuration. I propose to convert shapes and other annotation to this format as well.
* Zoom x2 . Hot key can be defined in config `HotKeys.Zoom`.
* GDI+ is used for highlighting and to capture frame when zoom>1 . New options are added to `Cursor` section
  RingThreshold = 1000;
  RingSize = 20;
  RingWidth = 2.0;
  ClickColorMiddle = -1610547456; // can be written as hex 0xa000ff00
* HotKeys are handled in camhook.dll and standard WM_HOTKEY is sent when triggered for maximum compatibility. `HotKeys.PassThrough` option controls if event should be passed to other application
* Codec suggestion is disabled since it doesn't respect my preferences in config
* Actual cursor is really actual now

Notes

* None (old: For some reason I can't delete camhook.dll sometimes. I have to rename it build new one. I don't know (didn't track) what is holding it.)
