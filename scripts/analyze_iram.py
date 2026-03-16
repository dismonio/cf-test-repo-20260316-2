# SPDX-License-Identifier: MIT
# Copyright (c) 2023-2026 Dismo Industries LLC

#!/usr/bin/env python
"""Analyze IRAM usage from linker map file - all code in IRAM address range."""
import re, sys

MAP_FILE = '.pio/build/adafruit_feather_esp32_v2/firmware.map'

with open(MAP_FILE, 'r') as f:
    lines = f.readlines()

# IRAM address range on ESP32
IRAM_START = 0x40080000
IRAM_END   = 0x400A0000  # 128KB

lib_sizes = {}
obj_sizes = {}
section_type_sizes = {}
total_iram = 0

# Match lines with: section_name  0xADDRESS  0xSIZE  archive(object)
pat = re.compile(
    r'\s+([\w.]+)\s+'              # section name (e.g. .iram1.5, .text, .literal)
    r'(0x[0-9a-f]+)\s+'           # address
    r'(0x[0-9a-f]+)\s+'           # size
    r'.*?([^\\/]+\.a)\(([^)]+)\)' # archive(object)
)

for line in lines:
    m = pat.match(line)
    if m:
        sec_name = m.group(1)
        addr = int(m.group(2), 16)
        size = int(m.group(3), 16)
        lib = m.group(4)
        obj = m.group(5)

        if IRAM_START <= addr < IRAM_END and size > 0:
            lib_sizes[lib] = lib_sizes.get(lib, 0) + size
            key = f'{lib}({obj})'
            obj_sizes[key] = obj_sizes.get(key, 0) + size
            total_iram += size
            # Track by section type prefix
            prefix = sec_name.split('.')[1] if '.' in sec_name else sec_name
            section_type_sizes[prefix] = section_type_sizes.get(prefix, 0) + size

print(f'Total code in IRAM range: {total_iram:,} bytes ({total_iram/1024:.1f} KB)')
print(f'IRAM region: 131,072 bytes (128.0 KB)')
print(f'Vectors overhead: ~1,028 bytes')
print(f'Available for .iram0.text: ~130,044 bytes')
print(f'Overflow: ~{total_iram - 130044:,} bytes')
print()

print('=== By section type ===')
for t, s in sorted(section_type_sizes.items(), key=lambda x: -x[1]):
    print(f'  .{t}*: {s:>8,} bytes ({s/1024:.1f} KB)')
print()

sorted_libs = sorted(lib_sizes.items(), key=lambda x: -x[1])
print('=== IRAM usage by LIBRARY ===')
print(f'{"Library":<55} {"Bytes":>8} {"KB":>8}')
print('-' * 75)
for lib, size in sorted_libs:
    print(f'{lib:<55} {size:>8,} {size/1024:>7.1f}')
print(f'{"TOTAL":<55} {total_iram:>8,} {total_iram/1024:>7.1f}')

print()
print('=== Top 50 object files in IRAM ===')
print(f'{"Object":<65} {"Bytes":>8}')
print('-' * 75)
sorted_objs = sorted(obj_sizes.items(), key=lambda x: -x[1])
for obj, size in sorted_objs[:50]:
    print(f'{obj:<65} {size:>8,}')
