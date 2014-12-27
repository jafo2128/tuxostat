#!/bin/bash

# Python handler for OWFS - write the current temp files
/usr/local/bin/tos-owfs-handler

# call tos-control to figure out what to do and do it
/usr/local/bin/tos-control