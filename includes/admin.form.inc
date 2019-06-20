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
function islandora_job_admin_form(array $form, array &$form_state) {
  $form['islandora_job_server_host'] = array(
    '#type' => 'textfield',
    '#title' => t('Job Server Hostname'),
    '#description' => t('Address to the job server'),
    '#required' => TRUE,
    '#default_value' => variable_get('islandora_job_server_host', 'localhost'),
  );
  $form['islandora_job_server_port'] = array(
    '#type' => 'textfield',
    '#title' => t('Job Server Port'),
    '#description' => t('Port that the job server is listening on'),
    '#required' => TRUE,
    '#default_value' => variable_get('islandora_job_server_port', 4730),
  );
  $form['islandora_job_multisite_prefix'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable multisite prefix'),
    '#description' => t('Add a multisite prefix to job queue name'),
    '#default_value' => variable_get('islandora_job_multisite_prefix', FALSE),
  );
  $form['islandora_job_display_messages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display Gearman reporting messages to users with permission.'),
    '#default_value' => variable_get('islandora_job_display_messages', FALSE),
    '#element_validate' => array('islandora_job_display_messages_validate'),
  );
  $form['islandora_job_display_messages_remote_database'] = array(
    '#type' => 'checkbox',
    '#title' => t('Attempt to connect to remote database as defined in the settings.php as per the README.'),
    '#default_value' => variable_get('islandora_job_display_messages_remote_database', FALSE),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_job_display_messages"]' => array('checked' => TRUE),
      ),
    ),
  );

  return system_settings_form($form);
}

/**
 * Validates that the gearman_queue can be found to drive reports.
 */
function islandora_job_display_messages_validate($element, &$form_state, $form) {
  if ($element['#value'] === 1) {
    $connection = $form_state['values']['islandora_job_display_messages_remote_database'] === 1 ? Database::getConnection('default', 'gearman_queue') : Database::getConnection();
    if (!$connection->schema()->tableExists('gearman_queue')) {
      form_error($element, t('Unable to find the gearman_queue table to query.'));
    }
  }
}
