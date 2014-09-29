#!/bin/bash
if [ ! -d "$1" ]; then
  echo "Please provide the path to drupal root as an argument."
  exit
fi

cd "$1"; gearman -v -w -f islandora_job xargs drush islandora-job-router &
