#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BASHRC="${HOME}/.bashrc"
START="# >>> ESP32 agent hooks >>>"
END="# <<< ESP32 agent hooks <<<"

BLOCK=$(cat <<HOOK
${START}
# Auto-route Codex prompts through project agents while inside ${REPO_ROOT}
codex() {
  if [[ "\$(pwd)" == "${REPO_ROOT}"* ]]; then
    "${REPO_ROOT}/scripts/codex_agent.sh" "\$@"
    return
  fi
  command codex "\$@"
}

# Escape hatch for raw Codex behavior
codex_raw() {
  command codex "\$@"
}
${END}
HOOK
)

if grep -Fq "$START" "$BASHRC"; then
  awk -v s="$START" -v e="$END" -v r="$BLOCK" '
    $0==s {print r; f=1; skip=1; next}
    $0==e && skip {skip=0; next}
    !skip {print}
    END {if (!f) print r}
  ' "$BASHRC" >"${BASHRC}.tmp"
  mv "${BASHRC}.tmp" "$BASHRC"
else
  {
    printf "\n%s\n" "$BLOCK"
  } >>"$BASHRC"
fi

echo "Installed hooks in ${BASHRC}"
echo "Run: source ~/.bashrc"
echo "Use: codex (auto-routed in this repo)"
echo "Raw: codex_raw"
