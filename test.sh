#!/bin/bash
set -euo pipefail

# Build as the current user; only the final symlink needs elevated privileges.
if [ "$EUID" -eq 0 ]; then
    echo "Run this script without sudo. It requests sudo only when installing gyt." >&2
    exit 1
fi

PROJECT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR="$PROJECT_DIR/build"
BUILD_TYPE=${BUILD_TYPE:-Debug}

case "${1:-}" in
    "") ;;
    -p) BUILD_TYPE=Release ;;
    *) echo "Usage: $0 [-p]" >&2; exit 1 ;;
esac
if [ "$#" -gt 1 ]; then
    echo "Usage: $0 [-p]" >&2
    exit 1
fi

# Previous sudo builds can leave dependency directories owned by root.
for directory in "$BUILD_DIR" "$BUILD_DIR/_deps"; do
    if [ -d "$directory" ] && [ ! -w "$directory" ]; then
        echo "Build directory is not writable: $directory" >&2
        echo "Fix ownership once with:" >&2
        printf '  sudo chown -R "%s:%s" "%s"\n' "$(id -un)" "$(id -gn)" "$BUILD_DIR" >&2
        exit 1
    fi
done

echo "Selected build type: $BUILD_TYPE"
echo "Building the project... This will take a while to install dependencies for the first time."

cmake --build "$BUILD_DIR"

# Keep the existing verbose status-test selection.
(cd "$BUILD_DIR" && ctest --output-on-failure -V -R status)

INSTALL_PATH=/usr/local/bin/gyt
if [ -d "$INSTALL_PATH" ]; then
    echo "Cannot install gyt: $INSTALL_PATH is a directory." >&2
    exit 1
fi
echo "Symlinking $BUILD_DIR/app/gyt to $INSTALL_PATH"
sudo mkdir -p /usr/local/bin
sudo ln -sfn "$BUILD_DIR/app/gyt" "$INSTALL_PATH"
