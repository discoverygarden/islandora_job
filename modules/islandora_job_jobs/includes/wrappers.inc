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
  $re_add = islandora_get_defer_derivatives_flag($object);
  islandora_remove_defer_derivatives_flag($object);

  $result = islandora_do_derivatives($object, $options);
  if ($re_add) {
    islandora_set_defer_derivatives_flag($object);
  }
  return $result;
}
