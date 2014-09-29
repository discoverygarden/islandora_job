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
  $payload = json_encode(
    array(
      "func" => $job_name,
      "args" => $args,
    )
  );
  // Don't forget to add 'dem slashes.
  // Otherwise bash will strip out the quotes.
  return addslashes($payload);
}

/**
 * Starts a worker function, respecting or creating the specified pid file.
 *
 * @param string $pidfile
 *   The pidfile to create (or respect)
 *
 * @return bool
 *   TRUE if the process started and a pidfile was created.  FALSE if the
 *   pidfile exists and its process is still running.
 */
function islandora_job_start_worker($pidfile) {
  // When needed, this'll have to work on other platforms...
  if (file_exists($pidfile)) {
    $pid = trim(file_get_contents($pidfile));
    if (file_exists("/proc/$pid")) {
      return FALSE;
    }
    else {
      unlink($pidfile);
    }
  }
  exec('cd ' . DRUPAL_ROOT . '; gearman -i ' . escapeshellarg($pidfile) . ' -v -w -f islandora_job xargs drush islandora-job-router > /dev/null &');
  return TRUE;
}

/**
 * Stops the worker process identified by the specified pid file.
 *
 * @param string $pidfile
 *   The file containing the process id to kill
 *
 * @return bool
 *   TRUE if the process was killed.  FALSE otherwise.
 */
function islandora_job_stop_worker($pidfile) {
  // When needed, this'll have to work on other platforms...
  if (file_exists($pidfile)) {
    $pid = trim(file_get_contents($pidfile));
    if (file_exists("/proc/$pid")) {
      exec('kill -TERM `cat ' . escapeshellarg($pidfile) . '`');
      unlink($pidfile);
      return TRUE;
    }
  }
  return FALSE;
}
