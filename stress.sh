#!/bin/bash

CONCURRENCY=100
REQUESTS=1000
ADDRESS="http://localhost:3005/"

for i in `seq 1 $CONCURRENCY`; do
  curl -s "$ADDRESS?[1-$REQUESTS]" & pidlist="$pidlist $!"
done

# Execute and wait
FAIL=0
for job in $pidlist; do
  echo $job
  wait $job || let "FAIL += 1"
done

# Verify if any failed
if [ "$FAIL" -eq 0 ]; then
  echo "SUCCESS!"
else
  echo "Failed Requests: ($FAIL)"
fi