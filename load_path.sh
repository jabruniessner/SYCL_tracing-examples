#!/bin/bash

: "${ADAPTIVECPP_TRACING_INSTALL_DIR:?Set ADAPTIVECPP_TRACING_INSTALL_DIR to your AdaptiveCpp (tracing branch) install prefix before sourcing this script}"

export PATH="$ADAPTIVECPP_TRACING_INSTALL_DIR/bin:$PATH"

