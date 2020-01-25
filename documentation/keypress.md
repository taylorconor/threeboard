# threeboard keypress algorithm

Registering keypresses in the threeboard requires differentiating between single key presses and combinations of multiple keys pressed simultaneously. There are no modifier keys (e.g. shift, ctrl), so all keys produce input when pressed individually.

### Debouncing
The state of the keyboard is sampled every 5ms. There's currently no debouncing code becuase the Cherry MX switches have a recommended debounce time of 5ms anyway.

### Key combinations
A tradeoff needs to be made between the speed of  registering a keypress and accuracy of identifying keypress combinations.

Consider the following keypress timeline:

```
          |--- t1 ---|----- t2 -----|
Key A:  --[=========================]-----------------
Key B:  -------------[========================]-------
          |          |              |         |
Action: Key A down.  Key B down.  Key A up.  Key B up.
```

On a traditional keyboard, keydown events for regular (non-modifier) keys A and B occur separately, as soon as they are pressed (after debounce time), as illustrated by _action_. The threeboard unfortunately can't register these actions immediately on keydown, as the keydown action may be part of a key combination. Instead, the type of action depends on the timing of the keypress relative to two constants; `MIN_KEYPRESS_TIME` and `MIN_COMBO_OVERLAP`. These are evaluated relative to `t1` and `t2` from the example above in the following order:

1. _`t1 < MIN_KEYPRESS_TIME`, keyup for A_: The keypress for A is registered. `MIN_KEYPRESS_TIME` is only relevant for keycombos.
2. _`t1 >= MIN_KEYPRESS_TIME`, A is the only key pressed_: The keypress for key A is registered, and A is deactivated until keyup.
3. _`t1 < MIN_KEYPRESS_TIME`, `t2 < MIN_COMBO_OVERLAP`, keyup for A, other keys are pressed_: The keypress for A is registered, and A is not considered part of the other potential keycombo unless it is pressed again and satisfies the conditions above.
4. _`t2 >= MIN_COMBO_OVERLAP`, keyup for A, key B still pressed_: No keypress is registered for A, instead it's added to the combo mask that will be registered upon final keyup of the combo. If A is pressed again during the combo it will be ignored, unless the time overlap between keydown for A and keyup for the final combo key is less than `MIN_COMBO_OVERLAP`, in which case the second press of A will be considered a separate event subject to the conditions above.

Consider the following keypress timeline:

```
                         |- t3 -|
          |--- t1 ---|--- t2 ---|
Key A:  --[=====================]---------------------
Key B:  -------------[=====================]----------
Key C:  -----------------[========================]---
```