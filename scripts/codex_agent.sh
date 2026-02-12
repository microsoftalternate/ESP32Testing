#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ROUTER="${REPO_ROOT}/scripts/agent_prompt_router.py"
CODEX_BIN="$(command -v codex)"

if [[ ! -x "$ROUTER" ]] || [[ -z "$CODEX_BIN" ]]; then
  exec codex "$@"
fi

# If not in this repository, pass through untouched.
if [[ "$(pwd)" != "$REPO_ROOT"* ]]; then
  exec "$CODEX_BIN" "$@"
fi

# Commands that should never be wrapped.
case "${1:-}" in
  login|logout|mcp|mcp-server|app-server|completion|sandbox|debug|apply|resume|fork|cloud|features|help|-h|--help|-V|--version)
    exec "$CODEX_BIN" "$@"
    ;;
esac

# Handle non-interactive exec form with a prompt argument.
if [[ "${1:-}" == "exec" || "${1:-}" == "e" ]]; then
  if [[ $# -ge 2 && "${2:-}" != -* ]]; then
    wrapped_prompt="$(python3 "$ROUTER" "$2")"
    exec "$CODEX_BIN" "$1" "$wrapped_prompt" "${@:3}"
  fi
  exec "$CODEX_BIN" "$@"
fi

# Handle plain interactive form: codex [PROMPT]
if [[ $# -eq 0 ]]; then
  wrapped_prompt="$(python3 "$ROUTER")"
  exec "$CODEX_BIN" "$wrapped_prompt"
fi

if [[ "${1:-}" != -* ]]; then
  wrapped_prompt="$(python3 "$ROUTER" "$1")"
  exec "$CODEX_BIN" "$wrapped_prompt" "${@:2}"
fi

# If using option-based startup, pass through untouched.
exec "$CODEX_BIN" "$@"
