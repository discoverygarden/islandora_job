<?php

/**
 * @file
 * Wrappers for serializations.
 */

/**
 * A wrapper to help serialize objects for islandora_job.
 *
 * @see islandora_do_derivatives()
 */
function islandora_job_jobs_islandora_do_derivatives($object_in, array $options = array()) {
  module_load_include('inc', 'islandora', 'includes/derivatives');
  // XXX: $object_in at one point was a serialized AbstractObject. Let's get its
  // string value (which should be its PID), to load it properly, to avoid any
  // potential issues.
  $object = islandora_object_load("$object_in");
  return islandora_do_derivatives($object, $options);
}
