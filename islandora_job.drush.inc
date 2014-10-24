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
    'description' => t('Job routing function for Islandora that is executed by the worker processes.  There is no need to ever execute this yourself.'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

/**
 * Routes a payload received from Gearman to the appropriate job function.
 *
 * Reads STDIN for JSON containing the function to execute, the module it comes
 * from, and the arguments to supply it.
 */
function drush_islandora_job_router() {
  // Parse the payload into an associative array.
  $payload = json_decode(stream_get_contents(STDIN), TRUE);
  // Get the list of all available jobs.
  $job_registry = module_invoke_all("register_jobs");
  $func_name = $payload['func'];
  $args = $payload['args'];

  // Dynamically execute the job function, if it's in the list.
  // Echo out the results so there's a return value for foreground jobs.
  if (isset($job_registry[$func_name])) {
    $job = $job_registry[$func_name];
    $type = $job['type'];
    $module = $job['module'];
    $name = isset($job['name']) ? $job['name'] : NULL;
    module_load_include($type, $module, $name);
    echo call_user_func_array($func_name, $args);
  }
}
