<?php

/**
 * @file
 * Database functionality for tracking job status in Islandora Job.
 */

/**
 * Inserts a job to have its state tracked.
 *
 * @param string $data
 *   A JSON encoded string representing the payload data.
 *
 * @return int
 *   The ID for the job's status in the database.
 */
function islandora_job_insert_job_status($data) {
  return db_insert('islandora_job_job_status')
    ->fields(array(
      'data' => $data,
    ))
    ->execute();
}

/**
 * Updates a job's state.
 *
 * @param string $job_id
 *   The ID of the job being tracked to be updated.
 * @param int $status
 *   The state of the job being created or updated.
 */
function islandora_job_update_job_status($job_id, $status) {
  db_update('islandora_job_job_status')
    ->fields(array(
      'state' => $status,
    ))
    ->condition('status_id', $job_id)
    ->execute();
}

/**
 * Selects the status entries for a given job.
 *
 * @param string $data
 *   A job payload for which to search.
 * @param int $state
 *   The state to which to constrain results.
 *
 * @return DatabaseStatementInterface
 *   An executed SELECT query.
 */
function _islandora_job_status_id_results($data, $state = ISLANDORA_JOB_STATE__WAITING) {
  $info = Database::getConnectionInfo();
  $driver = $info['default']['driver'];

  $query = db_select('islandora_job_job_status', 'i')
    ->fields('i', array('status_id'))
    ->condition('state', $state);

  if ($driver == 'pgsql') {
    // XXX: Not sure who's to blame (probably Drupal?); however, Postgres blows
    // up with insufficiently escaped input when querying in the data.
    // Specifically, seems to run into issues due to the escaping of forward-
    // slashes in the "site" part of the payload; however, could be encountered
    // elsewhere in different jobs... let's just fully escape 'em.
    $query->condition('data', addcslashes($data, '\\'));
  }
  else {
    $query->condition('data', $data);
  }

  return $query->execute();
}

/**
 * Updates a job's state given a payload.
 *
 * @param string $data
 *   The data of the payload being used to identify the job.
 *
 * @return int
 *   The ID of the status to be used for removal later.
 */
function islandora_job_start_job_status($data) {
  $tx = db_transaction();

  try {
    // XXX: Note this is not a race condition as there will only ever be one job
    // in the waiting state due to the deduplication introduced.
    // Get the status ID of the database to use to remove itself later on.
    $status_id = _islandora_job_status_id_results($data)->fetchField();
    islandora_job_update_job_status($status_id, ISLANDORA_JOB_STATE__RUNNING);

    return $status_id;
  }
  catch (Exception $e) {
    $tx->rollback();
  }
}

/**
 * Removes a job from tracking.
 *
 * @param string $job_id
 *   The ID of the job to be removed from tracking.
 */
function islandora_job_remove_job_status($job_id) {
  db_delete('islandora_job_job_status')
    ->condition('status_id', $job_id)
    ->execute();
}

/**
 * Ensures there's no duplicate job that already is waiting to be ran.
 *
 * @param string $data
 *   A JSON encoded string representing the payload data.
 *
 * @return bool
 *   TRUE if the job is able to be queued, FALSE otherwise.
 */
function islandora_job_ensure_job_deduping($data) {
  return !_islandora_job_status_id_results($data)
    ->fetchField();
}
