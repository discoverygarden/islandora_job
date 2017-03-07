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
function islandora_job_jobs_islandora_do_derivatives($object_in, array $options = array(), $entrances = 0) {
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
  if (!$object) {
    return;
  }
  $time = variable_get('islandora_job_jobs_derivatives_idle_wait', '10');
  if ($time > 0) {
    $diff = time() - $object->lastModifiedDate->getTimestamp();
    if ($diff < $time) {
      $max = variable_get('islandora_job_jobs_derivative_requeues', 3);
      if ($entrances >= $max) {
        // Avoid starvation, if an object were to continuously change in some
        // manner.
        $subs = array(
          '@pid' => $object->id,
          '@time' => $time,
          '@max' => $max,
        );
        drush_log(t('Proceeding with derivatives for @pid despite having been modified in the last @time seconds (requeued @max times).', $subs), 'warning');
        watchdog('islandora_job_jobs', 'Proceeding with derivatives for @pid despite having been modified in the last @time seconds (requeued @max times).', $subs, WATCHDOG_WARNING, l(t('object'), "islandora/object/{$object->id}"));
      }
      else {
        drush_log(t('Waiting for idle time for given object (@pid modified since @time seconds ago (@diff seconds, @entrances entrances)); requeuing.', array(
          '@pid' => $object->id,
          '@time' => $time,
          '@diff' => $diff,
          '@entrances' => $entrances,
        )), 'warning');
        if ($entrances > 0) {
          // A minor optimization if we get hit during heavy processing: If we
          // have been requeued before this attempt, sleep until the difference
          // passes. In the event we _do_ get hit during heavy processing, there
          // is a chance that there are enough items in the queue such that the
          // job will only be reattempted after our time is up, so no need to
          // waste a worker's time sleeping.
          sleep($time - $diff);
        }
        islandora_job_submit_background('islandora_job_jobs_islandora_do_derivatives', $object->id, $options, $entrances + 1);
        return;
      }
    }
  }

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
