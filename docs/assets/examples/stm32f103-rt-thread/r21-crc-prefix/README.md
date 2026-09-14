# r21 CRC prefix optimization — STM32F103RET6

This is a separate candidate qualification, not a replacement label for the r20 baseline.

Target: pure-sine demo SHA256 `0398dc532654d10a34fb19aea24ba19b3b18281849e4ccb053efda79de966303`; benchmark RAM `0x20008C88`, 4096 bytes.
Probe UF2 SHA256 `f28c012e547e7bc589e14463ebfd0835bc1cdcd5297b87d7ff768dfe764406cd`. Default batch remains 64; clock settings remain 4/10/20/30 MHz.

| MHz | Single duration s | Sustained kSa/s | Host kSa/s | 4-variable kgroup/s | 4 KiB KiB/s |
|---:|---:|---:|---:|---:|---:|
| 4 | 15 | 61.011 | 60.869 | 10.152 | 274.837 |
| 10 | 15 | 116.901 | 116.605 | 20.063 | 578.965 |
| 20 | 15 | 175.057 | 174.625 | 28.372 | 881.522 |
| 30 | 120 | 193.449 | 192.946 | 32.213 | 1061.556 |

Sustained rate uses all probe timestamps, including batch gaps, after a 200 ms warm-up. Every returned payload is checked against the deterministic RAM pattern; CRC, flags and SWD error deltas are checked. Target waveform update rate is not the acquisition rate.

Change: calculate the constant eight-byte frame magic CRC once per batch. Wire format and exact timestamp semantics are unchanged. Shared fixed-32 serializer covers ARM and HPM; HPM hardware regression remains pending.
Validation: 243 byte-equivalence cases, actual C batch framing/backpressure tests, successful SES build, unchanged SWD hot-function addresses. ILM free: 5248 bytes.

The old GUI CRC=1 observation remains unresolved. Its counters accumulate across stream restarts in remote/dashboards.py; the saved later interval had no additional CRC errors. This alone does not prove a harmless cancellation artifact.

Raw matrix: `E:\PHDZ\PROJECT\liu\STM32F103_test\STM32F103RC\.mklink\arm_deferred_20260914\r21-four-speed-sine-01`. A/B: `r20-batch-ab-sine-01` and `r21-batch-ab-sine-01`.

USB pause/restart suite: passed=True; completed=20. Raw: `E:\PHDZ\PROJECT\liu\STM32F103_test\STM32F103RC\.mklink\arm_deferred_20260914\r21-usb-stress-sine-01`.
