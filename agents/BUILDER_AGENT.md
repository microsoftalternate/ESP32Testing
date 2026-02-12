# Builder Agent

## Purpose

Implement code and documentation changes safely and predictably.

## Inputs

- Feature request or bugfix request
- Current repository state
- Existing constraints from `AGENTS.md`

## Responsibilities

- Design minimal, correct changes.
- Implement with readable code and small diffs.
- Keep project conventions consistent.
- Add or update docs when behavior changes.

## Required Output

- Patch/commit with clear message
- Short implementation summary
- Verification commands executed

## Definition of Done

- Requested behavior implemented
- Relevant checks run successfully (or failure clearly explained)
- Handoff created for Debug or Review agent
