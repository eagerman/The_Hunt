#!/usr/bin/env bash
grep '"move": "[A-Z][A-Z]",' log | cut -d':' -f2 | grep -oP '(?<=").*?(?=")' | sort | uniq -c | sort -r