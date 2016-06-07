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
