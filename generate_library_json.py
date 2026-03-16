import os
import json
import re

# To run, create new terminal and enter:
# python generate_library_json.py

LIB_DIR = "lib"
LIB_JSON_FILENAME = "library.json"

def extract_dependencies(library_path):
    """Extracts all #include statements from the given library folder."""
    dependencies = set()
    header_regex = re.compile(r'#include\s+[<"]([^">]+)[">]')
    
    for root, _, files in os.walk(library_path):
        for file in files:
            if file.endswith(".h") or file.endswith(".cpp"):
                with open(os.path.join(root, file), "r", encoding="utf-8") as f:
                    for line in f:
                        match = header_regex.search(line)
                        if match:
                            header = match.group(1)
                            if not header.startswith(("Arduino", "esp_", "Wire.h", "SPI.h", "WiFi.h")):  
                                dependencies.add(header)

    return list(dependencies)

def generate_library_json(lib_path):
    """Creates a library.json file for a given library directory."""
    lib_name = os.path.basename(lib_path)
    dependencies = extract_dependencies(lib_path)

    # Convert dependency headers to names (assumes they match PlatformIO registry names)
    lib_json = {
        "name": lib_name,
        "version": "1.0.0",
        "dependencies": [{"name": dep.replace(".h", "")} for dep in dependencies]
    }

    lib_json_path = os.path.join(lib_path, LIB_JSON_FILENAME)
    with open(lib_json_path, "w", encoding="utf-8") as json_file:
        json.dump(lib_json, json_file, indent=4)

    print(f"Generated {LIB_JSON_FILENAME} for {lib_name}")

def main():
    """Main function to scan the lib directory and create library.json files."""
    if not os.path.exists(LIB_DIR):
        print(f"Error: {LIB_DIR} directory not found.")
        return

    for library in os.listdir(LIB_DIR):
        lib_path = os.path.join(LIB_DIR, library)
        if os.path.isdir(lib_path):
            generate_library_json(lib_path)

if __name__ == "__main__":
    main()
