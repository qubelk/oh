# oh

Oh, build system i made for learning and practice.

## Overview

`oh` parse `Ohfile` or `ohfile` and execute rule, like `make`. Now supports only rules.

### Syntax

Ohfile syntax:

```makefile
# Comment

rule: ( deps.c )
	echo "Hello, World!"
```

```bash
Usage:
./oh rule # it calls echo "Hello, World!"
```

## Why i make it

I was bored and wanted to try to `make` like utility, also i want to practice my skills. I don't think i'm going to continue working on `oh`
