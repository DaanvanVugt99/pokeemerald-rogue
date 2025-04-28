#!/bin/bash
make -j$(sysctl -n hw.ncpu)
if [ $? -eq 0 ]; then
  echo "✅ Build successful!"
else
  echo "❌ Build failed!"
fi
