<?php

/**
 * @file
 * Misc utility functions.
 */

/**
 * Element validation callback; the value is either a positive integer or zero.
 *
 * @see element_validate_integer_positive()
 */
function islandora_job_jobs_element_validate_integer_positive_or_zero($element, &$form_state) {
  $value = $element['#value'];
  if ($value !== '' && (!is_numeric($value) || intval($value) != $value || $value < 0)) {
    form_error($element, t('%name must be either a positive integer or 0.', array('%name' => $element['#title'])));
  }
}

/**
 * Form validation callback for the isladnora_job_admin_form.
 */
function islandora_job_jobs_islandora_job_admin_form_validate($form, $form_state) {
  if ($form_state['values']['islandora_job_jobs_derivatives_idle_wait'] && !$form_state['values']['islandora_job_jobs_derivative_requeues']) {
    $element = $form['islandora_job_jobs_derivative_requeues'];
    form_error($element, t('@title field is required.', array(
      '@title' => $element['#title'],
    )));
  }
}

/**
 * Enforce wait time for derivative object.
 */
function islandora_job_jobs_enforce_idle_wait($object, $entrances) {
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
        return FALSE;

      }
    }
  }

  return TRUE;
}
