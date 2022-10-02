#!/bin/bash

echo "=== lsof -i -P -n"
if ! lsof -i -P -n | grep webserv; then
  echo "No open fd found" >&2;
  exit 1;
fi

echo "=== curl -v --max-time 1 localhost:8080/index.html"
curl -v --max-time 1 localhost:8080/index.html

echo "==="
curl -v --max-time 1 -d "key=value" localhost:8080/upload

echo "===curl -v --max-time 1 -F "inputName=@test/www/index.html" localhost:8080/upload"
curl -v --max-time 1 -F "inputName=@test/www/index.html" localhost:8080/upload
