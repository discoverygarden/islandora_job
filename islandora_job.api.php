<?php

/**
 * @file
 * Hooks provided by Islandora Job.
 */

/**
 * Hook to collect functions that have been registered as jobs.
 *
 * @return array
 *   An associative array, where keys are the name of the function to execute.
 *   Each entry contains the following parameters:
 *   - type: The extension of the file that contains the function. Required.
 *   - module: The name of the drupal module containing the function. Required.
 *   - name: The path to the file relative to the module directory, excluding
 *     the extension. Required.
 *   - alias: A shortened version of the function name. Optional.
 */
function hook_islandora_job_register_jobs() {
  return array(
    'islandora_job_test_job' => array(
      'type' => 'test',
      'module' => 'islandora_job',
      'name' => 'test/islandora_job',
      'alias' => 'ijtj',
    ),
  );
}
