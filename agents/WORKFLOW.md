# Multi-Agent Workflow

This project uses lightweight role-based agents so future contributors can pick up work quickly and safely.

## Roles

- Builder Agent: creates or changes firmware and project scripts
- Debug Agent: reproduces failures and isolates root cause
- Review Agent: checks risk, regressions, and test coverage
- Release Agent: prepares final branch state, release notes, and flash/run instructions

## Execution Order

1. Builder Agent implements the requested change.
2. Debug Agent validates behavior with focused checks.
3. Review Agent performs code and risk review.
4. Release Agent writes final summary and release-ready notes.

## Routing Rules

- New feature work: Builder -> Debug -> Review -> Release
- Bug report with reproduction: Debug -> Builder -> Review -> Release
- Review-only request: Review (then optional Debug)
- Documentation-only request: Builder (docs) -> Review

## Shared Rules

- Prefer correctness and verification over speed.
- Assume outbound network may be restricted.
- Request external dependencies before relying on them.
- Track progress with git commits.
- Do not ship unverified behavior.

## Required Artifacts Per Task

- Session note using `agents/SESSION_TEMPLATE.md`
- Handoff note using `agents/HANDOFF_TEMPLATE.md`
- Verification summary with exact commands and results
