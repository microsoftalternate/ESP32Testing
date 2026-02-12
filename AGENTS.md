# Project Agent Guide

This repository supports role-based collaboration for coding, debugging, review, and release handoff.

## Global Rules

- You may take large, multi-step actions to complete tasks end-to-end.
- Prefer correctness and verification over speed.
- Assume outbound network is restricted by firewall.
- External dependencies must be requested before relying on them.
- After changes, run appropriate checks/tests and summarize results.
- Use git for progress tracking.

## Agent Roles

- Builder Agent: `agents/BUILDER_AGENT.md`
- Debug Agent: `agents/DEBUG_AGENT.md`
- Review Agent: `agents/REVIEW_AGENT.md`
- Release Agent: `agents/RELEASE_AGENT.md`

## Workflow

- Multi-agent flow: `agents/WORKFLOW.md`
- Session planning: `agents/SESSION_TEMPLATE.md`
- Cross-agent handoff: `agents/HANDOFF_TEMPLATE.md`
- System auto invocation: `agents/SYSTEM_AUTORUN.md`

## Auto Invocation

To auto-route prompts through agent workflow when running `codex` in this repository:

1. Run `./scripts/install_agent_hooks.sh`
2. Run `source ~/.bashrc`

After install:

- `codex` is auto-routed in this repo
- `codex_raw` bypasses routing

## Standard of Evidence

Every task should include:

- exact commands executed
- outcome of each command
- explicit statement of what is done vs. what remains

## For Future Project Seers

If you are inheriting a partially completed session:

1. Read `AGENTS.md`.
2. Read `agents/WORKFLOW.md`.
3. Start a session note from `agents/SESSION_TEMPLATE.md`.
4. Continue from the latest handoff note format in `agents/HANDOFF_TEMPLATE.md`.
