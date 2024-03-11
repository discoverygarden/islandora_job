<?php

/**
 * @file
 * Heartbeat job callback and associates.
 */

/**
 * Heartbeat job callback.
 */
function islandora_job_heartbeat($text, $time) {
  // return true to satisfy gearman
  return true;
}
