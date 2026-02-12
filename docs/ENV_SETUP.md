# Environment Setup (Ubuntu VM)

This repository is prepared for ESP32 development using ESP-IDF.

## 1) Bootstrap the machine

Preview the commands first:

```bash
./scripts/bootstrap_ubuntu.sh
```

Apply the setup:

```bash
./scripts/bootstrap_ubuntu.sh --apply
```

Optional flags:

- `--idf-version=v5.2.2`
- `--idf-dir=$HOME/esp/esp-idf`

## 2) Reload shell environment

```bash
source ~/.bashrc
```

Or directly:

```bash
source ~/esp/esp-idf/export.sh
```

## 3) Validate toolchain

```bash
./scripts/check_env.sh
```

If setup fails with `ensurepip is not available` or `No module named 'rich'`, install missing host Python packages:

```bash
sudo apt-get install -y python3.12-venv python3-pip
```

The check validates:
- core host tools
- ESP-IDF CLI and compilers
- flashing/debug utilities
- required Python modules

## 4) Typical build flow (after project files exist)

```bash
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Notes

- If outbound network is firewalled, package downloads and git clone will fail until access is allowed.
- If `sudo` is unavailable in the VM, run equivalent package commands as root.
