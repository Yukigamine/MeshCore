# Heltec V3 vibration companion

Custom build based on official `companion-v1.18.0`, commit
`81dbae592ffad40b4212bc0b37e82b38da08f4bc`. Firmware version: `v1.18.0-dmvib2`.
The owner tested this revision on a Heltec V3: channel messages produce one
pulse and direct messages produce two. The earlier app-only update preserved
settings and phone pairing. This is a custom fork, not an official release.

## GPIO mapping

| Function | Official V3 configuration | This custom environment |
| --- | --- | --- |
| Vibration control | Not configured | GPIO47, active high, no PWM |
| GPS RX (connect GPS module TX here) | GPIO47 | GPIO4 |
| GPS TX (connect GPS module RX here) | GPIO48 | GPIO48, unchanged |
| GPS enable definition | GPIO26 | GPIO26, unchanged |

GPS support remains compiled in. Optional GPS hardware must use the new RX
mapping; GPS operation has not been physically tested. GPIO4 must be free of
other attached accessories. GPIO47 is a control signal; a bare motor needs a
suitable driver circuit and power supply. This firmware does not establish a
motor's electrical compatibility.

## Notification behavior

- Incoming channel text: one 200 ms pulse.
- Incoming direct plain text: 200 ms on, 200 ms off, 200 ms on.
- Works with the phone connected or disconnected.
- No alert for startup, CLI data, acknowledgements, contact discovery, or
  room-server/signed posts. Transmitting a message does not trigger this code.
- Messages during an active pattern coalesce. A DM upgrades a channel pattern
  to two pulses; a channel message never shortens a DM pattern.
- Pulses use non-blocking loop timing. Shutdown explicitly turns the output off.

Only `Heltec_v3_companion_radio_ble_dm_vibration` enables these changes. Stock
build environments and the generic upstream vibration helper are unchanged.
The official Bluetooth protocol and app radio settings are unchanged.
There is no app setting for the pulse pattern or motor on/off.

## Build

Install PlatformIO Core (the tested local version was 6.1.19), then run:

```sh
pio run -e Heltec_v3_companion_radio_ble_dm_vibration -t mergebin
```

Outputs under `.pio/build/Heltec_v3_companion_radio_ble_dm_vibration/`:

- `firmware.bin`: app-only, flash at `0x10000` with an existing compatible
  MeshCore partition layout. Leave erase disabled.
- `firmware-merged.bin`: full image, flash at `0x0`. Treat as a fresh install:
  it can overwrite identity, settings and pairing information.

Back up identity/configuration before flashing. Keep flash mode, frequency,
and size unchanged for an app-only update. The dmvib2 partition table was
verified byte-for-byte identical to dmvib1. Always verify pairing and settings
after updating. Radio settings remain configurable in the official app; this
build does not select a USA preset automatically.

The GitHub Actions workflow **Build Heltec V3 vibration** builds this target
on pushes to `heltec-v3-vibration` and uploads the binaries as an artifact.
Enable Actions in the fork if required before the next push. Manual dispatch
is also declared, but GitHub requires the workflow to exist on the default
branch before it exposes that trigger.
Dependencies follow upstream version ranges, so future builds may differ
from the original tested binaries.

## Verification

Run the standalone host timing test from the repository root:

```sh
c++ -std=c++11 -I extras/heltec_v3_vibration/test \
  -I examples/companion_radio/ui-new \
  extras/heltec_v3_vibration/test/vibration.cpp -o /tmp/meshcore-vibration-test
/tmp/meshcore-vibration-test
```

It checks idle startup, one/two pulse boundaries, coalescing, DM priority,
timer rollover and explicit stop. The ESP32 release build also passed locally.
After a future update, test DMs and channels both connected and disconnected,
no alert for outgoing messages/ACKs, motor off at idle, battery operation,
settings/pairing retention, and GPS if installed.

## Updating from upstream

Keep fork `main` available for upstream synchronization and maintain custom
work on `heltec-v3-vibration`. Start with the next desired official release tag,
rather than silently building an untested development branch.

```sh
git remote add upstream https://github.com/meshcore-dev/MeshCore.git
git fetch upstream --tags
git switch heltec-v3-vibration
# Replace this placeholder with the exact desired release tag:
git merge --no-commit --no-ff companion-vX.Y.Z
```

If upstream already exists, skip `git remote add`. Resolve any conflicts,
review upstream UI/message callback and pin changes, and update the custom
version/date in `variants/heltec_v3/platformio.ini` to reflect the new base.
Run the timing test and build, then commit the merge and push your branch.
Physically test before treating a new binary as known working. Keep the last
working app-only binary for rollback, subject to compatible settings/layout.

The custom changes are confined to the Heltec environment, new UI helper,
UI initialization/message callbacks/loop/shutdown, this documentation,
timing tests, README pointer, and the build workflow.
