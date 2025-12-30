[![progress-banner](https://backend.codecrafters.io/progress/shell/6a41fe25-8426-4f96-ac18-af954a9151ec)](https://app.codecrafters.io/users/codecrafters-bot?r=2qF)

["Build Your Own Shell" Challenge](https://app.codecrafters.io/courses/shell/overview).

---

# Build Your Own Shell (C)

This repository contains my solution to the **CodeCrafters “Build Your Own Shell” challenge**, implemented in **C**.

A shell is the program that interprets what you type into the terminal. It reads your commands, runs programs, and prints their output. Popular examples include **Bash** and **ZSH**.

In this challenge, I’m building my **own shell from scratch in C**.

---

## What am I building?

I’m building a **POSIX-style shell** that runs a **REPL** (Read–Eval–Print Loop), parses user input, spawns processes, and supports both built-in commands and external programs.

At the early stages, my shell already feels usable. I can type commands, see their output, and navigate the filesystem just like in a real shell.

### Core features implemented early on

* Reading and parsing user input
* Running a REPL
* Built-in commands like `pwd`, `cd`, `echo`, `exit`
* Locating executables using `$PATH`
* Launching external programs via OS processes

As the project progresses, I incrementally add more advanced shell features.

---

## Advanced features implemented over time

As I move through the stages, the shell gains support for:

* Piping and redirection
* Command history
* Autocompletion
* Quoting and escaping
* Multi-command pipelines
* Persistent history
* And more

By the end, this repository serves as a **complete, non-trivial systems project** that I can showcase.

---

## What am I learning?

### In the first stages, I learn:

* How a shell actually reads, parses, and executes commands (it’s not magic)
* What a REPL is and how it works internally
* How built-in commands (like `echo`) differ from external programs
* What `$PATH` means and how executable lookup works
* How processes are spawned and how exit codes propagate

### In later stages, I learn:

* How to parse more complex syntax (quotes, escapes, pipelines)
* How to manage file descriptors for redirection and pipes
* How real shells structure their logic to stay maintainable
* How to refactor code safely as complexity increases

As the codebase grows, I’m forced to **structure and refactor** my implementation to avoid regressions and make new features easier to add.

---

## Why build a shell?

Building a shell blends:

* System design
* Low-level programming
* Operating system concepts
* Real-world tooling knowledge

If you’ve mostly worked on application-level or web code, this project pulls you **closer to how your computer actually runs software**.

Beyond the technical depth, there’s something uniquely satisfying about understanding a tool you use every day. By the end of this challenge, I come away as a more confident and well-rounded developer.

---

## Prerequisites

* Comfort writing code in at least one programming language
* Familiarity with Git

No prior experience with shells or operating systems is required. Most concepts (like processes and file descriptors) are learned along the way.

What matters most is **curiosity and persistence**. This is not a follow-along tutorial — exploration, debugging, and discovery are part of the learning process.

---

## Implemented Stages

### Basics

* Print a prompt
* Handle invalid commands
* Implement a REPL
* Implement `exit`
* Implement `echo`
* Implement `type`
* Locate executable files
* Run a program

### Navigation

* The `pwd` builtin
* The `cd` builtin (absolute paths)
* The `cd` builtin (relative paths)
* The `cd` builtin (home directory)

### Quoting and Escaping

* Quoting
* Single quotes
* Double quotes
* Backslash outside quotes
* Backslash within single quotes
* Backslash within double quotes
* Executing a quoted executable

### Redirection

* Redirect stdout
* Redirect stderr
* Append stdout
* Append stderr

### Autocompletion

* Built-in completion
* Completion with arguments
* Missing completions
* Executable completion
* Multiple completions
* Partial completions

### Pipelines

* Dual-command pipeline
* Pipelines with built-ins
* Multi-command pipelines

### History

* The `history` builtin
* Listing history
* Limiting history entries
* Up-arrow navigation
* Down-arrow navigation
* Executing commands from history

### History Persistence

* Read history from file
* Write history to file
* Append history to file
* Read history on startup
* Write history on exit
* Append history on exit

---

If you want, I can also:

* Tighten the language for a **portfolio README**
* Add a **demo GIF / usage section**
* Write a **“What I struggled with” / lessons learned** section

Just say which direction you want.
