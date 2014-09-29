<?php

/**
 * @file
 * Drush commands for the islandora_job module.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_job_drush_command() {
  $items = array();
  $items['islandora-job-router'] = array(
    'description' => 'Job routing function for Islandora.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  $items['islandora-job-batch-test'] = array(
    'description' => 'Tests batching',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

/**
 * Routes a payload received from Gearman to the appropriate worker function.
 *
 * @param string $payload_str
 *   A string representing a JSON-encoded message.
 */
function drush_islandora_job_router($payload_str) {
  // Parse the payload into an associative array.
  $payload = json_decode($payload_str, TRUE);

  $job_registry = module_invoke_all("register_jobs");

  $func_name = $payload['func'];
  $args = $payload['args'];

  if (isset($job_registry[$func_name])) {
    $job = $job_registry[$func_name];
    $type = $job['type'];
    $module = $job['module'];
    $name = isset($job['name']) ? $job['name'] : NULL;
    module_load_include($type, $module, $name);
    echo call_user_func_array($func_name, $args);
  }
}
