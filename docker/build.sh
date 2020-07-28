#!/usr/bin/env bash

set -euo pipefail

BUILD_IMAGE_TAG="falltergeist:latest"
SRC_DIR="$(realpath $(dirname "${BASH_SOURCE[0]}")/..)"
docker run -v "${SRC_DIR}":/src/falltergeist -it --rm "${BUILD_IMAGE_TAG}" /src/falltergeist/docker/run_build.sh