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
function islandora_job_jobs_islandora_do_derivatives($object, array $options = array()) {
  module_load_include('inc', 'islandora', 'includes/derivatives');
  return islandora_do_derivatives(unserialize($object), $options);
}
