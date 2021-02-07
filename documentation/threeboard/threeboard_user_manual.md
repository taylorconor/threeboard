# threeboard user manual

This document explains all of the user functionality of the threeboard, and how to use it.

## Layers and modes

Layers are used to determine the functionality of the keys of the threeboard. The treeboard has four different layers:

- `DFLT`: This is the default layer of the threeboard, the layer that the threeboard starts in when it boots. No layer LEDs are lit in this layer.
- `R`: The letter reprogramming layer of the threeboard. This layer allows users to reassign a keycode to a letter. This allows faster input of common letters.
- `G`: The word shortcut layer. This layer allows users to program frequently used words into the threeboard. These words can then be accessed and sent over USB.
- `B`: The blob shortcut layer. This allows users to program arbitrary text blobs (including mod codes), and access them later.

`R`, `G` and `B` layers each have `DFLT` and `PROG` modes.

- The `DFLT` mode is used to retrieve programmed shortcuts and send over USB.
- The `PROG` modes are modes within these layers for programming the key shortcuts used in the `DFLT` mode of the layer.

The `DFLT` layer doesn't have a `PROG` mode.

## Key combinations

This table defines the full list of key combinations and their associated actions on each layer:

<img src="../../documentation/images/usage_table.svg" width="100%"/>