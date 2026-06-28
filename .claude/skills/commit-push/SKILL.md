---
name: commit-push
description: Decide what to stage (splitting into multiple commits when appropriate), draft commit messages in the diamo-world convention, get the user's approval, then commit and push.
disable-model-invocation: true
argument-hint: "[optional hint about scope or message]"
---

# Commit and Push

Stage the pending changes, propose commit message(s) in this project's convention, wait for
the user's approval, then commit and push.

## 1. Inspect the working tree

Run these to understand everything that is pending:

- `git status --porcelain=v1` — full picture of staged, unstaged, and untracked files
- `git diff` — unstaged changes
- `git diff --cached` — already-staged changes
- `git log --format='%H%n%B%n=====' -n 5` — recent messages to mirror the exact style
- `git branch --show-current` — the branch to push

If the working tree is clean (nothing staged, unstaged, or untracked), tell the user there is
nothing to commit and stop.

## 2. Decide what to stage — and whether to split

You judge the staging. Read the actual changes and group them into cohesive commits:

- If the changes are one logical unit, plan a single commit.
- If they cover unrelated concerns (e.g. a feature change plus an unrelated config tweak),
  split them into separate commits, each with its own file set and message, ordered
  sensibly.
- Exclude anything that should not be committed (build output, local-only files, secrets,
  stray debug edits). Call out anything you deliberately leave out.

Stage the files for the first planned commit with explicit paths (`git add -- <path> ...`),
not a blanket `git add -A`, so the grouping is deliberate. If `$ARGUMENTS` is provided, use it
as a hint for scope or wording.

## 3. Draft the message(s) — diamo-world convention

Follow the **Commit messages** section in `CONTRIBUTING.md` — that is the source of truth:
Conventional Commits (`<type>(<scope>): <subject>`), a why-focused body, no authorship
trailers, one type per commit. Do not restate the rules here; if they ever differ,
`CONTRIBUTING.md` wins. The repo's pre-convention history predates the rule — do not mirror
it. The `.githooks/commit-msg` hook rejects a malformed subject, so the message must pass it.

Example of the shape it produces:

```
refactor(engine): split the world tick into fixed and render steps

Decouple simulation cadence from frame rate so rendering can interpolate:
- Advance the simulation on a fixed timestep
- Interpolate render state between ticks
```

## 4. Show the plan and wait for approval

Present, for each planned commit, the files it will include and its full message in a code
block. Then **stop and wait** — do not commit or push until the user explicitly approves.

If the user gives feedback (reword, regroup, drop/add files, split differently), revise and
present the updated plan. Repeat until the user approves.

## 5. Commit and push

Only after explicit approval:

1. For each planned commit, in order: make sure exactly its files are staged, then commit.
   Write the message to a temporary file and use `git commit -F <file>` so multi-line bodies
   are preserved verbatim. Do not pass the message inline. Write the file as **UTF-8 without
   a BOM** — a BOM lands before the subject's first byte and makes the `.githooks/commit-msg`
   regex reject the subject. Windows PowerShell 5.1's `Set-Content -Encoding utf8` emits a
   BOM, so do not use it; write the bytes explicitly, e.g.
   `[System.IO.File]::WriteAllText($path, $message, (New-Object System.Text.UTF8Encoding($false)))`.
2. After all commits are made, push once: `git push`. If the branch has no upstream, use
   `git push -u origin <current-branch>`.
3. Never force-push and never rewrite already-pushed history.

## 6. Report

Report the result: each commit's short hash and subject, and the push outcome (or any error,
surfaced verbatim).
