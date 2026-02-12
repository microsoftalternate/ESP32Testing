#!/usr/bin/env python3
"""Build an orchestration preamble for Codex prompts."""

from __future__ import annotations

import sys


def classify(prompt: str) -> str:
    p = prompt.lower()

    review_tokens = ["review", "audit", "risk", "regression", "code review"]
    debug_tokens = ["bug", "error", "failure", "failing", "crash", "debug", "fix"]
    docs_tokens = ["readme", "docs", "documentation", "md file", "markdown"]

    if any(t in p for t in review_tokens):
        return "review_only"
    if any(t in p for t in docs_tokens) and not any(t in p for t in debug_tokens):
        return "docs"
    if any(t in p for t in debug_tokens):
        return "bugfix"
    return "feature"


def flow_for(kind: str) -> str:
    if kind == "review_only":
        return "Review"
    if kind == "docs":
        return "Builder -> Review"
    if kind == "bugfix":
        return "Debug -> Builder -> Review -> Release"
    return "Builder -> Debug -> Review -> Release"


def build_wrapped_prompt(user_prompt: str) -> str:
    kind = classify(user_prompt)
    flow = flow_for(kind)

    preamble = (
        "SYSTEM ORCHESTRATION (AUTO):\n"
        f"- Route this task through: {flow}.\n"
        "- Follow repository contracts in AGENTS.md and agents/WORKFLOW.md.\n"
        "- Produce explicit verification evidence (commands + outcomes).\n"
        "- Use git progress tracking for meaningful changes.\n"
        "- If any stage cannot complete, state blocker and continue with next best completed stage.\n"
        "- Keep final response concise but complete.\n\n"
        "USER REQUEST:\n"
    )
    return preamble + user_prompt.strip() + "\n"


def main() -> int:
    user_prompt = " ".join(sys.argv[1:]).strip()
    if not user_prompt:
        user_prompt = (
            "Start a new session using the repo agent workflow. "
            "Ask me for the first concrete task after loading AGENTS.md and agents/WORKFLOW.md."
        )
    sys.stdout.write(build_wrapped_prompt(user_prompt))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
