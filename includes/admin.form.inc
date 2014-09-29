<?php

/**
 * @file
 * Configuration form for the islandora_job module.
 */

/**
 * Form function to generate the configuration form.
 *
 * @param array $form
 *   Array of Drupal Form API elements.
 * @param array $form_state
 *   Array containing the form state.
 */
function islandora_job_admin_form($form, &$form_state) {
  $form['job_server_host'] = array(
    '#type' => 'textfield',
    '#title' => t('Job Server Hostname'),
    '#description' => t('Address to the job server'),
    '#required' => TRUE,
    '#default_value' => 'localhost',
  );
  $form['job_server_port'] = array(
    '#type' => 'textfield',
    '#title' => t('Job Server Port'),
    '#description' => t('Port that the job server is listening on'),
    '#required' => TRUE,
    '#default_value' => 4730,
  );
  $form['job_server_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Submit function for the configuration form.
 *
 * Persists config values using variable_set().
 *
 * @param array $form
 *   Array of Drupal Form API elements.
 * @param array $form_state
 *   Array containing the form state.
 */
function islandora_job_admin_form_submit($form, &$form_state) {
  variable_set('islandora_job_server_host', $form_state['values']['job_server_host']);
  variable_set('islandora_job_server_port', intval($form_state['values']['job_server_host']));
  drupal_set_message(t("Configuration has been successfully saved."));
}
