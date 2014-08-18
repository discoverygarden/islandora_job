<?php

/**
* Implements hook_drush_command().
*/
function islandora_job_drush_command() {
  $items = array();
  $items['islandora-job-router'] = array(
    'description' => 'Job routing function for Islandora.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

/**
 * Routes a message received from Gearman to the appropriate worker function.
 *
 * @param string $msg_str
 *   A string representing a JSON-encoded message.
 */
function drush_islandora_job_router($msg_str) {
  // Parse the message into an associative array.
  $msg = json_decode($msg_str, TRUE);

  // There's no real implementation yet, so just log something.
  dd("MESSAGE: ");
  dd($msg);

  // This gets returned to Gearman, oddly enough.
  echo "I GOT YER MESSAGE";
}
