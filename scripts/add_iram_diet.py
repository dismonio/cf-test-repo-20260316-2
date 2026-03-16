# SPDX-License-Identifier: MIT
# Copyright (c) 2023-2026 Dismo Industries LLC

"""
PlatformIO pre-build script: IRAM Diet

Patches the framework's sections.ld IN PLACE to move select libc.a
time functions from IRAM (.iram0.text) to flash (.flash.text).

Strategy:
1. Remove specific "libc.a:obj.*(... .text .text.*)" lines from .iram0.text
2. Remove the same objects from EXCLUDE_FILE lists in .flash.text's .text rules
   (but NOT from .rodata EXCLUDE_FILE lists - those stay in DRAM as intended)
"""
import os
import re
Import("env")

OBJECTS_TO_MOVE = [
    "lib_a-strftime",
    "lib_a-wcsftime",
    "lib_a-mktime",
    "lib_a-strptime",
    "lib_a-lcltime_r",
    "lib_a-lcltime",
    "lib_a-gmtime_r",
    "lib_a-gmtime",
    "lib_a-tzset_r",
    "lib_a-tzset",
    "lib_a-tzcalc_limits",
    "lib_a-tzvars",
    "lib_a-tzlock",
    "lib_a-time",
    "lib_a-timelocal",
    "lib_a-ctime",
    "lib_a-ctime_r",
    "lib_a-asctime",
    "lib_a-asctime_r",
    "lib_a-month_lengths",
    "lib_a-gettzinfo",
]

PATCH_MARKER = "/* IRAM_DIET_PATCHED */"

def patch_sections_ld():
    framework_libs = env.subst("$PROJECT_PACKAGES_DIR") + "/framework-arduinoespressif32-libs"
    original_ld = os.path.join(framework_libs, "esp32", "ld", "sections.ld")

    if not os.path.exists(original_ld):
        print(f"[IRAM Diet] WARNING: Cannot find {original_ld}")
        return

    with open(original_ld, "r") as f:
        content = f.read()

    if PATCH_MARKER in content:
        print("[IRAM Diet] sections.ld already patched, skipping")
        return

    # Save backup
    backup_ld = original_ld + ".orig"
    if not os.path.exists(backup_ld):
        with open(backup_ld, "w") as f:
            f.write(content)
        print(f"[IRAM Diet] Saved backup: {backup_ld}")

    lines = content.split('\n')
    new_lines = []
    removed_count = 0

    for line in lines:
        # Check if this line is an explicit IRAM inclusion for one of our objects
        # These look like: "    *libc.a:lib_a-strftime.*(.literal .literal.* .text .text.*)"
        skip = False
        for obj_name in OBJECTS_TO_MOVE:
            # Only remove lines with .literal/.text patterns (IRAM code placement)
            # NOT lines with .rodata patterns (DRAM data placement)
            pattern = rf'^\s*\*libc\.a:{re.escape(obj_name)}\.\*\(\.literal\s+\.literal\.\*\s+\.text\s+\.text\.\*\)\s*$'
            if re.match(pattern, line):
                skip = True
                removed_count += 1
                break
        if not skip:
            new_lines.append(line)

    patched = '\n'.join(new_lines)

    # Now handle EXCLUDE_FILE lists in .flash.text
    # The .flash.text section has EXCLUDE_FILE(...) .literal and .text rules
    # We need to remove our objects from those exclusion lists so they land in flash
    #
    # The EXCLUDE_FILE lists contain entries like: *libc.a:lib_a-strftime.*
    # These appear in long single-line EXCLUDE_FILE(...) blocks
    #
    # We need to be careful to ONLY modify EXCLUDE_FILE lists that are followed by
    # .literal or .text section patterns (not .rodata ones)

    # Find all EXCLUDE_FILE blocks and selectively remove our objects
    # The pattern in .flash.text looks like:
    # *(EXCLUDE_FILE(...long list...) .literal EXCLUDE_FILE(...) .literal.* EXCLUDE_FILE(...) .text EXCLUDE_FILE(...) .text.*)

    # Strategy: for each object, remove "*libc.a:obj.*" from EXCLUDE_FILE blocks
    # but ONLY in the .flash.text section (between .flash.text : { and } >default_code_seg)

    # Find .flash.text section boundaries
    flash_text_start = patched.find('.flash.text :')
    flash_text_end = patched.find('>default_code_seg', flash_text_start) if flash_text_start >= 0 else -1

    if flash_text_start >= 0 and flash_text_end >= 0:
        flash_section = patched[flash_text_start:flash_text_end]

        for obj_name in OBJECTS_TO_MOVE:
            # Remove "*libc.a:obj_name.* " or "*libc.a:obj_name.*" from EXCLUDE_FILE lists
            # Be careful with whitespace - don't leave double spaces
            esc = re.escape(obj_name)
            flash_section = re.sub(rf'\*libc\.a:{esc}\.\*\s*', '', flash_section)

        patched = patched[:flash_text_start] + flash_section + patched[flash_text_end:]
        print(f"[IRAM Diet] Cleaned EXCLUDE_FILE lists in .flash.text")
    else:
        print(f"[IRAM Diet] WARNING: Could not find .flash.text section boundaries")

    # Add patch marker at the top
    patched = PATCH_MARKER + "\n" + patched

    with open(original_ld, "w") as f:
        f.write(patched)

    print(f"[IRAM Diet] Patched sections.ld: removed {removed_count} IRAM placements")

patch_sections_ld()
