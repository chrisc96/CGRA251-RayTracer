#!/bin/bash
cd build && cmake ../work -DCMAKE_BUILD_TYPE=Release && make -j3 && cd .. && ./build/bin/a4
