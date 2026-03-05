#!/usr/bin/env python3
"""Generates a text file containing a password suffix with a cryptographically secure random string."""

import os
import sys
import string
import secrets

ALPHABET = string.ascii_letters + string.digits
SUFFIX_LENGTH = 24
OUTPUT_HEADER = "ks_password_suffix.txt"

def generate_suffix() -> str:
    return "_" + "".join(secrets.choice(ALPHABET) for _ in range(SUFFIX_LENGTH))


def main():
    output_file = "ks_password_suffix.txt"

    if os.path.exists(output_file):
        print(f"Error: {output_file} already exists. Please remove it before running this script.", file=sys.stderr)
        sys.exit(1)
    
    suffix = generate_suffix()

    try:
        with open(output_file, "w") as f:
            f.write(suffix)
            f.write("\n")
            
        print(f"Generated: {output_file}")
        print()
        print("*** IMPORTANT: This file is needed to read your secure storage either from new firmware installs or locally. ***")
        print("*** BACK IT UP IN A SECURE LOCATION AND/OR PRINT IT OUT IMMEDIATELY! ***")
        
    except OSError as e:
        print(f"Error writing {output_file}: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
