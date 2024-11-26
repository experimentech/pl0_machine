#!/bin/bash
# build.sh

set -e  # Exit on error

# Clean
make clean

# Build with warnings as errors
make

# Run tests if they exist
if [ -f "test_assembler" ]; then
    ./test_assembler
fi