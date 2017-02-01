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
  $items['islandora-job-start-managed-worker'] = array(
    'description' => t('Start a managed gearman worker.'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  $items['islandora-job-stop-managed-workers'] = array(
    'description' => t('Stop all managed gearman workers.'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  $items['islandora-job-count-managed-workers'] = array(
    'description' => t('Count all managed gearman workers.'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

/**
 * Start a managed worker command.
 */
function drush_islandora_job_start_managed_worker() {
  module_load_include('inc', 'islandora_job', 'includes/managed_workers');
  islandora_job_start_managed_worker();
}

/**
 * Stop all managed workers command.
 */
function drush_islandora_job_stop_managed_workers() {
  module_load_include('inc', 'islandora_job', 'includes/managed_workers');
  islandora_job_stop_managed_workers();
}

/**
 * Count all managed workers command.
 */
function drush_islandora_job_count_managed_workers() {
  module_load_include('inc', 'islandora_job', 'includes/managed_workers');
  drush_print(islandora_job_count_managed_workers());
}

/**
 * Routes a payload received from Gearman to the appropriate job function.
 *
 * Reads STDIN for JSON containing the function to execute, the module it comes
 * from, and the arguments to supply it.
 */
function drush_islandora_job_router() {
  // XXX: Due to how Drush bootstrapping works, the connection may be created
  // without credentials (when your site's front page is
  // 'islandora/object/some:object', for example). Resetting to ensure a new
  // connection gets created should fix it.
  drupal_static_reset('islandora_get_tuque_connection');
  // Parse the payload into an associative array.
  $payload = json_decode(stream_get_contents(STDIN), TRUE);
  // Get the list of all available jobs.
  $job_registry = module_invoke_all("islandora_job_register_jobs");

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
