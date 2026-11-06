<picture>
  <source media="(prefers-color-scheme: dark)" srcset="/docs/images/TOTEM_logo_dark.svg">
  <source media="(prefers-color-scheme: light)" srcset="/docs/images/TOTEM_logo_bright.svg">
  <img alt="TOTEM logo font" src="/docs/images/TOTEM_logo_bright.svg">
</picture>

# ZMK config: TOTEM and Corne

One branch (`master`) builds every keyboard and every mode. `build.yaml` decides
what CI actually builds; anything not in use is commented out.

ZMK is pinned to `v0.2` in `config/west.yml` and `.github/workflows/build.yml`.

![TOTEM layout](/docs/images/TOTEM_layout.svg)

## Layout of the repo

```
build.yaml                         what CI builds (uncomment to enable)
config/
  west.yml                         ZMK version (+ any external modules)
  totem.keymap / totem.conf        TOTEM keymap and settings
  corne.keymap / corne.conf        Corne keymap and settings
  keys_en_gb.h                     UK keycode aliases (GB_*)
  info.json                        TOTEM layout for Keymap Editor
  boards/shields/totem/            TOTEM shield, incl. totem_dongle
  boards/shields/corne_dongle/     Corne dongle shield (halves use ZMK's stock corne shield)
src/ dts/ Kconfig CMakeLists.txt   keepalive behaviour (this repo is also a ZMK module)
zephyr/module.yml
```

Each keyboard's keymap is a plain, self-contained file so Keymap Editor can
still edit it. The cost is that changes have to be copied between
`totem.keymap` and `corne.keymap` by hand.

## Modes

**Bluetooth** – left half is central and talks to the computer.
Flash `totem_left` and `totem_right`.

**Dongle** – a XIAO plugged into the computer is central; both halves are
peripherals. Uncomment the dongle block in `build.yaml`, then flash:

- dongle: `totem_dongle`
- left half: `totem_left_peripheral` (same shield, built with
  `-DCONFIG_ZMK_SPLIT_ROLE_CENTRAL=n`)
- right half: `totem_right` (identical in both modes)

When switching modes, flash `settings_reset` to every device first, then the
new firmware, then power them up together so they re-pair.

The dongle build has ZMK Studio enabled over USB.

**Prospector** (future) – a dongle with a colour screen. Needs ZMK `main`
(Zephyr 4.1), so it isn't a drop-in on `v0.2`:

1. In `config/west.yml` add the `carrefinho` remote and the
   `prospector-zmk-module` project, and move `zmk` off `v0.2`.
2. On Zephyr 4.1 the board is renamed: `seeeduino_xiao_ble` → `xiao_ble//zmk`
   everywhere in `build.yaml`.
3. Uncomment the Prospector entry in `build.yaml`.

See <https://github.com/carrefinho/prospector-zmk-module>.

## Flashing

1. Push; open the run under **Actions** and download `firmware.zip`.
2. Plug in the device, double-tap reset; it mounts as a USB drive.
3. Copy the matching `.uf2` onto it.

## Keepalive

Q+P starts tapping F15 every 4 minutes (first tap is immediate);
Z+/ stops it. Runs on whichever device is central. Settings are on the
`keepalive` node in each keymap.

## Corne: what to do when the build is finished

`config/corne.keymap` is already derived from `totem.keymap`: same layers,
behaviours and combos. The Corne has 42 keys, the TOTEM 38:

```
TOTEM (38)                                   Corne (42)
    0  1  2  3  4 |  5  6  7  8  9           0  1  2  3  4  5 |  6  7  8  9 10 11
   10 11 12 13 14 | 15 16 17 18 19          12 13 14 15 16 17 | 18 19 20 21 22 23
20 21 22 23 24 25 | 26 27 28 29 30 31       24 25 26 27 28 29 | 30 31 32 33 34 35
         32 33 34 | 35 36 37                         36 37 38 | 39 40 41
```

- Outer column rows 1–2 (Corne 0, 11, 12, 23) are spare: `&none` on Base,
  `&trans` elsewhere. Assign them if you like.
- ESC and CAPS/APOSTROPHE stay on the outer bottom row, as on the TOTEM.
- Combos use key numbers, so they're renumbered: Q+P `0 9`→`1 10`,
  Z+/ `21 30`→`25 34`, Game `32 37`→`36 41`.

Steps:

1. Check the controller. `build.yaml` assumes `nice_nano_v2` for the halves
   (ZMK's stock `corne_left`/`corne_right` shields, which need a Pro Micro
   footprint). Change `board:` if you use something else.
2. Uncomment the Corne Bluetooth block (or the dongle block) in `build.yaml`.
3. Copy across any `totem.keymap` changes made since, remembering the
   position mapping above for combos.
4. If you want Keymap Editor for the Corne, it'll need pointing at the Corne
   layout; `config/info.json` currently describes the TOTEM.

Corne dongle: `corne_dongle` (XIAO) as central, `corne_left` built as a
peripheral, `corne_right` unchanged – same pattern as the TOTEM.

## Old branches

The old per-keyboard branches are kept as tags under `archive/`
(e.g. `archive/totem-dongle`).
