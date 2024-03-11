<?php

/**
 * @file
 * Heartbeat job callback and associates.
 */

/**
 * Heartbeat job callback.
 */
function islandora_job_heartbeat($text, $time) {
  // XXX: gearman apparently needs a value. Let's return one.
  return true;
}
