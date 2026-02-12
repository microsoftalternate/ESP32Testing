#!/usr/bin/env bash
set -euo pipefail

# Bootstraps a Ubuntu host for ESP-IDF based development.
# This script intentionally does not run sudo automatically; it shows commands,
# then executes when --apply is passed.

APPLY="no"
ESP_IDF_DIR="${HOME}/esp/esp-idf"
ESP_IDF_VERSION="v5.2.2"

for arg in "$@"; do
  case "$arg" in
    --apply) APPLY="yes" ;;
    --idf-version=*) ESP_IDF_VERSION="${arg#*=}" ;;
    --idf-dir=*) ESP_IDF_DIR="${arg#*=}" ;;
    *)
      echo "Unknown argument: $arg"
      echo "Usage: $0 [--apply] [--idf-version=v5.2.2] [--idf-dir=$HOME/esp/esp-idf]"
      exit 1
      ;;
  esac
done

run_cmd() {
  echo "+ $*"
  if [[ "$APPLY" == "yes" ]]; then
    "$@"
  fi
}

print_note() {
  echo
  echo "$1"
}

print_note "Step 1: Install host packages"
run_cmd sudo apt-get update
run_cmd sudo apt-get install -y \
  git curl wget flex bison gperf python3 python3-venv python3-pip \
  cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

print_note "Step 2: Clone ESP-IDF"
run_cmd mkdir -p "$(dirname "$ESP_IDF_DIR")"
if [[ -d "$ESP_IDF_DIR/.git" ]]; then
  echo "+ git -C $ESP_IDF_DIR fetch --tags"
  if [[ "$APPLY" == "yes" ]]; then
    git -C "$ESP_IDF_DIR" fetch --tags
  fi
else
  run_cmd git clone --recursive https://github.com/espressif/esp-idf.git "$ESP_IDF_DIR"
fi

print_note "Step 3: Checkout requested ESP-IDF version"
run_cmd git -C "$ESP_IDF_DIR" checkout "$ESP_IDF_VERSION"
run_cmd git -C "$ESP_IDF_DIR" submodule update --init --recursive

print_note "Step 4: Install ESP-IDF tools"
if [[ "$APPLY" == "yes" ]]; then
  bash "$ESP_IDF_DIR/install.sh" esp32
else
  echo "+ bash $ESP_IDF_DIR/install.sh esp32"
fi

print_note "Step 5: Add environment hook"
EXPORT_LINE="source ${ESP_IDF_DIR}/export.sh"
if [[ "$APPLY" == "yes" ]]; then
  if ! grep -Fq "$EXPORT_LINE" "$HOME/.bashrc"; then
    echo "$EXPORT_LINE" >> "$HOME/.bashrc"
    echo "+ appended ESP-IDF export hook to ~/.bashrc"
  else
    echo "+ ~/.bashrc already contains ESP-IDF export hook"
  fi
else
  echo "+ echo '$EXPORT_LINE' >> ~/.bashrc"
fi

print_note "Finished"
if [[ "$APPLY" == "yes" ]]; then
  echo "Bootstrap completed. Open a new shell or run: source ${ESP_IDF_DIR}/export.sh"
else
  echo "Dry run only. Re-run with --apply to execute changes."
fi
