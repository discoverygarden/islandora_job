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
  // potential issues which come from using stale objects.
  module_load_include('inc', 'islandora', 'includes/utilities');
  $object_id = islandora_is_valid_pid($object_in) ?
    $object_in :
    strval(unserialize($object_in));
  // XXX: Due to how Drush bootstrapping works, the connection may be created
  // without credentials (when your site's front page is
  // 'islandora/object/some:object', for example). Resetting to ensure a new
  // connection gets created should fix it.
  drupal_static_reset('islandora_get_tuque_connection');
  $object = islandora_object_load($object_id);

  $re_add = islandora_get_defer_derivatives_flag($object);
  islandora_remove_defer_derivatives_flag($object);

  $result = islandora_do_derivatives($object, $options);

  // If a derivative callback returns a batch, it needs to be forced to process
  // as there's no form submits or existing batches to trigger it.
  $batch = &batch_get();
  if (!empty($batch)) {
    drush_backend_batch_process();
  }

  if ($re_add) {
    islandora_set_defer_derivatives_flag($object);
  }
  return $result;
}
