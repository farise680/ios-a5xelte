# HERO2LTE (Galaxy S7 edge SM-G935F, Exynos 8890): progress

Branch `hero2lte`, forked from `herolte`. The S7 edge is the same SoC, UFS, DECON
framebuffer (1440x2560 @ 0xe2a00000), GIC, PMU and PMIC as the S7, so
`MACHINE_CONFIG=HERO2LTE` builds the whole HEROLTE port (it also defines
`ARM64_BOARD_CONFIG_HEROLTE`) plus `ARM64_BOARD_CONFIG_HERO2LTE` for the differences.

Claims are kept separate, as in `AGENTS.md`: **source-derived**, **builds**,
**seen on the phone**. Nothing below is in the third class yet.

## What differs from herolte (source-derived)

Compared in `LineageOS/android_kernel_samsung_universal8890` (`lineage-18.1`),
`exynos8890-hero2lte_*` vs `exynos8890-herolte_*`:

| Item | herolte | hero2lte | State |
| --- | --- | --- | --- |
| Battery | EB-BG930ABE, 3000 mAh | EB-BG935ABE, 3600 mAh (DTB fuelgauge,capacity 0x1B5E vs 0x173B) | `0250`: `BATTERY_DESIGN_MAH` |
| Touch | Samsung sec_ts @ 0x48, IRQ GPA1-0 | **STM FTS @ 0x49, IRQ GPA2-0**; the DTB disables touchscreen@48 | **No driver.** `0250` skips the sec_ts probe on HERO2LTE |
| GPIO sleep/NC tables | | minor (UB_SPI on GPE4-0..2, TSP_ID on GPE7-1/2) | not used by this port |
| Everything else (SoC, UFS 155a0000, DECON, PMU 105c0000, GIC-400, S2MPS16, MPIDR 0x100) | | same | reused unchanged |

## Changes in this branch

- `xnu/patches/0250-hero2lte-board-variant.patch`: new machine config `HERO2LTE`
  (MakeInc.def flags and filter lists, `config/MASTER.arm64.hero2lte`, battery
  capacity, touch gate). Verified to apply on top of 0001-0240 on a clean
  `xnu-8796.141.3` (the HFS-dependent 0031/0041 need the staging scripts, as in CI).
- Staging scripts and `xnu.yml` accept `HERO2LTE`; it is the default on this branch.
- `tools/mkadt_herolte.py --device hero2lte` adds `samsung,hero2lte` to the ADT
  compatible list and labels the firmware version `hero2lte-entry`.

## Not done / unverified

- **Touch**: needs an STM FTS driver (I2C 0x49, GPA2-0). Until then the phone is
  driven by the power/volume keys and the drop zone only.
- Fuel gauge: assumed the same MAX77854 as the S7 (same `herolte_battery.c`); the
  hero2lte battery DTB was only compared for capacity, not the register map.
- Loader/probe/UFS/GPU code is reused as is. Nothing has been booted on an S7 edge.
- Before the first boot: back up the S7 edge's stock BOOT and EFS from TWRP, and
  follow `AGENTS.md` (only the BOOT partition may be written).
- `tools/validate-repo.py` reports 4 patch-numbering gaps; they exist on `herolte` too.
