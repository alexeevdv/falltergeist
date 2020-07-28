#!/usr/bin/env bash

set -euo pipefail

cd /build
cmake /src/falltergeist
make