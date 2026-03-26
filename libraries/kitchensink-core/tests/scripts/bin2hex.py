#!/usr/bin/env python3

import sys

def main():
    data = sys.stdin.buffer.read()
    print(data.hex().upper())

if __name__ == "__main__":
    main()
