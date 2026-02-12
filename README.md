# ESP32 VM Workspace

This repository is the control surface for an ESP32-focused development VM.

## What we can do here

- write and modify embedded firmware source
- run local build, flash, and monitor commands
- run toolchain and environment diagnostics
- iterate with git-tracked changes

## Quick start

1. Run environment bootstrap (dry run first):

```bash
./scripts/bootstrap_ubuntu.sh
```

2. Apply environment bootstrap:

```bash
./scripts/bootstrap_ubuntu.sh --apply
```

3. Validate environment:

```bash
./scripts/check_env.sh
```

Detailed setup notes: `docs/ENV_SETUP.md`

## Current status in this VM

At the time of setup validation, core ESP tools were missing from PATH:
- `idf.py`
- ESP-IDF Python environment (`ensurepip` / `python3.12-venv`)
- `openocd`
- `pip3`/Python package tooling

Use the bootstrap script to install these.
