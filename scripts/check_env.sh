#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

print_header() {
  printf "\n== %s ==\n" "$1"
}

check_cmd() {
  local name="$1"
  local required="${2:-yes}"

  if command -v "$name" >/dev/null 2>&1; then
    printf "[OK] %s -> %s\n" "$name" "$(command -v "$name")"
    return 0
  fi

  if [[ "$required" == "yes" ]]; then
    printf "[MISSING] %s (required)\n" "$name"
    return 1
  fi

  printf "[MISSING] %s (optional)\n" "$name"
  return 0
}

check_python_module() {
  local module="$1"
  local required="${2:-yes}"
  if python3 -c "import ${module}" >/dev/null 2>&1; then
    printf "[OK] python module %s\n" "$module"
    return 0
  fi

  if [[ "$required" == "yes" ]]; then
    printf "[MISSING] python module %s (required)\n" "$module"
    return 1
  fi

  printf "[MISSING] python module %s (optional)\n" "$module"
  return 0
}

status=0

print_header "Host"
uname -a || true

print_header "Core Tools"
check_cmd git || status=1
check_cmd bash || status=1
check_cmd python3 || status=1
check_cmd pip3 || status=1

print_header "ESP Toolchains"
check_cmd idf.py || status=1
check_cmd xtensa-esp32-elf-gcc || status=1
check_cmd riscv32-esp-elf-gcc optional || true

print_header "Debug and Flash"
check_cmd openocd optional || true
check_cmd esptool.py optional || true

print_header "Python Packages"
check_python_module serial || status=1
check_python_module click || status=1

print_header "Repository"
if [[ -d "${ROOT_DIR}/.git" ]]; then
  echo "[OK] git repo found at ${ROOT_DIR}"
else
  echo "[MISSING] git repo metadata not found"
  status=1
fi

if [[ $status -ne 0 ]]; then
  echo
  echo "Environment check failed. See docs/ENV_SETUP.md for setup steps."
  exit 1
fi

echo
echo "Environment check passed."
