#!/usr/bin/env python3
import sys


def emit_header(data, out_path):
    with open(out_path, "w", encoding="ascii") as out:
        out.write("#pragma once\n\n")
        out.write("static const unsigned char dana_runtime_bitcode[] = {\n")
        for i, b in enumerate(data):
            if i % 12 == 0:
                out.write("    ")
            out.write(f"0x{b:02x}, ")
            if i % 12 == 11:
                out.write("\n")
        if len(data) % 12 != 0:
            out.write("\n")
        out.write("};\n")
        out.write(f"static const unsigned int dana_runtime_bitcode_len = {len(data)};\n")


def main():
    if len(sys.argv) != 3:
        sys.stderr.write("Usage: embed_bitcode.py <input.bc> <output.hpp>\n")
        return 1
    in_path, out_path = sys.argv[1], sys.argv[2]
    with open(in_path, "rb") as f:
        data = f.read()
    emit_header(data, out_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
