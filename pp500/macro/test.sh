#!/bin/bash -l

root -l <<EOF
.L deta_dphi_pythia.C+
deta_dphi_pythia(6,2,2000);
.q
EOF

