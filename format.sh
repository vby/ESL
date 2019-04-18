#!/bin/sh

find esl -name *.hpp | xargs clang-format -style=file -i

