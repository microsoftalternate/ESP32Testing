# System-Level Auto Invocation

This project can auto-route Codex prompts through agent workflow when you run `codex` from this repository.

## What It Does

- wraps user prompts with an orchestration preamble
- selects a role flow from prompt intent
- keeps normal Codex behavior outside this repository

## Router Files

- Wrapper: `scripts/codex_agent.sh`
- Prompt router: `scripts/agent_prompt_router.py`
- Hook installer: `scripts/install_agent_hooks.sh`

## Install

```bash
./scripts/install_agent_hooks.sh
source ~/.bashrc
```

## Behavior

- In `/home/ai/Projects/ESP32`: `codex` is auto-routed
- Outside this repo: `codex` is unchanged
- Escape hatch for raw behavior: `codex_raw`

## Notes

- This is CLI-level automation. It cannot force hidden multi-agent workers inside Codex internals.
- It guarantees prompt-time orchestration instructions are automatically injected.
