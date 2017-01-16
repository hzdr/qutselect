#!/bin/sh

kill -9 `ps ax | grep thinlinc | grep -v grep | awk '{ print $1; }'`

