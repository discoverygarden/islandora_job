<?php

/**
 * @file
 * Utility functions for the islandora_job module.
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
 * Get job function name.
 *
 * This function allows support for multisites by adding the site
 * name to each job function.
 *
 * @param string $job_name
 *   Name of the job without site.
 *
 * @return string
 *   Job name with site added.
 */
function islandora_job_function_name($job_name) {
  // Chop the 'sites/' part of the path off. This gives you the name of the
  // site dir for the multisite, for example sites/example would be example
  // we use this to create the queue for this multisite.
  $site = substr(conf_path(), 6);

  if ($site == 'default' || !variable_get('islandora_job_multisite_prefix', FALSE)) {
    return $job_name;
  }
  else {
    return "{$site}_{$job_name}";
  }
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
function islandora_job_format_payload($job_name, array $args) {
  global $user;
  global $base_url;
  array_shift($args);
  return json_encode(
    array(
      "func" => $job_name,
      "args" => $args,
      "site" => $base_url,
      "uid" => $user->uid,
    )
  );
}
