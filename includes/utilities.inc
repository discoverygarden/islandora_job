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
