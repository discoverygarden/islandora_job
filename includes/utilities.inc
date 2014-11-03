<?php

/**
 * @file
 * Smattering of utility functions for the islandora_job module.
 */

/**
 * Utility function for bootstrapping a GearmanClient from configuration.
 *
 * @return GearmanClient
 *   A GearmanClient pointing at the job server defined in configuration.
 */
function islandora_job_get_client() {
  $host = variable_get('islandora_job_server_host', 'localhost');
  $port = variable_get('islandora_job_server_port', 4730);
  $client = new GearmanClient();
  $client->addServer($host, $port);
  return $client;
}

/**
 * Constructs a payload from arguments passed to various API functions.
 *
 * @param string $job_name
 *   Name of the job to run.
 * @param array $args
 *   Array of arguments to get JSON encoded.  Expects there to be an extra
 *   argument at the beginning from func_get_args().
 *
 * @return string
 *   A JSON encoded payload for Gearman.
 */
function islandora_job_format_payload($job_name, $args) {
  array_shift($args);
  return json_encode(
    array(
      "func" => $job_name,
      "args" => $args,
    )
  );
}

/**
 * Starts a worker function, respecting or creating the specified pid file.
 *
 * @param string $pidfile
 *   The base file name of the pidfile to create (or respect).
 *
 * @return bool
 *   TRUE if the process started and a pidfile was created.  FALSE if the
 *   pidfile exists and its process is still running.
 */
function islandora_job_start_worker($pidfile) {
  // Look in the tmp directory for the pidfile.
  $pidfile = file_directory_temp() . "/$pidfile";

  // We have to manage starting workers because gearman doesn't respect its
  // pidfiles. When needed, this'll have to work on other platforms.
  // If the pidfile exists, exit early if the process is running.
  // Otherwise, ditch the pidfile if it's stale.
  if (file_exists($pidfile)) {
    $pid = trim(file_get_contents($pidfile));
    if (posix_getpgid($pid)) {
      return FALSE;
    }
    else {
      unlink($pidfile);
    }
  }

  // Get the list of all available jobs to run the router for.
  // Be sure to sanitize them.  No funny business, mister!
  $job_registry = module_invoke_all("islandora_job_register_jobs");
  $job_names = array_map("escapeshellarg", array_keys($job_registry));
  $job_names_string = '-f ' . implode(' -f ', $job_names);

  // Start the worker in the background after moving into drupal root.
  // Sanitize and use the specified pid file.
  $cmd = '(cd ' . DRUPAL_ROOT . ' && gearman -v -w ' . $job_names_string . ' -i ' . escapeshellarg($pidfile) . ' drush -u 1 islandora-job-router) > /dev/null 2>&1 &';
  exec($cmd);
  return TRUE;
}

/**
 * Stops the worker process identified by the specified pid file.
 *
 * @param string $pidfile
 *   The base file name of the file containing the process id to kill.
 *
 * @return bool
 *   TRUE if the process was killed.  FALSE otherwise.
 */
function islandora_job_stop_worker($pidfile) {
  // Look in the tmp directory for the pidfile.
  $pidfile = file_directory_temp() . "/$pidfile";

  // When needed, this'll have to work on other platforms...
  // Kill the process whose id is in the pidfile.
  if (file_exists($pidfile)) {
    $pid = trim(file_get_contents($pidfile));
    if (file_exists("/proc/$pid")) {
      exec('kill -TERM ' . escapeshellarg($pid));
      unlink($pidfile);
      return TRUE;
    }
  }
  return FALSE;
}
