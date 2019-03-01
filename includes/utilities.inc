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
 * name to each job function, and swaps out for the alias when
 * necessary.
 *
 * @param string $job_name
 *   Name of the job without site.
 *
 * @return string
 *   Job name with site added.
 */
function islandora_job_function_name($job_name) {
  $job_name = islandora_job_get_aliased_func_name($job_name);
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

/**
 * Attempts to get the correct function for a given payload func_name.
 *
 * It's expected that this is either the actual function name or an alias.
 *
 * @param string $func_name
 *   The function name to get the full function for.
 *
 * @return string
 *   The full function name.
 */
function islandora_job_get_full_func_name($func_name) {
  $alias_map = &drupal_static(__FUNCTION__);
  if (!isset($alias_map)) {
    foreach (module_invoke_all("islandora_job_register_jobs") as $func => $job) {
      if (isset($job['alias'])) {
        $alias_map[$job['alias']] = $func;
      }
    }
  }
  if (isset($alias_map[$func_name])) {
    return $alias_map[$func_name];
  }
  return $func_name;
}

/**
 * Attempts to get the correct alias for a given function name.
 *
 * If the function doesn't provide an alias, it will be returned unaltered.
 *
 * @param string $func_name
 *   The function name to get the alias for.
 *
 * @return string
 *   The aliased function name.
 */
function islandora_job_get_aliased_func_name($func_name) {
  $alias_map = &drupal_static(__FUNCTION__);
  if (!isset($alias_map)) {
    foreach (module_invoke_all("islandora_job_register_jobs") as $func => $job) {
      if (isset($job['alias'])) {
        $alias_map[$func] = $job['alias'];
      }
    }
  }
  if (isset($alias_map[$func_name])) {
    return $alias_map[$func_name];
  }
  return $func_name;
}

/**
 * Gets the entire list of function names, aliased if available.
 *
 * @return array
 *   The list of (possibly aliased) function names.
 */
function islandora_job_get_aliased_function_names() {
  $names = array();
  foreach (module_invoke_all('islandora_job_register_jobs') as $func => $job) {
    if (isset($job['alias'])) {
      $names[] = $job['alias'];
    }
    else {
      $names[] = $func;
    }
  }
  return $names;
}
